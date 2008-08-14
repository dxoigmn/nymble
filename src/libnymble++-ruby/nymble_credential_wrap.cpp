#include "nymble_credential_wrap.h"

VALUE rb_credential_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Credential* credential = Nymble::Credential::unmarshal(bytes, size);
  
  if (credential == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_credential_delete, credential);
}

VALUE rb_credential_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cCredential);
  
  Nymble::Credential* credential = (Nymble::Credential*) DATA_PTR(rb_self);
  u_int marshalled_size = credential->marshal();
  u_char marshalled[marshalled_size];
  
  credential->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_credential_delete(Nymble::Credential* credential)
{
  delete credential;
}