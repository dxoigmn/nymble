#include "nymble_ticket_wrap.h"

VALUE rb_ticket_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Ticket* ticket = Nymble::Ticket::unmarshal(bytes, size);
  
  if (ticket == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_ticket_delete, ticket);
}

VALUE rb_ticket_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cTicket);

  Nymble::Ticket* ticket = (Nymble::Ticket*) DATA_PTR(rb_self);
  u_int marshalled_size = ticket->marshal();
  u_char marshalled[marshalled_size];
  
  ticket->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_ticket_delete(Nymble::Ticket* ticket)
{
  delete ticket;
}