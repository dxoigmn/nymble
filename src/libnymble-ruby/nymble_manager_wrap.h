#ifndef __NYMBLE_MANAGER_WRAP_H__
#define __NYMBLE_MANAGER_WRAP_H__

#include "ruby.h"
#include "nymble_manager.h"

VALUE rb_nm_initialize(VALUE rb_self, VALUE rb_hmac_key_np);
VALUE rb_nm_verify_key(VALUE rb_self, VALUE rb_nm_state);
VALUE rb_nm_verify_private_key(VALUE rb_self, VALUE rb_nm_state);
VALUE rb_nm_entry_exists(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id); 
VALUE rb_nm_entry_add(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id);
VALUE rb_nm_entry_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period);
VALUE rb_nm_pseudonym_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_link_window, VALUE rb_mac_np);
VALUE rb_nm_blacklist_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period, VALUE rb_link_window);
VALUE rb_nm_blacklist_cert_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist_cert, VALUE rb_server_id, VALUE rb_link_window);
VALUE rb_nm_blacklist_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_server_id, VALUE rb_link_window);
VALUE rb_nm_blacklist_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window);
VALUE rb_nm_credential_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_server_id, VALUE rb_link_window);
VALUE rb_nm_tokens_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window);

#endif
