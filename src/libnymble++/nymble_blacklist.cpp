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
    u_char* new_nymble = new u_char[DIGEST_SIZE];
    memcpy(new_nymble, *nymble, DIGEST_SIZE);
    
    this->push_back(new_nymble);
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

u_int Blacklist::marshall(char* out)
{
  struct json_object* json_blacklist = json_object_new_object();
  
  Nymble::json_marshall_int(json_blacklist, "link_window", this->link_window);
  Nymble::json_marshall_int(json_blacklist, "time_period", this->time_period);
  Nymble::json_marshall_str(json_blacklist, "server_id", this->server_id, DIGEST_SIZE);
  Nymble::json_marshall_str(json_blacklist, "bl_hash", this->bl_hash, DIGEST_SIZE);
  Nymble::json_marshall_str(json_blacklist, "bmac_n", this->bmac_n, DIGEST_SIZE);
  Nymble::json_marshall_str(json_blacklist, "sig", this->sig, SIGNATURE_SIZE);
  
  struct json_object* json_nymbles = json_object_new_array();
  
  for (Nymbles::iterator nymble = this->begin(); nymble != this->end(); ++nymble) {
    u_int encoded_len = Nymble::hexencode(*nymble, DIGEST_SIZE);
    char encoded[encoded_len];

    Nymble::hexencode(*nymble, DIGEST_SIZE, encoded);
    
    json_object_array_add(json_nymbles, json_object_new_string(encoded));
  }
  
  json_object_object_add(json_blacklist, "nymbles", json_nymbles);
  
  char* json = json_object_to_json_string(json_blacklist);
  
  if (out) {
    strcpy(out, json);
  }
  
  return strlen(json);
}

void Blacklist::unmarshall(char* bytes, Blacklist* out)
{
  struct json_object* json_blacklist = json_tokener_parse(bytes);
  
  Nymble::json_unmarshall_int(json_blacklist, "link_window", &(out->link_window));
  Nymble::json_unmarshall_int(json_blacklist, "time_period", &(out->time_period));
  Nymble::json_unmarshall_str(json_blacklist, "server_id", out->server_id, DIGEST_SIZE);
  Nymble::json_unmarshall_str(json_blacklist, "bl_hash", out->bl_hash, DIGEST_SIZE);
  Nymble::json_unmarshall_str(json_blacklist, "bmac_n", out->bmac_n, DIGEST_SIZE);
  Nymble::json_unmarshall_str(json_blacklist, "sig", out->sig, SIGNATURE_SIZE);
  
  struct json_object* json_nymbles = json_object_object_get(json_blacklist, "nymbles");
  
  for (int i = 0; i < json_object_array_length(json_nymbles); i++) {
    char* encoded = json_object_get_string(json_object_array_get_idx(json_nymbles, i));
    
    u_int decoded_len = Nymble::hexdecode(encoded);
    u_char* decoded = new u_char[decoded_len];

    Nymble::hexdecode(encoded, decoded);
    
    if (decoded_len == DIGEST_SIZE) {
      out->push_back(decoded);
    } else {
      fprintf(stderr, "Couldn't unmarshall nymble because due to unexpected length %d (expected %d)", decoded_len, DIGEST_SIZE);
    }
  }
}
