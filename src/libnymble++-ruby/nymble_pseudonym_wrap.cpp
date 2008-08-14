#include "nymble_pseudonym_wrap.h"

VALUE rb_pseudonym_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Pseudonym* pseudonym = Nymble::Pseudonym::unmarshal(bytes, size);
  
  if (pseudonym == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_pseudonym_delete, pseudonym);
}

VALUE rb_pseudonym_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonym);
  
  Nymble::Pseudonym* pseudonym = (Nymble::Pseudonym*) DATA_PTR(rb_self);
  u_int marshalled_size = pseudonym->marshal();
  u_char marshalled[marshalled_size];
  
  pseudonym->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}

void rb_pseudonym_delete(Nymble::Pseudonym* pseudonym)
{
  delete pseudonym;
}