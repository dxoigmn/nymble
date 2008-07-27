#include "nymble_ticket_wrap.h"

VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Ticket* ticket = new Ticket();
  
  Ticket::unmarshall(bytes, ticket);
  
  if (ticket == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_ticket_delete, ticket);
}

VALUE rb_ticket_marshall(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cTicket);

  Ticket* ticket = (Ticket*) DATA_PTR(rb_self);
  char marshalled_ticket[ticket->marshall() + 1];
  
  ticket->marshall(marshalled_ticket);
  
  return rb_str_new2(marshalled_ticket);
}

void rb_ticket_delete(Ticket* ticket)
{
  delete ticket;
}