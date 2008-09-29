#include "nymble_credential.h"

namespace Nymble {

Credential::Credential()
{
  
}

Credential::Credential(Credential* credential)
{
  memcpy(this->nymble0, credential->nymble0, DIGEST_SIZE);

  for (Tickets::iterator ticket = credential->begin(); ticket != credential->end(); ++ticket) {
    this->push_back(new Ticket(*ticket));
  }
}

Credential::Credential(Marshal::Credential* credential)
{
  memcpy(this->nymble0, credential->nymble0().data(), DIGEST_SIZE);
  
  for (int i = 0; i < credential->ticket_size(); i++) {
    this->push_back(new Ticket((Marshal::Ticket*) &credential->ticket(i)));
  }
}

Credential::Credential(NymbleManager* nm, NymbleManagerEntry* entry, Pseudonym* pseudonym, u_int time_periods)
{
  u_char seed[DIGEST_SIZE];
  u_char nymble[DIGEST_SIZE];
  
  nm->seed(entry, pseudonym->getPseudonym(), seed);
  Ticket::computeNymble(seed, nymble);
  
  memcpy(this->nymble0, nymble, DIGEST_SIZE);
  
  for (u_int time_period = 1; time_period <= time_periods; time_period++) {
    Ticket::evolveSeed(seed, 1, seed);
    Ticket::computeNymble(seed, nymble);

    Ticket* ticket = new Ticket(nymble);
    
    ticket->encrypt(nm->getEncryptKeyN(), this->nymble0, seed);
    ticket->mac(nm->getMacKeyN(), entry->getServerId(), nm->getLinkWindow(), time_period);
    ticket->mac(entry->getMacKeyNS(), entry->getServerId(), nm->getLinkWindow(), time_period, true);
    
    this->push_back(ticket);
  }
}

Credential::~Credential()
{
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    delete *ticket;
  }
}

u_char* Credential::getNymble0()
{
  return this->nymble0;
}

u_int Credential::marshal(u_char* out, u_int size)
{
  Marshal::Credential credential;
  
  credential.set_nymble0(this->nymble0, DIGEST_SIZE);
  
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
