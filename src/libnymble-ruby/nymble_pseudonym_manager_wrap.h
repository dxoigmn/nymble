#ifndef __PSEUDONYM_MANAGER_WRAP_H__
#define __PSEUDONYM_MANAGER_WRAP_H__

#include "ruby.h"
#include "nymble_pseudonym_manager.h"

VALUE rb_pm_initialize(VALUE rb_self, VALUE rb_hmac_key_np);
VALUE rb_pm_pseudonym_create(VALUE rb_self, VALUE rb_pm_state, VALUE rb_user_id, VALUE rb_link_window);

#endif