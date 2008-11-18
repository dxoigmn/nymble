#ifndef __NYMBLE_USER_H__
#define __NYMBLE_USER_H__

#include "nymble.h"
#include "nymble_user_entry.h"

#include "nymble_pseudonym.pb.h"
#include "nymble_ticket.pb.h"
#include "nymble_credential.pb.h"
#include "nymble_blacklist.pb.h"
#include "nymble_blacklist_cert.pb.h"

namespace Nymble {

typedef std::vector<UserEntry*> UserEntries;

class User : public Nymble, public UserEntries
{
  Pseudonym* pnym;
  RSA* ver_key_n;
  
  UserEntry* findEntry(std::string sid);
  bool verifyBlacklist(std::string sid, Blacklist blist, BlacklistCert cert);
  bool checkIfBlacklisted(std::string sid, Blacklist blist);
  
  public:
    User();
    ~User();
    
    void setPseudonym(Pseudonym pnym);
    Pseudonym* getPseudonym();
    
    bool readVerifyKeyN(std::string ver_key_n_path);
    
    void addCredential(std::string sid, Credential cred);
    Ticket* getTicket(std::string sid);
    bool isBlacklisted(std::string sid, Blacklist blist, BlacklistCert cert);
};

}

#endif
