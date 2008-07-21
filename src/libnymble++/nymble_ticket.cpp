#include "nymble_ticket.h"

Ticket::Ticket()
{
  
}

Ticket::Ticket(u_int link_window, u_int time_period, u_char* server_id, u_char* seed)
{
  this->link_window = link_window;
  this->time_period = time_period;
  memcpy(this->server_id, server_id, DIGEST_SIZE);
  
  u_char trapdoor[DIGEST_SIZE];
  
  Ticket::evolveTrapdoor(seed, this->time_period, trapdoor);
  Ticket::computeNymble(trapdoor, this->nymble);
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

void Ticket::encrypt(u_char* encrypt_key_n, u_char* seed, u_char* pseudonym)
{
  u_char in[DIGEST_SIZE * 2];
  u_char iv[CIPHER_BLOCK_SIZE];
  AES_KEY key;
  
  Ticket::evolveTrapdoor(seed, this->time_period, in);
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

void Ticket::marshall(Json::Value* ticket)
{
  (*ticket)["link_window"] = this->link_window;
  (*ticket)["time_period"] = this->time_period;
  
  char encoded[DIGEST_SIZE*2+1];
  encoded[DIGEST_SIZE*2] = 0;
  
  Nymble::hexencode(encoded, this->server_id, DIGEST_SIZE);
  (*ticket)["server_id"] = encoded;
  
  Nymble::hexencode(encoded, this->nymble, DIGEST_SIZE);
  (*ticket)["nymble"] = encoded;
  
  Nymble::hexencode(encoded, this->mac_n, DIGEST_SIZE);
  (*ticket)["mac_n"] = encoded;
  
  Nymble::hexencode(encoded, this->mac_ns, DIGEST_SIZE);
  (*ticket)["mac_ns"] = encoded;
  
  char trapdoorenc_encoded[TRAPDOORENC_SIZE*2+1];
  trapdoorenc_encoded[TRAPDOORENC_SIZE*2] = 0;
  
  Nymble::hexencode(trapdoorenc_encoded, this->trapdoorenc, TRAPDOORENC_SIZE);
  (*ticket)["trapdoorenc"] = trapdoorenc_encoded;
}


u_int Ticket::marshall(char* out)
{
  Json::FastWriter writer;
  Json::Value ticket;
  
  this->marshall(&ticket);
  
  std::string json = writer.write(ticket);
  
  if (out) {
    strncpy(out, json.c_str(), json.size());
  }
  
  return json.size();
}

Ticket* Ticket::unmarshall(Json::Value json)
{
  Ticket* ticket = new Ticket();
  
  ticket->link_window = json["link_window"].asUInt();
  ticket->time_period = json["time_period"].asUInt();
  
  Nymble::hexdecode(ticket->server_id, (char*) json["server_id"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(ticket->nymble, (char*) json["nymble"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(ticket->mac_n, (char*) json["mac_n"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(ticket->mac_ns, (char*) json["mac_ns"].asCString(), DIGEST_SIZE*2);
  Nymble::hexdecode(ticket->trapdoorenc, (char*) json["trapdoorenc"].asCString(), TRAPDOORENC_SIZE*2);

  return ticket;
}

Ticket* Ticket::unmarshall(char* bytes)
{
  Json::Value json;
  Json::Reader reader;
  
  if (!reader.parse(bytes, json)) {
    return NULL;
  }
  
  return Ticket::unmarshall(json);
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