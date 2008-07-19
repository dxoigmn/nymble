#ifndef __NYMBLE_SEXP_WRAP_H__
#define __NYMBLE_SEXP_WRAP_H__

#include "ruby.h"
#include "nymble_sexp.h"
#include "nymble_wrap_util.h"

VALUE rb_pseudonym_marshall(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np);
VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_pseudonym_str);
VALUE rb_blacklist_cert_marshall(VALUE rb_self, VALUE rb_blacklist_cert);
VALUE rb_blacklist_cert_unmarshall(VALUE rb_self, VALUE rb_blacklist_cert_str);
VALUE rb_blacklist_marshall(VALUE rb_self, VALUE rb_blacklist);
VALUE rb_blacklist_unmarshall(VALUE rb_self, VALUE rb_blacklist_str);
VALUE rb_ticket_marshall(VALUE rb_self, VALUE rb_ticket);
VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_ticket_str);
VALUE rb_linking_token_marshall(VALUE rb_self, VALUE rb_linking_token);
VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_linking_token_str);
VALUE rb_credential_marshall(VALUE rb_self, VALUE rb_credential);
VALUE rb_credential_unmarshall(VALUE rb_self, VALUE rb_credential_str);

#endif