#include "nymble_server.h"

namespace Nymble {

bool Server::verifyTicket(Ticket ticket)
{
  char mac[DIGEST_SIZE];
  HMAC_CTX hmac_ctx;
  
  HMAC_Init(&hmac_ctx, (u_char*)this->mac_key_ns().c_str(), this->mac_key_ns().size(), EVP_sha256());
  HMAC_Update(&hmac_ctx, (u_char*)this->sid().c_str(), this->sid().size());
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_time_period, sizeof(this->cur_time_period));
  HMAC_Update(&hmac_ctx, (u_char*)&this->cur_link_window, sizeof(this->cur_link_window));
  HMAC_Update(&hmac_ctx, (u_char*)ticket.nymble().c_str(), ticket.nymble().size());
  HMAC_Update(&hmac_ctx, (u_char*)ticket.ctxt().c_str(), ticket.ctxt().size());
  HMAC_Update(&hmac_ctx, (u_char*)ticket.mac_n().c_str(), ticket.mac_n().size());
  HMAC_Final(&hmac_ctx, (u_char*)mac, NULL);
  
  return (ticket.mac_ns() == std::string(mac, sizeof(mac)));
}

bool Server::linkTicket(Ticket ticket)
{
  bool isLinked = false;
  
  for (int i = 0; i < this->llist().tokens_size(); i++) {
    if (ticket.nymble() == this->llist().tokens(i).nymble()) {
      isLinked = true;
    }
  }
  
  return isLinked;
}

bool Server::isValidTicket(Ticket ticket)
{
  if (!this->verifyTicket(ticket)) {
    return false;
  }
  
  if (this->linkTicket(ticket)) {
    return false;
  }
  
  return true;
}

void Server::add_complaint(Ticket ticket, u_int time)
{
  Complaint* complaint = this->mutable_clist()->add_complaints();
  
  complaint->mutable_ticket()->CopyFrom(ticket);
  complaint->set_time(time);
}

bool Server::complain(ServerState* server_state)
{
  if (this->clist().complaints_size() == 0) {
    return false;
  }
  
  server_state->mutable_blist()->CopyFrom(this->blist());
  server_state->mutable_cert()->CopyFrom(this->cert());
  server_state->mutable_clist()->CopyFrom(this->clist());
  
  this->mutable_clist()->clear_complaints();
  
  return true;
}


}; // namespace Nymble
