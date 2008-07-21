#ifndef __NYMBLE_TOKEN_WRAP_H__
#define __NYMBLE_TOKEN_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_token.h"

extern VALUE rb_cToken;

void rb_token_delete(Token* token);

#endif