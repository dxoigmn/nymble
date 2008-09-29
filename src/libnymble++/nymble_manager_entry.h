#ifndef __NYMBLE_MANAGER_ENTRY_H__
#define __NYMBLE_MANAGER_ENTRY_H__

#include "nymble.h"
#include "nymble_credential.h"

namespace Nymble {

class Credential;

class NymbleManagerEntry {
  u_char  sid[DIGEST_SIZE];
  u_int   bl_last_updated;
  u_char  mac_key_ns[DIGEST_SIZE];
  
  public:
    NymbleManagerEntry(u_char* sid, u_int cur_time_period);
    
    void setServerId(u_char* sid);
    u_char* getServerId();
    
    void setLastUpdated(u_int bl_last_updated);
    u_int getLastUpdated();
    
    void setMacKeyNS(u_char* mac_key_ns);
    u_char* getMacKeyNS();
};

}; // namespace Nymble

#endif
