#include "nymble_pseudonym_manager.h"

namespace Nymble {

PseudonymManager::PseudonymManager()
{
  RAND_bytes(this->nym_key_p, DIGEST_SIZE);
}

PseudonymManager::PseudonymManager(u_char* mac_key_np)
{
  PseudonymManager();
  this->setMacKeyNP(mac_key_np);
}

u_char* PseudonymManager::getMacKeyNP()
{
  return this->mac_key_np;
}

void PseudonymManager::setMacKeyNP(u_char* mac_key_np)
{
  memcpy(this->mac_key_np, mac_key_np, DIGEST_SIZE);
}

Pseudonym* PseudonymManager::createPseudonym(u_char* user_id)
{
  Pseudonym* pseudonym = new Pseudonym();

  HMAC_CTX ctx;
  u_char buffer[DIGEST_SIZE];
  
  HMAC_Init(&ctx, this->nym_key_p, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, user_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->set_nym(buffer, DIGEST_SIZE);
  
  HMAC_Init(&ctx, this->mac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, (u_char*)pseudonym->nym().c_str(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->set_mac(buffer, DIGEST_SIZE);
  
  return pseudonym;
}

}; // namespace Nymble
