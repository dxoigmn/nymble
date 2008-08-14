#include "nymble_ticket_wrap.h"

VALUE rb_ticket_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Nymble::Ticket* ticket = new Nymble::Ticket();
  
  Nymble::Ticket::unmarshal(bytes, ticket);
  
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
  char marshaled_ticket[ticket->marshal() + 1];
  
  ticket->marshal(marshaled_ticket);
  
  return rb_str_new2(marshaled_ticket);
}

void rb_ticket_delete(Nymble::Ticket* ticket)
{
  delete ticket;
}