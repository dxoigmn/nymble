#include "nymble_blacklist_wrap.h"


VALUE rb_blacklist_unmarshall(VALUE rb_self, VALUE rb_bytes)
{
  char* bytes = (char*) RSTRING_PTR(rb_bytes);
  Blacklist* blacklist = new Blacklist();
  
  Blacklist::unmarshall(bytes, blacklist);
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_self, NULL, rb_blacklist_delete, blacklist);
}

VALUE rb_blacklist_marshall(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cBlacklist);
  
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_self);
  char marshalled_blacklist[blacklist->marshall() + 1];
  
  blacklist->marshall(marshalled_blacklist);
  
  return rb_str_new2(marshalled_blacklist);
}


void rb_blacklist_delete(Blacklist* blacklist)
{
  delete blacklist;
}