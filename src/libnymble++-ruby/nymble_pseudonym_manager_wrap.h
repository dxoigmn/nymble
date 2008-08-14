#ifndef __NYMBLE_PSEUDONYM_MANAGER_WRAP_H__
#define __NYMBLE_PSEUDONYM_MANAGER_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_pseudonym_manager.h"
#include "nymble_pseudonym_wrap.h"

extern VALUE rb_cPseudonymManager;

VALUE rb_pm_new(VALUE rb_self);
VALUE rb_pm_init(VALUE rb_self, VALUE rb_hmac_key_np);
VALUE rb_pm_link_window(VALUE rb_self);
VALUE rb_pm_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_pm_time_period(VALUE rb_self);
VALUE rb_pm_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_pm_create_pseudonym(VALUE rb_self, VALUE rb_user_id);

void rb_pm_delete(Nymble::PseudonymManager* pm);

#endif