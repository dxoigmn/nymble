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
  void computeNymble(std::string seed, std::string* nymble);
  void evolveSeed(std::string seed, int delta, std::string* seed_out);
  
  public:
    bool isValidTicket(Ticket ticket);
    void add_complaint(Ticket ticket, u_int t);
    bool complain(ServerState* server_state);
    void update(ServerState new_server_state);
};

} // namespace Nymble

#endif
