#include "nymble_ticket_wrap.h"

VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Ticket* ticket = Ticket::unmarshall(bytes);
  
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
  u_int bytes_len = ticket->marshall();
  char bytes[bytes_len];
  
  ticket->marshall(bytes);
  
  return rb_str_new(bytes, bytes_len);
}

void rb_ticket_delete(Ticket* ticket)
{
  delete ticket;
}