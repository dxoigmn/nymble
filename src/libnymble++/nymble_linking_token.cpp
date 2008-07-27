#include "nymble_linking_token.h"

LinkingToken::LinkingToken()
{
  
}

LinkingToken::LinkingToken(LinkingToken* linking_token)
{
  this->time_period = linking_token->time_period;
  memcpy(this->trapdoor, linking_token->trapdoor, DIGEST_SIZE);
  memcpy(this->nymble, linking_token->nymble, DIGEST_SIZE);
}

LinkingToken::LinkingToken(u_int time_period, u_char* trapdoor)
{
  this->time_period = time_period;
  
  memcpy(this->trapdoor, trapdoor, DIGEST_SIZE);
  Ticket::computeNymble(this->trapdoor, this->nymble);
}

void LinkingToken::setTimePeriod(u_int time_period)
{
  if (time_period < this->time_period) {
    return;
  }
  
  u_int time_period_delta = time_period - this->time_period;
  
  this->time_period = time_period;
  Ticket::evolveTrapdoor(this->trapdoor, time_period_delta, this->trapdoor);
  Ticket::computeNymble(this->trapdoor, this->nymble);
}

u_char* LinkingToken::getNymble()
{
  return this->nymble;
}

u_int LinkingToken::marshall(char* out)
{
  struct json_object* json_linking_token = json_object_new_object();
  
  Nymble::json_marshall_int(json_linking_token, "time_period", this->time_period);
  Nymble::json_marshall_str(json_linking_token, "trapdoor", this->trapdoor, DIGEST_SIZE);
  
  char* json = json_object_to_json_string(json_linking_token);
  
  if (out != NULL) {
    strcpy(out, json);
  }
  
  return strlen(json);
}

void LinkingToken::unmarshall(char* bytes, LinkingToken* out)
{
  struct json_object* json_linking_token = json_tokener_parse(bytes);
  
  Nymble::json_unmarshall_int(json_linking_token, "time_period", &out->time_period);
  Nymble::json_unmarshall_str(json_linking_token, "trapdoor", out->trapdoor, DIGEST_SIZE);
}
