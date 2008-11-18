#ifndef __NYMBLE_SERVER_H__
#define __NYMBLE_SERVER_H__

#include "nymble.h"

#include "nymble_server_state.pb.h"
#include "nymble_ticket.pb.h"

namespace Nymble {

class Server : public Nymble, public ServerState
{
  bool verifyTicket(Ticket ticket);
  bool linkTicket(Ticket ticket);
  
  public:
    bool isValidTicket(Ticket ticket);
    
};

} // namespace Nymble

#endif
