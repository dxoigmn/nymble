#ifndef __NYMBLE_MANAGER_WRAP_H__
#define __NYMBLE_MANAGER_WRAP_H__

#include "ruby.h"
#include "nymble_wrap_util.h"
#include "nymble_blacklist_wrap.h"
#include "nymble_credential_wrap.h"
#include "nymble_token_wrap.h"
#include "nymble_pseudonym_wrap.h"
#include "nymble_ticket_wrap.h"
#include "nymble_manager.h"

extern VALUE rb_cNymbleManager;

VALUE rb_nm_new(VALUE rb_self);
VALUE rb_nm_init(VALUE rb_self, VALUE rb_hmac_key_np);
VALUE rb_nm_verify_key(VALUE rb_self);
VALUE rb_nm_add_server(VALUE rb_self, VALUE rb_server_id);
VALUE rb_nm_verify_pseudonym(VALUE rb_self, VALUE rb_pseudonym);
VALUE rb_nm_verify_blacklist(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist);
VALUE rb_nm_create_blacklist(VALUE rb_self, VALUE rb_server_id);
VALUE rb_nm_update_blacklist(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_tickets);
VALUE rb_nm_create_credential(VALUE rb_self, VALUE rb_server_id, VALUE rb_pseudonym, VALUE rb_time_periods);
VALUE rb_nm_create_tokens(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_tickets);

void rb_nm_delete(NymbleManager* nm);

#endif