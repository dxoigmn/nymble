#include "nymble_user_wrap.h"

VALUE rb_user_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_user_delete, new Nymble::User());
}

VALUE rb_user_init(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_verify_key_path)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_pseudonym, T_DATA);
  Check_Class(rb_pseudonym, rb_cPseudonym);
  Check_Type(rb_verify_key_path, T_STRING);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  Nymble::Pseudonym* pseudonym = (Nymble::Pseudonym*) DATA_PTR(rb_pseudonym);
  char* verify_key_path = RSTRING_PTR(rb_verify_key_path);
  
  user->setPseudonym(pseudonym);
  user->readVerifyKey(verify_key_path);
  
  return rb_self;
}

VALUE rb_user_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  
  return INT2FIX(user->getLinkWindow());
}

VALUE rb_user_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  user->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_user_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  
  return INT2FIX(user->getTimePeriod());
}

VALUE rb_user_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  user->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_user_pseudonym(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  
  return Data_Wrap_Struct(rb_cPseudonym, NULL, NULL, user->getPseudonym());
}

VALUE rb_user_find_or_create_entry(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  
  Nymble::UserEntry* entry = user->findOrCreateEntry(server_id);
  
  return Data_Wrap_Struct(rb_cUserEntry, NULL, NULL, entry);
}

void rb_user_delete(Nymble::User* user)
{
  delete user;
}
