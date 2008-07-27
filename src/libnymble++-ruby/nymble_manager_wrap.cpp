#include "nymble_manager_wrap.h"

VALUE rb_nm_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_nm_delete, new NymbleManager());
}

VALUE rb_nm_init(VALUE rb_self, VALUE rb_hmac_key_np)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_hmac_key_np, T_STRING);
  Check_Size(rb_hmac_key_np, DIGEST_SIZE);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* hmac_key_np = (u_char*) RSTRING_PTR(rb_hmac_key_np);
  
  nm->setHmacKeyNP(hmac_key_np);
  
  return rb_self;
}

VALUE rb_nm_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  
  return INT2FIX(nm->getLinkWindow());
}

VALUE rb_nm_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_link_window, T_FIXNUM);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  nm->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_nm_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  
  return INT2FIX(nm->getTimePeriod());
}

VALUE rb_nm_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_time_period, T_FIXNUM);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  nm->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_nm_verify_key(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* verify_key_n = NULL;
  u_int verify_key_n_len = nm->getVerifyKeyN(&verify_key_n);
  
  VALUE rb_verify_key_n = rb_str_new((char*) verify_key_n, verify_key_n_len);
  
  free(verify_key_n);
  
  return rb_verify_key_n;
}

VALUE rb_nm_add_server(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
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
    
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  Pseudonym* pseudonym = (Pseudonym*) DATA_PTR(rb_pseudonym);
  
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
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  
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
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Blacklist* blacklist = nm->createBlacklist(server_id);
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cBlacklist, NULL, NULL, blacklist);
}

VALUE rb_nm_update_blacklist(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_tickets)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  Check_Type(rb_tickets, T_ARRAY);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  Tickets tickets;
  
  for (int i = 0; i < RARRAY_LEN(rb_tickets); i++) {
    VALUE rb_ticket = rb_ary_entry(rb_tickets, i);
    
    Check_Type(rb_ticket, T_DATA);
    Check_Class(rb_ticket, rb_cTicket);
    
    Ticket* ticket = (Ticket*) DATA_PTR(rb_ticket);
    
    tickets.push_back(ticket);
  }
  
  Blacklist* new_blacklist = nm->updateBlacklist(server_id, blacklist, &tickets);
  
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
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Pseudonym* pseudonym = (Pseudonym*) DATA_PTR(rb_pseudonym);
  u_int time_periods = NUM2UINT(rb_time_periods);
  Credential* credential = nm->createCredential(server_id, pseudonym, time_periods);
  
  if (credential == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cCredential, NULL, rb_credential_delete, credential);
}

VALUE rb_nm_create_linking_tokens(VALUE rb_self, VALUE rb_server_id, VALUE rb_blacklist, VALUE rb_tickets)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cNymbleManager);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  Check_Type(rb_tickets, T_ARRAY);
  
  NymbleManager* nm = (NymbleManager*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  Tickets tickets;
  
  for (int i = 0; i < RARRAY_LEN(rb_tickets); i++) {
    VALUE rb_ticket = rb_ary_entry(rb_tickets, i);
    
    Check_Type(rb_ticket, T_DATA);
    Check_Class(rb_ticket, rb_cTicket);
    
    Ticket* ticket = (Ticket*) DATA_PTR(rb_ticket);
    
    tickets.push_back(ticket);
  }
  
  LinkingTokens* linking_tokens = nm->createLinkingTokens(server_id, blacklist, &tickets);
  
  VALUE rb_linking_tokens = rb_ary_new();
  
  if (linking_tokens) {
    for (LinkingTokens::iterator linking_token = linking_tokens->begin(); linking_token != linking_tokens->end(); ++linking_token) {
      rb_ary_push(rb_linking_tokens, Data_Wrap_Struct(rb_cLinkingToken, NULL, rb_linking_token_delete, *linking_token));
    }
    
    delete linking_tokens;
  }
  
  return rb_linking_tokens;
}

void rb_nm_delete(NymbleManager* nm)
{
  delete nm;
}
