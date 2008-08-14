#include "nymble_credential.h"

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

Credential::Credential(NymbleManager* nm, NymbleManagerEntry* entry, Pseudonym* pseudonym, u_int time_periods)
{
  u_char trapdoor[DIGEST_SIZE];
  u_char nymble[DIGEST_SIZE];
  
  this->seedTrapdoor(nm, entry, pseudonym->getPseudonym());
  
  memcpy(trapdoor, seed, DIGEST_SIZE);
  
  for (u_int time_period = 1; time_period <= time_periods; time_period++) {
    Ticket::evolveTrapdoor(trapdoor, 1, trapdoor);
    Ticket::computeNymble(trapdoor, nymble);

    Ticket* ticket = new Ticket(nm->getLinkWindow(), time_period, entry->getServerId(), nymble);
    
    ticket->encrypt(nm->getEncryptKeyN(), trapdoor, pseudonym->getPseudonym());
    ticket->hmac(nm->getHmacKeyN());
    ticket->hmac(entry->getHmacKeyNS(), true);
    
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

u_int Credential::marshal(char* out)
{
  struct json_object* json_credential = json_object_new_object();
  
  Nymble::json_marshal_str(json_credential, "seed", this->seed, DIGEST_SIZE);
  
  struct json_object* json_tickets = json_object_new_array();
  
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    struct json_object* json_ticket = json_object_new_object();
    
    (*ticket)->marshal(json_ticket);
    
    json_object_array_add(json_tickets, json_ticket);
  }
  
  json_object_object_add(json_credential, "tickets", json_tickets);
  
  char* json = json_object_to_json_string(json_credential);
  
  if (out != NULL) {
    strcpy(out, json);
  }
  
  return strlen(json);
}

void Credential::unmarshal(char* bytes, Credential* out)
{
  struct json_object* json_credential = json_tokener_parse(bytes);
  
  Nymble::json_unmarshal_str(json_credential, "seed", out->seed, DIGEST_SIZE);
  
  struct json_object* json_tickets = json_object_object_get(json_credential, "tickets");
  
  for (int i = 0; i < json_object_array_length(json_tickets); i++) {
    struct json_object* json_ticket = json_object_array_get_idx(json_tickets, i);
    Ticket* ticket = new Ticket();
    
    Ticket::unmarshal(json_ticket, ticket);
    
    out->push_back(ticket);
  }
}
