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
    PseudonymManager(std::string mac_key_np);
    
    std::string getMacKeyNP();
    void setMacKeyNP(std::string mac_key_np);
    
    Pseudonym* createPseudonym(std::string user_id);
};

}; // namespace Nymble

#endif