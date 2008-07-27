#include "nymble_linking_token_wrap.h"

VALUE rb_linking_token_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  LinkingToken* linking_token = new LinkingToken();
  
  LinkingToken::unmarshall(bytes, linking_token);
  
  if (linking_token == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_linking_token_delete, linking_token);
}

VALUE rb_linking_token_marshall(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cLinkingToken);

  LinkingToken* linking_token = (LinkingToken*) DATA_PTR(rb_self);
  char marshalled_linking_token[linking_token->marshall() + 1];
  
  linking_token->marshall(marshalled_linking_token);
  
  return rb_str_new2(marshalled_linking_token);
}

void rb_linking_token_delete(LinkingToken* linking_token)
{
  delete linking_token;
}