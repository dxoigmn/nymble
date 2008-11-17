#ifndef __NYMBLE_PSEUDONYM_MANAGER_H__
#define __NYMBLE_PSEUDONYM_MANAGER_H__

#include <openssl/hmac.h>

#include "nymble.h"

#include "nymble_pseudonym.pb.h"

namespace Nymble {

class PseudonymManager : public Nymble
{
  std::string mac_key_np;
  std::string nym_key_p;
  
  public:
    PseudonymManager();
    
    void setMacKeyNP(std::string mac_key_np);
    
    bool createPseudonym(std::string user_id, Pseudonym& pseudonym);
};

}; // namespace Nymble

#endif