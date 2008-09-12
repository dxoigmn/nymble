#ifndef __NYMBLE_TOKEN_H__
#define __NYMBLE_TOKEN_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_token.pb.h"

namespace Nymble {

class Token {
  u_char  seed[DIGEST_SIZE];
  u_char  nymble[DIGEST_SIZE];
  
  public:
    Token();
    Token(Token* token);
    Token(Marshal::Token* token);
    Token(u_char* seed);
    
    u_char* getNymble();
    
    void evolve(u_int time_period_delta);
    
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Token* unmarshal(u_char* bytes, u_int size);
};

}; // namespace Nymble

#endif
