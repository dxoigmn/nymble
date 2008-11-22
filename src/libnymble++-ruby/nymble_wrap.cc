#include "nymble_wrap.h"

void Init_nymble() {
  rb_mNymble = rb_define_module("Nymble");
  rb_define_const(rb_mNymble, "DIGEST_SIZE", INT2FIX(DIGEST_SIZE));
  rb_define_singleton_method(rb_mNymble, "digest", RUBY_METHOD_FUNC(rb_nymble_digest), 1);
  rb_define_singleton_method(rb_mNymble, "random_bytes", RUBY_METHOD_FUNC(rb_nymble_random_bytes), 1);
  rb_define_singleton_method(rb_mNymble, "hexencode", RUBY_METHOD_FUNC(rb_nymble_hexencode), 1);
  rb_define_singleton_method(rb_mNymble, "hexdecode", RUBY_METHOD_FUNC(rb_nymble_hexdecode), 1);

#ifdef __NYMBLE_PSEUDONYM_MANAGER_WRAP_H__
  rb_cPseudonymManager = rb_define_class_under(rb_mNymble, "PseudonymManager", rb_cObject);
  rb_define_alloc_func(rb_cPseudonymManager, rb_pm_new);
  rb_define_method(rb_cPseudonymManager, "initialize", RUBY_METHOD_FUNC(rb_pm_init), 1);
  rb_define_method(rb_cPseudonymManager, "link_window", RUBY_METHOD_FUNC(rb_pm_link_window), 0);
  rb_define_method(rb_cPseudonymManager, "link_window=", RUBY_METHOD_FUNC(rb_pm_link_window_set), 1);
  rb_define_method(rb_cPseudonymManager, "time_period", RUBY_METHOD_FUNC(rb_pm_time_period), 0);
  rb_define_method(rb_cPseudonymManager, "time_period=", RUBY_METHOD_FUNC(rb_pm_time_period_set), 1);
  rb_define_method(rb_cPseudonymManager, "create_pseudonym", RUBY_METHOD_FUNC(rb_pm_create_pseudonym), 1);
#endif


#ifdef __NYMBLE_MANAGER_WRAP_H__
  rb_cNymbleManager = rb_define_class_under(rb_mNymble, "NymbleManager", rb_cObject);
  rb_define_alloc_func(rb_cNymbleManager, rb_nm_new);
  rb_define_method(rb_cNymbleManager, "link_window", RUBY_METHOD_FUNC(rb_nm_link_window), 0);
  rb_define_method(rb_cNymbleManager, "link_window=", RUBY_METHOD_FUNC(rb_nm_link_window_set), 1);
  rb_define_method(rb_cNymbleManager, "time_period", RUBY_METHOD_FUNC(rb_nm_time_period), 0);
  rb_define_method(rb_cNymbleManager, "time_period=", RUBY_METHOD_FUNC(rb_nm_time_period_set), 1);
  rb_define_method(rb_cNymbleManager, "mac_key_np", RUBY_METHOD_FUNC(rb_nm_mac_key_np), 0);
  rb_define_method(rb_cNymbleManager, "write_verify_key_n", RUBY_METHOD_FUNC(rb_nm_write_verify_key_n), 1);
  rb_define_method(rb_cNymbleManager, "register_server", RUBY_METHOD_FUNC(rb_nm_register_server), 1);
  rb_define_method(rb_cNymbleManager, "create_credential", RUBY_METHOD_FUNC(rb_nm_create_credential), 2);
  rb_define_method(rb_cNymbleManager, "update_server", RUBY_METHOD_FUNC(rb_nm_update_server), 2);
  rb_define_method(rb_cNymbleManager, "reset!", RUBY_METHOD_FUNC(rb_nm_reset), -2);
#endif

#ifdef __NYMBLE_SERVER_WRAP_H__
  rb_cServer = rb_define_class_under(rb_mNymble, "Server", rb_cObject);
  rb_define_alloc_func(rb_cServer, rb_server_new);
  rb_define_method(rb_cServer, "initialize", RUBY_METHOD_FUNC(rb_server_init), 1);
  rb_define_method(rb_cServer, "link_window", RUBY_METHOD_FUNC(rb_server_link_window), 0);
  rb_define_method(rb_cServer, "link_window=", RUBY_METHOD_FUNC(rb_server_link_window_set), 1);
  rb_define_method(rb_cServer, "time_period", RUBY_METHOD_FUNC(rb_server_time_period), 0);
  rb_define_method(rb_cServer, "time_period=", RUBY_METHOD_FUNC(rb_server_time_period_set), 1);
  rb_define_method(rb_cServer, "blacklist", RUBY_METHOD_FUNC(rb_server_blacklist), 0);
  rb_define_method(rb_cServer, "cert", RUBY_METHOD_FUNC(rb_server_blacklist_cert), 0);
  rb_define_method(rb_cServer, "valid_ticket?", RUBY_METHOD_FUNC(rb_server_valid_ticket), 1);
  rb_define_method(rb_cServer, "add_complaint", RUBY_METHOD_FUNC(rb_server_add_complaint), 2);
  rb_define_method(rb_cServer, "complain!", RUBY_METHOD_FUNC(rb_server_complain), 0);
  rb_define_method(rb_cServer, "update!", RUBY_METHOD_FUNC(rb_server_update), 1);
#endif

#ifdef __NYMBLE_USER_WRAP_H__
  rb_cUser = rb_define_class_under(rb_mNymble, "User", rb_cObject);
  rb_define_alloc_func(rb_cUser, rb_user_new);
  rb_define_method(rb_cUser, "initialize", RUBY_METHOD_FUNC(rb_user_init), 2);
  rb_define_method(rb_cUser, "link_window", RUBY_METHOD_FUNC(rb_user_link_window), 0);
  rb_define_method(rb_cUser, "link_window=", RUBY_METHOD_FUNC(rb_user_link_window_set), 1);
  rb_define_method(rb_cUser, "time_period", RUBY_METHOD_FUNC(rb_user_time_period), 0);
  rb_define_method(rb_cUser, "time_period=", RUBY_METHOD_FUNC(rb_user_time_period_set), 1);
  rb_define_method(rb_cUser, "pseudonym", RUBY_METHOD_FUNC(rb_user_pseudonym), 0);
  rb_define_method(rb_cUser, "add_credential", RUBY_METHOD_FUNC(rb_user_add_credential), 2);
  rb_define_method(rb_cUser, "blacklisted?", RUBY_METHOD_FUNC(rb_user_blacklisted), 3);
  rb_define_method(rb_cUser, "ticket", RUBY_METHOD_FUNC(rb_user_ticket), 1);
#endif
}

