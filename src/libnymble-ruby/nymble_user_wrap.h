#ifndef __USER_WRAP_H__
#define __USER_WRAP_H__

#include "ruby.h"
#include "nymble_user.h"

VALUE rb_user_initialize(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np, VALUE rb_verify_key_n);
VALUE rb_user_pseudonym(VALUE rb_self, VALUE rb_user_state);
VALUE rb_user_pseudonym_mac(VALUE rb_self, VALUE rb_user_state);
VALUE rb_user_entry_initialize(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_credential);
VALUE rb_user_entry_exists(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id);
VALUE rb_user_blacklist_update(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_link_window, VALUE rb_time_period);
VALUE rb_user_blacklist_check(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id);
VALUE rb_user_credential_get(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_time_period);
VALUE rb_user_credential_unmarshall(VALUE rb_self, VALUE rb_credential_str);

#endif