#include "nymble_ticket.h"

namespace Nymble {

Ticket::Ticket()
{
  
}

Ticket::Ticket(u_int link_window, u_int time_period, u_char* server_id, u_char* nymble)
{
  this->link_window = link_window;
  this->time_period = time_period;
  memcpy(this->server_id, server_id, DIGEST_SIZE);
  memcpy(this->nymble, nymble, DIGEST_SIZE);
}

Ticket::Ticket(Ticket* ticket)
{
  this->link_window = ticket->link_window;
  this->time_period = ticket->time_period;
  memcpy(this->server_id, ticket->server_id, DIGEST_SIZE);
  memcpy(this->nymble, ticket->nymble, DIGEST_SIZE);
  memcpy(this->trapdoorenc, ticket->trapdoorenc, TRAPDOORENC_SIZE);
  memcpy(this->mac_n, ticket->mac_n, DIGEST_SIZE);
  memcpy(this->mac_ns, ticket->mac_ns, DIGEST_SIZE);
}

u_int Ticket::getLinkWindow()
{
  return this->link_window;
}

u_int Ticket::getTimePeriod()
{
  return this->time_period;
}

u_char* Ticket::getServerId()
{
  return this->server_id;
}

u_char* Ticket::getNymble()
{
  return this->nymble;
}

u_char* Ticket::getMacNS()
{
  return this->mac_ns;
}

void Ticket::hmac(u_char* hmac_key, bool include_mac_n, u_char* out)
{
  HMAC_CTX ctx;

  HMAC_Init(&ctx, hmac_key, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, this->server_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&this->link_window, sizeof(this->link_window));
  HMAC_Update(&ctx, (u_char *)&this->time_period, sizeof(this->time_period));
  HMAC_Update(&ctx, this->nymble, DIGEST_SIZE);
  HMAC_Update(&ctx, this->trapdoorenc, TRAPDOORENC_SIZE);
  
  if (include_mac_n) {
    HMAC_Update(&ctx, this->mac_n, DIGEST_SIZE);
  }
  
  if (out == NULL) {
    if (include_mac_n) {
      out = this->mac_ns;
    } else {
      out = this->mac_n;
    }
  }
  
  HMAC_Final(&ctx, out, NULL);
  HMAC_CTX_cleanup(&ctx);
}

void Ticket::encrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym)
{
  u_char in[DIGEST_SIZE * 2];
  u_char iv[CIPHER_BLOCK_SIZE];
  AES_KEY key;
  
  memcpy(in, trapdoor, DIGEST_SIZE);
  memcpy(in + DIGEST_SIZE, pseudonym, DIGEST_SIZE);
  
  /* iv must be separate from buffer as the AES function messes with it as it works */
  RAND_bytes(iv, CIPHER_BLOCK_SIZE);
  memcpy(this->trapdoorenc, iv, CIPHER_BLOCK_SIZE);
  
  AES_set_encrypt_key(encrypt_key_n, CIPHER_BLOCK_SIZE * 8, &key);
  AES_cbc_encrypt(in, this->trapdoorenc + CIPHER_BLOCK_SIZE, DIGEST_SIZE * 2, &key, iv, AES_ENCRYPT);
}

void Ticket::decrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym)
{
  u_char buffer[CIPHER_BLOCK_SIZE * 4];
  u_char iv[CIPHER_BLOCK_SIZE];
  AES_KEY key;

  memcpy(iv, this->trapdoorenc, CIPHER_BLOCK_SIZE);

  AES_set_decrypt_key(encrypt_key_n, CIPHER_BLOCK_SIZE * 8, &key);
  AES_cbc_encrypt(this->trapdoorenc + CIPHER_BLOCK_SIZE, buffer, TRAPDOORENC_SIZE - CIPHER_BLOCK_SIZE, &key, iv, AES_DECRYPT);
  
  if (trapdoor) {
    memcpy(trapdoor, buffer, DIGEST_SIZE);
  }
  
  if (pseudonym) {
    memcpy(pseudonym, buffer + DIGEST_SIZE, DIGEST_SIZE);
  }
}

void Ticket::marshal(json_object* out)
{
  json_marshal_int(out, "link_window", this->link_window);
  json_marshal_int(out, "time_period", this->time_period);
  json_marshal_str(out, "server_id", this->server_id, DIGEST_SIZE);
  json_marshal_str(out, "nymble", this->nymble, DIGEST_SIZE);
  json_marshal_str(out, "mac_n", this->mac_n, DIGEST_SIZE);
  json_marshal_str(out, "mac_ns", this->mac_ns, DIGEST_SIZE);
  json_marshal_str(out, "trapdoorenc", this->trapdoorenc, TRAPDOORENC_SIZE);
}

u_int Ticket::marshal(char* out)
{
  struct json_object* json_ticket = json_object_new_object();
  
  this->marshal(json_ticket);
  
  char* json = json_object_to_json_string(json_ticket);
  
  if (out != NULL) {
    strcpy(out, json);
  }
  
  return strlen(json);
}

void Ticket::unmarshal(json_object* json_ticket, Ticket* out)
{
  json_unmarshal_int(json_ticket, "link_window", &out->link_window);
  json_unmarshal_int(json_ticket, "time_period", &out->time_period);
  json_unmarshal_str(json_ticket, "server_id", out->server_id, DIGEST_SIZE);
  json_unmarshal_str(json_ticket, "nymble", out->nymble, DIGEST_SIZE);
  json_unmarshal_str(json_ticket, "mac_n", out->mac_n, DIGEST_SIZE);
  json_unmarshal_str(json_ticket, "mac_ns", out->mac_ns, DIGEST_SIZE);
  json_unmarshal_str(json_ticket, "trapdoorenc", out->trapdoorenc, TRAPDOORENC_SIZE);
}

void Ticket::unmarshal(char* bytes, Ticket* out)
{
  Ticket::unmarshal(json_tokener_parse(bytes), out);
}

void Ticket::computeNymble(u_char *trapdoor, u_char *out)
{
  u_char nonce[] = "g";
  SHA256_CTX ctx;
  
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, nonce, sizeof(nonce));
  SHA256_Update(&ctx, trapdoor, DIGEST_SIZE);
  SHA256_Final(out, &ctx);
}

void Ticket::evolveTrapdoor(u_char* trapdoor, u_int delta, u_char* out)
{
  memcpy(out, trapdoor, DIGEST_SIZE);
  
  for (u_int i = 0; i < delta; i++) {
    SHA256_CTX ctx;
    u_char buffer[DIGEST_SIZE];
    u_char nonce[] = "f";
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, nonce, 1);
    SHA256_Update(&ctx, out, DIGEST_SIZE);
    SHA256_Final(buffer, &ctx);
    
    memcpy(out, buffer, DIGEST_SIZE);
  }
}

}; // namespace Nymble
