#include "nymble_server_wrap.h"

VALUE rb_server_initialize(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist)
{
  Check_Type(rb_server_id, T_STRING);
  Check_Type(rb_hmac_key_ns, T_STRING);
  Check_Type(rb_blacklist, T_DATA);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Size(rb_hmac_key_ns, DIGEST_SIZE);

  u_char* server_id = (u_char*)RSTRING(rb_server_id)->ptr;
  u_char* hmac_key_ns = (u_char*)RSTRING(rb_hmac_key_ns)->ptr;
  blacklist_t* blacklist = (blacklist_t*)DATA_PTR(rb_blacklist);

  server_t* server = server_initialize(server_id, hmac_key_ns, blacklist);

  return Data_Wrap_Struct(rb_self, NULL, server_free, server);
}

VALUE rb_server_ticket_verify(VALUE rb_self, VALUE rb_server_state, VALUE rb_nymble_ticket, VALUE rb_link_window, VALUE rb_time_period)
{
  Check_Type(rb_server_state, T_DATA);
  Check_Type(rb_nymble_ticket, T_DATA);
  Check_Type(rb_link_window, T_FIXNUM);
  Check_Type(rb_time_period, T_FIXNUM);

  server_t* server = (server_t*)DATA_PTR(rb_server_state);
  ticket_t* nymble_ticket = (ticket_t*)DATA_PTR(rb_nymble_ticket);
  u_int link_window = NUM2UINT(rb_link_window);
  u_int time_period = NUM2UINT(rb_time_period);
  
  if (server_ticket_verify(server, nymble_ticket, link_window, time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_server_blacklist(VALUE rb_self, VALUE rb_server_state)
{
  Check_Type(rb_server_state, T_DATA);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);

  blacklist_t* blacklist = server_blacklist(server);
  
  return Data_Wrap_Struct(rb_self, NULL, NULL, blacklist);
}

VALUE rb_server_blacklist_finalize(VALUE rb_self, VALUE rb_server_state)
{
  Check_Type(rb_server_state, T_DATA);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);

  server_blacklist_finalize(server);

  return Qtrue;
}

VALUE rb_server_blacklist_finalized(VALUE rb_self, VALUE rb_server_state, VALUE rb_cur_time_period)
{
  Check_Type(rb_server_state, T_DATA);
  Check_Type(rb_cur_time_period, T_FIXNUM);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);
  u_int cur_time_period = NUM2UINT(rb_cur_time_period);
  
  if (server_blacklist_finalized(server, cur_time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_server_iterate(VALUE rb_self, VALUE rb_server_state, VALUE rb_time_period_delta)
{
  Check_Type(rb_server_state, T_DATA);
  Check_Type(rb_time_period_delta, T_FIXNUM);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);
  u_int time_period_delta = NUM2UINT(rb_time_period_delta);

  server_iterate(server, time_period_delta);

  return Qtrue;
}

VALUE rb_server_update_cert(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist_cert)
{
  Check_Type(rb_server_state, T_DATA);
  Check_Type(rb_blacklist_cert, T_DATA);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);
  blacklist_cert_t* cert = (blacklist_cert_t*)DATA_PTR(rb_blacklist_cert);
  
  server_update_cert(server, cert);
  
  return Qtrue;
}

VALUE rb_server_update(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist, VALUE rb_linking_tokens)
{
  Check_Type(rb_server_state, T_DATA);
  Check_Type(rb_blacklist, T_DATA);
  Check_Type(rb_linking_tokens, T_ARRAY);
  
  server_t* server = (server_t*)DATA_PTR(rb_server_state);
  blacklist_t* blacklist = (blacklist_t*)DATA_PTR(rb_blacklist);

  linking_token_t* linking_tokens = NULL;

  u_int linking_tokens_size = NUM2UINT(rb_funcall(rb_linking_tokens, rb_intern("size"), 0));
  u_int i;

  for (i = 0; i < linking_tokens_size; i++) {
    linking_token_t* linking_token = (linking_token_t*)DATA_PTR(rb_ary_entry(rb_linking_tokens, i));
    linking_token->next = linking_tokens;
    linking_tokens = linking_token;
  }
  
  server_update(server, blacklist, linking_tokens);

  return Qtrue;
}
