#ifndef __NYMBLE_SERVER_WRAP_H__
#define __NYMBLE_SERVER_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_ticket_wrap.h"
#include "nymble_blacklist_wrap.h"
#include "nymble_server.h"
#include "nymble_token_wrap.h"

extern VALUE rb_cServer;

VALUE rb_server_new(VALUE rb_self);
VALUE rb_server_init(VALUE rb_self, VALUE rb_server_id);
VALUE rb_server_link_window(VALUE rb_self);
VALUE rb_server_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_server_time_period(VALUE rb_self);
VALUE rb_server_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_server_server_id(VALUE rb_self);
VALUE rb_server_finalized(VALUE rb_self);
VALUE rb_server_hmac_key_ns_set(VALUE rb_self, VALUE rb_hmac_key_ns);
VALUE rb_server_blacklist(VALUE rb_self);
VALUE rb_server_blacklist_set(VALUE rb_self, VALUE rb_blacklist);
VALUE rb_server_add_tokens(VALUE rb_self, VALUE rb_tokens);
VALUE rb_server_verify_ticket(VALUE rb_self, VALUE rb_ticket);

void rb_server_delete(Nymble::Server* server);

#endif