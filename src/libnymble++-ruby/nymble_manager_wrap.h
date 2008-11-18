#ifndef __NYMBLE_MANAGER_WRAP_H__
#define __NYMBLE_MANAGER_WRAP_H__

#include "nymble_wrap_util.h"

#include "nymble_manager.h"

extern VALUE rb_cNymbleManager;

VALUE rb_nm_new(VALUE rb_self);
VALUE rb_nm_link_window(VALUE rb_self);
VALUE rb_nm_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_nm_time_period(VALUE rb_self);
VALUE rb_nm_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_nm_mac_key_np(VALUE rb_self);
VALUE rb_nm_write_verify_key_n(VALUE rb_self, VALUE rb_path);
VALUE rb_nm_register_server(VALUE rb_self, VALUE rb_sid);
VALUE rb_nm_create_credential(VALUE rb_self, VALUE rb_sid, VALUE rb_pseudonym_str);
VALUE rb_nm_update_server(VALUE rb_self, VALUE rb_sid, VALUE rb_server_state_str);

void rb_nm_delete(Nymble::NymbleManager* nm);

#endif