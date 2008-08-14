#include "nymble_blacklist.h"

namespace Nymble {

Blacklist::Blacklist()
{

}

Blacklist::Blacklist(Blacklist* blacklist)
{
  memcpy(this->server_id, blacklist->server_id, DIGEST_SIZE);
  memcpy(this->bl_hash, blacklist->bl_hash, DIGEST_SIZE);
  memcpy(this->bmac_n, blacklist->bmac_n, DIGEST_SIZE);
  memcpy(this->sig, blacklist->sig, SIGNATURE_SIZE);

  this->link_window = blacklist->link_window;
  this->time_period = blacklist->time_period;
  
  for (Nymbles::iterator nymble = blacklist->begin(); nymble != blacklist->end(); ++nymble) {
    u_char* new_nymble = new u_char[DIGEST_SIZE];
    memcpy(new_nymble, *nymble, DIGEST_SIZE);
    
    this->push_back(new_nymble);
  }
}

Blacklist::Blacklist(Marshal::Blacklist* blacklist)
{
  this->link_window = blacklist->link_window();
  this->time_period = blacklist->time_period();
  memcpy(this->server_id, blacklist->server_id().data(), DIGEST_SIZE);
  memcpy(this->bl_hash, blacklist->bl_hash().data(), DIGEST_SIZE);
  memcpy(this->bmac_n, blacklist->bmac_n().data(), DIGEST_SIZE);
  memcpy(this->sig, blacklist->sig().data(), SIGNATURE_SIZE);
  
  for (int i = 0; i < blacklist->nymble_size(); i++) {
    u_char* nymble = (u_char*) malloc(DIGEST_SIZE);
    
    memcpy(nymble, blacklist->nymble(i).data(), DIGEST_SIZE);
    
    this->push_back(nymble);
  }
}

Blacklist::Blacklist(u_char* server_id, u_int link_window, u_int time_period)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);

  this->link_window = link_window;
  this->time_period = time_period;
}

Blacklist::~Blacklist()
{
  for (Nymbles::iterator nymble = this->begin(); nymble != this->end(); ++nymble) {
    delete *nymble;
  }
}

u_char* Blacklist::getServerId()
{
  return this->server_id;
}

u_char* Blacklist::getHash()
{
  return this->bl_hash;
}

u_char* Blacklist::getHmac()
{
  return this->bmac_n;
}

u_int Blacklist::getLinkWindow()
{
  return this->link_window;
}

u_int Blacklist::getTimePeriod()
{
  return this->time_period;
}

void Blacklist::hash(u_char* out)
{
  if (out == NULL) {
    out = this->bl_hash;
  }
  
  SHA256_CTX ctx;
  
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, this->server_id, DIGEST_SIZE);
  SHA256_Update(&ctx, &this->link_window, sizeof(this->link_window));
  
  for (Nymbles::iterator nymble = this->begin(); nymble != this->end(); ++nymble) {
    SHA256_Update(&ctx, *nymble, DIGEST_SIZE);
  }
  
  SHA256_Final(out, &ctx);
}

void Blacklist::hmac(u_char* hmac_key_n, u_char* out)
{
  if (out == NULL) {
    out = this->bmac_n;
  }

  HMAC_CTX hmac_ctx;
  
  HMAC_Init(&hmac_ctx, hmac_key_n, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&hmac_ctx, this->bl_hash, DIGEST_SIZE);
  HMAC_Update(&hmac_ctx, (u_char *)&this->time_period, sizeof(this->time_period));
  HMAC_Final(&hmac_ctx, out, NULL);
  HMAC_CTX_cleanup(&hmac_ctx);
}

void Blacklist::sign(RSA* sign_key_n, u_char* out)
{
  if (out == NULL) {
    out = this->sig;
  }
  
  SHA256_CTX sha_ctx;
  u_char hashed[DIGEST_SIZE];
  u_char buffer[SIGNATURE_SIZE];
  
  SHA256_Init(&sha_ctx);
  SHA256_Update(&sha_ctx, this->bl_hash, DIGEST_SIZE);
  SHA256_Update(&sha_ctx, (u_char *)&this->time_period, sizeof(this->time_period));
  SHA256_Final(hashed, &sha_ctx);
  
  RSA_padding_add_PKCS1_PSS(sign_key_n, buffer, hashed, EVP_sha256(), -2);
  RSA_private_encrypt(SIGNATURE_SIZE, buffer, out, sign_key_n, RSA_NO_PADDING);
}

bool Blacklist::verify(RSA* verify_key_n, u_int link_window, u_int time_period)
{
  bool valid = true;
  
  if (this->link_window != link_window) {
    valid = false;
  }
  
  if (this->time_period != time_period) {
    valid = false;
  }
  
  u_char bl_hash[DIGEST_SIZE];
  
  this->hash(bl_hash);
  
  if (memcmp(this->bl_hash, bl_hash, DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  SHA256_CTX sha_ctx;
  u_char hashed[DIGEST_SIZE];
  u_char buffer[SIGNATURE_SIZE];
  
  SHA256_Init(&sha_ctx);
  SHA256_Update(&sha_ctx, this->bl_hash, DIGEST_SIZE);
  SHA256_Update(&sha_ctx, (u_char*) &this->time_period, sizeof(this->time_period));
  SHA256_Final(hashed, &sha_ctx);
  
  RSA_public_decrypt(SIGNATURE_SIZE, this->sig, buffer, verify_key_n, RSA_NO_PADDING);
  
  if (RSA_verify_PKCS1_PSS(verify_key_n, hashed, EVP_sha256(), buffer, -2) == 0) {
    valid = false;
  }
  
  return valid;
}

u_int Blacklist::marshal(u_char* out, u_int size)
{
  Marshal::Blacklist blacklist;
  
  blacklist.set_link_window(this->link_window);
  blacklist.set_time_period(this->time_period);
  blacklist.set_server_id(this->server_id, DIGEST_SIZE);
  blacklist.set_bl_hash(this->bl_hash, DIGEST_SIZE);
  blacklist.set_bmac_n(this->bmac_n, DIGEST_SIZE);
  blacklist.set_sig(this->sig, SIGNATURE_SIZE);
  
  for (Nymbles::iterator nymble = this->begin(); nymble != this->end(); ++nymble) {
    blacklist.add_nymble(*nymble, DIGEST_SIZE);
  }
  
  if (out != NULL) {
    blacklist.SerializeToArray(out, size);
  }
  
  return blacklist.ByteSize();
}

Blacklist* Blacklist::unmarshal(u_char* bytes, u_int size)
{
  Marshal::Blacklist blacklist;
  
  if (blacklist.ParseFromArray(bytes, size)) {
    return new Blacklist(&blacklist);
  }
  
  return NULL;
}

}; // namespace Nymble
