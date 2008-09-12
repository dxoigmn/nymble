#ifndef __NYMBLE_TOKEN_WRAP_H__
#define __NYMBLE_TOKEN_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_token.h"

extern VALUE rb_cToken;

VALUE rb_token_unmarshal(VALUE rb_self, VALUE rb_bytes);
VALUE rb_token_marshal(VALUE rb_self);

void rb_token_delete(Nymble::Token* token);

#endif