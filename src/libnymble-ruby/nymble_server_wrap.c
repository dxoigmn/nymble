#include "nymble_server_wrap.h"

VALUE rb_server_initialize(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist) { 
  if ((TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_hmac_key_ns) != T_STRING) ||
      (RSTRING(rb_hmac_key_ns)->len != DIGEST_SIZE) ||
      (TYPE(rb_blacklist) != T_DATA)) {
    return Qnil;
  }

  u_char  *server_id    = (u_char *)RSTRING(rb_server_id)->ptr;
  u_char  *hmac_key_ns  = (u_char *)RSTRING(rb_hmac_key_ns)->ptr;
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);

  server_t *server = server_initialize(server_id, hmac_key_ns, blacklist);

  return Data_Wrap_Struct(rb_self, NULL, server_free, server);
}

VALUE rb_server_ticket_verify(VALUE rb_self, VALUE rb_server_state, VALUE rb_nymble_ticket, VALUE rb_link_window, VALUE rb_time_period) { 
  if ((TYPE(rb_server_state) != T_DATA) ||
      (TYPE(rb_nymble_ticket) != T_DATA) ||
      (TYPE(rb_link_window) != T_FIXNUM) ||
      (TYPE(rb_time_period) != T_FIXNUM)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  ticket_t *nymble_ticket; Data_Get_Struct(rb_nymble_ticket, ticket_t, nymble_ticket);
  u_int   link_window = NUM2UINT(rb_link_window);
  u_int   time_period = NUM2UINT(rb_time_period);
  
  if (server_ticket_verify(server, nymble_ticket, link_window, time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_server_blacklist(VALUE rb_self, VALUE rb_server_state) { 
  if ((TYPE(rb_server_state) != T_DATA)) {
    return Qnil;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);

  blacklist_t *blacklist = server_blacklist(server);
  return Data_Wrap_Struct(rb_self, NULL, NULL, blacklist);
}

VALUE rb_server_blacklist_finalize(VALUE rb_self, VALUE rb_server_state) { 
  if ((TYPE(rb_server_state) != T_DATA)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);

  server_blacklist_finalize(server);

  return Qtrue;
}

VALUE rb_server_blacklist_finalized(VALUE rb_self, VALUE rb_server_state, VALUE rb_cur_time_period) { 
  if ((TYPE(rb_server_state) != T_DATA) ||
      (TYPE(rb_cur_time_period) != T_FIXNUM)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  u_int cur_time_period = NUM2UINT(rb_cur_time_period);
  
  if (server_blacklist_finalized(server, cur_time_period)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE rb_server_iterate(VALUE rb_self, VALUE rb_server_state, VALUE rb_time_period_delta) { 
  if ((TYPE(rb_server_state) != T_DATA) ||
      (TYPE(rb_time_period_delta) != T_FIXNUM)) {
    return Qfalse;
  }
  
  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  u_int time_period_delta = NUM2UINT(rb_time_period_delta);

  server_iterate(server, time_period_delta);

  return Qtrue;
}

VALUE rb_server_update_cert(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist_cert) {
  if (TYPE(rb_server_state) != T_DATA || TYPE(rb_blacklist_cert) != T_DATA) {
    return Qnil;
  }
  
  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  blacklist_cert_t *blacklist_cert; Data_Get_Struct(rb_blacklist_cert, blacklist_cert_t, blacklist_cert);
  
  server_update_cert(server, blacklist_cert);
  
  return Qtrue;
}

VALUE rb_server_update(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist, VALUE rb_linking_tokens) { 
  if ((TYPE(rb_server_state) != T_DATA) ||
      (TYPE(rb_blacklist) != T_DATA) ||
      (rb_linking_tokens != Qnil && TYPE(rb_linking_tokens) != T_ARRAY)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);

  linking_token_t *linking_tokens = NULL;

  if (rb_linking_tokens != Qnil) {
    u_int linking_tokens_size       = NUM2UINT(rb_funcall(rb_linking_tokens, rb_intern("size"), 0));
    u_int i;
  
    for (i = 0; i < linking_tokens_size; i++) {
      linking_token_t *linking_token; Data_Get_Struct(rb_ary_entry(rb_linking_tokens, i), linking_token_t, linking_token);
      linking_token->next = linking_tokens;
      linking_tokens      = linking_token;
    }
  }
  
  server_update(server, blacklist, linking_tokens);

  return Qtrue;
}
