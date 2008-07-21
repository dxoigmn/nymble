#include "nymble_pseudonym_manager.h"

PseudonymManager::PseudonymManager()
{
  RAND_bytes(this->keyedhash_key_p, DIGEST_SIZE);
}

PseudonymManager::PseudonymManager(u_char* hmac_key_np)
{
  PseudonymManager();
  this->setHmacKeyNP(hmac_key_np);
}

u_char* PseudonymManager::getHmacKeyNP()
{
  return this->hmac_key_np;
}

void PseudonymManager::setHmacKeyNP(u_char* hmac_key_np)
{
  memcpy(this->hmac_key_np, hmac_key_np, DIGEST_SIZE);
}

Pseudonym* PseudonymManager::createPseudonym(u_char* user_id)
{
  Pseudonym* pseudonym = new Pseudonym();

  HMAC_CTX ctx;
  u_char buffer[DIGEST_SIZE];
  
  HMAC_Init(&ctx, this->keyedhash_key_p, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, user_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->setPseudonym(buffer);
  
  HMAC_Init(&ctx, this->hmac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym->getPseudonym(), DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Final(&ctx, buffer, NULL);
  
  pseudonym->setMacNP(buffer);
  
  return pseudonym;
}