#include "nymble_server.h"

Server::Server()
{
  this->finalized = false;
  this->linking_tokens = new LinkingTokens();
  this->blacklist = NULL;
}

Server::~Server()
{
  for (LinkingTokens::iterator linking_token = this->linking_tokens->begin(); linking_token != this->linking_tokens->end(); ++linking_token) {
    delete *linking_token;
  }
  
  delete this->linking_tokens;
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
  
  Nymble::setTimePeriod(time_period);
  
  for (LinkingTokens::iterator linking_token = this->linking_tokens->begin(); linking_token != this->linking_tokens->end(); ++linking_token) {
    (*linking_token)->setTimePeriod(time_period);
  }
  
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

bool Server::setBlacklist(Blacklist* blacklist)
{
  this->blacklist = new Blacklist(blacklist);
  // TODO: Should verify blacklist here.
  
  this->finalized = true;
  
  return true;
}

void Server::addLinkingTokens(LinkingTokens* linking_tokens)
{
  for (LinkingTokens::iterator linking_token = linking_tokens->begin(); linking_token != linking_tokens->end(); ++linking_token) {
    this->linking_tokens->push_back(new LinkingToken(*linking_token));
  }
}

bool Server::verifyTicket(Ticket* ticket)
{
  bool valid = true;
  u_char mac_ns[DIGEST_SIZE];
  
  ticket->hmac(this->hmac_key_ns, true, mac_ns);
  
  if (memcmp(ticket->getServerId(), this->server_id, DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  if (ticket->getLinkWindow() != this->cur_link_window) {
    valid = false;
  }
  
  if (ticket->getTimePeriod() != this->cur_time_period) {
    valid = false;
  }
  
  if (memcmp(ticket->getMacNS(), mac_ns, DIGEST_SIZE) != 0) {
    valid = false;
  }
  
  for (LinkingTokens::iterator linking_token = this->linking_tokens->begin(); linking_token != this->linking_tokens->end(); ++linking_token) {
    if (memcmp((*linking_token)->getNymble(), ticket->getNymble(), DIGEST_SIZE) == 0) {
      valid = false;
    }
  }
  
  return valid;
}
