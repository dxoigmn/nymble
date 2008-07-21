#ifndef __NYMBLE_CREDENTIAL_WRAP_H__
#define __NYMBLE_CREDENTIAL_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_credential.h"

extern VALUE rb_cCredential;

VALUE rb_credential_unmarshall(VALUE rb_self, VALUE rb_bytes);
VALUE rb_credential_marshall(VALUE rb_self);

void rb_credential_delete(Credential* credential);

#endif