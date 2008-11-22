#include "nymble_manager.h"

namespace Nymble {

NymbleManager::NymbleManager()
{
  random_bytes(DIGEST_SIZE, &this->mac_key_np);
  random_bytes(DIGEST_SIZE, &this->mac_key_n);
  random_bytes(DIGEST_SIZE, &this->seed_key_n);
  random_bytes(CIPHER_BLOCK_SIZE, &this->enc_key_n);
  
  this->sign_key_n = RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
  this->time_periods = TIME_PERIODS;
}

NymbleManager::~NymbleManager()
{
  for (NymbleManagerEntries::iterator entry = this->entries.begin(); entry != this->entries.end(); entry++) {
    delete *entry;
  }
  
  RSA_free(this->sign_key_n);
}

void NymbleManager::reset(u_int time_periods)
{
  for (NymbleManagerEntries::iterator entry = this->entries.begin(); entry != this->entries.end(); entry++) {
    delete *entry;
  }
  
  this->entries.clear();
  this->setTimePeriod(1);
  this->setLinkWindow(0);
  this->time_periods = time_periods;
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

bool NymbleManager::writeVerifyKeyN(std::string path)
{
  FILE* verify_key_n = fopen(path.c_str(), "w+b");
  
  if (verify_key_n == NULL) {
    return false;
  }
  
  int ret = PEM_write_RSA_PUBKEY(verify_key_n, this->sign_key_n);
  fclose(verify_key_n);
  
  return (ret != 0);
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

bool NymbleManager::createCredential(std::string sid, Pseudonym pnym, Credential* cred)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  if (!this->verifyPseudonym(pnym)) {
    return false;
  }
  
  if (cred == NULL) {
    return false;
  }
  
  char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;

  HMAC_Init(&hmac_ctx, this->seed_key_n.c_str(), this->seed_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)pnym.nym().c_str(), pnym.nym().size());
  HMAC_Update(&hmac_ctx, (u_char*)pnym.mac().c_str(), pnym.mac().size());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
  
  std::string seed(mac, sizeof(mac));
  std::string nymble0;
  std::string nymble;
  
  computeNymble(seed, &nymble0);
  
  for (u_int t = 1; t <= this->time_periods; t++) {
    evolveSeed(seed, 1, &seed);
    computeNymble(seed, &nymble);
    
    
    EVP_CIPHER_CTX cipher_ctx;
    char cipher[1024];
    int cipher_len;
    std::string iv;
    std::string ctxt;
    
    random_bytes(CIPHER_BLOCK_SIZE, &iv);
    
    EVP_CIPHER_CTX_init(&cipher_ctx);
    EVP_EncryptInit_ex(&cipher_ctx, EVP_aes_128_cbc(), NULL, (u_char*)this->enc_key_n.c_str(), (u_char*)iv.c_str());
    ctxt += iv;
    
    EVP_EncryptUpdate(&cipher_ctx, (u_char*)cipher, &cipher_len, (u_char*)nymble0.c_str(), nymble0.size());
    ctxt += std::string(cipher, cipher_len);
    
    EVP_EncryptUpdate(&cipher_ctx, (u_char*)cipher, &cipher_len, (u_char*)seed.c_str(), seed.size());
    ctxt += std::string(cipher, cipher_len);
    
    EVP_EncryptFinal_ex(&cipher_ctx, (u_char*)cipher, &cipher_len);
    ctxt += std::string(cipher, cipher_len);
    
    EVP_CIPHER_CTX_cleanup(&cipher_ctx);
    
    
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
    
    Ticket* ticket = cred->add_tickets();
    
    ticket->set_nymble(nymble);
    ticket->set_ctxt(ctxt);
    ticket->set_mac_n(mac_n);
    ticket->set_mac_ns(mac_ns);
  }
  
  cred->set_nymble0(nymble0);
  
  return true;
}

bool NymbleManager::verifyTicket(std::string sid, u_int t, u_int w, Ticket ticket)
{
  char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;
  
  HMAC_Init(&hmac_ctx, (u_char*)this->mac_key_n.c_str(), this->mac_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&t, sizeof(t));
  HMAC_Update(&hmac_ctx, (u_char*)&w, sizeof(w));
  HMAC_Update(&hmac_ctx, (u_char*)ticket.nymble().c_str(), ticket.nymble().size());
  HMAC_Update(&hmac_ctx, (u_char*)ticket.ctxt().c_str(), ticket.ctxt().size());
  HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
  
  return (ticket.mac_n() == std::string(mac, sizeof(mac)));
}

bool NymbleManager::signBlacklist(std::string sid, std::string target, Blacklist* blist, BlacklistCert* cert)
{
  u_char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;

  HMAC_Init(&hmac_ctx, this->mac_key_n.c_str(), this->mac_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_time_period, sizeof(this->cur_time_period));
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Update(&hmac_ctx, (u_char*)target.c_str(), target.size());
  
  if (blist != NULL) {
    for (int i = 0; i < blist->nymbles_size(); i++) {
      std::string nymble = blist->nymbles(i);
      HMAC_Update(&hmac_ctx, (u_char*)nymble.c_str(), nymble.size());
    }
  }
  
  HMAC_Final(&hmac_ctx, mac, NULL);
  
  
  u_char hash[DIGEST_SIZE];
  SHA256_CTX hash_ctx;
  
  SHA256_Init(&hash_ctx);
  SHA256_Update(&hash_ctx, (u_char*)sid.c_str(), sid.size());
  SHA256_Update(&hash_ctx, (u_char*)&this->cur_time_period, sizeof(this->cur_time_period));
  SHA256_Update(&hash_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  SHA256_Update(&hash_ctx, (u_char*)target.c_str(), target.size());
  
  if (blist != NULL) {
    for (int i = 0; i < blist->nymbles_size(); i++) {
      std::string nymble = blist->nymbles(i);
      SHA256_Update(&hash_ctx, (u_char*)nymble.c_str(), nymble.size());
    }
  }
  
  SHA256_Final(hash, &hash_ctx);
  
  
  u_char buffer[SIGNATURE_SIZE];
  RSA_padding_add_PKCS1_PSS(this->sign_key_n, buffer, hash, EVP_sha256(), -2);
  
  
  u_char sig[SIGNATURE_SIZE];
  RSA_private_encrypt(SIGNATURE_SIZE, buffer, sig, this->sign_key_n, RSA_NO_PADDING);
  
  cert->set_t(this->cur_time_period);
  cert->set_daisy(target);
  cert->set_mac(mac, sizeof(mac));
  cert->set_sig(sig, sizeof(sig));
  
  return true;
}

bool NymbleManager::verifyBlacklist(std::string sid, u_int t, u_int w, Blacklist blist, BlacklistCert cert)
{
  if (t < cert.t()) {
    fprintf(stderr, "verifyBlacklist: t (%d) < cert.t (%d)\n", t, cert.t());
    return false;
  }
  
  char h[] = "h";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, cert.daisy().c_str(), sizeof(hash));
  
  for (u_int i = 0; i < t - cert.t(); i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)h, sizeof(h));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  std::string target = std::string(hash, sizeof(hash));
  
  
  char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;
  
