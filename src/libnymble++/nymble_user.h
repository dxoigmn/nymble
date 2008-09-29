#ifndef __NYMBLE_USER_H__
#define __NYMBLE_USER_H__

#include "nymble.h"
#include "nymble_user_entry.h"

namespace Nymble {

typedef std::vector<UserEntry*> UserEntries;

class User : public Nymble, public UserEntries
{
  Pseudonym* pnym;
  RSA* ver_key_n;
  
  UserEntry* findEntry(u_char* server_id);
  
  public:
    User();
    ~User();
    
    void setPseudonym(Pseudonym* pnym);
    Pseudonym* getPseudonym();
    
    void readVerifyKey(char* ver_key_path);

    UserEntry* findOrCreateEntry(u_char* sid);
    bool User::verifyBlacklist(Blacklist* blacklist);
};

}

#endif
