#include "nymble_manager_wrap.h"

VALUE rb_nm_initialize(VALUE rb_self, VALUE rb_hmac_key_np)
{
  Check_Type(rb_hmac_key_np, T_STRING);
  
  u_char *hmac_key_np = (u_char *)RSTRING(rb_hmac_key_np)->ptr;

  nm_t *nm = nm_initialize(hmac_key_np);

  return Data_Wrap_Struct(rb_self, NULL, nm_free, nm);
}

VALUE rb_nm_verify_key(VALUE rb_self, VALUE rb_nm_state)
{
  Check_Type(rb_nm_state, T_DATA);
  
  nm_t  *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);

  RSA *rsa = nm_verify_key(nm);
  
  u_char *der = 0;
  
  u_int len = i2d_RSAPublicKey(rsa, &der);

  return rb_str_new((char *)der, len);
}

VALUE rb_nm_verify_private_key(VALUE rb_self, VALUE rb_nm_state)
{
  Check_Type(rb_nm_state, T_DATA);
  
  nm_t  *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);

  RSA *rsa = nm_verify_private_key(nm);
  
  u_char *der = 0;
  
  u_int len = i2d_RSAPrivateKey(rsa, &der);

  return rb_str_new((char *)der, len);
}

VALUE rb_nm_entry_exists(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qfalse;
  }
  
  nm_t   *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
    
  if (nm_entry_exists(nm, server_id) == 0) {
    return Qfalse;
  } else {
    return Qtrue;
  }
}

VALUE rb_nm_entry_add(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id)
{ 
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qnil;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  return rb_str_new((char *)nm_entry_add(nm, server_id), DIGEST_SIZE);
}

VALUE rb_nm_entry_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_time_period, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qfalse;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   time_period = NUM2UINT(rb_time_period);
  
  if (nm_entry_update(nm, server_id, time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_nm_pseudonym_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_link_window, VALUE rb_mac_np)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_pseudonym, T_STRING);
  Check_Type(rb_link_window, T_FIXNUM);
  Check_Type(rb_mac_np, T_STRING);
  
  if ((RSTRING(rb_pseudonym)->len != DIGEST_SIZE) ||
      (RSTRING(rb_mac_np)->len != DIGEST_SIZE)) {
    return Qfalse;
  }

  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *pseudo       = (u_char *)RSTRING(rb_pseudonym)->ptr;
  u_int   link_window   = NUM2UINT(rb_link_window);
  u_char  *mac_np       = (u_char *)RSTRING(rb_mac_np)->ptr;

  pseudonym_t pseudonym;
  
  memcpy(pseudonym.pseudonym, pseudo, DIGEST_SIZE);
  memcpy(pseudonym.mac_np, mac_np, DIGEST_SIZE);

  if (nm_pseudonym_verify(nm, &pseudonym, link_window)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_nm_blacklist_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_time_period, T_FIXNUM);
  Check_Type(rb_link_window, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qnil;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   time_period = NUM2UINT(rb_time_period);
  u_int   link_window = NUM2UINT(rb_link_window);
  u_int   L           = NUM2UINT(rb_gv_get("L"));
  
  if (time_period > L) {
    return Qnil;
  }

  blacklist_t *blacklist = nm_blacklist_create(nm, server_id, time_period, link_window);
  return Data_Wrap_Struct(rb_self, NULL, blacklist_free, blacklist);
}

