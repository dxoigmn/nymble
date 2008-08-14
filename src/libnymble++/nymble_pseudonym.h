#ifndef __NYMBLE_PSEUDONYM_H__
#define __NYMBLE_PSEUDONYM_H__

#include "nymble.h"

namespace Nymble {

class Pseudonym
{
  u_char  pseudonym[DIGEST_SIZE];
  u_char  mac_np[DIGEST_SIZE];
  
  public:
    Pseudonym();
    Pseudonym(Pseudonym* pseudonym);
    Pseudonym(u_char* pseudonym, u_char* mac_np);
    
    void setPseudonym(u_char* pseudonym);
    u_char* getPseudonym();
    
    void setMacNP(u_char* mac_np);
    u_char* getMacNP();
    
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Pseudonym* unmarshal(u_char* bytes, u_int size);
};

}; // namespace Nymble

#endif