#ifndef __NYMBLE_USER_ENTRY_H__
#define __NYMBLE_USER_ENTRY_H__

#include "nymble.h"
#include "nymble_credential.h"
#include "nymble_blacklist.h"

class UserEntry
{
  u_char  server_id[DIGEST_SIZE];
  u_int   blacklisted;
  u_int   last_authenticated;
  
  Credential* credential;
  Blacklist* blacklist;
  
  public:
    UserEntry(u_char* server_id, Blacklist* blacklist, Credential* credential);
    
    u_char* getServerId();
    Credential* getCredential();
};

#endif