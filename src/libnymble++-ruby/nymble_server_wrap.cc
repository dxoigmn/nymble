#include "nymble_server_wrap.h"

VALUE rb_cServer;

VALUE rb_server_new(VALUE rb_self)
{
  return Data_Wrap_Struct(rb_self, NULL, rb_server_delete, new Nymble::Server());
}

VALUE rb_server_init(VALUE rb_self, VALUE rb_server_state_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_server_state_str, T_STRING);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  std::string server_state_str(RSTRING_PTR(rb_server_state_str), RSTRING_LEN(rb_server_state_str));
  
  Nymble::ServerState server_state;
  
  if (!server_state.ParseFromString(server_state_str)) {
    return Qnil;
  }
  
  server->CopyFrom(server_state);
  server->setTimePeriod(server->cert().t());
  
  return rb_self;
}

VALUE rb_server_link_window(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  
  return INT2FIX(server->getLinkWindow());
}

VALUE rb_server_link_window_set(VALUE rb_self, VALUE rb_link_window)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_link_window, T_FIXNUM);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  u_int link_window = FIX2UINT(rb_link_window);
  
  server->setLinkWindow(link_window);
  
  return rb_self;
}

VALUE rb_server_time_period(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  
  return INT2FIX(server->getTimePeriod());
}

VALUE rb_server_time_period_set(VALUE rb_self, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_time_period, T_FIXNUM);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  u_int time_period = FIX2UINT(rb_time_period);
  
  server->setTimePeriod(time_period);
  
  return rb_self;
}

VALUE rb_server_blacklist(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  Nymble::Blacklist* blacklist = server->mutable_blist();
  
  if (blacklist == NULL) {
    return Qnil;
  }
  
  std::string blacklist_str;
  
  if (!blacklist->SerializeToString(&blacklist_str)) {
    return Qnil;
  }
  
  return rb_str_new(blacklist_str.data(), blacklist_str.size());
}

VALUE rb_server_blacklist_cert(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  Nymble::BlacklistCert* cert = server->mutable_cert();
  
  if (cert == NULL) {
    return Qnil;
  }
  
  std::string cert_str;
  
  if (!cert->SerializeToString(&cert_str)) {
    return Qnil;
  }
  
  return rb_str_new(cert_str.data(), cert_str.size());
}

VALUE rb_server_valid_ticket(VALUE rb_self, VALUE rb_ticket_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_ticket_str, T_STRING);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  std::string ticket_str(RSTRING_PTR(rb_ticket_str), RSTRING_LEN(rb_ticket_str));
  
  Nymble::Ticket ticket;
  
  if (!ticket.ParseFromString(ticket_str)) {
    return Qfalse;
  }
  
  if (!server->isValidTicket(ticket)) {
    return Qfalse;
  }
  
  return Qtrue;
}

VALUE rb_server_add_complaint(VALUE rb_self, VALUE rb_ticket_str, VALUE rb_time_period)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_ticket_str, T_STRING);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  std::string ticket_str(RSTRING_PTR(rb_ticket_str), RSTRING_LEN(rb_ticket_str));
  u_int time_period = NUM2UINT(rb_time_period);
  
  Nymble::Ticket ticket;
  
  if (!ticket.ParseFromString(ticket_str)) {
    return Qfalse;
  }
  
  server->add_complaint(ticket, time_period);
  
  return rb_self;
}

VALUE rb_server_complain(VALUE rb_self)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  
  Nymble::ServerState server_state;
  
  if (!server->complain(&server_state)) {
    return rb_str_new("", 0); // Return empty string
  }
  
  std::string server_state_str;
  
  if (!server_state.SerializeToString(&server_state_str)) {
    return Qnil;
  }
  
  return rb_str_new(server_state_str.data(), server_state_str.size());
}

VALUE rb_server_update(VALUE rb_self, VALUE rb_server_state_str)
{
  Check_Type(rb_self, T_DATA);
  Check_Class(rb_self, rb_cServer);
  Check_Type(rb_server_state_str, T_STRING);
  
  Nymble::Server* server = (Nymble::Server*) DATA_PTR(rb_self);
  std::string server_state_str(RSTRING_PTR(rb_server_state_str), RSTRING_LEN(rb_server_state_str));
  
  Nymble::ServerState server_state;
  
  if (!server_state.ParseFromString(server_state_str)) {
    return Qnil;
  }
  
  server->update(server_state);
  
  return rb_self;
}

void rb_server_delete(Nymble::Server* server)
{
  delete server;
}