  t = cert.t();
  
  HMAC_Init(&hmac_ctx, this->mac_key_n.c_str(), this->mac_key_n.size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)sid.c_str(), sid.size());
  HMAC_Update(&hmac_ctx, (u_char*)&t, sizeof(t));
  HMAC_Update(&hmac_ctx, (u_char*)&w, sizeof(w));
  HMAC_Update(&hmac_ctx, (u_char*)target.c_str(), target.size());
  
  for (int i = 0; i < blist.nymbles_size(); i++) {
    std::string nymble = blist.nymbles(i);
    HMAC_Update(&hmac_ctx, (u_char*)nymble.c_str(), nymble.size());
  }
  
  HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
  
  return (cert.mac() == std::string(mac, sizeof(mac)));
}


bool NymbleManager::registerServer(std::string sid, ServerState* server_state)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry != NULL) {
    return false;
  }
  
  entry = new NymbleManagerEntry(sid, this->cur_time_period);
  
  this->entries.push_back(entry);
  
  char h[] = "h";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, entry->getDaisyL().c_str(), sizeof(hash));
  
  for (u_int i = 0; i < this->time_periods - this->cur_time_period + 1; i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)h, sizeof(h));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  std::string target = std::string(hash, sizeof(hash));
  
  
  if (!signBlacklist(sid, target, NULL, server_state->mutable_cert())) {
    return false;
  }
  
  server_state->set_sid(sid);
  server_state->set_mac_key_ns(entry->getMacKeyNS());
  server_state->set_time_last_updated(this->cur_time_period);
  
  return true;
}


