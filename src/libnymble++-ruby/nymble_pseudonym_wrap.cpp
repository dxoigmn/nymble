#include "nymble_pseudonym_wrap.h"

VALUE rb_pseudonym_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Pseudonym* pseudonym = new Pseudonym();
  
  Pseudonym::unmarshall(bytes, pseudonym);
  
  if (pseudonym == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_pseudonym_delete, pseudonym);
}

VALUE rb_pseudonym_marshall(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cPseudonym);
  
  Pseudonym* pseudonym = (Pseudonym*) DATA_PTR(rb_self);
  char marshalled_pseudonym[pseudonym->marshall() + 1];
  
  pseudonym->marshall(marshalled_pseudonym);
  
  return rb_str_new2(marshalled_pseudonym);
}

void rb_pseudonym_delete(Pseudonym* pseudonym)
{
  delete pseudonym;
}