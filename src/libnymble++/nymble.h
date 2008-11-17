#ifndef __NYMBLE_H__
#define __NYMBLE_H__

#include "nymble_util.h"

namespace Nymble {

class Nymble
{
  public:
    u_int cur_link_window;
    u_int cur_time_period;
    
    Nymble();
    
    void setLinkWindow(u_int link_window);
    u_int getLinkWindow();
    
    void setTimePeriod(u_int time_period);
    u_int getTimePeriod();
};

}; // namespace Nymble

#endif
