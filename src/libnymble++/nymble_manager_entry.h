#ifndef __NYMBLE_MANAGER_ENTRY_H__
#define __NYMBLE_MANAGER_ENTRY_H__

#include "nymble.h"

#include "nymble_credential.pb.h"

namespace Nymble {

class Credential;

class NymbleManagerEntry {
  std::string sid;
  u_int   bl_last_updated;
  std::string mac_key_ns;
  
  public:
    NymbleManagerEntry(std::string sid, u_int cur_time_period);
    
    void setServerId(std::string sid);
    std::string getServerId();
    
    void setLastUpdated(u_int bl_last_updated);
    u_int getLastUpdated();
    
    void setMacKeyNS(std::string mac_key_ns);
    std::string getMacKeyNS();
};

}; // namespace Nymble

#endif
