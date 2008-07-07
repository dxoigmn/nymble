#include "ruby.h"
#include "nymble_manager.h"
#include "pseudonym_manager.h"
#include "server.h"
#include "user.h"

static VALUE rb_cNymble;

static VALUE rb_nymble_hash(VALUE rb_self, VALUE rb_value) {
  rb_value      = rb_funcall(rb_value, rb_intern("to_s"), 0);
  u_char *value = (u_char *)RSTRING(rb_value)->ptr;
  u_int size    = RSTRING(rb_value)->len;
  
  u_char buffer[DIGEST_SIZE];
  
  hash(buffer, value, size);

  return rb_str_new((char *)&buffer, sizeof(buffer));
}

static VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count) {
  if (TYPE(rb_count) != T_FIXNUM) {
    return Qnil;
  }

  u_int size = NUM2UINT(rb_count);
  u_char buffer[size];
  
  random_bytes(buffer, size);
  
  return rb_str_new((char *)buffer, sizeof(buffer));
}

static VALUE rb_pm_initialize(VALUE rb_self, VALUE rb_hmac_key_np) {
  if (TYPE(rb_hmac_key_np) != T_STRING) {
    return Qnil;
  }

  u_char *hmac_key_np = (u_char *)RSTRING(rb_hmac_key_np)->ptr;

  pm_t *pm = pm_initialize(hmac_key_np);

  //printf("Data_Wrap_Struct(pm_initialize = %x)\n", (u_int)pm);
  return Data_Wrap_Struct(rb_self, NULL, pm_free, pm);
}

static VALUE rb_pm_pseudonym_create(VALUE rb_self, VALUE rb_pm_state, VALUE rb_user_id, VALUE rb_link_window) {
  if ((TYPE(rb_pm_state) != T_DATA) ||
      (TYPE(rb_user_id) != T_STRING) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
    return Qnil;
  }
    
  pm_t    *pm; Data_Get_Struct(rb_pm_state, pm_t, pm);
  u_char  *user_id = (u_char *)RSTRING(rb_user_id)->ptr;
  u_int   link_window = NUM2UINT(rb_link_window);
  
  pseudonym_t *pseudonym = pm_pseudonym_create(pm, user_id, link_window);
  
  VALUE ret = rb_ary_new();
  
  rb_ary_push(ret, rb_str_new((char *)pseudonym, sizeof(pseudonym_t) / 2));
  rb_ary_push(ret, rb_str_new((char *)pseudonym + sizeof(pseudonym_t) / 2, sizeof(pseudonym_t) / 2));
  
  //printf("free(%x,pseudonym)\n", (u_int)pseudonym);
  free(pseudonym);
  
  return ret;
}

static VALUE rb_pseudonym_marshall(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np) {
  if (TYPE(rb_pseudonym) != T_STRING || TYPE(rb_mac_np) != T_STRING) {
    return Qnil;
  }
  
  pseudonym_t pseudonym;
  
  memcpy(pseudonym.pseudonym, RSTRING(rb_pseudonym)->ptr, DIGEST_SIZE);
  memcpy(pseudonym.mac_np, RSTRING(rb_mac_np)->ptr, DIGEST_SIZE);

  sexpSimpleString *str = pseudonym_to_str(&pseudonym, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_pseudonym_str) {
  if (TYPE(rb_pseudonym_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_pseudonym_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_pseudonym_str)->ptr;
  
  pseudonym_t *pseudonym = str_to_pseudonym(str);
  
  if (pseudonym) {    
    VALUE ret = rb_ary_new();

    rb_ary_push(ret, rb_str_new((char *)pseudonym, sizeof(pseudonym_t) / 2));
    rb_ary_push(ret, rb_str_new((char *)pseudonym + sizeof(pseudonym_t) / 2, sizeof(pseudonym_t) / 2));
    return ret;
  } else {
    return Qnil;
  }
}

static VALUE rb_nm_initialize(VALUE rb_self, VALUE rb_hmac_key_np) { 
  if (TYPE(rb_hmac_key_np) != T_STRING) {
    return Qnil;
  }
  
  u_char *hmac_key_np = (u_char *)RSTRING(rb_hmac_key_np)->ptr;

  nm_t *nm = nm_initialize(hmac_key_np);

  //printf("Data_Wrap_Struct(nm_initialize = %x)\n", (u_int)nm);
  return Data_Wrap_Struct(rb_self, NULL, nm_free, nm);
}

static VALUE rb_nm_verify_key(VALUE rb_self, VALUE rb_nm_state) {
  if (TYPE(rb_nm_state) != T_DATA) {
    return Qnil;
  }
  
  nm_t  *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);

  RSA *rsa = nm_verify_key(nm);
  
  u_char *der = 0;
  
  u_int len = i2d_RSAPublicKey(rsa, &der);

  //printf("Data_Wrap_Struct(nm_verify_key_n = %x)\n", (u_int)rsa);
  return rb_str_new((char *)der, len);
}

static VALUE rb_nm_verify_private_key(VALUE rb_self, VALUE rb_nm_state) {
  if (TYPE(rb_nm_state) != T_DATA) {
    return Qnil;
  }
  
  nm_t  *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);

  RSA *rsa = nm_verify_private_key(nm);
  
  u_char *der = 0;
  
  u_int len = i2d_RSAPrivateKey(rsa, &der);

  //printf("Data_Wrap_Struct(nm_verify_key_n = %x)\n", (u_int)rsa);
  return rb_str_new((char *)der, len);
}

