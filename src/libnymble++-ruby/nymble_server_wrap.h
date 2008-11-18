#ifndef __NYMBLE_SERVER_WRAP_H__
#define __NYMBLE_SERVER_WRAP_H__

#include "nymble_wrap_util.h"

#include "nymble_server.h"

extern VALUE rb_cServer;

VALUE rb_server_new(VALUE rb_self);
VALUE rb_server_init(VALUE rb_self, VALUE rb_server_id);
VALUE rb_server_link_window(VALUE rb_self);
VALUE rb_server_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_server_time_period(VALUE rb_self);
VALUE rb_server_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_server_blacklist(VALUE rb_self);
VALUE rb_server_blacklist_cert(VALUE rb_self);
VALUE rb_server_valid_ticket(VALUE rb_self, VALUE rb_ticket_str);
VALUE rb_server_complain(VALUE rb_self, VALUE rb_ticket_str, VALUE rb_time_period);

void rb_server_delete(Nymble::Server* server);

#endif