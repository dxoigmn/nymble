#include "nymble_pseudonym.h"

namespace Nymble {

Pseudonym::Pseudonym()
{

}

Pseudonym::Pseudonym(Pseudonym* pnym)
{
  this->setPseudonym(pnym->getPseudonym());
  this->setMacNP(pnym->getMacNP());
}

Pseudonym::Pseudonym(u_char* nym, u_char* mac)
{
  this->setPseudonym(nym);
  this->setMacNP(mac);
}

void Pseudonym::setPseudonym(u_char* nym)
{
  memcpy(this->nym, nym, DIGEST_SIZE);
}

u_char* Pseudonym::getPseudonym()
{
  return this->nym;
}

void Pseudonym::setMacNP(u_char* mac)
{
  memcpy(this->mac, mac, DIGEST_SIZE);
}

u_char* Pseudonym::getMacNP()
{
  return this->mac;
}

u_int Pseudonym::marshal(u_char* out, u_int size)
{
  u_char in[DIGEST_SIZE*2];
  
  memcpy(in, this->nym, DIGEST_SIZE);
  memcpy(in+DIGEST_SIZE, this->mac, DIGEST_SIZE);
  
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
