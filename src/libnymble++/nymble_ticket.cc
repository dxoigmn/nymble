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
  memcpy(this->ctxt, ticket->ctxt, CTXT_SIZE);
  memcpy(this->mac_n, ticket->mac_n, DIGEST_SIZE);
  memcpy(this->mac_ns, ticket->mac_ns, DIGEST_SIZE);
}

Ticket::Ticket(Marshal::Ticket* ticket)
{
  memcpy(this->nymble, ticket->nymble().data(), DIGEST_SIZE);
  memcpy(this->ctxt, ticket->ctxt().data(), CTXT_SIZE);
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


void Ticket::hmac(u_char* mac_key, u_char* sid, u_int t, u_int w, bool include_mac_n, u_char* out)
{
  HMAC_CTX ctx;
  
  HMAC_Init(&ctx, mac_key, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, sid, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&t, sizeof(t));
  HMAC_Update(&ctx, (u_char *)&w, sizeof(w));
  HMAC_Update(&ctx, this->nymble, DIGEST_SIZE);
  HMAC_Update(&ctx, this->ctxt, CTXT_SIZE);
  
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

void Ticket::encrypt(u_char* enc_key, u_char* nymble, u_char* seed)
{
  u_char in[DIGEST_SIZE * 2];
  u_char iv[CIPHER_BLOCK_SIZE];
  AES_KEY key;
  
  memcpy(in, nymble, DIGEST_SIZE);
  memcpy(in + DIGEST_SIZE, seed, DIGEST_SIZE);
  
  /* iv must be separate from buffer as the AES function messes with it as it works */
  RAND_bytes(iv, CIPHER_BLOCK_SIZE);
  memcpy(this->ctxt, iv, CIPHER_BLOCK_SIZE);
  
  AES_set_encrypt_key(enc_key, CIPHER_BLOCK_SIZE * 8, &key);
  AES_cbc_encrypt(in, this->ctxt + CIPHER_BLOCK_SIZE, DIGEST_SIZE * 2, &key, iv, AES_ENCRYPT);
}

void Ticket::decrypt(u_char* enc_key, u_char* nymble, u_char* seed)
{
  u_char buffer[CIPHER_BLOCK_SIZE * 4];
  u_char iv[CIPHER_BLOCK_SIZE];
  AES_KEY key;

  memcpy(iv, this->ctxt, CIPHER_BLOCK_SIZE);

  AES_set_decrypt_key(enc_key, CIPHER_BLOCK_SIZE * 8, &key);
  AES_cbc_encrypt(this->ctxt + CIPHER_BLOCK_SIZE, buffer, CTXT_SIZE - CIPHER_BLOCK_SIZE, &key, iv, AES_DECRYPT);
  
  if (nymble) {
    memcpy(nymble, buffer, DIGEST_SIZE);
  }
  
  if (seed) {
    memcpy(seed, buffer + DIGEST_SIZE, DIGEST_SIZE);
  }
}

Complaint* Ticket::createComplaint(u_int time_period)
{
  return new Complaint(time_period, this);
}

void Ticket::marshal(Marshal::Ticket* out)
{
  out->set_nymble(this->nymble, DIGEST_SIZE);
  out->set_ctxt(this->ctxt, CTXT_SIZE);
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

void Ticket::computeNymble(u_char *seed, u_char *out)
{
  u_char nonce[] = "g";
  SHA256_CTX ctx;
  
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, nonce, sizeof(nonce));
  SHA256_Update(&ctx, seed, DIGEST_SIZE);
  SHA256_Final(out, &ctx);
}

void Ticket::evolveSeed(u_char* seed, u_int delta, u_char* out)
{
  memcpy(out, seed, DIGEST_SIZE);
  
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
