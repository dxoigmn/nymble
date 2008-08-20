#include "nymble_wrap.h"

void Init_nymble() {
  rb_mNymble = rb_define_module("Nymble");
  rb_define_const(rb_mNymble, "DIGEST_SIZE", INT2FIX(DIGEST_SIZE));
  rb_define_singleton_method(rb_mNymble, "digest", RUBY_METHOD_FUNC(rb_nymble_digest), 1);
  rb_define_singleton_method(rb_mNymble, "random_bytes", RUBY_METHOD_FUNC(rb_nymble_random_bytes), 1);
  rb_define_singleton_method(rb_mNymble, "hexencode", RUBY_METHOD_FUNC(rb_nymble_hexencode), 1);
  rb_define_singleton_method(rb_mNymble, "hexdecode", RUBY_METHOD_FUNC(rb_nymble_hexdecode), 1);
  
  rb_cPseudonym = rb_define_class_under(rb_mNymble, "Pseudonym", rb_cObject);
  rb_define_singleton_method(rb_cPseudonym, "unmarshal", RUBY_METHOD_FUNC(rb_pseudonym_unmarshal), 1);
  rb_define_method(rb_cPseudonym, "marshal", RUBY_METHOD_FUNC(rb_pseudonym_marshal), 0);
  
  rb_cBlacklist = rb_define_class_under(rb_mNymble, "Blacklist", rb_cObject);
  rb_define_singleton_method(rb_cBlacklist, "unmarshal", RUBY_METHOD_FUNC(rb_blacklist_unmarshal), 1);
  rb_define_method(rb_cBlacklist, "marshal", RUBY_METHOD_FUNC(rb_blacklist_marshal), 0);
  
  rb_cCredential = rb_define_class_under(rb_mNymble, "Credential", rb_cObject);
  rb_define_singleton_method(rb_cCredential, "unmarshal", RUBY_METHOD_FUNC(rb_credential_unmarshal), 1);
  rb_define_method(rb_cCredential, "marshal", RUBY_METHOD_FUNC(rb_credential_marshal), 0);
  
  rb_cTicket = rb_define_class_under(rb_mNymble, "Ticket", rb_cObject);
  rb_define_method(rb_cTicket, "complain", RUBY_METHOD_FUNC(rb_ticket_complain), 1);
  rb_define_singleton_method(rb_cTicket, "unmarshal", RUBY_METHOD_FUNC(rb_ticket_unmarshal), 1);
  rb_define_method(rb_cTicket, "marshal", RUBY_METHOD_FUNC(rb_ticket_marshal), 0);
  
  rb_cComplaint = rb_define_class_under(rb_mNymble, "Complaint", rb_cObject);
  rb_define_singleton_method(rb_cComplaint, "unmarshal", RUBY_METHOD_FUNC(rb_complaint_unmarshal), 1);
  rb_define_method(rb_cComplaint, "marshal", RUBY_METHOD_FUNC(rb_complaint_marshal), 0);
  
  rb_cLinkingToken = rb_define_class_under(rb_mNymble, "LinkingToken", rb_cObject);
  rb_define_singleton_method(rb_cLinkingToken, "unmarshal", RUBY_METHOD_FUNC(rb_linking_token_unmarshal), 1);
  rb_define_method(rb_cLinkingToken, "marshal", RUBY_METHOD_FUNC(rb_linking_token_marshal), 0);
  
  rb_cPseudonymManager = rb_define_class_under(rb_mNymble, "PseudonymManager", rb_cObject);
  rb_define_alloc_func(rb_cPseudonymManager, rb_pm_new);
  rb_define_method(rb_cPseudonymManager, "initialize", RUBY_METHOD_FUNC(rb_pm_init), 1);
  rb_define_method(rb_cPseudonymManager, "link_window", RUBY_METHOD_FUNC(rb_pm_link_window), 0);
  rb_define_method(rb_cPseudonymManager, "link_window=", RUBY_METHOD_FUNC(rb_pm_link_window_set), 1);
  rb_define_method(rb_cPseudonymManager, "time_period", RUBY_METHOD_FUNC(rb_pm_time_period), 0);
  rb_define_method(rb_cPseudonymManager, "time_period=", RUBY_METHOD_FUNC(rb_pm_time_period_set), 1);
  rb_define_method(rb_cPseudonymManager, "create_pseudonym", RUBY_METHOD_FUNC(rb_pm_create_pseudonym), 1);
  
  rb_cNymbleManager = rb_define_class_under(rb_mNymble, "NymbleManager", rb_cObject);
  rb_define_alloc_func(rb_cNymbleManager, rb_nm_new);
  rb_define_method(rb_cNymbleManager, "initialize", RUBY_METHOD_FUNC(rb_nm_init), 2);
  rb_define_method(rb_cNymbleManager, "link_window", RUBY_METHOD_FUNC(rb_nm_link_window), 0);
  rb_define_method(rb_cNymbleManager, "link_window=", RUBY_METHOD_FUNC(rb_nm_link_window_set), 1);
  rb_define_method(rb_cNymbleManager, "time_period", RUBY_METHOD_FUNC(rb_nm_time_period), 0);
  rb_define_method(rb_cNymbleManager, "time_period=", RUBY_METHOD_FUNC(rb_nm_time_period_set), 1);
  rb_define_method(rb_cNymbleManager, "add_server", RUBY_METHOD_FUNC(rb_nm_add_server), 1);
  rb_define_method(rb_cNymbleManager, "valid_pseudonym?", RUBY_METHOD_FUNC(rb_nm_verify_pseudonym), 1);
  rb_define_method(rb_cNymbleManager, "valid_blacklist?", RUBY_METHOD_FUNC(rb_nm_verify_blacklist), 2);
  rb_define_method(rb_cNymbleManager, "create_blacklist", RUBY_METHOD_FUNC(rb_nm_create_blacklist), 1);
  rb_define_method(rb_cNymbleManager, "update_blacklist", RUBY_METHOD_FUNC(rb_nm_update_blacklist), 3);
  rb_define_method(rb_cNymbleManager, "create_linking_tokens", RUBY_METHOD_FUNC(rb_nm_create_linking_tokens), 3);
  rb_define_method(rb_cNymbleManager, "create_credential", RUBY_METHOD_FUNC(rb_nm_create_credential), 3);
  
  rb_cServer = rb_define_class_under(rb_mNymble, "Server", rb_cObject);
  rb_define_alloc_func(rb_cServer, rb_server_new);
  rb_define_method(rb_cServer, "initialize", RUBY_METHOD_FUNC(rb_server_init), 1);
  rb_define_method(rb_cServer, "link_window", RUBY_METHOD_FUNC(rb_server_link_window), 0);
  rb_define_method(rb_cServer, "link_window=", RUBY_METHOD_FUNC(rb_server_link_window_set), 1);
  rb_define_method(rb_cServer, "time_period", RUBY_METHOD_FUNC(rb_server_time_period), 0);
  rb_define_method(rb_cServer, "time_period=", RUBY_METHOD_FUNC(rb_server_time_period_set), 1);
  rb_define_method(rb_cServer, "server_id", RUBY_METHOD_FUNC(rb_server_server_id), 0);
  rb_define_method(rb_cServer, "finalized?", RUBY_METHOD_FUNC(rb_server_finalized), 0);
  rb_define_method(rb_cServer, "hmac_key_ns=", RUBY_METHOD_FUNC(rb_server_hmac_key_ns_set), 1);
  rb_define_method(rb_cServer, "blacklist", RUBY_METHOD_FUNC(rb_server_blacklist), 0);
  rb_define_method(rb_cServer, "blacklist=", RUBY_METHOD_FUNC(rb_server_blacklist_set), 1);
  rb_define_method(rb_cServer, "add_linking_tokens", RUBY_METHOD_FUNC(rb_server_add_linking_tokens), 1);
  rb_define_method(rb_cServer, "valid_ticket?", RUBY_METHOD_FUNC(rb_server_verify_ticket), 1);
  
  rb_cUser = rb_define_class_under(rb_mNymble, "User", rb_cObject);
  rb_define_alloc_func(rb_cUser, rb_user_new);
  rb_define_method(rb_cUser, "initialize", RUBY_METHOD_FUNC(rb_user_init), 2);
  rb_define_method(rb_cUser, "link_window", RUBY_METHOD_FUNC(rb_user_link_window), 0);
  rb_define_method(rb_cUser, "link_window=", RUBY_METHOD_FUNC(rb_user_link_window_set), 1);
  rb_define_method(rb_cUser, "time_period", RUBY_METHOD_FUNC(rb_user_time_period), 0);
  rb_define_method(rb_cUser, "time_period=", RUBY_METHOD_FUNC(rb_user_time_period_set), 1);
  rb_define_method(rb_cUser, "pseudonym", RUBY_METHOD_FUNC(rb_user_pseudonym), 0);
  rb_define_method(rb_cUser, "find_or_create_entry", RUBY_METHOD_FUNC(rb_user_find_or_create_entry), 1);
  
  rb_cUserEntry = rb_define_class_under(rb_mNymble, "UserEntry", rb_cObject);
  rb_define_method(rb_cUserEntry, "blacklist=", RUBY_METHOD_FUNC(rb_user_entry_blacklist_set), 1);
  rb_define_method(rb_cUserEntry, "credential=", RUBY_METHOD_FUNC(rb_user_entry_credential_set), 1);
  rb_define_method(rb_cUserEntry, "blacklisted?", RUBY_METHOD_FUNC(rb_user_entry_is_blacklisted), 0);
  rb_define_method(rb_cUserEntry, "ticket", RUBY_METHOD_FUNC(rb_user_entry_ticket), 1);
  
}