static VALUE rb_nm_entry_exists(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE)) {
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

static VALUE rb_nm_entry_add(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE)) {
    return Qnil;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  return rb_str_new((char *)nm_entry_add(nm, server_id), DIGEST_SIZE);
}

static VALUE rb_nm_entry_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_time_period) != T_FIXNUM)) {
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

static VALUE rb_nm_pseudonym_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_link_window, VALUE rb_mac_np) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_pseudonym) != T_STRING) ||
      (RSTRING(rb_pseudonym)->len != DIGEST_SIZE) ||
      (TYPE(rb_link_window) != T_FIXNUM) ||
      (TYPE(rb_mac_np) != T_STRING) ||
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

static VALUE rb_nm_blacklist_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_time_period, VALUE rb_link_window) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_time_period) != T_FIXNUM) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
    return Qnil;
  }
  
  nm_t    *nm; Data_Get_Struct(rb_nm_state, nm_t, nm);
  u_char  *server_id  = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int   time_period = NUM2UINT(rb_time_period);
  u_int   link_window = NUM2UINT(rb_link_window);

  blacklist_t *blacklist = nm_blacklist_create(nm, server_id, time_period, link_window);
  //printf("Data_Wrap_Struct(nm_blacklist_create = %x)\n", (u_int)blacklist);
  return Data_Wrap_Struct(rb_self, NULL, blacklist_free, blacklist);
}

static VALUE rb_nm_blacklist_cert_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist_cert, VALUE rb_server_id, VALUE rb_link_window) {
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_blacklist_cert) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
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

static VALUE rb_nm_blacklist_verify(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_server_id, VALUE rb_link_window) {
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_blacklist) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
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

static VALUE rb_nm_blacklist_update(VALUE rb_self, VALUE rb_nm_state, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_blacklist) != T_DATA) ||
      ((TYPE(rb_complaints) != T_ARRAY) &&
       (TYPE(rb_complaints) != T_NIL)) ||
      (TYPE(rb_time_period) != T_FIXNUM) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
    return Qnil;
  }

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
  //printf("Data_Wrap_Struct(nm_blacklist_update = %x)\n", (u_int)new_blacklist);
  return Data_Wrap_Struct(rb_self, NULL, blacklist_free, new_blacklist);
}

static VALUE rb_blacklist_cert(VALUE rb_self, VALUE rb_blacklist) {
  if (TYPE(rb_blacklist) != T_DATA) {
    return Qnil;
  }
  
  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  
  return Data_Wrap_Struct(rb_self, NULL, NULL, &blacklist->cert);
}

