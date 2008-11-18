#ifndef __NYMBLE_MANAGER_ENTRY_H__
#define __NYMBLE_MANAGER_ENTRY_H__

#include "nymble.h"

namespace Nymble {

class NymbleManagerEntry {
  std::string sid;
  std::string mac_key_ns;
  std::string daisy_l;
  u_int time_last_updated;
  
  public:
    NymbleManagerEntry(std::string sid);
    
    void setServerId(std::string sid);
    std::string getServerId();
    
    void setMacKeyNS(std::string mac_key_ns);
    std::string getMacKeyNS();
    
    void setDaisyL(std::string daisy_l);
    std::string getDaisyL();
    
    void setTimeLastUpdated(u_int time_last_updated);
    u_int getTimeLastUpdated();
};

}; // namespace Nymble

#endif
