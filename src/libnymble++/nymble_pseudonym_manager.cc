#include "nymble_pseudonym_manager.h"

namespace Nymble {

PseudonymManager::PseudonymManager()
{
  char nym_key_p[DIGEST_SIZE];
  
  RAND_bytes((u_char*)nym_key_p, sizeof(nym_key_p));
  
  this->nym_key_p = std::string(nym_key_p, sizeof(nym_key_p));
}

void PseudonymManager::setMacKeyNP(std::string mac_key_np)
{
  this->mac_key_np = mac_key_np;
}

bool PseudonymManager::createPseudonym(std::string user_id, Pseudonym& pseudonym)
{
  HMAC_CTX ctx;
  u_char buffer[DIGEST_SIZE];
  
  HMAC_Init(&ctx, this->nym_key_p.c_str(), this->nym_key_p.size(), EVP_sha256());
  HMAC_Update(&ctx, (u_char*)user_id.c_str(), this->nym_key_p.size());
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym.set_nym(buffer, sizeof(buffer));
  
  HMAC_Init(&ctx, this->mac_key_np.c_str(), this->mac_key_np.size(), EVP_sha256());
  HMAC_Update(&ctx, (u_char*)pseudonym.nym().c_str(), pseudonym.nym().size());
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym.set_mac(buffer, sizeof(buffer));
  
  return true;
}

}; // namespace Nymble
