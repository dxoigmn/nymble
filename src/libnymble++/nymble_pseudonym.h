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
    
    u_int marshal(char* out = NULL);
    static void unmarshal(char* bytes, Pseudonym* out);
};

}; // namespace Nymble

#endif