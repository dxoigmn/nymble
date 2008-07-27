#include "nymble_manager.h"

NymbleManager::NymbleManager()
{
  RAND_bytes(this->hmac_key_n, DIGEST_SIZE);
  RAND_bytes(this->keyedhash_key_n, DIGEST_SIZE);
  RAND_bytes(this->encrypt_key_n, CIPHER_BLOCK_SIZE);
  
  this->sign_key_n = RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
  this->entries = new NymbleManagerEntries();
}

NymbleManager::NymbleManager(u_char* hmac_key_np)
{
  NymbleManager();
  this->setHmacKeyNP(hmac_key_np);
}

NymbleManager::~NymbleManager()
{
  RSA_free(this->sign_key_n);
  
  for (NymbleManagerEntries::iterator entry = this->entries->begin(); entry != this->entries->end(); entry++) {
    delete *entry;
  }
  
  delete this->entries;
}

void NymbleManager::setHmacKeyNP(u_char* hmac_key_np)
{
  memcpy(this->hmac_key_np, hmac_key_np, DIGEST_SIZE);
}

u_int NymbleManager::getVerifyKeyN(u_char** out)
{
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSA_PUBKEY(bio, this->sign_key_n);
  RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
  BIO_free(bio);
  
  return i2d_RSAPublicKey(rsa, out);
}

u_int NymbleManager::getSignKeyN(u_char** out) {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSAPrivateKey(bio, this->sign_key_n, NULL, NULL, 0, NULL, NULL);
  RSA *rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
  BIO_free(bio);

  return i2d_RSAPrivateKey(rsa, out);
}

u_char* NymbleManager::addServer(u_char* server_id)
{
  NymbleManagerEntry* entry = this->findServer(server_id);
  
  if (entry == NULL) {
    entry = new NymbleManagerEntry(server_id, this->cur_time_period);
    this->entries->push_back(entry);
  }
  
  return entry->getHmacKeyNS();
}

NymbleManagerEntry* NymbleManager::findServer(u_char *server_id)
{
  for (NymbleManagerEntries::iterator entry = this->entries->begin(); entry != this->entries->end(); entry++) {
    if (memcmp((*entry)->getServerId(), server_id, DIGEST_SIZE) == 0) {
      return *entry;
    }
  }

  return NULL;
}

