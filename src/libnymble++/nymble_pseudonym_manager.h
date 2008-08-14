#ifndef __NYMBLE_PSEUDONYM_MANAGER_H__
#define __NYMBLE_PSEUDONYM_MANAGER_H__

#include <openssl/hmac.h>

#include "nymble.h"
#include "nymble_pseudonym.h"

namespace Nymble {

class PseudonymManager : public Nymble
{
  u_char  hmac_key_np[DIGEST_SIZE];
  u_char  keyedhash_key_p[DIGEST_SIZE];
  
  public:
    PseudonymManager();
    PseudonymManager(u_char* hmac_key_np);
    
    u_char* getHmacKeyNP();
    void setHmacKeyNP(u_char* hmac_key_np);
    
    Pseudonym* createPseudonym(u_char* user_id);
};

}; // namespace Nymble

#endif