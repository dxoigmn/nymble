#include "nymble_sexp_wrap.h"

VALUE rb_pseudonym_marshall(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np)
{
  Check_Type(rb_pseudonym, T_STRING);
  Check_Type(rb_mac_np, T_STRING);
  
  pseudonym_t pseudonym;
  
  memcpy(pseudonym.pseudonym, RSTRING(rb_pseudonym)->ptr, DIGEST_SIZE);
  memcpy(pseudonym.mac_np, RSTRING(rb_mac_np)->ptr, DIGEST_SIZE);

  sexpSimpleString *str = pseudonym_to_str(&pseudonym, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_pseudonym_str)
{
  Check_Type(rb_pseudonym_str, T_STRING);
  
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

VALUE rb_blacklist_cert(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_blacklist, T_DATA);

  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);
  
  return Data_Wrap_Struct(rb_self, NULL, NULL, &blacklist->cert);
}

VALUE rb_blacklist_cert_marshall(VALUE rb_self, VALUE rb_blacklist_cert)
{
  Check_Type(rb_blacklist_cert, T_DATA);
  
  blacklist_cert_t *blacklist_cert; Data_Get_Struct(rb_blacklist_cert, blacklist_cert_t, blacklist_cert);
  
  sexpSimpleString *str = blacklist_cert_to_str(blacklist_cert, ADVANCED);
  
  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_blacklist_cert_unmarshall(VALUE rb_self, VALUE rb_blacklist_cert_str)
{
  Check_Type(rb_blacklist_cert_str, T_STRING);
  
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


VALUE rb_blacklist_marshall(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_blacklist, T_DATA);

  blacklist_t *blacklist; Data_Get_Struct(rb_blacklist, blacklist_t, blacklist);

  sexpSimpleString *str = blacklist_to_str(blacklist, ADVANCED);

  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_blacklist_unmarshall(VALUE rb_self, VALUE rb_blacklist_str)
{
  Check_Type(rb_blacklist_str, T_STRING);
  
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

VALUE rb_ticket_marshall(VALUE rb_self, VALUE rb_ticket)
{
  Check_Type(rb_ticket, T_DATA);

  ticket_t *ticket; Data_Get_Struct(rb_ticket, ticket_t, ticket);

  sexpSimpleString *str = ticket_to_str(ticket, ADVANCED);

  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_ticket_str)
{
  Check_Type(rb_ticket_str, T_STRING);
  
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

VALUE rb_linking_token_marshall(VALUE rb_self, VALUE rb_linking_token)
{
  Check_Type(rb_linking_token, T_DATA);

  linking_token_t *linking_token; Data_Get_Struct(rb_linking_token, linking_token_t, linking_token);

  sexpSimpleString *str = linking_token_to_str(linking_token, ADVANCED);

  return rb_str_new((char *)str->string, str->length);
}

VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_linking_token_str)
{
  Check_Type(rb_linking_token_str, T_STRING);
  
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

VALUE rb_user_credential_unmarshall(VALUE rb_self, VALUE rb_credential_str) {
  Check_Type(rb_credential_str, T_DATA);
  
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
