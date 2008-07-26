#include "nymble_credential_wrap.h"

VALUE rb_credential_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Credential* credential = Credential::unmarshall(bytes);
  
  if (credential == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_credential_delete, credential);
}

VALUE rb_credential_marshall(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cCredential);
  
  Credential* credential = (Credential*) DATA_PTR(rb_self);
  
  return rb_str_new2(credential->marshall());
}

void rb_credential_delete(Credential* credential)
{
  delete credential;
}