VALUE rb_nymble_digest(VALUE rb_self, VALUE rb_value)
{
  rb_value      = rb_funcall(rb_value, rb_intern("to_s"), 0);
  u_char* value = (u_char*) RSTRING_PTR(rb_value);
  u_int size    = RSTRING_LEN(rb_value);
  u_char out[DIGEST_SIZE];
  
  Nymble::digest(value, size, out);
  
  return rb_str_new((char*) out, DIGEST_SIZE);
}

VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count)
{
  Check_Type(rb_count, T_FIXNUM);
  
  u_int size = NUM2UINT(rb_count);
  u_char out[size];
  
  Nymble::random_bytes(size, out);
  
  return rb_str_new((char*) out, size);
}

VALUE rb_nymble_hexencode(VALUE rb_self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);
  
  u_char* in = (u_char*) RSTRING_PTR(rb_string);
  u_int len = RSTRING_LEN(rb_string);
  char out[Nymble::hexencode(in, len) + 1];
  
  Nymble::hexencode(in, len, out);
  
  return rb_str_new2(out);
}

VALUE rb_nymble_hexdecode(VALUE rb_self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);
  
  char* in = RSTRING_PTR(rb_string);
  u_int out_len = Nymble::hexdecode(in);
  u_char out[out_len];
  
  Nymble::hexdecode(in, out);
  
  return rb_str_new((char*) out, out_len);
}