VALUE rb_nymble_digest(VALUE rb_self, VALUE rb_value)
{
  rb_value = rb_funcall(rb_value, rb_intern("to_s"), 0);
  
  std::string in(RSTRING_PTR(rb_value), RSTRING_LEN(rb_value));
  std::string out;
  
  Nymble::digest(in, &out);
  
  return rb_str_new(out.c_str(), out.size());
}

VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count)
{
  Check_Type(rb_count, T_FIXNUM);
  
  size_t size = NUM2UINT(rb_count);
  std::string out;
  
  Nymble::random_bytes(size, &out);
  
  return rb_str_new(out.c_str(), out.size());
}

VALUE rb_nymble_hexencode(VALUE rb_self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);
  
  std::string in(RSTRING_PTR(rb_string), RSTRING_LEN(rb_string));
  std::string out;
  
  Nymble::hexencode(in, &out);
  
  return rb_str_new(out.c_str(), out.size());
}

VALUE rb_nymble_hexdecode(VALUE rb_self, VALUE rb_string)
{
  Check_Type(rb_string, T_STRING);
  
  std::string in(RSTRING_PTR(rb_string), RSTRING_LEN(rb_string));
  std::string out;
  
  Nymble::hexdecode(in, &out);
  
  return rb_str_new(out.c_str(), out.size());
}