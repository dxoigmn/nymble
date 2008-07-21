#include "nymble_linking_token.h"

LinkingToken::LinkingToken(u_int time_period, u_char* trapdoor, u_char* nymble)
{
  this->time_period = time_period;
  
  memcpy(this->trapdoor, trapdoor, DIGEST_SIZE);
  memcpy(this->nymble, nymble, DIGEST_SIZE);
}