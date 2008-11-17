#include "nymble_manager_wrap.h"

VALUE rb_cNymbleManager;

VALUE rb_nm_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_nm_delete, new Nymble::NymbleManager());
}

VALUE rb_nm_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  
  return INT2FIX(nm->getLinkWindow());
}

VALUE rb_nm_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  nm->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_nm_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  
  return INT2FIX(nm->getTimePeriod());
}

VALUE rb_nm_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  nm->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_nm_mac_key_np(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string mac_key_np = nm->getMacKeyNP();
  
  return rb_str_new(mac_key_np.c_str(), mac_key_np.size());
}

VALUE rb_nm_valid_pseudonym(VALUE rb_self, VALUE rb_pseudonym_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_pseudonym_str, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string pseudonym_str(RSTRING_PTR(rb_pseudonym_str), RSTRING_LEN(rb_pseudonym_str));
  
  Nymble::Pseudonym pseudonym;
  
  if (!pseudonym.ParseFromString(pseudonym_str)) {
    return Qfalse;
  }
  
  if (!nm->verifyPseudonym(pseudonym)) {
    return Qfalse;
  }
  
  return Qtrue;
}

VALUE rb_nm_register_server(VALUE rb_self, VALUE rb_sid)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_sid, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  
  Nymble::ServerState server_state;
  
  if (!nm->registerServer(sid, &server_state)) {
    return Qnil;
  }
  
  std::string server_state_str;
  server_state.SerializeToString(&server_state_str);
  
  return rb_str_new(server_state_str.c_str(), server_state_str.size());
}

VALUE rb_nm_create_credential(VALUE rb_self, VALUE rb_sid, VALUE rb_pseudonym_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_sid, T_STRING);
  Check_Type(rb_pseudonym_str, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  std::string pseudonym_str(RSTRING_PTR(rb_pseudonym_str), RSTRING_LEN(rb_pseudonym_str));
  
  Nymble::Pseudonym pseudonym;
  
  if (!pseudonym.ParseFromString(pseudonym_str)) {
    return Qnil;
  }
  
  Nymble::Credential credential;
  
  if (!nm->createCredential(sid, pseudonym, &credential)) {
    return Qnil;
  }
  
  std::string credential_str;
  credential.SerializeToString(&credential_str);
  
  return rb_str_new(credential_str.c_str(), credential_str.size());
}

VALUE rb_nm_valid_ticket(VALUE rb_self, VALUE rb_sid, VALUE rb_ticket_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_sid, T_STRING);
  Check_Type(rb_ticket_str, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  std::string ticket_str(RSTRING_PTR(rb_ticket_str), RSTRING_LEN(rb_ticket_str));
  
  Nymble::Ticket ticket;
  
  if (!ticket.ParseFromString(ticket_str)) {
    return Qfalse;
  }
  
  if (!nm->verifyTicket(sid, ticket)) {
    return Qfalse;
  }
  
  return Qtrue;
}

void rb_nm_delete(Nymble::NymbleManager* nm)
{
  delete nm;
}
