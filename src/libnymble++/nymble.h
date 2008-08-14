#ifndef __NYMBLE_H__
#define __NYMBLE_H__

#include <vector>

#include "nymble_util.h"

#define DIGEST_SIZE         SHA256_DIGEST_LENGTH
#define CIPHER_BLOCK_SIZE   AES_BLOCK_SIZE
#define SIGNATURE_SIZE      256
#define TRAPDOORENC_SIZE    (CIPHER_BLOCK_SIZE + DIGEST_SIZE + DIGEST_SIZE + CIPHER_BLOCK_SIZE)

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
