#include "nymble_pseudonym.h"

Pseudonym::Pseudonym()
{

}

Pseudonym::Pseudonym(Pseudonym* pseudonym)
{
  this->setPseudonym(pseudonym->getPseudonym());
  this->setMacNP(pseudonym->getMacNP());
}

Pseudonym::Pseudonym(u_char* pseudonym, u_char* mac_np)
{
  this->setPseudonym(pseudonym);
  this->setMacNP(mac_np);
}

void Pseudonym::setPseudonym(u_char* pseudonym)
{
  memcpy(this->pseudonym, pseudonym, DIGEST_SIZE);
}

u_char* Pseudonym::getPseudonym()
{
  return this->pseudonym;
}

void Pseudonym::setMacNP(u_char* mac_np)
{
  memcpy(this->mac_np, mac_np, DIGEST_SIZE);
}

u_char* Pseudonym::getMacNP()
{
  return this->mac_np;
}

u_int Pseudonym::marshall(char* out)
{
  u_char in[DIGEST_SIZE*2];

  memcpy(in, this->pseudonym, DIGEST_SIZE);
  memcpy(in+DIGEST_SIZE, this->mac_np, DIGEST_SIZE);
  
  u_int out_len = Nymble::hexencode(in, DIGEST_SIZE*2);
  
  if (out) {
    Nymble::hexencode(in, DIGEST_SIZE*2, out);
  }
  
  return out_len;
}

void Pseudonym::unmarshall(char* bytes, Pseudonym* out)
{
  u_int decoded_len = Nymble::hexdecode(bytes);
  u_char decoded[decoded_len];
  
  Nymble::hexdecode(bytes, decoded);
  
  memcpy(out->pseudonym, decoded, DIGEST_SIZE);
  memcpy(out->mac_np, decoded + DIGEST_SIZE, DIGEST_SIZE);
}