bool NymbleManager::updateServer(std::string sid, ServerState* server_state, ServerState* new_server_state)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    fprintf(stderr, "Couldn't find entry\n");
    return false;
  }
  
  if (new_server_state == NULL) {
    fprintf(stderr, "new_server_state == NULL\n");
    return false;
  }
  
  // NOTE: What happens if getTimeLastUpdated > cur_time_period? Maybe link_window is different or something.
  if (entry->getTimeLastUpdated() == this->cur_time_period) {
    fprintf(stderr, "cur_time_period == last time period\n");
    return false;
  }
  
  if (server_state == NULL) {
    char h[] = "h";
    char hash[DIGEST_SIZE];
    SHA256_CTX ctx;
    
    memcpy(hash, (u_char*)entry->getDaisyL().c_str(), sizeof(hash));
    
    for (u_int i = 0; i < this->time_periods - this->cur_time_period + 1; i++) {
      SHA256_Init(&ctx);
      SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
      SHA256_Update(&ctx, (u_char*)h, sizeof(h));
      SHA256_Final((u_char*)hash, &ctx);
    }
    
    new_server_state->mutable_cert()->set_daisy(hash, sizeof(hash));
  } else if (server_state->has_blist() && server_state->has_cert() && server_state->has_clist()) {
    if (!this->verifyBlacklist(sid, entry->getTimeLastUpdated(), this->cur_link_window, server_state->blist(), server_state->cert())) {
      fprintf(stderr, "invalid blacklist\n");
      return false;
    }
    
    for (int i = 0; i < server_state->clist().complaints_size(); i++) {
      if (server_state->clist().complaints(i).time() >= this->cur_time_period) {
        fprintf(stderr, "bad complaint time\n");
        return false;
      }
      
      if (!this->verifyTicket(sid, server_state->clist().complaints(i).time(), this->cur_link_window, server_state->clist().complaints(i).ticket())) {
        fprintf(stderr, "invalid ticket\n");
        return false;
      }
    }
    
    if (!this->computeBlacklistUpdate(sid, server_state->blist(), server_state->clist(), new_server_state->mutable_blist(), new_server_state->mutable_cert())) {
      fprintf(stderr, "computeBlacklistUpdate failed\n");
      return false;
    }
    
    if (!this->computeTokens(this->cur_time_period, server_state->blist(), server_state->clist(), new_server_state->mutable_seeds())) {
      fprintf(stderr, "computeTokens failed\n");
      return false;
    }
  } else {
    return false;
  }
  
  entry->setTimeLastUpdated(this->cur_time_period);
  
  return true;
}

