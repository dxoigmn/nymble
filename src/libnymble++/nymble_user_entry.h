#ifndef __NYMBLE_USER_ENTRY_H__
#define __NYMBLE_USER_ENTRY_H__

#include "nymble.h"

#include "nymble_credential.pb.h"

namespace Nymble {

class UserEntry
{
  std::string sid;
  Credential credential;
  bool has_been_silent;
  
  public:
    UserEntry(std::string sid, Credential credential);
    
    std::string getServerId();
    Credential* getCredential();
};

}; // namespace Nymble

#endif