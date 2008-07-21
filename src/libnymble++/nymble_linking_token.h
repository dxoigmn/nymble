#ifndef __NYMBLE_LINKING_TOKEN_H__
#define __NYMBLE_LINKING_TOKEN_H__

#include "nymble.h"

class LinkingToken {
  public:
    u_int   time_period;
    u_char  trapdoor[DIGEST_SIZE];
    u_char  nymble[DIGEST_SIZE];
    
    LinkingToken(u_int time_period, u_char* trapdoor, u_char* nymble);
};

#endif