bool NymbleManager::computeBlacklistUpdate(std::string sid, Blacklist blist, Complaints clist, Blacklist* blist_out, BlacklistCert* cert_out)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  for (int i = 0; i < clist.complaints_size(); i++) {
    std::string ctxt = clist.complaints(i).ticket().ctxt();
    
    EVP_CIPHER_CTX cipher_ctx;
    char buffer[1024];
    int buffer_len;
    int final_len;
    
    std::string iv = ctxt.substr(0, CIPHER_BLOCK_SIZE);
    std::string cipher = ctxt.substr(CIPHER_BLOCK_SIZE, ctxt.size());
    
    EVP_CIPHER_CTX_init(&cipher_ctx);
    EVP_DecryptInit_ex(&cipher_ctx, EVP_aes_128_cbc(), NULL, (u_char*)this->enc_key_n.c_str(), (u_char*)iv.c_str());
    EVP_DecryptUpdate(&cipher_ctx, (u_char*)buffer, &buffer_len, (u_char*)cipher.c_str(), cipher.size());
    EVP_DecryptFinal_ex(&cipher_ctx, (u_char*)buffer + buffer_len, &final_len);
    EVP_CIPHER_CTX_cleanup(&cipher_ctx);
    
    std::string nymble0(buffer, DIGEST_SIZE);
    std::string seed(buffer + DIGEST_SIZE, DIGEST_SIZE);
    
    bool already_blacklisted = false;
    
    for (int j = 0; j < blist.nymbles_size(); j++) {
      if (nymble0 == blist.nymbles(j)) {
        already_blacklisted = true;
      }
    }
    
    if (already_blacklisted) {
      std::string random;
      random_bytes(DIGEST_SIZE, &random);
      blist_out->add_nymbles(random);
      fprintf(stderr, "User already blacklisted, adding random nymble!\n");
    } else {
      blist_out->add_nymbles(nymble0);
    }
  }
  
  std::string daisy_l;
  random_bytes(DIGEST_SIZE, &daisy_l);
  
  
  char h[] = "h";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, (u_char*)daisy_l.c_str(), sizeof(hash));
  
  for (u_int i = 0; i < this->time_periods - this->cur_time_period + 1; i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)h, sizeof(h));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  std::string target = std::string(hash, sizeof(hash));
  
  Blacklist combined_blist;
  
  for (int i = 0; i < blist.nymbles_size(); i++) {
    combined_blist.add_nymbles(blist.nymbles(i));
  }
  
  for (int i = 0; i < blist_out->nymbles_size(); i++) {
    combined_blist.add_nymbles(blist_out->nymbles(i));
  }
  
  if (!signBlacklist(sid, target, &combined_blist, cert_out)) {
    return false;
  }
  
  entry->setDaisyL(daisy_l);
  
  return true;
}

bool NymbleManager::computeTokens(u_int t_prime, Blacklist blist, Complaints clist, Seeds* seeds)
{
  for (int i = 0; i < clist.complaints_size(); i++) {
    std::string ctxt = clist.complaints(i).ticket().ctxt();
    
    EVP_CIPHER_CTX cipher_ctx;
    char buffer[1024];
    int buffer_len;
    int final_len;
    std::string iv = ctxt.substr(0, CIPHER_BLOCK_SIZE);
    std::string cipher = ctxt.substr(CIPHER_BLOCK_SIZE, ctxt.size());
    
    EVP_CIPHER_CTX_init(&cipher_ctx);
    EVP_DecryptInit_ex(&cipher_ctx, EVP_aes_128_cbc(), NULL, (u_char*)this->enc_key_n.c_str(), (u_char*)iv.c_str());
    EVP_DecryptUpdate(&cipher_ctx, (u_char*)buffer, &buffer_len, (u_char*)cipher.c_str(), cipher.size());
    EVP_DecryptFinal_ex(&cipher_ctx, (u_char*)buffer + buffer_len, &final_len);
    EVP_CIPHER_CTX_cleanup(&cipher_ctx);
    
    std::string nymble0(buffer, DIGEST_SIZE);
    std::string seed(buffer + DIGEST_SIZE, DIGEST_SIZE);
    
    bool already_blacklisted = false;
    
    for (int j = 0; j < blist.nymbles_size(); j++) {
      if (nymble0 == blist.nymbles(j)) {
        already_blacklisted = true;
      }
    }
    
    std::string seed_prime;
    
    if (already_blacklisted) {
      random_bytes(DIGEST_SIZE, &seed_prime);
      fprintf(stderr, "User already blacklisted, adding random seed!\n");
    } else {
      evolveSeed(seed, t_prime - clist.complaints(i).time(), &seed_prime);
    }
    
    seeds->add_seeds(seed_prime);
  }
  
  return true;
}

}; // namespace Nymble
