#include "nymble_manager_wrap.h"

VALUE rb_nm_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_nm_delete, new Nymble::NymbleManager());
}

VALUE rb_nm_init(VALUE rb_self, VALUE rb_hmac_key_np, VALUE rb_sign_key_path)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_hmac_key_np, T_STRING);
  Check_Size(rb_hmac_key_np, DIGEST_SIZE);
  Check_Type(rb_sign_key_path, T_STRING);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* hmac_key_np = (u_char*) RSTRING_PTR(rb_hmac_key_np);
  char* sign_key_path = RSTRING_PTR(rb_sign_key_path);
  
  nm->setMacKeyNP(hmac_key_np);
  nm->readSignKey(sign_key_path);
  
  return rb_self;
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

VALUE rb_nm_add_server(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  u_char* hmac_key_ns = nm->addServer(server_id);
  
  if (hmac_key_ns == NULL) {
    return Qnil;
  }
  
  return rb_str_new((char*) hmac_key_ns, DIGEST_SIZE);
}

VALUE rb_nm_verify_pseudonym(VALUE rb_self, VALUE rb_pseudonym)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_pseudonym, T_DATA);
  Check_Class(rb_pseudonym, rb_cPseudonym);
    
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  Nymble::Pseudonym* pseudonym = (Nymble::Pseudonym*) DATA_PTR(rb_pseudonym);
  
  if (nm->verifyPseudonym(pseudonym)) {
    return Qtrue;
  }
  
  return Qfalse;
}

VALUE rb_nm_verify_blacklist(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Nymble::Blacklist* blacklist = (Nymble::Blacklist*) DATA_PTR(rb_blacklist);
  
  if (nm->verifyBlacklist(server_id, blacklist)) {
    return Qtrue;
  }
  
  return Qfalse;
}

VALUE rb_nm_create_blacklist(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Nymble::Blacklist* blacklist = nm->createBlacklist(server_id);
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cBlacklist, NULL, NULL, blacklist);
}

VALUE rb_nm_update_blacklist(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_complaints)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  Check_Type(rb_complaints, T_ARRAY);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Nymble::Blacklist* blacklist = (Nymble::Blacklist*) DATA_PTR(rb_blacklist);
  Nymble::Complaints complaints;
  
  for (int i = 0; i < RARRAY_LEN(rb_complaints); i++) {
    VALUE rb_complaint = rb_ary_entry(rb_complaints, i);
    
    Check_Type(rb_complaint, T_DATA);
    Check_Class(rb_complaint, rb_cComplaint);
    
    Nymble::Complaint* complaint = (Nymble::Complaint*) DATA_PTR(rb_complaint);
    
    complaints.push_back(complaint);
  }
  
  Nymble::Blacklist* new_blacklist = nm->updateBlacklist(server_id, blacklist, &complaints);
  
  if (new_blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cBlacklist, NULL, rb_blacklist_delete, new_blacklist);
}

VALUE rb_nm_create_credential(VALUE rb_self, VALUE rb_server_id, VALUE rb_pseudonym, VALUE rb_time_periods)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_pseudonym, T_DATA);
  Check_Class(rb_pseudonym, rb_cPseudonym);
  Check_Type(rb_time_periods, T_FIXNUM);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Nymble::Pseudonym* pseudonym = (Nymble::Pseudonym*) DATA_PTR(rb_pseudonym);
  u_int time_periods = NUM2UINT(rb_time_periods);
  Nymble::Credential* credential = nm->createCredential(server_id, pseudonym, time_periods);
  
  if (credential == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cCredential, NULL, rb_credential_delete, credential);
}

VALUE rb_nm_create_tokens(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_complaints)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  Check_Type(rb_complaints, T_ARRAY);
  
  Nymble::NymbleManager* nm = (Nymble::NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Nymble::Blacklist* blacklist = (Nymble::Blacklist*) DATA_PTR(rb_blacklist);
  Nymble::Complaints complaints;
  
  for (int i = 0; i < RARRAY_LEN(rb_complaints); i++) {
    VALUE rb_complaint = rb_ary_entry(rb_complaints, i);
    
    Check_Type(rb_complaint, T_DATA);
    Check_Class(rb_complaint, rb_cComplaint);
    
    Nymble::Complaint* complaint = (Nymble::Complaint*) DATA_PTR(rb_complaint);
    
    complaints.push_back(complaint);
  }
  
  Nymble::Tokens* tokens = nm->createTokens(server_id, blacklist, &complaints);
  
  VALUE rb_tokens = rb_ary_new();
  
  if (tokens) {
    for (Nymble::Tokens::iterator token = tokens->begin(); token != tokens->end(); ++token) {
      rb_ary_push(rb_tokens, Data_Wrap_Struct(rb_cToken, NULL, rb_token_delete, *token));
    }
    
    delete tokens;
  }
  
  return rb_tokens;
}

void rb_nm_delete(Nymble::NymbleManager* nm)
{
  delete nm;
}
