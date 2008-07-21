#include "nymble_blacklist.h"

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
    u_char* new_nymble = (u_char*)malloc(DIGEST_SIZE);
    memcpy(new_nymble, *nymble, DIGEST_SIZE);
    
    this->push_back(new_nymble);
  }
}

Blacklist::Blacklist(u_char* server_id, u_int link_window, u_int time_period)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);

  this->link_window = link_window;
  this->time_period = time_period;

  this->hash();
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
  RSA_private_encrypt(RSA_size(sign_key_n), buffer, out, sign_key_n, RSA_NO_PADDING);
}

u_int Blacklist::marshall(char* out)
{
  Json::FastWriter writer;
  Json::Value blacklist;
  
  blacklist["link_window"] = this->link_window;
  blacklist["time_period"] = this->time_period;
  
  char encoded[DIGEST_SIZE*2+1];
  encoded[DIGEST_SIZE*2] = 0;
  
  Nymble::hexencode(encoded, this->server_id, DIGEST_SIZE);
  blacklist["server_id"] = encoded;
  
  Nymble::hexencode(encoded, this->bl_hash, DIGEST_SIZE);
  blacklist["bl_hash"] = encoded;
  
  Nymble::hexencode(encoded, this->bmac_n, DIGEST_SIZE);
  blacklist["bmac_n"] = encoded;
  
  char sig_encoded[SIGNATURE_SIZE*2+1];
  sig_encoded[SIGNATURE_SIZE*2] = 0;
  
  Nymble::hexencode(sig_encoded, this->sig, SIGNATURE_SIZE);
  blacklist["sig"] = sig_encoded;
  
  std::string json = writer.write(blacklist);
  
  if (out) {
    strncpy(out, json.c_str(), json.size());
  }
  
  return json.size();
}

Blacklist* Blacklist::unmarshall(char* bytes)
{
  Json::Value json;
  Json::Reader reader;
  
  if (!reader.parse(bytes, json)) {
    return NULL;
  }
  
  Blacklist* blacklist = new Blacklist();
  
  blacklist->link_window = json["link_window"].asUInt();
  blacklist->time_period = json["time_period"].asUInt();
  
  Nymble::hexdecode(blacklist->server_id, (char*) json["server_id"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(blacklist->bl_hash, (char*) json["bl_hash"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(blacklist->bmac_n, (char*) json["bmac_n"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(blacklist->sig, (char*) json["sig"].asCString(), SIGNATURE_SIZE*2);

  return blacklist;
}
