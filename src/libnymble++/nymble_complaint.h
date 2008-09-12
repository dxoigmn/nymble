#ifndef __NYMBLE_COMPLAINT_H__
#define __NYMBLE_COMPLAINT_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_complaint.pb.h"

namespace Nymble {

class Ticket;
class Complaint;
typedef std::vector<Complaint*> Complaints;

class Complaint {
  u_int time;
  Ticket* ticket;
  
  public:
    Complaint();
    Complaint(Complaint* complaint);
    Complaint(Marshal::Complaint* complaint);
    Complaint(u_int time, Ticket* ticket);
    
    u_int getTime();
    Ticket* getTicket();
    
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Complaint* Complaint::unmarshal(u_char* bytes, u_int size);
};

}; // namespace Nymble

#endif
