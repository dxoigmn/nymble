#include "nymble_user_wrap.h"

VALUE rb_user_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_user_delete, new User());
}

VALUE rb_user_init(VALUE rb_self, VALUE rb_pseudonym, VALUE rb_verify_key_path)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_pseudonym, T_DATA);
  Check_Class(rb_pseudonym, rb_cPseudonym);
  Check_Type(rb_verify_key_path, T_STRING);
  
  User* user = (User*) DATA_PTR(rb_self);
  Pseudonym* pseudonym = (Pseudonym*) DATA_PTR(rb_pseudonym);
  char* verify_key_path = RSTRING_PTR(rb_verify_key_path);
  
  user->setPseudonym(pseudonym);
  user->readVerifyKey(verify_key_path);
  
  return rb_self;
}

VALUE rb_user_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  User* user = (User*) DATA_PTR(rb_self);
  
  return INT2FIX(user->getLinkWindow());
}

VALUE rb_user_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_link_window, T_FIXNUM);
  
  User* user = (User*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  user->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_user_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  User* user = (User*) DATA_PTR(rb_self);
  
  return INT2FIX(user->getTimePeriod());
}

VALUE rb_user_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_time_period, T_FIXNUM);
  
  User* user = (User*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  user->setTimePeriod(time_period);
  
  return rb_self;
}


VALUE rb_user_pseudonym(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  
  User* user = (User*) DATA_PTR(rb_self);
  
  return Data_Wrap_Struct(rb_cPseudonym, NULL, NULL, user->getPseudonym());
}

VALUE rb_user_add_blacklist(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  
  User* user = (User*) DATA_PTR(rb_self);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  u_char* server_id = user->addBlacklist(blacklist);
  
  if (server_id == NULL) {
    return Qnil;
  }
  
  return rb_str_new((char*) server_id, DIGEST_SIZE);
}

VALUE rb_user_add_credential(VALUE rb_self, VALUE rb_credential)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_credential, T_DATA);
  Check_Class(rb_credential, rb_cCredential);
  
  User* user = (User*) DATA_PTR(rb_self);
  Credential* credential = (Credential*) DATA_PTR(rb_credential);
  
  if (user->addCredential(credential)) {
    return Qtrue;
  }
  
  return Qfalse;
}

VALUE rb_user_ticket(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  User* user = (User*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Ticket* ticket = user->getTicket(server_id);
  
  if (ticket ==  NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cTicket, NULL, NULL, ticket);
}

void rb_user_delete(User* user)
{
  delete user;
}
