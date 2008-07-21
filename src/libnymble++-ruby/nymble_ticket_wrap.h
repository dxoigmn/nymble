#ifndef __NYMBLE_TICKET_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_ticket.h"

extern VALUE rb_cTicket;

VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_bytes);
VALUE rb_ticket_marshall(VALUE rb_self);

void rb_ticket_delete(Ticket* ticket);

#endif