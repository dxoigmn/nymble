#include "nymble_credential_wrap.h"

VALUE rb_credential_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Credential* credential = new Credential();
  
  Credential::unmarshal(bytes, credential);
  
  if (credential == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_credential_delete, credential);
}

VALUE rb_credential_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cCredential);
  
  Credential* credential = (Credential*) DATA_PTR(rb_self);
  char marshaled_credential[credential->marshal() + 1];
  
  credential->marshal(marshaled_credential);
  
  return rb_str_new2(marshaled_credential);
}

void rb_credential_delete(Credential* credential)
{
  delete credential;
}