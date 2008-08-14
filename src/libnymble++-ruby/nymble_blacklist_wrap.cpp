#include "nymble_blacklist_wrap.h"

VALUE rb_blacklist_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  u_char* bytes = (u_char*) RSTRING_PTR(rb_bytes);
  u_int size = RSTRING_LEN(rb_bytes);
  Nymble::Blacklist* blacklist = Nymble::Blacklist::unmarshal(bytes, size);
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_blacklist_delete, blacklist);
}

VALUE rb_blacklist_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cBlacklist);
  
  Nymble::Blacklist* blacklist = (Nymble::Blacklist*) DATA_PTR(rb_self);
  u_int marshalled_size = blacklist->marshal();
  u_char marshalled[marshalled_size];
  
  blacklist->marshal(marshalled, marshalled_size);
  
  return rb_str_new((char*) marshalled, marshalled_size);
}


void rb_blacklist_delete(Nymble::Blacklist* blacklist)
{
  delete blacklist;
}