#include "ruby.h"
#include "nymble_util_wrap.h"
#include "nymble_pseudonym_manager_wrap.h"
#include "nymble_sexp_wrap.h"
#include "nymble_manager_wrap.h"
#include "nymble_server_wrap.h"
#include "nymble_user_wrap.h"

void Init_nymble() {
  VALUE rb_cNymble = rb_define_class("Nymble", rb_cObject);
  
  rb_define_singleton_method(rb_cNymble, "digest", rb_nymble_hash, 1);
  rb_define_singleton_method(rb_cNymble, "random_bytes", rb_nymble_random_bytes, 1);
  rb_define_singleton_method(rb_cNymble, "blacklist_cert", rb_blacklist_cert, 1);
  
  rb_define_singleton_method(rb_cNymble, "pm_initialize", rb_pm_initialize, 1);
  rb_define_singleton_method(rb_cNymble, "pm_pseudonym_create", rb_pm_pseudonym_create, 3);

  rb_define_singleton_method(rb_cNymble, "nm_initialize", rb_nm_initialize, 1);
  rb_define_singleton_method(rb_cNymble, "nm_verify_key", rb_nm_verify_key, 1);
  rb_define_singleton_method(rb_cNymble, "nm_verify_private_key", rb_nm_verify_private_key, 1);
  rb_define_singleton_method(rb_cNymble, "nm_entry_exists", rb_nm_entry_exists, 2);
  rb_define_singleton_method(rb_cNymble, "nm_entry_add", rb_nm_entry_add, 2);
  rb_define_singleton_method(rb_cNymble, "nm_entry_update", rb_nm_entry_update, 3);
  rb_define_singleton_method(rb_cNymble, "nm_pseudonym_verify", rb_nm_pseudonym_verify, 4);
  rb_define_singleton_method(rb_cNymble, "nm_blacklist_create", rb_nm_blacklist_create, 4);
  rb_define_singleton_method(rb_cNymble, "nm_blacklist_verify", rb_nm_blacklist_verify, 4);
  rb_define_singleton_method(rb_cNymble, "nm_blacklist_cert_verify", rb_nm_blacklist_cert_verify, 4);
  rb_define_singleton_method(rb_cNymble, "nm_blacklist_update", rb_nm_blacklist_update, 5);
  rb_define_singleton_method(rb_cNymble, "nm_credential_create", rb_nm_credential_create, 4);
  rb_define_singleton_method(rb_cNymble, "nm_tokens_create", rb_nm_tokens_create, 6);
                                         
  rb_define_singleton_method(rb_cNymble, "server_initialize", rb_server_initialize, 3);
  rb_define_singleton_method(rb_cNymble, "server_ticket_verify", rb_server_ticket_verify, 4);
  rb_define_singleton_method(rb_cNymble, "server_blacklist", rb_server_blacklist, 1);
  rb_define_singleton_method(rb_cNymble, "server_blacklist_finalize", rb_server_blacklist_finalize, 1);
  rb_define_singleton_method(rb_cNymble, "server_blacklist_finalized", rb_server_blacklist_finalized, 2);
  rb_define_singleton_method(rb_cNymble, "server_iterate", rb_server_iterate, 2);
  rb_define_singleton_method(rb_cNymble, "server_update", rb_server_update, 3);
  rb_define_singleton_method(rb_cNymble, "server_update_cert", rb_server_update_cert, 2);
                                         
  rb_define_singleton_method(rb_cNymble, "user_initialize", rb_user_initialize, 3);
  rb_define_singleton_method(rb_cNymble, "user_pseudonym", rb_user_pseudonym, 1);
  rb_define_singleton_method(rb_cNymble, "user_pseudonym_mac", rb_user_pseudonym_mac, 1);
  rb_define_singleton_method(rb_cNymble, "user_entry_initialize", rb_user_entry_initialize, 3);
  rb_define_singleton_method(rb_cNymble, "user_entry_exists", rb_user_entry_exists, 2);
  rb_define_singleton_method(rb_cNymble, "user_blacklist_update", rb_user_blacklist_update, 5);
  rb_define_singleton_method(rb_cNymble, "user_blacklist_check", rb_user_blacklist_check, 2);
  rb_define_singleton_method(rb_cNymble, "user_credential_get", rb_user_credential_get, 3);
  
  rb_define_singleton_method(rb_cNymble, "pseudonym_marshall", rb_pseudonym_marshall, 2);
  rb_define_singleton_method(rb_cNymble, "pseudonym_unmarshall", rb_pseudonym_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "blacklist_cert_marshall", rb_blacklist_cert_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "blacklist_cert_unmarshall", rb_blacklist_cert_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "blacklist_marshall", rb_blacklist_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "blacklist_unmarshall", rb_blacklist_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "ticket_marshall", rb_ticket_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "ticket_unmarshall", rb_ticket_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "linking_token_marshall", rb_linking_token_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "linking_token_unmarshall", rb_linking_token_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "credential_marshall", rb_credential_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "credential_unmarshall", rb_credential_unmarshall, 1);
}
