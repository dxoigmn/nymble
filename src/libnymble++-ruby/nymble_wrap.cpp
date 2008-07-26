#include "nymble_wrap.h"

void Init_nymble() {
  rb_cNymble = rb_define_class("Nymble", rb_cObject);
  rb_define_alloc_func(rb_cNymble, rb_nymble_new);
  rb_define_const(rb_cNymble, "DIGEST_SIZE", INT2FIX(DIGEST_SIZE));
  rb_define_singleton_method(rb_cNymble, "digest", RUBY_METHOD_FUNC(rb_nymble_digest), 1);
  rb_define_singleton_method(rb_cNymble, "random_bytes", RUBY_METHOD_FUNC(rb_nymble_random_bytes), 1);
  rb_define_singleton_method(rb_cNymble, "hexencode", RUBY_METHOD_FUNC(rb_nymble_hexencode), 1);
  rb_define_singleton_method(rb_cNymble, "hexdecode", RUBY_METHOD_FUNC(rb_nymble_hexdecode), 1);
  rb_define_method(rb_cNymble, "link_window", RUBY_METHOD_FUNC(rb_nymble_link_window), 0);
  rb_define_method(rb_cNymble, "link_window=", RUBY_METHOD_FUNC(rb_nymble_link_window_set), 1);
  rb_define_method(rb_cNymble, "time_period", RUBY_METHOD_FUNC(rb_nymble_time_period), 0);
  rb_define_method(rb_cNymble, "time_period=", RUBY_METHOD_FUNC(rb_nymble_time_period_set), 1);
  
  rb_cPseudonym = rb_define_class_under(rb_cNymble, "Pseudonym", rb_cObject);
  rb_define_singleton_method(rb_cPseudonym, "unmarshall", RUBY_METHOD_FUNC(rb_pseudonym_unmarshall), 1);
  rb_define_method(rb_cPseudonym, "marshall", RUBY_METHOD_FUNC(rb_pseudonym_marshall), 0);
  
  rb_cBlacklist = rb_define_class_under(rb_cNymble, "Blacklist", rb_cObject);
  rb_define_singleton_method(rb_cBlacklist, "unmarshall", RUBY_METHOD_FUNC(rb_blacklist_unmarshall), 1);
  rb_define_method(rb_cBlacklist, "marshall", RUBY_METHOD_FUNC(rb_blacklist_marshall), 0);
  
  rb_cCredential = rb_define_class_under(rb_cNymble, "Credential", rb_cObject);
  rb_define_singleton_method(rb_cCredential, "unmarshall", RUBY_METHOD_FUNC(rb_credential_unmarshall), 1);
  rb_define_method(rb_cCredential, "marshall", RUBY_METHOD_FUNC(rb_credential_marshall), 0);
  
  rb_cTicket = rb_define_class_under(rb_cNymble, "Ticket", rb_cObject);
  rb_define_singleton_method(rb_cTicket, "unmarshall", RUBY_METHOD_FUNC(rb_ticket_unmarshall), 1);
  rb_define_method(rb_cTicket, "marshall", RUBY_METHOD_FUNC(rb_ticket_marshall), 0);
  
  rb_cToken = rb_define_class_under(rb_cNymble, "Token", rb_cObject);
  
  
  rb_cPseudonymManager = rb_define_class_under(rb_cNymble, "PseudonymManager", rb_cNymble);
  rb_define_alloc_func(rb_cPseudonymManager, rb_pm_new);
  rb_define_method(rb_cPseudonymManager, "initialize", RUBY_METHOD_FUNC(rb_pm_init), 1);
  rb_define_method(rb_cPseudonymManager, "create_pseudonym", RUBY_METHOD_FUNC(rb_pm_create_pseudonym), 1);
  
  rb_cNymbleManager = rb_define_class_under(rb_cNymble, "NymbleManager", rb_cNymble);
  rb_define_alloc_func(rb_cNymbleManager, rb_nm_new);
  rb_define_method(rb_cNymbleManager, "initialize", RUBY_METHOD_FUNC(rb_nm_init), 1);
  rb_define_method(rb_cNymbleManager, "verify_key", RUBY_METHOD_FUNC(rb_nm_verify_key), 0);
  rb_define_method(rb_cNymbleManager, "add_server", RUBY_METHOD_FUNC(rb_nm_add_server), 1);
  rb_define_method(rb_cNymbleManager, "valid_pseudonym?", RUBY_METHOD_FUNC(rb_nm_verify_pseudonym), 1);
  rb_define_method(rb_cNymbleManager, "valid_blacklist?", RUBY_METHOD_FUNC(rb_nm_verify_blacklist), 2);
  rb_define_method(rb_cNymbleManager, "create_blacklist", RUBY_METHOD_FUNC(rb_nm_create_blacklist), 1);
  rb_define_method(rb_cNymbleManager, "update_blacklist", RUBY_METHOD_FUNC(rb_nm_update_blacklist), 3);
  rb_define_method(rb_cNymbleManager, "create_tokens", RUBY_METHOD_FUNC(rb_nm_create_tokens), 3);
  rb_define_method(rb_cNymbleManager, "create_credential", RUBY_METHOD_FUNC(rb_nm_create_credential), 3);
  
  rb_cServer = rb_define_class_under(rb_cNymble, "Server", rb_cNymble);
  rb_define_alloc_func(rb_cServer, rb_server_new);
  rb_define_method(rb_cServer, "initialize", RUBY_METHOD_FUNC(rb_server_init), 3);
  rb_define_method(rb_cServer, "blacklist", RUBY_METHOD_FUNC(rb_server_blacklist), 0);
  rb_define_method(rb_cServer, "valid_ticket?", RUBY_METHOD_FUNC(rb_server_verify_ticket), 1);
  
  rb_cUser = rb_define_class_under(rb_cNymble, "User", rb_cNymble);
  rb_define_alloc_func(rb_cUser, rb_user_new);
  rb_define_method(rb_cUser, "initialize", RUBY_METHOD_FUNC(rb_user_init), 2);
  rb_define_method(rb_cUser, "pseudonym", RUBY_METHOD_FUNC(rb_user_pseudonym), 0);
  rb_define_method(rb_cUser, "add_blacklist", RUBY_METHOD_FUNC(rb_user_add_blacklist), 1);
  rb_define_method(rb_cUser, "add_credential", RUBY_METHOD_FUNC(rb_user_add_credential), 1);
  rb_define_method(rb_cUser, "ticket", RUBY_METHOD_FUNC(rb_user_ticket), 1);
}

VALUE rb_nymble_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_nymble_delete, new Nymble());
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

VALUE rb_nymble_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  
  Nymble* nymble = (Nymble*) DATA_PTR(rb_self);
  
  return INT2FIX(nymble->getLinkWindow());
}

VALUE rb_nymble_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Nymble* nymble = (Nymble*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  nymble->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_nymble_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  
  Nymble* nymble = (Nymble*) DATA_PTR(rb_self);
  
  return INT2FIX(nymble->getTimePeriod());
}

VALUE rb_nymble_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble* nymble = (Nymble*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  nymble->setTimePeriod(time_period);
  
  return rb_self;
}

void rb_nymble_delete(Nymble* nymble)
{
  delete nymble;
}