#include "nymble_manager.h"

namespace Nymble {

NymbleManager::NymbleManager()
{
  random_bytes(DIGEST_SIZE, this->mac_key_np);
  random_bytes(DIGEST_SIZE, this->mac_key_n);
  random_bytes(DIGEST_SIZE, this->seed_key_n);
  random_bytes(CIPHER_BLOCK_SIZE, this->enc_key_n);
  RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
  this->entries();
}

NymbleManager::~NymbleManager()
{
  for (NymbleManagerEntries::iterator entry = this->entries->begin(); entry != this->entries->end(); entry++) {
    delete *entry;
  }
}

void NymbleManager::setMacKeyNP(std::string mac_key_np)
{
  memcpy(this->mac_key_np, mac_key_np, DIGEST_SIZE);
}

void NymbleManager::readSignKey(char* sign_key_path)
{
  FILE* sign_key = fopen(sign_key_path, "r");
  
  PEM_read_RSAPrivateKey(sign_key, this->sign_key_n, NULL, NULL);
  
  fclose(sign_key);
}

NymbleManagerEntry* NymbleManager::findServer(std::string sid)
{
  for (NymbleManagerEntries::iterator entry = this->entries->begin(); entry != this->entries->end(); entry++) {
    if (memcmp((*entry)->getServerId(), sid, DIGEST_SIZE) == 0) {
      return *entry;
    }
  }

  return NULL;
}

bool NymbleManager::verifyPseudonym(Pseudonym* pseudonym)
{
  u_char mac[DIGEST_SIZE];
  HMAC_CTX ctx;

  HMAC_Init(&ctx, this->mac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym->nym().c_str(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, mac, NULL);

  return (memcmp(mac, pseudonym->mac().c_str(), DIGEST_SIZE) == 0);
}


bool NymbleManager::createCredential(std::string sid, Pseudonym* pseudonym, Credential* credential);
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::verifyTicket(std::string sid, Ticket* ticket)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::signBlacklist(std::string sid, std::string target, Blacklist* blist, BlacklistCert* cert)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::verifyBlacklist(std::string sid, Blacklist* blist, BlacklistCert* cert)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
}


bool NymbleManager::registerServer(std::string sid, Server* server)
{
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::computeBlacklistUpdate(std::string sid, Blacklist* blist, Complaints clist)
{
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::computeTokens(u_int t_prime, Blacklist* blist, Complaints clist)
{
  // IMPLEMENT
  
  return false;
}

}; // namespace Nymble
