#include "nymble_complaint.h"

namespace Nymble {

Complaint::Complaint()
{
  
}

Complaint::Complaint(Complaint* complaint)
{
  this->time_period = complaint->time_period;
  this->ticket = new Ticket(complaint->ticket);
}

Complaint::Complaint(Marshal::Complaint* complaint)
{
  this->time_period = complaint->time_period();
  this->ticket = new Ticket((Marshal::Ticket*) &complaint->ticket());
}

Complaint::Complaint(u_int time_period, Ticket* ticket)
{
  this->time_period = time_period;
  this->ticket = new Ticket(ticket);
}

u_int Complaint::getTimePeriod()
{
  return this->time_period;
}

Ticket* Complaint::getTicket()
{
  return this->ticket;
}

u_int Complaint::marshal(u_char* out, u_int size)
{
  Marshal::Complaint complaint;
  
  complaint.set_time_period(this->time_period);
  this->ticket->marshal(complaint.mutable_ticket());
  
  if (out != NULL) {
    complaint.SerializeToArray(out, size);
  }
  
  return complaint.ByteSize();
}


Complaint* Complaint::unmarshal(u_char* bytes, u_int size)
{
  Marshal::Complaint complaint;
  
  if (complaint.ParseFromArray(bytes, size)) {
    return new Complaint(&complaint);
  }
  
  return NULL;
}

  
} // namespaceNymble