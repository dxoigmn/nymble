#ifndef __NYMBLE_WRAP_H__
#define __NYMBLE_WRAP_H__

#include "nymble_wrap_util.h"
#include "nymble_pseudonym_manager_wrap.h"
#include "nymble_manager_wrap.h"
#include "nymble_user_wrap.h"
#include "nymble_server_wrap.h"

#include "nymble.h"

extern "C" {
  void Init_nymble();
}

VALUE rb_mNymble;
VALUE rb_nymble_digest(VALUE rb_self, VALUE rb_value);
VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count);
VALUE rb_nymble_hexencode(VALUE rb_self, VALUE rb_string);
VALUE rb_nymble_hexdecode(VALUE rb_self, VALUE rb_string);

#endif