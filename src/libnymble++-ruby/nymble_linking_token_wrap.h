#ifndef __NYMBLE_LINKING_TOKEN_WRAP_H__
#define __NYMBLE_LINKING_TOKEN_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_linking_token.h"

extern VALUE rb_cLinkingToken;

VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_bytes);
VALUE rb_linking_token_marshall(VALUE rb_self);

void rb_linking_token_delete(LinkingToken* linking_token);

#endif