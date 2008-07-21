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
  if (out) {
    Nymble::hexencode(out, this->pseudonym, DIGEST_SIZE);
    Nymble::hexencode(out + DIGEST_SIZE*2, this->mac_np, DIGEST_SIZE);
  }
  
  return DIGEST_SIZE*4;
}

Pseudonym* Pseudonym::unmarshall(char* bytes)
{
  u_char buffer[DIGEST_SIZE*4];
  
  if (strlen(bytes) != DIGEST_SIZE*4) {
    return NULL;
  }
  
  Nymble::hexdecode(buffer, bytes, DIGEST_SIZE*4);
  
  return new Pseudonym(buffer, buffer + DIGEST_SIZE);
}
