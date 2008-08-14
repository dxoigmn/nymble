#include "nymble_blacklist_wrap.h"


VALUE rb_blacklist_unmarshal(VALUE rb_self, VALUE rb_bytes)
{
  Check_Type(rb_bytes, T_STRING);
  
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Blacklist* blacklist = new Blacklist();
  
  Blacklist::unmarshal(bytes, blacklist);
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_blacklist_delete, blacklist);
}

VALUE rb_blacklist_marshal(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cBlacklist);
  
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_self);
  char marshaled_blacklist[blacklist->marshal() + 1];
  
  blacklist->marshal(marshaled_blacklist);
  
  return rb_str_new2(marshaled_blacklist);
}


void rb_blacklist_delete(Blacklist* blacklist)
{
  delete blacklist;
}