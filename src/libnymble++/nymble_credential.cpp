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

Credential::Credential(u_char* seed, u_char* server_id, u_int link_window, u_int time_periods)
{
  memcpy(this->seed, seed, DIGEST_SIZE);
  
  for (u_int time_period = 1; time_period <= time_periods; time_period++) {
    Ticket* ticket = new Ticket(link_window, time_period, server_id, seed);
    
    this->push_back(ticket);
  }
}

Credential::~Credential()
{
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    delete *ticket;
  }
}

char* Credential::marshall()
{
  struct json_object* json_credential = json_object_new_object();
  struct json_object* json_tickets = json_object_new_array();
  
  JSON_MARSHALL_STR(credential, seed, DIGEST_SIZE);
  
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    struct json_object* json_ticket = json_object_new_object();
    
    (*ticket)->marshall(json_ticket);
    
    json_object_array_add(json_tickets, json_ticket);
  }
  
  json_object_object_add(json_credential, "tickets", json_tickets);
  
  return json_object_to_json_string(json_credential);
}

Credential* Credential::unmarshall(char* bytes)
{
  struct json_object* json_credential = json_tokener_parse(bytes);
  Credential* credential = new Credential();
  
  JSON_UNMARSHALL_STR(credential, seed, DIGEST_SIZE);
  
  struct json_object* json_tickets = json_object_object_get(json_credential, "tickets");
  
  for (int i = 0; i < json_object_array_length(json_tickets); i++) {
    struct json_object* json_ticket = json_object_array_get_idx(json_tickets, i);
    
    credential->push_back(Ticket::unmarshall(json_ticket));
  }
  
  return credential;
}
