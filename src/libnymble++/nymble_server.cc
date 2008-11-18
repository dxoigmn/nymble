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
  
}; // namespace Nymble
