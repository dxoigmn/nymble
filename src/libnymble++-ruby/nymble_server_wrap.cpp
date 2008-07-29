#include "nymble_server_wrap.h"

VALUE rb_server_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_server_delete, new Server());
}

VALUE rb_server_init(VALUE rb_self, VALUE rb_server_id)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  
  server->setServerId(server_id);
  
  return rb_self;
}

VALUE rb_server_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  return INT2FIX(server->getLinkWindow());
}

VALUE rb_server_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  server->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_server_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  return INT2FIX(server->getTimePeriod());
}

VALUE rb_server_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  server->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_server_server_id(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  return rb_str_new((char*) server->getServerId(), DIGEST_SIZE);
}

VALUE rb_server_finalized(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  if (server->isFinalized()) {
    return Qtrue;
  }
  
  return Qfalse;
}

VALUE rb_server_hmac_key_ns_set(VALUE rb_self, VALUE rb_hmac_key_ns)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_hmac_key_ns, T_STRING);
  Check_Size(rb_hmac_key_ns, DIGEST_SIZE);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  u_char* hmac_key_ns = (u_char*) RSTRING_PTR(rb_hmac_key_ns);
  
  server->setHmacKeyNS(hmac_key_ns);
  
  return rb_self;
}

VALUE rb_server_blacklist(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  Blacklist* blacklist = server->getBlacklist();
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  return Data_Wrap_Struct(rb_cBlacklist, NULL, NULL, server->getBlacklist());
}

VALUE rb_server_blacklist_set(VALUE rb_self, VALUE rb_blacklist)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  
  server->setBlacklist(blacklist);

  return rb_self;
}

VALUE rb_server_add_linking_tokens(VALUE rb_self, VALUE rb_linking_tokens)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_linking_tokens, T_ARRAY);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  LinkingTokens linking_tokens;
  
  for (int i = 0; i < RARRAY_LEN(rb_linking_tokens); i++) {
    VALUE rb_linking_token = rb_ary_entry(rb_linking_tokens, i);
    
    Check_Type(rb_linking_token, T_DATA);
    Check_Class(rb_linking_token, rb_cLinkingToken);
    
    LinkingToken* linking_token = (LinkingToken*) DATA_PTR(rb_linking_token);
    
    linking_tokens.push_back(linking_token);
  }
  
  server->addLinkingTokens(&linking_tokens);
  
  return rb_self;
}

VALUE rb_server_verify_ticket(VALUE rb_self, VALUE rb_ticket)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_ticket, T_DATA);
  Check_Class(rb_ticket, rb_cTicket);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  Ticket* ticket = (Ticket*) DATA_PTR(rb_ticket);
  
  if (server->verifyTicket(ticket)) {
    return Qtrue;
  }
  
  return Qfalse;
}


void rb_server_delete(Server* server)
{
  delete server;
}
