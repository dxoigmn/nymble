#include "nymble_util_wrap.h"

VALUE rb_nymble_hash(VALUE rb_self, VALUE rb_value)
{
  rb_value  = rb_funcall(rb_value, rb_intern("to_s"), 0);
  u_char* value = (u_char*)RSTRING_PTR(rb_value);
  u_int size = RSTRING_LEN(rb_value);
  
  u_char buffer[DIGEST_SIZE];
  
  hash(buffer, value, size);

  return rb_str_new((char *)&buffer, sizeof(buffer));
}

VALUE rb_nymble_random_bytes(VALUE rb_self, VALUE rb_count)
{
  Check_Type(rb_count, T_FIXNUM);

  u_int size = NUM2UINT(rb_count);
  u_char buffer[size];
  
  random_bytes(buffer, size);
  
  return rb_str_new((char *)buffer, sizeof(buffer));
}

VALUE rb_blacklist_cert(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_blacklist, T_DATA);

  blacklist_t* blacklist = (blacklist_t*)DATA_PTR(rb_blacklist);
  
  return Data_Wrap_Struct(rb_self, NULL, NULL, &blacklist->cert);
}