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
  
  return rb_str_new(mac_key_np.data(), mac_key_np.size());
}

VALUE rb_nm_write_verify_key_n(VALUE rb_self, VALUE rb_path)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_path, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string path(RSTRING_PTR(rb_path), RSTRING_LEN(rb_path));
  
  if (!nm->writeVerifyKeyN(path)) {
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
  
  if (!server_state.SerializeToString(&server_state_str)) {
    return Qnil;
  }
  
  return rb_str_new(server_state_str.data(), server_state_str.size());
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
  
  if (!credential.SerializeToString(&credential_str)) {
    return Qnil;
  }
  
  return rb_str_new(credential_str.data(), credential_str.size());
}

VALUE rb_nm_update_server(VALUE rb_self, VALUE rb_sid, VALUE rb_server_state_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_sid, T_STRING);
  Check_Type(rb_server_state_str, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  std::string sid(RSTRING_PTR(rb_sid), RSTRING_LEN(rb_sid));
  std::string server_state_str(RSTRING_PTR(rb_server_state_str), RSTRING_LEN(rb_server_state_str));
  
  Nymble::ServerState* server_state = NULL;
  Nymble::ServerState new_server_state;
  
  if (server_state_str != "") {
    server_state = new Nymble::ServerState();
    
    if (!server_state->ParseFromString(server_state_str)) {
      return Qnil;
    }
  }
  
  if (!nm->updateServer(sid, server_state, &new_server_state)) {
    return Qnil;
  }
  
  if (server_state != NULL) {
    delete server_state;
  }
  
  std::string new_server_state_str;
  
  if (!new_server_state.SerializeToString(&new_server_state_str)) {
    return Qnil;
  }
  
  return rb_str_new(new_server_state_str.data(), new_server_state_str.size());
}

VALUE rb_nm_reset(VALUE rb_self, VALUE rb_args)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  VALUE rb_time_periods = rb_ary_entry(rb_args, 0);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  
  
  if (NIL_P(rb_time_periods)) {
    nm->reset();
  } else {
    Check_Type(rb_time_periods, T_FIXNUM);
    nm->reset(NUM2UINT(rb_time_periods));
  }
  
  return rb_self;
}

void rb_nm_delete(Nymble::NymbleManager* nm)
{
  delete nm;
}
