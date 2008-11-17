#include "nymble_manager.h"

namespace Nymble {

NymbleManager::NymbleManager()
{
  random_bytes(DIGEST_SIZE, this->mac_key_np);
  random_bytes(DIGEST_SIZE, this->mac_key_n);
  random_bytes(DIGEST_SIZE, this->seed_key_n);
  random_bytes(CIPHER_BLOCK_SIZE, this->enc_key_n);
  RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
}

NymbleManager::~NymbleManager()
{
  for (NymbleManagerEntries::iterator entry = this->entries.begin(); entry != this->entries.end(); entry++) {
    delete *entry;
  }
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

std::string NymbleManager::getMacKeyNP()
{
  return this->mac_key_np;
}

bool NymbleManager::verifyPseudonym(Pseudonym pseudonym)
{
  u_char mac[DIGEST_SIZE];
  HMAC_CTX ctx;

  HMAC_Init(&ctx, this->mac_key_np.c_str(), this->mac_key_np.size(), EVP_sha256());
  HMAC_Update(&ctx, (u_char*)pseudonym.nym().c_str(), pseudonym.nym().size());
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, mac, NULL);
  
  return (std::string((char*)mac, sizeof(mac)) == pseudonym.mac());
}

bool NymbleManager::createCredential(std::string sid, Pseudonym pseudonym, Credential& credential)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
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

bool NymbleManager::signBlacklist(std::string sid, std::string target, Blacklist blist, BlacklistCert& cert)
{
  NymbleManagerEntry* entry = findServer(sid);
  
  if (entry == NULL) {
    return false;
  }
  
  // IMPLEMENT
  
  return false;
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


bool NymbleManager::registerServer(std::string sid, Server& server)
{
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::computeBlacklistUpdate(std::string sid, Blacklist blist, Complaints clist, Blacklist& blist_out, BlacklistCert& cert_out)
{
  // IMPLEMENT
  
  return false;
}

bool NymbleManager::computeTokens(u_int t_prime, Blacklist* blist, Complaints clist, Seeds& seeds)
{
  // IMPLEMENT
  
  return false;
}

}; // namespace Nymble
