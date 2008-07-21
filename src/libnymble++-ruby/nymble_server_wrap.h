#ifndef __NYMBLE_SERVER_WRAP_H__
#define __NYMBLE_SERVER_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_ticket_wrap.h"
#include "nymble_blacklist_wrap.h"
#include "nymble_server.h"

extern VALUE rb_cServer;

VALUE rb_server_new(VALUE rb_self);
VALUE rb_server_init(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist);
VALUE rb_server_server_id(VALUE rb_self);
VALUE rb_server_blacklist(VALUE rb_self);
VALUE rb_server_verify_ticket(VALUE rb_self, VALUE rb_ticket);

void rb_server_delete(Server* server);

#endif