#ifndef __NYMBLE_LINKING_TOKEN_H__
#define __NYMBLE_LINKING_TOKEN_H__

#include "nymble.h"
#include "nymble_ticket.h"

class LinkingToken {
  u_int   time_period;
  u_char  trapdoor[DIGEST_SIZE];
  u_char  nymble[DIGEST_SIZE];
  
  public:
    LinkingToken();
    LinkingToken(LinkingToken* linking_token);
    LinkingToken(u_int time_period, u_char* trapdoor);
    
    void setTimePeriod(u_int time_period);
    
    u_char* getNymble();
    
    u_int marshal(char* out = NULL);
    static void unmarshal(char* bytes, LinkingToken* out);
};

#endif
