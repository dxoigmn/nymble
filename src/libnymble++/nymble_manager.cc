#include "nymble_manager.h"

namespace Nymble {

NymbleManager::NymbleManager()
{
  random_bytes(DIGEST_SIZE, &this->mac_key_np);
  random_bytes(DIGEST_SIZE, &this->mac_key_n);
  random_bytes(DIGEST_SIZE, &this->seed_key_n);
  random_bytes(CIPHER_BLOCK_SIZE, &this->enc_key_n);
  
  this->sign_key_n = RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
}

NymbleManager::~NymbleManager()
{
  for (NymbleManagerEntries::iterator entry = this->entries.begin(); entry != this->entries.end(); entry++) {
    delete *entry;
  }
  
  RSA_free(this->sign_key_n);
}

NymbleManagerEntry* NymbleManager::findServer(std::string sid)
{
  for (NymbleManagerEntries::iterator entry = this->entries.begin(); entry != this->entries.end(); entry++) {
    if (sid == (*entry)->getServerId()) {
      return *entry;
    }
  }

  return NULL;
}

void NymbleManager::computeNymble(std::string seed, std::string* nymble)
{
  char g[] = "g";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, (u_char*)seed.c_str(), seed.size());
  SHA256_Update(&ctx, (u_char*)g, sizeof(g));
  SHA256_Final((u_char*)hash, &ctx);
  
  *nymble = std::string(hash, sizeof(hash));
}

void NymbleManager::evolveSeed(std::string seed, int delta, std::string* seed_out)
{
  char f[] = "f";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, seed.c_str(), sizeof(hash));
  
  for (int i = 0; i < delta; i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)f, sizeof(f));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  *seed_out = std::string(hash, sizeof(hash));
}

std::string NymbleManager::getMacKeyNP()
{
  return this->mac_key_np;
}

bool NymbleManager::verifyPseudonym(Pseudonym pseudonym)
{
  char mac[DIGEST_SIZE];
  HMAC_CTX ctx;

  HMAC_Init(&ctx, this->mac_key_np.c_str(), this->mac_key_np.size(), EVP_sha256());
  HMAC_Update(&ctx, (u_char*)pseudonym.nym().c_str(), pseudonym.nym().size());
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, (u_char*)mac, NULL);
  
  return (std::string(mac, sizeof(mac)) == pseudonym.mac());
}

bool NymbleManager::createCredential(std::string sid, Pseudonym pseudonym, Credential* credential)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;

  HMAC_Init(&hmac_ctx, this->seed_key_n.c_str(), this->seed_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)pseudonym.nym().c_str(), pseudonym.nym().size());
  HMAC_Update(&hmac_ctx, (u_char*)pseudonym.mac().c_str(), pseudonym.mac().size());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
  
  std::string seed(mac, sizeof(mac));
  std::string nymble0;
  std::string nymble;
  
  computeNymble(seed, &nymble0);
  
  for (int t = 1; t < TIME_PERIODS; t++) {
    evolveSeed(seed, 1, &seed);
    computeNymble(seed, &nymble);
    
    EVP_CIPHER_CTX cipher_ctx;
    char cipher[1024];
    int cipher_len;
    int final_len;
    std::string iv;
    
    random_bytes(CIPHER_BLOCK_SIZE, &iv);
    
    EVP_CIPHER_CTX_init(&cipher_ctx);
    EVP_EncryptInit_ex(&cipher_ctx, EVP_aes_128_cbc(), NULL, (u_char*)this->enc_key_n.c_str(), (u_char*)iv.c_str());
    EVP_EncryptUpdate(&cipher_ctx, (u_char*)cipher, &cipher_len, (u_char*)nymble0.c_str(), nymble0.size());
    EVP_EncryptUpdate(&cipher_ctx, (u_char*)cipher, &cipher_len, (u_char*)seed.c_str(), seed.size());
    EVP_EncryptFinal_ex(&cipher_ctx, (u_char*)(cipher + cipher_len), &final_len);
    EVP_CIPHER_CTX_cleanup(&cipher_ctx);
    
    std::string ctxt(cipher, cipher_len + final_len);
    
    HMAC_Init(&hmac_ctx, (u_char*)this->mac_key_n.c_str(), this->mac_key_n.size(), EVP_sha256());
    HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
    HMAC_Update(&hmac_ctx, (u_char*)&t, sizeof(t));
    HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
    HMAC_Update(&hmac_ctx, (u_char*)nymble.c_str(), nymble.size());
    HMAC_Update(&hmac_ctx, (u_char*)ctxt.c_str(), ctxt.size());
    HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
    
    std::string mac_n(mac, sizeof(mac));
    
    HMAC_Init(&hmac_ctx, (u_char*)entry->getMacKeyNS().c_str(), entry->getMacKeyNS().size(), EVP_sha256());
    HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
    HMAC_Update(&hmac_ctx, (u_char*)&t, sizeof(t));
    HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
    HMAC_Update(&hmac_ctx, (u_char*)nymble.c_str(), nymble.size());
    HMAC_Update(&hmac_ctx, (u_char*)ctxt.c_str(), ctxt.size());
    HMAC_Update(&hmac_ctx, (u_char*)mac_n.c_str(), mac_n.size());
    HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
    
    std::string mac_ns(mac, sizeof(mac));
    
    Ticket* ticket = credential->add_ticket();
    
    ticket->set_nymble(nymble);
    ticket->set_ctxt(ctxt);
    ticket->set_mac_n(mac_n);
    ticket->set_mac_ns(mac_ns);
  }
  
  credential->set_nymble0(nymble0);
  
  return true;
}

