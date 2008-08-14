#include "nymble_linking_token_wrap.h"

VALUE rb_linking_token_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::LinkingToken* linking_token = Nymble::LinkingToken::unmarshal(bytes, size);
  
  if (linking_token == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_linking_token_delete, linking_token);
}

VALUE rb_linking_token_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cLinkingToken);

  Nymble::LinkingToken* linking_token = (Nymble::LinkingToken*) DATA_PTR(rb_self);
  u_int marshalled_size = linking_token->marshal();
  u_char marshalled[marshalled_size];
  
  linking_token->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_linking_token_delete(Nymble::LinkingToken* linking_token)
{
  delete linking_token;
}