bool NymbleManager::verifyPseudonym(Pseudonym* pseudonym)
{
  u_char mac[DIGEST_SIZE];
  HMAC_CTX ctx;

  HMAC_Init(&ctx, this->hmac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym->getPseudonym(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, mac, NULL);

  return (memcmp(mac, pseudonym->getMacNP(), DIGEST_SIZE) == 0);
}

bool NymbleManager::verifyBlacklist(u_char* server_id, Blacklist* blacklist)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return false;
  }
  
  bool valid = true;
  
  if (memcmp(entry->getServerId(), blacklist->getServerId(), DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  u_char bl_hash[DIGEST_SIZE];
  blacklist->hash(bl_hash);
  
  if (memcmp(bl_hash, blacklist->getHash(), DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  u_char bmac_n[DIGEST_SIZE];
  blacklist->hmac(this->hmac_key_n, bmac_n);

  if (memcmp(bmac_n, blacklist->getHmac(), DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  if (blacklist->getLinkWindow() != this->cur_link_window) {
    valid = false;
  }
  
  if (blacklist->getTimePeriod() != entry->getLastUpdated()) {
    valid = false;
  }

  return valid;
}

Blacklist* NymbleManager::createBlacklist(u_char* server_id)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  Blacklist *blacklist = new Blacklist(entry->getServerId(), this->cur_link_window, this->cur_time_period);
  
  blacklist->hash();
  blacklist->hmac(this->hmac_key_n);
  blacklist->sign(this->sign_key_n);
  
  return blacklist;
}

Blacklist* NymbleManager::updateBlacklist(u_char* server_id, Blacklist* blacklist, Tickets* complaints)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  Blacklist* new_blacklist = new Blacklist(blacklist->getServerId(), this->cur_link_window, this->cur_time_period);
  
  for (Nymbles::iterator nymble = blacklist->begin(); nymble != blacklist->end(); ++nymble) {
    u_char* new_nymble = new u_char[DIGEST_SIZE];
    memcpy(new_nymble, *nymble, DIGEST_SIZE);
    new_blacklist->push_back(new_nymble);
  }
  
  if (!complaints->empty()) {
    for (Tickets::iterator ticket = complaints->begin(); ticket != complaints->end(); ++ticket) {
      u_char* new_nymble = new u_char[DIGEST_SIZE];
      u_char pseudonym[DIGEST_SIZE];
      u_char seed[DIGEST_SIZE];
      u_char nymble0[DIGEST_SIZE];
      
      // Compute the nymble0 for the current complaint ticket
      this->decryptTrapdoor(NULL, pseudonym, *ticket);
      this->seedTrapdoor(seed, entry, pseudonym);
      Ticket::computeNymble(seed, nymble0);
      
      if (this->userIsBlacklisted(new_blacklist, nymble0)) {
        RAND_bytes(new_nymble, DIGEST_SIZE);
      } else {
        memcpy(new_nymble, nymble0, DIGEST_SIZE);
      }
      
      new_blacklist->push_back(new_nymble);
    }
  }
  
  new_blacklist->hash();
  new_blacklist->hmac(this->hmac_key_n);
  new_blacklist->sign(this->sign_key_n);
  
  entry->setLastUpdated(this->cur_time_period);
  
  return new_blacklist;
}

LinkingTokens* NymbleManager::createLinkingTokens(u_char* server_id, Blacklist* blacklist, Tickets* complaints)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  LinkingTokens* linking_tokens = new LinkingTokens();
  
  for (Tickets::iterator ticket = complaints->begin(); ticket != complaints->end(); ++ticket) {
    u_char trapdoor[DIGEST_SIZE];
    u_char pseudonym[DIGEST_SIZE];
    u_char seed[DIGEST_SIZE];
    u_char nymble0[DIGEST_SIZE];
    
    this->decryptTrapdoor(trapdoor, pseudonym, *ticket);
    this->seedTrapdoor(seed, entry, pseudonym);
    Ticket::computeNymble(seed, nymble0);
    
    if (this->userIsBlacklisted(blacklist, nymble0)) {
      RAND_bytes(trapdoor, DIGEST_SIZE);
    } else {
      u_int delta = this->cur_time_period - (*ticket)->getTimePeriod();
      
      Ticket::evolveTrapdoor(trapdoor, delta, trapdoor);
      
      // FIXME: This should add nymble0 to be checked in userIsBlacklisted.
    }
    
    linking_tokens->push_back(new LinkingToken(this->cur_time_period, trapdoor));
  }
  
  return linking_tokens;
}

bool NymbleManager::userIsBlacklisted(Nymbles* nymbles, u_char* nymble0)
{
  for (Nymbles::iterator nymble = nymbles->begin(); nymble != nymbles->end(); ++nymble) {
    if (memcmp(nymble0, *nymble, DIGEST_SIZE) == 0) {
      return true;
    }
  }
  
  return false;
}

Credential* NymbleManager::createCredential(u_char* server_id, Pseudonym* pseudonym, u_int time_periods)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  u_char seed[DIGEST_SIZE];
  
  this->seedTrapdoor(seed, entry, pseudonym->getPseudonym());
  
  Credential* credential = new Credential(seed, entry->getServerId(), this->cur_link_window, time_periods);
  
  this->signCredential(credential, pseudonym, seed);
  entry->signCredential(credential);
  
  return credential;
}

void NymbleManager::seedTrapdoor(u_char *out, NymbleManagerEntry *entry, u_char *pseudonym)
{
  HMAC_CTX ctx;

  HMAC_Init(&ctx, this->keyedhash_key_n, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym, DIGEST_SIZE);
  HMAC_Update(&ctx, entry->getServerId(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, out, NULL);
  HMAC_CTX_cleanup(&ctx);
}

void NymbleManager::signCredential(Credential* credential, Pseudonym* pseudonym, u_char* seed)
{
  for (Tickets::iterator ticket = credential->begin(); ticket != credential->end(); ++ticket)
  {
    this->encryptTrapdoor(*ticket, pseudonym, seed);
    (*ticket)->hmac(this->hmac_key_n);
  }
}

void NymbleManager::encryptTrapdoor(Ticket* ticket, Pseudonym* pseudonym, u_char* seed)
{
  ticket->encrypt(this->encrypt_key_n, seed, pseudonym->getPseudonym());
}

void NymbleManager::decryptTrapdoor(u_char *trapdoor, u_char *pseudonym, Ticket* ticket)
{
  ticket->decrypt(this->encrypt_key_n, trapdoor, pseudonym);
}
