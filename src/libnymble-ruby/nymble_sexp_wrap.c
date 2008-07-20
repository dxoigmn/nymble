#include "nymble_sexp_wrap.h"

VALUE rb_pseudonym_marshall(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_mac_np)
{
  Check_Type(rb_pseudonym, T_STRING);
  Check_Type(rb_mac_np, T_STRING);
  Check_Size(rb_pseudonym, DIGEST_SIZE);
  Check_Size(rb_mac_np, DIGEST_SIZE);
  
  pseudonym_t pseudonym;
  
  memcpy(pseudonym.pseudonym, RSTRING_PTR(rb_pseudonym), DIGEST_SIZE);
  memcpy(pseudonym.mac_np, RSTRING_PTR(rb_mac_np), DIGEST_SIZE);

  sexpSimpleString* str = pseudonym_to_str(&pseudonym, ADVANCED);
  
  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_pseudonym_str)
{
  Check_Type(rb_pseudonym_str, T_STRING);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_pseudonym_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_pseudonym_str);
  
  pseudonym_t* pseudonym = str_to_pseudonym(str);
  
  if (pseudonym) {    
    VALUE ret = rb_ary_new();

    rb_ary_push(ret, rb_str_new((char*)pseudonym->pseudonym, DIGEST_SIZE));
    rb_ary_push(ret, rb_str_new((char*)pseudonym->mac_np, DIGEST_SIZE));
    return ret;
  } else {
    return Qnil;
  }
}

VALUE rb_blacklist_cert_marshall(VALUE rb_self, VALUE rb_blacklist_cert)
{
  Check_Type(rb_blacklist_cert, T_DATA);
  
  blacklist_cert_t* blacklist_cert = (blacklist_cert_t*)DATA_PTR(rb_blacklist_cert);
  
  sexpSimpleString* str = blacklist_cert_to_str(blacklist_cert, ADVANCED);
  
  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_blacklist_cert_unmarshall(VALUE rb_self, VALUE rb_blacklist_cert_str)
{
  Check_Type(rb_blacklist_cert_str, T_STRING);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_blacklist_cert_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_blacklist_cert_str);
  
  blacklist_cert_t* blacklist_cert = str_to_blacklist_cert(str);
  
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

  blacklist_t* blacklist = (blacklist_t*)DATA_PTR(rb_blacklist);

  sexpSimpleString* str = blacklist_to_str(blacklist, ADVANCED);

  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_blacklist_unmarshall(VALUE rb_self, VALUE rb_blacklist_str)
{
  Check_Type(rb_blacklist_str, T_STRING);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_blacklist_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_blacklist_str);
  
  blacklist_t* blacklist = str_to_blacklist(str);
  
  if (blacklist) {    
    return Data_Wrap_Struct(rb_self, NULL, blacklist_free, blacklist);
  } else {
    return Qnil;
  }
}

VALUE rb_ticket_marshall(VALUE rb_self, VALUE rb_ticket)
{
  Check_Type(rb_ticket, T_DATA);

  ticket_t* ticket = (ticket_t*)DATA_PTR(rb_ticket);

  sexpSimpleString* str = ticket_to_str(ticket, ADVANCED);

  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_ticket_unmarshall(VALUE rb_self, VALUE rb_ticket_str)
{
  Check_Type(rb_ticket_str, T_STRING);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_ticket_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_ticket_str);
  
  ticket_t* ticket = str_to_ticket(str);
  
  if (ticket) {    
    return Data_Wrap_Struct(rb_self, NULL, NULL, ticket);
  } else {
    return Qnil;
  }
}

VALUE rb_linking_token_marshall(VALUE rb_self, VALUE rb_linking_token)
{
  Check_Type(rb_linking_token, T_DATA);

  linking_token_t* linking_token = (linking_token_t*)DATA_PTR(rb_linking_token);

  sexpSimpleString* str = linking_token_to_str(linking_token, ADVANCED);

  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_linking_token_str)
{
  Check_Type(rb_linking_token_str, T_STRING);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_linking_token_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_linking_token_str);
  
  linking_token_t* linking_token = str_to_linking_token(str);
  
  if (linking_token) {    
    return Data_Wrap_Struct(rb_self, NULL, NULL, linking_token);
  } else {
    return Qnil;
  }
}

VALUE rb_credential_marshall(VALUE rb_self, VALUE rb_credential)
{
  Check_Type(rb_credential, T_DATA);
  
  credential_t* credential = (credential_t*)DATA_PTR(rb_credential);
  
  sexpSimpleString* str = credential_to_str(credential, ADVANCED);
  
  return rb_str_new((char*)str->string, str->length);
}

VALUE rb_credential_unmarshall(VALUE rb_self, VALUE rb_credential_str) {
  Check_Type(rb_credential_str, T_DATA);
  
  sexpSimpleString* str = malloc(sizeof(sexpSimpleString));
  
  str->length = str->allocatedLength = RSTRING_LEN(rb_credential_str) + 1;
  str->string = (u_char*)RSTRING_PTR(rb_credential_str);
  
  credential_t* credential = str_to_credential(str);
  
  if (credential) {    
    return Data_Wrap_Struct(rb_self, NULL, credential_free, credential);
  } else {
    return Qnil;
  }
}
