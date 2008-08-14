#include "nymble_pseudonym.h"

namespace Nymble {

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

u_int Pseudonym::marshal(u_char* out, u_int size)
{
  u_char in[DIGEST_SIZE*2];
  
  memcpy(in, this->pseudonym, DIGEST_SIZE);
  memcpy(in+DIGEST_SIZE, this->mac_np, DIGEST_SIZE);
  
  u_int out_len = hexencode(in, DIGEST_SIZE*2);
  
  if (out && size == out_len) {
    hexencode(in, DIGEST_SIZE*2, (char*) out);
  }
  
  return out_len;
}

Pseudonym* Pseudonym::unmarshal(u_char* bytes, u_int size)
{
  u_int decoded_len = hexdecode((char*) bytes);
  u_char decoded[decoded_len];
  
  hexdecode((char*) bytes, decoded);
  
  return new Pseudonym(decoded, decoded + DIGEST_SIZE);
}

}; // namespace Nymble
