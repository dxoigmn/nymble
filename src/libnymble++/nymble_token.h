#ifndef __NYMBLE_LINKING_TOKEN_H__
#define __NYMBLE_LINKING_TOKEN_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_linking_token.pb.h"

namespace Nymble {

class LinkingToken {
  u_char  trapdoor[DIGEST_SIZE];
  u_char  nymble[DIGEST_SIZE];
  
  public:
    LinkingToken();
    LinkingToken(LinkingToken* linking_token);
    LinkingToken(Marshal::LinkingToken* linking_token);
    LinkingToken(u_char* trapdoor);
    
    u_char* getNymble();
    
    void evolve(u_int time_period_delta);
    
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static LinkingToken* unmarshal(u_char* bytes, u_int size);
};

}; // namespace Nymble

#endif
