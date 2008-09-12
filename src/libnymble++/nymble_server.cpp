#include "nymble_server.h"

namespace Nymble {

Server::Server()
{
  this->finalized = false;
  this->tokens = new Tokens();
  this->blacklist = NULL;
}

Server::~Server()
{
  for (Tokens::iterator token = this->tokens->begin(); token != this->tokens->end(); ++token) {
    delete *token;
  }
  
  delete this->tokens;
  delete this->blacklist;
}

u_char* Server::getServerId()
{
  return this->server_id;
}

void Server::setServerId(u_char* server_id)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);
}

void Server::setTimePeriod(u_int time_period)
{
  if (time_period == this->cur_time_period) {
    return;
  }
  
  for (Tokens::iterator token = this->tokens->begin(); token != this->tokens->end(); ++token) {
    (*token)->evolve(time_period - this->cur_time_period);
  }
  
  Nymble::setTimePeriod(time_period);
  
  this->finalized = false;
}

void Server::setHmacKeyNS(u_char* hmac_key_ns)
{
  memcpy(this->hmac_key_ns, hmac_key_ns, DIGEST_SIZE);
}

bool Server::isFinalized()
{
  return this->finalized;
}

Blacklist* Server::getBlacklist()
{
  return this->blacklist;
}

void Server::setBlacklist(Blacklist* blacklist)
{
  this->blacklist = new Blacklist(blacklist);
  this->finalized = true;
}

void Server::addTokens(Tokens* tokens)
{
  for (Tokens::iterator token = tokens->begin(); token != tokens->end(); ++token) {
    this->tokens->push_back(new Token(*token));
  }
}

bool Server::verifyTicket(Ticket* ticket)
{
  bool valid = true;
  u_char mac_ns[DIGEST_SIZE];
  
  ticket->hmac(this->hmac_key_ns, this->server_id, this->cur_link_window, this->cur_time_period, true, mac_ns);
  
  if (memcmp(ticket->getMacNS(), mac_ns, DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  for (Tokens::iterator token = this->tokens->begin(); token != this->tokens->end(); ++token) {
    if (memcmp((*token)->getNymble(), ticket->getNymble(), DIGEST_SIZE) == 0) {
      valid = false;
    }
  }
  
  return valid;
}

}; // namespace Nymble
