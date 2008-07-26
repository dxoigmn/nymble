#ifndef __NYMBLE_PSEUDONYM_H__
#define __NYMBLE_PSEUDONYM_H__

#include "nymble.h"

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
    
    u_int marshall(char* out = NULL);
    static void unmarshall(char* bytes, Pseudonym* out);
};

#endif