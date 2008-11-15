#include "nymble_pseudonym_manager.h"

namespace Nymble {

PseudonymManager::PseudonymManager()
{
  char nym_key_p[DIGEST_SIZE];
  
  RAND_bytes((u_char*)nym_key_p, DIGEST_SIZE);
  
  this->nym_key_p = std::string(nym_key_p, DIGEST_SIZE);
}

void PseudonymManager::setMacKeyNP(std::string mac_key_np)
{
  this->mac_key_np = mac_key_np;
}

bool PseudonymManager::createPseudonym(std::string user_id, Pseudonym* pseudonym)
{
  HMAC_CTX ctx;
  u_char buffer[DIGEST_SIZE];
  
  HMAC_Init(&ctx, this->nym_key_p.c_str(), DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, (u_char*)user_id.c_str(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->set_nym(buffer, DIGEST_SIZE);
  
  HMAC_Init(&ctx, this->mac_key_np.c_str(), DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, buffer, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->set_mac(buffer, DIGEST_SIZE);
  
  return true;
}

}; // namespace Nymble