bool NymbleManager::verifyTicket(std::string sid, Ticket ticket)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::signBlacklist(std::string sid, u_int t, u_int w, std::string target, Blacklist blist, BlacklistCert* cert)
{
  u_char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;

  HMAC_Init(&hmac_ctx, this->mac_key_n.c_str(), this->mac_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&t, sizeof(t));
  HMAC_Update(&hmac_ctx, (u_char*)&w, sizeof(w));
  
  for (int i = 0; i < blist.nymble_size(); i++) {
    std::string nymble = blist.nymble(i);
    HMAC_Update(&hmac_ctx, (u_char*)nymble.c_str(), nymble.size());
  }
  
  HMAC_Update(&hmac_ctx, (u_char*)target.c_str(), target.size());
  HMAC_Final(&hmac_ctx, mac, NULL);
  
  
  u_char hash[DIGEST_SIZE];
  SHA256_CTX hash_ctx;
  
  SHA256_Init(&hash_ctx);
  SHA256_Update(&hash_ctx, (u_char*)sid.c_str(), sid.size());
  SHA256_Update(&hash_ctx, (u_char*)&t, sizeof(t));
  SHA256_Update(&hash_ctx, (u_char*)&w, sizeof(w));
  
  for (int i = 0; i < blist.nymble_size(); i++) {
    std::string nymble = blist.nymble(i);
    SHA256_Update(&hash_ctx, (u_char*)nymble.c_str(), nymble.size());
  }
  
  SHA256_Update(&hash_ctx, (u_char*)target.c_str(), target.size());
  SHA256_Final(hash, &hash_ctx);
  
  
  u_char buffer[SIGNATURE_SIZE];
  RSA_padding_add_PKCS1_PSS(this->sign_key_n, buffer, hash, EVP_sha256(), -2);
  
  
  u_char sig[SIGNATURE_SIZE];
  RSA_private_encrypt(SIGNATURE_SIZE, buffer, sig, this->sign_key_n, RSA_NO_PADDING);
  
  cert->set_t(t);
  cert->set_daisy(target);
  cert->set_mac(mac, sizeof(mac));
  cert->set_sig(sig, sizeof(sig));
  
  return true;
}

bool NymbleManager::verifyBlacklist(std::string sid, Blacklist blist, BlacklistCert cert)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}


bool NymbleManager::registerServer(std::string sid, ServerState* server_state)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry != NULL) {
    return false;
  }
  
  entry = new NymbleManagerEntry(sid);
  
  this->entries.push_back(entry);
  
  char h[] = "h";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, entry->getDaisyL().c_str(), sizeof(hash));
  
  for (u_int i = 0; i < TIME_PERIODS - this->cur_time_period + 1; i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)h, sizeof(h));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  std::string target = std::string(hash, sizeof(hash));
  
  Blacklist* blist = server_state->mutable_blist();
  BlacklistCert* cert = server_state->mutable_cert();
  
  if (!signBlacklist(sid, this->cur_time_period, this->cur_link_window, target, *blist, cert)) {
    return false;
  }
  
  server_state->set_sid(sid);
  server_state->set_mac_key_ns(entry->getMacKeyNS());
  server_state->set_time_last_updated(this->cur_time_period);
  
  return true;
}

bool NymbleManager::computeBlacklistUpdate(std::string sid, Blacklist blist, Complaints clist, Blacklist* blist_out, BlacklistCert* cert_out)
{
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::computeTokens(u_int t_prime, Blacklist* blist, Complaints clist, Seeds* seeds)
{
  // IMPLEMENT
  
  return false;
}

}; // namespace Nymble
