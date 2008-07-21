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
  
  for (u_int time_period = 1; time_period < time_periods; time_period++) {
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

u_int Credential::marshall(char* out)
{
  Json::FastWriter writer;
  Json::Value credential;
  Json::Value tickets;
  
  char encoded[DIGEST_SIZE*2+1];
  encoded[DIGEST_SIZE*2] = 0;
  
  Nymble::hexencode(encoded, this->seed, DIGEST_SIZE);
  credential["seed"] = encoded;
  
  for (Tickets::iterator ticket = this->begin(); ticket != this->end(); ++ticket) {
    Json::Value json_ticket;
    
    (*ticket)->marshall(&json_ticket);
    
    tickets.append(json_ticket);
  }
  
  credential["tickets"] = tickets;
  
  std::string json = writer.write(credential);
  
  if (out) {
    strncpy(out, json.c_str(), json.size());
  }
  
  return json.size();
}

Credential* Credential::unmarshall(char* bytes)
{
  Json::Value json;
  Json::Reader reader;
  
  if (!reader.parse(bytes, json)) {
    return NULL;
  }
  
  Credential* credential = new Credential();
  
  Nymble::hexdecode(credential->seed, (char*) json["seed"].asCString(), DIGEST_SIZE*2);
  
  Json::Value json_tickets = json["tickets"];
  
  for (Json::Value::iterator json_ticket = json_tickets.begin(); json_ticket != json_tickets.end(); ++json_ticket) {
    credential->push_back(Ticket::unmarshall(*json_ticket));
  }

  return credential;
}
