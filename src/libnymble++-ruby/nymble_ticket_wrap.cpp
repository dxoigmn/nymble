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
  char* marshalled_ticket = ticket->marshall();
  VALUE rb_marshalled_ticket = rb_str_new(marshalled_ticket, strlen(marshalled_ticket));
  
  free(marshalled_ticket);
  
  return rb_marshalled_ticket;
}

void rb_ticket_delete(Ticket* ticket)
{
  delete ticket;
}