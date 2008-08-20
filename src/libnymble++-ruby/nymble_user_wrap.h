#ifndef __NYMBLE_USER_WRAP_H__
#define __NYMBLE_USER_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_user_entry_wrap.h"
#include "nymble_pseudonym_wrap.h"
#include "nymble_user.h"

extern VALUE rb_cUser;

VALUE rb_user_new(VALUE rb_self);
VALUE rb_user_init(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_verify_key_path);
VALUE rb_user_link_window(VALUE rb_self);
VALUE rb_user_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_user_time_period(VALUE rb_self);
VALUE rb_user_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_user_pseudonym(VALUE rb_self);
VALUE rb_user_find_or_create_entry(VALUE rb_self, VALUE rb_server_id);

void rb_user_delete(Nymble::User* user);

#endif