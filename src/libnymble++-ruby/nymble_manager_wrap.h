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
VALUE rb_nm_valid_pseudonym(VALUE rb_self, VALUE rb_pseudonym_str);

void rb_nm_delete(Nymble::NymbleManager* nm);

#endif