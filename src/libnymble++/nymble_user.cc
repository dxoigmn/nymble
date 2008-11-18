#include "nymble_user.h"

namespace Nymble {

User::User()
{
  this->pnym = NULL;
  this->ver_key_n = NULL;
}

User::~User()
{
  RSA_free(this->ver_key_n);
  
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    delete *entry;
  }
  
  delete this->pnym;
}

void User::setPseudonym(Pseudonym pnym)
{
  this->pnym = new Pseudonym(pnym);
}

Pseudonym* User::getPseudonym()
{
  return this->pnym;
}

bool User::readVerifyKeyN(std::string ver_key_n_path)
{
  FILE* ver_key_n = fopen(ver_key_n_path.c_str(), "r");
  
  if (ver_key_n == NULL) {
    return false;
  }
  
  this->ver_key_n = PEM_read_RSA_PUBKEY(ver_key_n, NULL, NULL, NULL);
  
  fclose(ver_key_n);
  
  if (this->ver_key_n == NULL) {
    return false;
  }
  
  return true;
}

void User::addCredential(std::string sid, Credential cred)
{
  UserEntry* entry = new UserEntry(sid, cred);
  
  this->push_back(entry);
}

Ticket* User::getTicket(std::string sid)
{
  UserEntry* entry = findEntry(sid);
  
  if (entry == NULL) {
    return NULL;
  }
  
  Credential* cred = entry->getCredential();
  
  if (this->cur_time_period < 0 || this->cur_time_period > (u_int)cred->tickets_size()) {
    return NULL;
  }
  
  return cred->mutable_tickets(this->cur_time_period);
}

bool User::isBlacklisted(std::string sid, Blacklist blist, BlacklistCert cert)
{
  bool isSafe = true;
  
  if (!verifyBlacklist(sid, blist, cert)) {
    isSafe = false;
  }
  
  if (!checkIfBlacklisted(sid, blist)) {
    isSafe = false;
  }
  
  return isSafe;
}

UserEntry* User::findEntry(std::string sid)
{
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    if (sid == (*entry)->getServerId()) {
      return *entry;
    }
  }
  
  return NULL;
}

bool User::verifyBlacklist(std::string sid, Blacklist blist, BlacklistCert cert)
{
  if (this->cur_time_period < cert.t()) {
    return false;
  }
  
  char h[] = "h";
  char hash[DIGEST_SIZE];
  SHA256_CTX ctx;
  
  memcpy(hash, cert.daisy().c_str(), sizeof(hash));
  
  for (u_int i = 0; i < this->cur_time_period - cert.t(); i++) {
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_char*)hash, sizeof(hash));
    SHA256_Update(&ctx, (u_char*)h, sizeof(h));
    SHA256_Final((u_char*)hash, &ctx);
  }
  
  std::string target = std::string(hash, sizeof(hash));
  
  
  SHA256_CTX hash_ctx;
  
  SHA256_Init(&hash_ctx);
  SHA256_Update(&hash_ctx, (u_char*)sid.c_str(), sid.size());
  SHA256_Update(&hash_ctx, (u_char*)&this->cur_time_period, sizeof(this->cur_time_period));
  SHA256_Update(&hash_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  SHA256_Update(&hash_ctx, (u_char*)target.c_str(), target.size());
  
  for (int i = 0; i < blist.nymbles_size(); i++) {
    std::string nymble = blist.nymbles(i);
    SHA256_Update(&hash_ctx, (u_char*)nymble.c_str(), nymble.size());
  }
  
  SHA256_Final((u_char*)hash, &hash_ctx);
  
  
  u_char buffer[SIGNATURE_SIZE];
  RSA_public_decrypt(SIGNATURE_SIZE, (u_char*)cert.sig().c_str(), buffer, this->ver_key_n, RSA_NO_PADDING);
  
  return (RSA_verify_PKCS1_PSS(this->ver_key_n, (u_char*)hash, EVP_sha256(), buffer, -2) != 0);
}

bool User::checkIfBlacklisted(std::string sid, Blacklist blist)
{
  UserEntry* entry = findEntry(sid);
  
  if (entry == NULL) {
    return true;
  }
  
  bool blacklisted = false;
  
  std::string nymble0 = entry->getCredential()->nymble0();
  
  for (int i = 0; i < blist.nymbles_size(); i++) {
    if (nymble0 == blist.nymbles(i)) {
      blacklisted = true;
    }
  }
  
  return blacklisted;
}

}; // namespace Nymble
