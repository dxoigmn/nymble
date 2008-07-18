#ifndef __UTIL_WRAP_H__
#define __UTIL_WRAP_H__

#include "ruby.h"
#include "nymble_util.h"

VALUE rb_nymble_hash(VALUE rb_self, VALUE rb_value);
VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count);

#endif