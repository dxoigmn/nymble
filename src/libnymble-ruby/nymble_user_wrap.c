#include "nymble_user_wrap.h"

VALUE rb_user_initialize(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np, VALUE rb_verify_key_n)
{
  Check_Type(rb_pseudonym, T_STRING);
  Check_Type(rb_mac_np, T_STRING);
  Check_Type(rb_verify_key_n, T_STRING);
  Check_Size(rb_pseudonym, DIGEST_SIZE);
  Check_Size(rb_mac_np, DIGEST_SIZE);
  
  pseudonym_t pseudonym;
  memcpy(pseudonym.pseudonym, RSTRING(rb_pseudonym)->ptr, DIGEST_SIZE);
  memcpy(pseudonym.mac_np, RSTRING(rb_mac_np)->ptr, DIGEST_SIZE);

  const u_char *ptr = (const u_char *)RSTRING(rb_verify_key_n)->ptr;

  RSA *rsa = d2i_RSAPublicKey(NULL, &ptr, RSTRING(rb_verify_key_n)->len);
  

  user_t *user = user_initialize(&pseudonym, rsa);

  return Data_Wrap_Struct(rb_self, NULL, user_free, user);
}

VALUE rb_user_pseudonym(VALUE rb_self, VALUE rb_user_state)
{
  Check_Type(rb_user_state, T_DATA);
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  
  pseudonym_t *pseudonym = user_pseudonym(user);

  return rb_str_new((char *)pseudonym->pseudonym, DIGEST_SIZE);
}

VALUE rb_user_pseudonym_mac(VALUE rb_self, VALUE rb_user_state)
{
  Check_Type(rb_user_state, T_DATA);
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  
  pseudonym_t *pseudonym = user_pseudonym(user);

  return rb_str_new((char *)pseudonym->mac_np, DIGEST_SIZE);
}

VALUE rb_user_entry_initialize(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_credential)
{
  Check_Type(rb_user_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_credential, T_DATA);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
  credential_t *credential; Data_Get_Struct(rb_credential, credential_t, credential);
  u_int L = NUM2UINT(rb_gv_get("L"));
  
  user_entry_initialize(user, server_id, credential, L);

  return Qtrue;
}

VALUE rb_user_entry_exists(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id)
{
  Check_Type(rb_user_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  if (user_entry_exists(user, server_id)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_user_blacklist_update(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_link_window, VALUE rb_time_period)
{
  Check_Type(rb_user_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_blacklist, T_DATA);
  Check_Type(rb_link_window, T_FIXNUM);
  Check_Type(rb_time_period, T_FIXNUM);
  Check_Size(rb_server_id, DIGEST_SIZE);

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  u_int link_window = NUM2UINT(rb_link_window);
  u_int time_period = NUM2UINT(rb_time_period);

  if (user_blacklist_update(user, server_id, blacklist, link_window, time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_user_blacklist_check(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id)
{
  Check_Type(rb_user_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  if (user_blacklist_check(user, server_id)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_user_credential_get(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_time_period)
{
  Check_Type(rb_user_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_time_period, T_FIXNUM);
  Check_Size(rb_server_id, DIGEST_SIZE);

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int time_period = NUM2UINT(rb_time_period);

  ticket_t *ticket = user_credential_get(user, server_id, time_period);
  
  if (ticket) {
    return Data_Wrap_Struct(rb_self, NULL, NULL, ticket);
  } else {
    return Qnil;
  }
}
