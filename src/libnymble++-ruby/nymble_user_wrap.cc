#include "nymble_user_wrap.h"

VALUE rb_cUser;

VALUE rb_user_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_user_delete, new Nymble::User());
}

VALUE rb_user_init(VALUE rb_self, VALUE rb_pseudonym_str, VALUE rb_verify_key_path)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_pseudonym_str, T_STRING);
  Check_Type(rb_verify_key_path, T_STRING);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  std::string pseudonym_str(RSTRING_PTR(rb_pseudonym_str), RSTRING_LEN(rb_pseudonym_str));
  std::string verify_key_path(RSTRING_PTR(rb_verify_key_path), RSTRING_LEN(rb_verify_key_path));
  
  Nymble::Pseudonym pseudonym;
  
  if (!pseudonym.ParseFromString(pseudonym_str)) {
    return Qnil;
  }
  
  user->setPseudonym(pseudonym);
  
  if (!user->readVerifyKeyN(verify_key_path)) {
    return Qnil;
  }
  
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
  Nymble::Pseudonym* pseudonym = user->getPseudonym();
  
  if (pseudonym == NULL) {
    return Qnil;
  }
  
  std::string pseudonym_str;
  
  if (!pseudonym->SerializeToString(&pseudonym_str)) {
    return Qnil;
  }
  
  return rb_str_new(pseudonym_str.c_str(), pseudonym_str.size());
}

VALUE rb_user_add_credential(VALUE rb_self, VALUE rb_sid, VALUE rb_credential_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_sid, T_STRING);
  Check_Type(rb_credential_str, T_STRING);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  std::string credential_str(RSTRING_PTR(rb_credential_str), RSTRING_LEN(rb_credential_str));
  
  Nymble::Credential credential;
  
  if (!credential.ParseFromString(credential_str)) {
    return Qfalse;
  }
  
  user->addCredential(sid, credential);
  
  return Qtrue;
}

VALUE rb_user_ticket(VALUE rb_self, VALUE rb_sid)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_sid, T_STRING);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  
  Nymble::Ticket* ticket = user->getTicket(sid);
  
  if (ticket == NULL) {
    return Qnil;
  }
  
  std::string ticket_str;
  
  if (!ticket->SerializeToString(&ticket_str)) {
    return Qnil;
  }
  
  return rb_str_new(ticket_str.c_str(), ticket_str.size());
}

VALUE rb_user_blacklisted(VALUE rb_self, VALUE rb_sid, VALUE rb_blacklist_str, VALUE rb_cert_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cUser);
  Check_Type(rb_sid, T_STRING);
  Check_Type(rb_blacklist_str, T_STRING);
  Check_Type(rb_cert_str, T_STRING);
  
  Nymble::User* user = (Nymble::User*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  std::string blacklist_str(RSTRING_PTR(rb_blacklist_str), RSTRING_LEN(rb_blacklist_str));
  std::string cert_str(RSTRING_PTR(rb_cert_str), RSTRING_LEN(rb_cert_str));
  
  Nymble::Blacklist blacklist;
  
  if (!blacklist.ParseFromString(blacklist_str)) {
    return Qtrue; // Assume blacklisted
  }
  
  Nymble::BlacklistCert cert;
  
  if (!cert.ParseFromString(cert_str)) {
    return Qtrue; // Assume blacklisted
  }
  
  if (user->isBlacklisted(sid, blacklist, cert)) {
    return Qtrue;
  }
  
  return Qfalse;
}

void rb_user_delete(Nymble::User* user)
{
  delete user;
}
