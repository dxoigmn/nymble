#include "nymble_token.h"

Token::Token(u_char* server_id, u_int link_window, u_int time_period, u_char* trapdoor, Ticket *ticket)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);
  memcpy(this->trapdoor, trapdoor, DIGEST_SIZE);
  this->link_window = link_window;
  this->time_period = time_period;
  this->ticket = new Ticket(ticket);
}

Token::~Token()
{
  delete this->ticket;
}