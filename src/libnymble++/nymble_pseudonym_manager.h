#ifndef __NYMBLE_PSEUDONYM_MANAGER_H__
#define __NYMBLE_PSEUDONYM_MANAGER_H__

#include <openssl/hmac.h>

#include "nymble.h"

#include "nymble_pseudonym.pb.h"

namespace Nymble {

class PseudonymManager : public Nymble
{
  u_char  mac_key_np[DIGEST_SIZE];
  u_char  nym_key_p[DIGEST_SIZE];
  
  public:
    PseudonymManager();
    PseudonymManager(u_char* mac_key_np);
    
    u_char* getMacKeyNP();
    void setMacKeyNP(u_char* mac_key_np);
    
    Pseudonym* createPseudonym(u_char* user_id);
};

}; // namespace Nymble

#endif