#ifndef __NYMBLE_CREDENTIAL_H__
#define __NYMBLE_CREDENTIAL_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_pseudonym.h"

typedef std::vector<Ticket*> Tickets;

class Credential : public Tickets
{
  u_char  seed[DIGEST_SIZE];
  
  public:
    Credential();
    Credential(Credential* credential);
    Credential(u_char* seed, u_char* server_id, u_int link_window, u_int time_periods);
    ~Credential();
    
    u_char* getSeed();
    
    u_int marshall(char* out = NULL);
    static void unmarshall(char* bytes, Credential* out);
};

#endif
