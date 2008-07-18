#ifndef __SERVER_WRAP_H__
#define __SERVER_WRAP_H__

#include "ruby.h"
#include "nymble_server.h"

VALUE rb_server_initialize(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist);
VALUE rb_server_ticket_verify(VALUE rb_self, VALUE rb_server_state, VALUE rb_nymble_ticket, VALUE rb_link_window, VALUE rb_time_period);
VALUE rb_server_blacklist(VALUE rb_self, VALUE rb_server_state);
VALUE rb_server_blacklist_finalize(VALUE rb_self, VALUE rb_server_state);
VALUE rb_server_blacklist_finalized(VALUE rb_self, VALUE rb_server_state, VALUE rb_cur_time_period);
VALUE rb_server_iterate(VALUE rb_self, VALUE rb_server_state, VALUE rb_time_period_delta);
VALUE rb_server_update_cert(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist_cert);
VALUE rb_server_update(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist, VALUE rb_linking_tokens);

#endif