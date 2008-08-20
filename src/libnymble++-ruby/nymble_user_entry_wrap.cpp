#include "nymble_user_entry_wrap.h"

VALUE rb_user_entry_is_blacklisted(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUserEntry);
  
  Nymble::UserEntry* entry = (Nymble::UserEntry*) DATA_PTR(rb_self);
  
  if (entry->isBlacklisted()) {
    return Qtrue;
  }
  
  return Qfalse;
}

VALUE rb_user_entry_ticket(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUserEntry);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble::UserEntry* entry = (Nymble::UserEntry*) DATA_PTR(rb_self);
  u_int time_period = NUM2UINT(rb_time_period);
  
  Nymble::Ticket* ticket = entry->getTicket(time_period);
  
  if (ticket ==  NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cTicket, NULL, NULL, ticket);
}

VALUE rb_user_entry_blacklist_set(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUserEntry);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  
  Nymble::UserEntry* entry = (Nymble::UserEntry*) DATA_PTR(rb_self);
  Nymble::Blacklist* blacklist = (Nymble::Blacklist*) DATA_PTR(rb_blacklist);
  
  entry->setBlacklist(blacklist);
  
  return Qnil;
}

VALUE rb_user_entry_credential_set(VALUE rb_self, VALUE rb_credential)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUserEntry);
  Check_Type(rb_credential, T_DATA);
  Check_Class(rb_credential, rb_cCredential);
  
  Nymble::UserEntry* entry = (Nymble::UserEntry*) DATA_PTR(rb_self);
  Nymble::Credential* credential = (Nymble::Credential*) DATA_PTR(rb_credential);
  
  entry->setCredential(credential);
  
  return Qnil;
}
