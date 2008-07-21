#ifndef __NYMBLE_PSEUDONYM_WRAP_H__
#define __NYMBLE_PSEUDONYM_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_pseudonym.h"

extern VALUE rb_cPseudonym;

VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_bytes);
VALUE rb_pseudonym_marshall(VALUE rb_self);

void rb_pseudonym_delete(Pseudonym* pseudonym);

#endif