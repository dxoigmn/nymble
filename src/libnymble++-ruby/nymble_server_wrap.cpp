#include "nymble_server_wrap.h"

VALUE rb_server_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_server_delete, new Server());
}

VALUE rb_server_init(VALUE rb_self, VALUE rb_server_id, VALUE rb_hmac_key_ns, VALUE rb_blacklist)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_server_id, T_STRING);
  Check_Size(rb_server_id, DIGEST_SIZE);
  Check_Type(rb_hmac_key_ns, T_STRING);
  Check_Size(rb_hmac_key_ns, DIGEST_SIZE);
  Check_Type(rb_blacklist, T_DATA);
  Check_Class(rb_blacklist, rb_cBlacklist);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  u_char* server_id = (u_char*) RSTRING_PTR(rb_server_id);
  u_char* hmac_key_ns = (u_char*) RSTRING_PTR(rb_hmac_key_ns);
  Blacklist* blacklist = (Blacklist*) DATA_PTR(rb_blacklist);
  
  server->setServerId(server_id);
  server->setHmacKeyNS(hmac_key_ns);
  server->setBlacklist(blacklist);
  
  return rb_self;
}

VALUE rb_server_server_id(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  return rb_str_new((char*) server->getServerId(), DIGEST_SIZE);
}

VALUE rb_server_blacklist(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Server* server = (Server*) DATA_PTR(rb_self);
  
  return Data_Wrap_Struct(rb_cBlacklist, NULL, NULL, server->getBlacklist());
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
