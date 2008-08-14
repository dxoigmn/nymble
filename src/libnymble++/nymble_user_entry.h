#ifndef __NYMBLE_USER_ENTRY_H__
#define __NYMBLE_USER_ENTRY_H__

#include "nymble.h"
#include "nymble_credential.h"
#include "nymble_blacklist.h"

namespace Nymble {

class UserEntry
{
  u_char  server_id[DIGEST_SIZE];
  
  Credential* credential;
  Blacklist* blacklist;
  
  public:
    UserEntry(Blacklist* blacklist);
    
    u_char* getServerId();
    bool isBlacklisted();
    Ticket* getTicket(u_int time_period);
    
    void setCredential(Credential* credential);
    void setBlacklist(Blacklist* blacklist);
};

}; // namespace Nymble

#endif