#include "nymble_manager.h"

namespace Nymble {

NymbleManager::NymbleManager()
{
  RAND_bytes(this->hmac_key_n, DIGEST_SIZE);
  RAND_bytes(this->keyedhash_key_n, DIGEST_SIZE);
  RAND_bytes(this->encrypt_key_n, CIPHER_BLOCK_SIZE);
  
  this->entries = new NymbleManagerEntries();
}

NymbleManager::~NymbleManager()
{
  RSA_free(this->sign_key_n);
  
  for (NymbleManagerEntries::iterator entry = this->entries->begin(); entry != this->entries->end(); entry++) {
    delete *entry;
  }
  
  delete this->entries;
}

u_char* NymbleManager::getHmacKeyN()
{
  return this->hmac_key_n;
}

u_char* NymbleManager::getEncryptKeyN()
{
  return this->encrypt_key_n;
}

u_char* NymbleManager::getKeyedhashKeyN()
{
  return this->keyedhash_key_n;
}

void NymbleManager::setHmacKeyNP(u_char* hmac_key_np)
{
  memcpy(this->hmac_key_np, hmac_key_np, DIGEST_SIZE);
}

void NymbleManager::readSignKey(char* sign_key_path)
{
  FILE* sign_key = fopen(sign_key_path, "r");
  
  this->sign_key_n = PEM_read_RSAPrivateKey(sign_key, NULL, NULL, NULL);
  
  fclose(sign_key);
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

Blacklist* NymbleManager::updateBlacklist(u_char* server_id, Blacklist* blacklist, Complaints* complaints)
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
    for (Complaints::iterator complaint = complaints->begin(); complaint != complaints->end(); ++complaint) {
      u_char* new_nymble = new u_char[DIGEST_SIZE];
      u_char pseudonym[DIGEST_SIZE];
      u_char seed[DIGEST_SIZE];
      u_char nymble0[DIGEST_SIZE];
      Ticket* ticket = (*complaint)->getTicket();
      
      // Compute the nymble0 for the current complaint ticket
      ticket->decrypt(this->encrypt_key_n, NULL, pseudonym);
      this->seed(entry, pseudonym, seed);
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

Tokens* NymbleManager::createTokens(u_char* server_id, Blacklist* blacklist, Complaints* complaints)
{
  NymbleManagerEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  Tokens* tokens = new Tokens();
  
  for (Complaints::iterator complaint = complaints->begin(); complaint != complaints->end(); ++complaint) {
    u_char trapdoor[DIGEST_SIZE];
    u_char pseudonym[DIGEST_SIZE];
    u_char seed[DIGEST_SIZE];
    u_char nymble0[DIGEST_SIZE];
    Ticket* ticket = (*complaint)->getTicket();
    
    ticket->decrypt(this->encrypt_key_n, trapdoor, pseudonym);
    this->seed(entry, pseudonym, seed);
    Ticket::computeNymble(seed, nymble0);
    
    if (this->userIsBlacklisted(blacklist, nymble0)) {
      RAND_bytes(trapdoor, DIGEST_SIZE);
    } else {
      u_int delta = this->cur_time_period - (*complaint)->getTime();
      
      Ticket::evolveSeed(trapdoor, delta, trapdoor);
      
      // FIXME: This should add nymble0 to be checked in userIsBlacklisted.
    }
    
    tokens->push_back(new Token(trapdoor));
  }
  
  return tokens;
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
  
  Credential* credential = new Credential(this, entry, pseudonym, time_periods);
  
  return credential;
}

void NymbleManager::seed(NymbleManagerEntry *entry, u_char *pseudonym, u_char *out)
{
  HMAC_CTX ctx;
  
  HMAC_Init(&ctx, this->getKeyedhashKeyN(), DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym, DIGEST_SIZE);
  HMAC_Update(&ctx, entry->getServerId(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, out, NULL);
  HMAC_CTX_cleanup(&ctx);
}

}; // namespace Nymble
