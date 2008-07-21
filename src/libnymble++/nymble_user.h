#ifndef __NYMBLE_USER_H__
#define __NYMBLE_USER_H__

#include <vector>

#include "nymble.h"
#include "nymble_user_entry.h"

typedef std::vector<UserEntry*> UserEntries;

class User : public Nymble, public UserEntries
{
  Pseudonym* pseudonym;
  RSA* verify_key_n;
  
  UserEntry* findServer(u_char* server_id);
  
  public:
    User();
    ~User();
    
    void setPseudonym(Pseudonym* pseudonym);
    Pseudonym* getPseudonym();
    
    void setVerifyKeyN(u_char** verify_key_n, u_int verify_key_n_len);
    
    UserEntry* addServer(u_char* server_id, Blacklist* blacklist, Credential* credential);
    Ticket* getTicket(u_char* server_id);
};

#endif
