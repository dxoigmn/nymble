#ifndef __NYMBLE_TOKEN_H__
#define __NYMBLE_TOKEN_H__

#include "nymble.h"
#include "nymble_ticket.h"

class Token
{
  public:
    u_char  server_id[DIGEST_SIZE];
    u_int   link_window;
    u_int   time_period;
    u_char  trapdoor[DIGEST_SIZE];
    Ticket* ticket;
    
    Token(u_char* server_id, u_int link_window, u_int time_period, u_char* trapdoor, Ticket *ticket);
    ~Token();
};

#endif
