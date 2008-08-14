#include "nymble_linking_token.h"

namespace Nymble {

LinkingToken::LinkingToken()
{
  
}

LinkingToken::LinkingToken(LinkingToken* linking_token)
{
  this->time_period = linking_token->time_period;
  memcpy(this->trapdoor, linking_token->trapdoor, DIGEST_SIZE);
  memcpy(this->nymble, linking_token->nymble, DIGEST_SIZE);
}

LinkingToken::LinkingToken(Marshal::LinkingToken* linking_token)
{
  this->time_period = linking_token->time_period();
  memcpy(this->trapdoor, linking_token->trapdoor().data(), DIGEST_SIZE);
  
  Ticket::computeNymble(this->trapdoor, this->nymble);
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

u_int LinkingToken::marshal(u_char* out, u_int size)
{
  Marshal::LinkingToken linking_token;
  
  linking_token.set_time_period(this->time_period);
  linking_token.set_trapdoor(this->trapdoor, DIGEST_SIZE);
  
  if (out != NULL) {
    linking_token.SerializeToArray(out, size);
  }
  
  return linking_token.ByteSize();
}

LinkingToken* LinkingToken::unmarshal(u_char* bytes, u_int size)
{
  Marshal::LinkingToken linking_token;
  
  if (linking_token.ParseFromArray(bytes, size)) {
    return new LinkingToken(&linking_token);
  }
  
  return NULL;
}

}; // namespace Nymble
