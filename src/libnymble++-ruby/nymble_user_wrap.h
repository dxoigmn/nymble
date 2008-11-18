#ifndef __NYMBLE_USER_WRAP_H__
#define __NYMBLE_USER_WRAP_H__

#include "nymble_wrap_util.h"

#include "nymble_user.h"

extern VALUE rb_cUser;

VALUE rb_user_new(VALUE rb_self);
VALUE rb_user_init(VALUE rb_self, VALUE rb_pseudonym_str, VALUE rb_verify_key_path);
VALUE rb_user_link_window(VALUE rb_self);
VALUE rb_user_link_window_set(VALUE rb_self, VALUE rb_link_window);
VALUE rb_user_time_period(VALUE rb_self);
VALUE rb_user_time_period_set(VALUE rb_self, VALUE rb_time_period);
VALUE rb_user_pseudonym(VALUE rb_self);
VALUE rb_user_add_credential(VALUE rb_self, VALUE rb_sid, VALUE rb_credential_str);
VALUE rb_user_ticket(VALUE rb_self, VALUE rb_sid);
VALUE rb_user_blacklisted(VALUE rb_self, VALUE rb_sid, VALUE rb_blacklist_str, VALUE rb_cert_str);

void rb_user_delete(Nymble::User* user);

#endif