static VALUE rb_blacklist_cert_marshall(VALUE rb_self, VALUE rb_blacklist_cert) {
  if (TYPE(rb_blacklist_cert) != T_DATA) {
    return Qnil;
  }
  
  blacklist_cert_t *blacklist_cert; Data_Get_Struct(rb_blacklist_cert, blacklist_cert_t, blacklist_cert);
  
  sexpSimpleString *str = blacklist_cert_to_str(blacklist_cert, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_blacklist_cert_unmarshall(VALUE rb_self, VALUE rb_blacklist_cert_str) {
  if (TYPE(rb_blacklist_cert_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_blacklist_cert_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_blacklist_cert_str)->ptr;
  
  blacklist_cert_t *blacklist_cert = str_to_blacklist_cert(str);
  
  if (blacklist_cert) {    
    // FIXME: leaking memory here, with no blacklist_cert free method
    return Data_Wrap_Struct(rb_self, NULL, NULL, blacklist_cert);
  } else {
    return Qnil;
  }
}


static VALUE rb_blacklist_marshall(VALUE rb_self, VALUE rb_blacklist) {
 if (TYPE(rb_blacklist) != T_DATA) {
   return Qnil;
 }
 
 blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
 
 sexpSimpleString *str = blacklist_to_str(blacklist, ADVANCED);
 
 return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_blacklist_unmarshall(VALUE rb_self, VALUE rb_blacklist_str) {
  if (TYPE(rb_blacklist_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_blacklist_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_blacklist_str)->ptr;
  
  blacklist_t *blacklist = str_to_blacklist(str);
  
  if (blacklist) {    
    return Data_Wrap_Struct(rb_self, NULL, blacklist_free, blacklist);
  } else {
    return Qnil;
  }
}

static VALUE rb_ticket_marshall(VALUE rb_self, VALUE rb_ticket) {
 if (TYPE(rb_ticket) != T_DATA) {
   return Qnil;
 }
 
 ticket_t *ticket; Data_Get_Struct(rb_ticket, ticket_t, ticket);
 
 sexpSimpleString *str = ticket_to_str(ticket, ADVANCED);
 
 return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_ticket_str) {
  if (TYPE(rb_ticket_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_ticket_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_ticket_str)->ptr;
  
  ticket_t *ticket = str_to_ticket(str);
  
  if (ticket) {    
    return Data_Wrap_Struct(rb_self, NULL, NULL, ticket);
  } else {
    return Qnil;
  }
}

static VALUE rb_linking_token_marshall(VALUE rb_self, VALUE rb_linking_token) {
 if (TYPE(rb_linking_token) != T_DATA) {
   return Qnil;
 }
 
 linking_token_t *linking_token; Data_Get_Struct(rb_linking_token, linking_token_t, linking_token);
 
 sexpSimpleString *str = linking_token_to_str(linking_token, ADVANCED);
 
 return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_linking_token_str) {
  if (TYPE(rb_linking_token_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_linking_token_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_linking_token_str)->ptr;
  
  linking_token_t *linking_token = str_to_linking_token(str);
  
  if (linking_token) {    
    return Data_Wrap_Struct(rb_self, NULL, NULL, linking_token);
  } else {
    return Qnil;
  }
}


static VALUE rb_nm_credential_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_pseudonym, VALUE rb_server_id, VALUE rb_link_window) { 
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_pseudonym) != T_STRING) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
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
    //printf("Data_Wrap_Struct(nm_credential_create = %x)\n", (u_int)credential);
    return Data_Wrap_Struct(rb_self, NULL, credential_free, credential);
  } else {
    return Qnil;
  }
}

static VALUE rb_nm_credential_marshall(VALUE rb_self, VALUE rb_credential) {
  if (TYPE(rb_credential) != T_DATA) {
    return Qnil;
  }
  
  credential_t *credential; Data_Get_Struct(rb_credential, credential_t, credential);
  
  sexpSimpleString *str = credential_to_str(credential, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

static VALUE rb_nm_tokens_create(VALUE rb_self, VALUE rb_nm_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_complaints, VALUE rb_time_period, VALUE rb_link_window) {
  if ((TYPE(rb_nm_state) != T_DATA) ||
      (TYPE(rb_blacklist) != T_DATA) ||
      ((TYPE(rb_complaints) != T_ARRAY) &&
       (TYPE(rb_complaints) != T_NIL)) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_time_period) != T_FIXNUM) ||
      (TYPE(rb_link_window) != T_FIXNUM)) {
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
    //printf("Data_Wrap_Struct(nm_tokens_create = %x)\n", (u_int)linking_token);
    rb_ary_push(rb_linking_tokens, Data_Wrap_Struct(rb_self, NULL, token_free, linking_token));

    linking_token = linking_token->next;
  }
  
  return rb_linking_tokens;
}

static VALUE rb_server_initialize(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist) { 
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

  //printf("Data_Wrap_Struct(server_initialize = %x)\n", (u_int)server);
  return Data_Wrap_Struct(rb_self, NULL, server_free, server);
}

static VALUE rb_server_ticket_verify(VALUE rb_self, VALUE rb_server_state, VALUE rb_nymble_ticket, VALUE rb_link_window, VALUE rb_time_period) { 
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

static VALUE rb_server_blacklist(VALUE rb_self, VALUE rb_server_state) { 
  if ((TYPE(rb_server_state) != T_DATA)) {
    return Qnil;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);

  blacklist_t *blacklist = server_blacklist(server);
  //printf("Data_Wrap_Struct(server_blacklist = %x)\n", (u_int)blacklist);
  return Data_Wrap_Struct(rb_self, NULL, NULL, blacklist);
}

static VALUE rb_server_blacklist_finalize(VALUE rb_self, VALUE rb_server_state) { 
  if ((TYPE(rb_server_state) != T_DATA)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);

  server_blacklist_finalize(server);

  return Qtrue;
}

static VALUE rb_server_blacklist_finalized(VALUE rb_self, VALUE rb_server_state) { 
  if ((TYPE(rb_server_state) != T_DATA)) {
    return Qfalse;
  }

  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);

  if (server_blacklist_finalized(server)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE rb_server_iterate(VALUE rb_self, VALUE rb_server_state, VALUE rb_time_period_delta) { 
  if ((TYPE(rb_server_state) != T_DATA) ||
      (TYPE(rb_time_period_delta) != T_FIXNUM)) {
    return Qfalse;
  }
  
  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  u_int time_period_delta = NUM2UINT(rb_time_period_delta);

  server_iterate(server, time_period_delta);

  return Qtrue;
}


static VALUE rb_server_update_cert(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist_cert) {
  if (TYPE(rb_server_state) != T_DATA || TYPE(rb_blacklist_cert) != T_DATA) {
    return Qnil;
  }
  
  server_t *server; Data_Get_Struct(rb_server_state, server_t, server);
  blacklist_cert_t *blacklist_cert; Data_Get_Struct(rb_blacklist_cert, blacklist_cert_t, blacklist_cert);
  
  server_update_cert(server, blacklist_cert);
  
  return Qtrue;
}

static VALUE rb_server_update(VALUE rb_self, VALUE rb_server_state, VALUE rb_blacklist, VALUE rb_linking_tokens) { 
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

static VALUE rb_user_initialize(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np, VALUE rb_verify_key_n) { 
  if ((TYPE(rb_pseudonym) != T_STRING) ||
      (RSTRING(rb_pseudonym)->len != DIGEST_SIZE) ||
      (TYPE(rb_mac_np) != T_STRING) ||
      (RSTRING(rb_mac_np)->len != DIGEST_SIZE) ||
      (TYPE(rb_verify_key_n) != T_STRING)) {
    return Qnil;
  }

  pseudonym_t pseudonym;
  memcpy(pseudonym.pseudonym, RSTRING(rb_pseudonym)->ptr, DIGEST_SIZE);
  memcpy(pseudonym.mac_np, RSTRING(rb_mac_np)->ptr, DIGEST_SIZE);

  const u_char *ptr = (const u_char *)RSTRING(rb_verify_key_n)->ptr;

  RSA *rsa = d2i_RSAPublicKey(NULL, &ptr, RSTRING(rb_verify_key_n)->len);
  

  user_t *user = user_initialize(&pseudonym, rsa);

  //printf("Data_Wrap_Struct(user_initialize = %x)\n", (u_int)user);
  return Data_Wrap_Struct(rb_self, NULL, user_free, user);
}

static VALUE rb_user_pseudonym(VALUE rb_self, VALUE rb_user_state) { 
  if (TYPE(rb_user_state) != T_DATA) {
    return Qnil;
  }
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  
  pseudonym_t *pseudonym = user_pseudonym(user);

  return rb_str_new((char *)pseudonym->pseudonym, DIGEST_SIZE);
}

static VALUE rb_user_pseudonym_mac(VALUE rb_self, VALUE rb_user_state) { 
  if (TYPE(rb_user_state) != T_DATA) {
    return Qnil;
  }
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  
  pseudonym_t *pseudonym = user_pseudonym(user);

  return rb_str_new((char *)pseudonym->mac_np, DIGEST_SIZE);
}

static VALUE rb_user_entry_initialize(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_credential) { 
  if ((TYPE(rb_user_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_credential) != T_DATA)) {
    return Qfalse;
  }
  
  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
  credential_t *credential; Data_Get_Struct(rb_credential, credential_t, credential);
  u_int L = NUM2UINT(rb_gv_get("L"));
  
  user_entry_initialize(user, server_id, credential, L);

  return Qtrue;
}

static VALUE rb_user_entry_exists(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id) { 
  if ((TYPE(rb_user_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE)) {
    return Qfalse;
  }

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  if (user_entry_exists(user, server_id)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE rb_user_blacklist_update(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_link_window, VALUE rb_time_period) { 
  if ((TYPE(rb_user_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_blacklist) != T_DATA) ||
      (TYPE(rb_link_window) != T_FIXNUM) ||
      (TYPE(rb_time_period) != T_FIXNUM)) {
    return Qfalse;
  }

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

static VALUE rb_user_blacklist_check(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id) { 
  if ((TYPE(rb_user_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE)) {
    return Qnil;
  }

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;

  if (user_blacklist_check(user, server_id)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE rb_user_credential_get(VALUE rb_self, VALUE rb_user_state, VALUE rb_server_id, VALUE rb_time_period) { 
  if ((TYPE(rb_user_state) != T_DATA) ||
      (TYPE(rb_server_id) != T_STRING) ||
      (RSTRING(rb_server_id)->len != DIGEST_SIZE) ||
      (TYPE(rb_time_period) != T_FIXNUM)) {
    return Qnil;
  }

  user_t *user; Data_Get_Struct(rb_user_state, user_t, user);
  u_char *server_id = (u_char *)RSTRING(rb_server_id)->ptr;
  u_int time_period = NUM2UINT(rb_time_period);

  ticket_t *ticket = user_credential_get(user, server_id, time_period);
  
  if (ticket) {
    //printf("Data_Wrap_Struct(user_credential_get = %x)\n", (u_int)ticket);
    return Data_Wrap_Struct(rb_self, NULL, NULL, ticket);
  } else {
    return Qnil;
  }
}

static VALUE rb_user_credential_unmarshall(VALUE rb_self, VALUE rb_credential_str) {
  if (TYPE(rb_credential_str) != T_STRING) {
    return Qnil;
  }
  
  sexpSimpleString *str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING(rb_credential_str)->len + 1;
  str->string = (u_char *)RSTRING(rb_credential_str)->ptr;
  
  credential_t *credential = str_to_credential(str);
  
  if (credential) {    
    return Data_Wrap_Struct(rb_self, NULL, credential_free, credential);
  } else {
    return Qnil;
  }
}

void Init_libnymble() {
  rb_cNymble = rb_define_class("Nymble", rb_cObject);
  
  rb_define_singleton_method(rb_cNymble, "digest", rb_nymble_hash, 1);
  rb_define_singleton_method(rb_cNymble, "random_bytes", rb_nymble_random_bytes, 1);
  
  rb_define_singleton_method(rb_cNymble, "pm_initialize", rb_pm_initialize, 1);
  rb_define_singleton_method(rb_cNymble, "pm_pseudonym_create", rb_pm_pseudonym_create, 3);

  rb_define_singleton_method(rb_cNymble, "pseudonym_marshall", rb_pseudonym_marshall, 2);  
  rb_define_singleton_method(rb_cNymble, "pseudonym_unmarshall", rb_pseudonym_unmarshall, 1);  

  rb_define_singleton_method(rb_cNymble, "blacklist_cert", rb_blacklist_cert, 1);    

  rb_define_singleton_method(rb_cNymble, "blacklist_cert_marshall", rb_blacklist_cert_marshall, 1);    
  rb_define_singleton_method(rb_cNymble, "blacklist_cert_unmarshall", rb_blacklist_cert_unmarshall, 1);

  rb_define_singleton_method(rb_cNymble, "blacklist_marshall", rb_blacklist_marshall, 1);    
  rb_define_singleton_method(rb_cNymble, "blacklist_unmarshall", rb_blacklist_unmarshall, 1);
  
  rb_define_singleton_method(rb_cNymble, "ticket_marshall", rb_ticket_marshall, 1);  
  rb_define_singleton_method(rb_cNymble, "ticket_unmarshall", rb_ticket_unmarshall, 1);  

  rb_define_singleton_method(rb_cNymble, "linking_token_marshall", rb_linking_token_marshall, 1);  
  rb_define_singleton_method(rb_cNymble, "linking_token_unmarshall", rb_linking_token_unmarshall, 1);  

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
  rb_define_singleton_method(rb_cNymble, "nm_credential_marshall", rb_nm_credential_marshall, 1);
  rb_define_singleton_method(rb_cNymble, "nm_tokens_create", rb_nm_tokens_create, 6);
                                         
  rb_define_singleton_method(rb_cNymble, "server_initialize", rb_server_initialize, 3);
  rb_define_singleton_method(rb_cNymble, "server_ticket_verify", rb_server_ticket_verify, 4);
  rb_define_singleton_method(rb_cNymble, "server_blacklist", rb_server_blacklist, 1);
  rb_define_singleton_method(rb_cNymble, "server_blacklist_finalize", rb_server_blacklist_finalize, 1);
  rb_define_singleton_method(rb_cNymble, "server_blacklist_finalized", rb_server_blacklist_finalized, 1);
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
  rb_define_singleton_method(rb_cNymble, "user_credential_unmarshall", rb_user_credential_unmarshall, 1);
  rb_define_singleton_method(rb_cNymble, "user_credential_get", rb_user_credential_get, 3);
}
