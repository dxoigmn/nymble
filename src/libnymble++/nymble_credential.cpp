#include "nymble_credential.h"

namespace Nymble {

Credential::Credential()
{
  
}

Credential::Credential(Credential* credential)
{
  memcpy(this->seed, credential->seed, DIGEST_SIZE);

  for (Tickets::iterator ticket = credential->begin(); ticket != credential->end(); ++ticket) {
    this->push_back(new Ticket(*ticket));
  }
}

Credential::Credential(Marshal::Credential* credential)
{
  memcpy(this->seed, credential->seed().data(), DIGEST_SIZE);
  
  for (int i = 0; i < credential->ticket_size(); i++) {
    this->push_back(new Ticket((Marshal::Ticket*) &credential->ticket(i)));
  }
}

Credential::Credential(NymbleManager* nm, NymbleManagerEntry* entry, Pseudonym* pseudonym, u_int time_periods)
{
  u_char trapdoor[DIGEST_SIZE];
  u_char nymble[DIGEST_SIZE];
  
  this->seedTrapdoor(nm, entry, pseudonym->getPseudonym());
  
  memcpy(trapdoor, seed, DIGEST_SIZE);
  
  for (u_int time_period = 1; time_period <= time_periods; time_period++) {
    Ticket::evolveTrapdoor(trapdoor, 1, trapdoor);
    Ticket::computeNymble(trapdoor, nymble);

    Ticket* ticket = new Ticket(nymble);
    
    ticket->encrypt(nm->getEncryptKeyN(), trapdoor, pseudonym->getPseudonym());
    ticket->hmac(nm->getHmacKeyN(), entry->getServerId(), nm->getLinkWindow(), time_period);
    ticket->hmac(entry->getHmacKeyNS(), entry->getServerId(), nm->getLinkWindow(), time_period, true);
    
    this->push_back(ticket);
  }
}

Credential::~Credential()
{
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    delete *ticket;
  }
}

u_char* Credential::getSeed()
{
  return this->seed;
}

void Credential::seedTrapdoor(NymbleManager* nm, NymbleManagerEntry *entry, u_char *pseudonym)
{
  nm->seedTrapdoor(entry, pseudonym, this->seed);
}

u_int Credential::marshal(u_char* out, u_int size)
{
  Marshal::Credential credential;
  
  credential.set_seed(this->seed, DIGEST_SIZE);
  
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    (*ticket)->marshal(credential.add_ticket());
  }
  
  if (out != NULL) {
    credential.SerializeToArray(out, size);
  }
  
  return credential.ByteSize();
}

Credential* Credential::unmarshal(u_char* bytes, u_int size)
{
  Marshal::Credential credential;
  
  if (credential.ParseFromArray(bytes, size)) {
    return new Credential(&credential);
  }
  
  return NULL;
}

}; // namespace Nymble
