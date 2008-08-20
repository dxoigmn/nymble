#ifndef __NYMBLE_USER_ENTRY_WRAP_H__
#define __NYMBLE_USER_ENTRY_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_ticket_wrap.h"
#include "nymble_blacklist_wrap.h"
#include "nymble_credential_wrap.h"
#include "nymble_user_entry.h"

extern VALUE rb_cUserEntry;

VALUE rb_user_entry_is_blacklisted(VALUE rb_self);
VALUE rb_user_entry_ticket(VALUE rb_self, VALUE rb_time_period);
VALUE rb_user_entry_blacklist_set(VALUE rb_self, VALUE rb_blacklist);
VALUE rb_user_entry_credential_set(VALUE rb_self, VALUE rb_credential);

#endif