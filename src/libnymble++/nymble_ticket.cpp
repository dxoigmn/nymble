#include "nymble_ticket.h"

namespace Nymble {

Ticket::Ticket()
{
  
}

Ticket::Ticket(u_char* nymble)
{
  memcpy(this->nymble, nymble, DIGEST_SIZE);
}

Ticket::Ticket(Ticket* ticket)
{
  memcpy(this->nymble, ticket->nymble, DIGEST_SIZE);
  memcpy(this->trapdoorenc, ticket->trapdoorenc, TRAPDOORENC_SIZE);
  memcpy(this->mac_n, ticket->mac_n, DIGEST_SIZE);
  memcpy(this->mac_ns, ticket->mac_ns, DIGEST_SIZE);
}

Ticket::Ticket(Marshal::Ticket* ticket)
{
  memcpy(this->nymble, ticket->nymble().data(), DIGEST_SIZE);
  memcpy(this->trapdoorenc, ticket->trapdoorenc().data(), TRAPDOORENC_SIZE);
  memcpy(this->mac_n, ticket->mac_n().data(), DIGEST_SIZE);
  memcpy(this->mac_ns, ticket->mac_ns().data(), DIGEST_SIZE);
}

u_char* Ticket::getNymble()
{
  return this->nymble;
}

u_char* Ticket::getMacNS()
{
  return this->mac_ns;
}

void Ticket::hmac(u_char* hmac_key, u_char* server_id, u_int link_window, u_int time_period, bool include_mac_n, u_char* out)
{
  HMAC_CTX ctx;
  
  HMAC_Init(&ctx, hmac_key, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, server_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&link_window, sizeof(link_window));
  HMAC_Update(&ctx, (u_char *)&time_period, sizeof(time_period));
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

Complaint* Ticket::createComplaint(u_int time_period)
{
  return new Complaint(time_period, this);
}

void Ticket::marshal(Marshal::Ticket* out)
{
  out->set_nymble(this->nymble, DIGEST_SIZE);
  out->set_trapdoorenc(this->trapdoorenc, TRAPDOORENC_SIZE);
  out->set_mac_n(this->mac_n, DIGEST_SIZE);
  out->set_mac_ns(this->mac_ns, DIGEST_SIZE);
}

u_int Ticket::marshal(u_char* out, u_int size)
{
  Marshal::Ticket ticket;
  
  this->marshal(&ticket);
  
  if (out != NULL) {
    ticket.SerializeToArray(out, size);
  }
  
  return ticket.ByteSize();
}

Ticket* Ticket::unmarshal(u_char* bytes, u_int size)
{
  Marshal::Ticket ticket;
  
  if (ticket.ParseFromArray(bytes, size)) {
    return new Ticket(&ticket);
  }
  
  return NULL;
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