VALUE rb_nm_blacklist_cert_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist_cert, VALUE rb_server_id, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_blacklist_cert, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_link_window, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qfalse;
  }

  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  blacklist_cert_t *blacklist_cert; Data_Get_Struct(rb_blacklist_cert, blacklist_cert_t, blacklist_cert);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   link_window = NUM2UINT(rb_link_window);

  if (nm_blacklist_cert_verify(nm, blacklist_cert, server_id, link_window)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_nm_blacklist_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_server_id, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_blacklist, T_DATA);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_link_window, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qfalse;
  }

  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   link_window = NUM2UINT(rb_link_window);

  if (nm_blacklist_verify(nm, blacklist, server_id, link_window)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_nm_blacklist_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_blacklist, T_DATA);
  Check_Type(rb_complaints, T_ARRAY);
  Check_Type(rb_time_period, T_FIXNUM);
  Check_Type(rb_link_window, T_FIXNUM);
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  u_int   time_period = NUM2UINT(rb_time_period);
  u_int   link_window = NUM2UINT(rb_link_window);
  
  ticket_t  *complaints = NULL;
  
  if (TYPE(rb_complaints) == T_ARRAY) {
    u_int complaints_size = NUM2UINT(rb_funcall(rb_complaints, rb_intern("size"), 0));
    u_int i;
  
    for (i = 0; i < complaints_size; i++) {
      VALUE rb_complaint = rb_ary_entry(rb_complaints, i);
      
      if (TYPE(rb_complaint) != T_DATA) {
        return Qnil;
      }

      ticket_t *ticket; Data_Get_Struct(rb_complaint, ticket_t, ticket);
      ticket->next  = complaints;
      complaints    = ticket;
    }
  }
  
  blacklist_t *new_blacklist = nm_blacklist_update(nm, blacklist, complaints, time_period, link_window);
  return Data_Wrap_Struct(rb_self, NULL, blacklist_free, new_blacklist);
}


VALUE rb_nm_credential_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_server_id, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_pseudonym, T_STRING);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_link_window, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qnil;
  }

  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *pseudo = (u_char *)RSTRING(rb_pseudonym)->ptr;
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   link_window = NUM2UINT(rb_link_window);
  u_int   L = NUM2UINT(rb_gv_get("L"));

  pseudonym_t pseudonym;
  memcpy(pseudonym.pseudonym, pseudo, DIGEST_SIZE);
  
  credential_t *credential = nm_credential_create(nm, &pseudonym, server_id, link_window, L);

  if (credential) {
    return Data_Wrap_Struct(rb_self, NULL, credential_free, credential);
  } else {
    return Qnil;
  }
}

VALUE rb_nm_credential_marshall(VALUE rb_self, VALUE rb_credential)
{
  Check_Type(rb_credential, T_DATA);
  
  credential_t *credential; Data_Get_Struct(rb_credential, credential_t, credential);
  
  sexpSimpleString *str = credential_to_str(credential, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_nm_tokens_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window)
{
  Check_Type(rb_nm_state, T_DATA);
  Check_Type(rb_blacklist, T_DATA);
  Check_Type(rb_complaints, T_ARRAY);
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_time_period, T_FIXNUM);
  Check_Type(rb_link_window, T_FIXNUM);
  
  if (RSTRING(rb_server_id)->len != DIGEST_SIZE) {
    return Qnil;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   link_window = NUM2UINT(rb_link_window);
  u_int   time_period = NUM2UINT(rb_time_period);

  ticket_t  *complaints = NULL;
  
  if (TYPE(rb_complaints) == T_ARRAY) {
    u_int complaints_size = NUM2UINT(rb_funcall(rb_complaints, rb_intern("size"), 0));
    u_int i;

    for (i = 0; i < complaints_size; i++) {
      VALUE rb_complaint = rb_ary_entry(rb_complaints, i);
      
      if (TYPE(rb_complaint) != T_DATA) {
        return Qnil;
      }
      
      ticket_t *ticket; Data_Get_Struct(rb_complaint, ticket_t, ticket);
      ticket->next  = complaints;
      complaints    = ticket;
    }
  }
  
  linking_token_t *linking_tokens = nm_tokens_create(nm, server_id, blacklist, complaints, time_period, link_window);
  linking_token_t *linking_token  = linking_tokens;
  
  VALUE rb_linking_tokens = rb_ary_new();

  while (linking_token) {
    rb_ary_push(rb_linking_tokens, Data_Wrap_Struct(rb_self, NULL, token_free, linking_token));

    linking_token = linking_token->next;
  }
  
  return rb_linking_tokens;
}
