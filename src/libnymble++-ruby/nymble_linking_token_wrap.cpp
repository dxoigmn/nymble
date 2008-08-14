#include "nymble_linking_token_wrap.h"

VALUE rb_linking_token_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Nymble::LinkingToken* linking_token = new Nymble::LinkingToken();
  
  Nymble::LinkingToken::unmarshal(bytes, linking_token);
  
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
  char marshaled_linking_token[linking_token->marshal() + 1];
  
  linking_token->marshal(marshaled_linking_token);
  
  return rb_str_new2(marshaled_linking_token);
}

void rb_linking_token_delete(Nymble::LinkingToken* linking_token)
{
  delete linking_token;
}