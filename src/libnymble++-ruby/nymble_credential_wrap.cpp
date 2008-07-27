#include "nymble_credential_wrap.h"

VALUE rb_credential_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Credential* credential = new Credential();
  
  Credential::unmarshall(bytes, credential);
  
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
  char marshalled_credential[credential->marshall() + 1];
  
  credential->marshall(marshalled_credential);
  
  return rb_str_new2(marshalled_credential);
}

void rb_credential_delete(Credential* credential)
{
  delete credential;
}