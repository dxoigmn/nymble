#include "nymble_token_wrap.h"

VALUE rb_token_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Token* token = Nymble::Token::unmarshal(bytes, size);
  
  if (token == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_token_delete, token);
}

VALUE rb_token_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cToken);

  Nymble::Token* token = (Nymble::Token*) DATA_PTR(rb_self);
  u_int marshalled_size = token->marshal();
  u_char marshalled[marshalled_size];
  
  token->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_token_delete(Nymble::Token* token)
{
  delete token;
}