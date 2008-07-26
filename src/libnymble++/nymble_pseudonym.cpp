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

char* Pseudonym::marshall()
{
  u_char in[DIGEST_SIZE*2];
  
  memcpy(in, this->pseudonym, DIGEST_SIZE);
  memcpy(in+DIGEST_SIZE, this->mac_np, DIGEST_SIZE);

  return Nymble::hexencode(in, DIGEST_SIZE*2);
}

Pseudonym* Pseudonym::unmarshall(char* bytes)
{
  u_int len;
  u_char* out = Nymble::hexdecode(bytes, &len);
  Pseudonym* pseudonym = NULL;
  
  if (len == DIGEST_SIZE*2) {
    pseudonym = new Pseudonym(out, out + DIGEST_SIZE);
  }
  
  free(out);
  
  return pseudonym;
}
