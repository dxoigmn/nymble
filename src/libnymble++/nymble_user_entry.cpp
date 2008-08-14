#include "nymble_user_entry.h"

namespace Nymble {

UserEntry::UserEntry(Blacklist* blacklist)
{
  this->setBlacklist(blacklist);
  this->credential = NULL;
}

u_char* UserEntry::getServerId()
{
  return this->blacklist->getServerId();
}

bool UserEntry::isBlacklisted()
{
  bool blacklisted = false;
  
  if (this->blacklist == NULL || this->credential == NULL) {
    return false;
  }
  
  u_char nymble0[DIGEST_SIZE];
  
  Ticket::computeNymble(this->credential->getSeed(), nymble0);
  
  for (Nymbles::iterator nymble = this->blacklist->begin(); nymble != this->blacklist->end(); ++nymble) {
    if (memcmp(*nymble, nymble0, DIGEST_SIZE) == 0) {
      blacklisted = true;
    }
  }
  
  return blacklisted;
}

Ticket* UserEntry::getTicket(u_int time_period)
{
  if (this->credential == NULL) {
    return NULL;
  }
  
  for (Tickets::iterator ticket = this->credential->begin(); ticket != this->credential->end(); ++ticket) {
    if ((*ticket)->getTimePeriod() == time_period) {
      return *ticket;
    }
  }
  
  return NULL;
}

void UserEntry::setCredential(Credential* credential)
{
  if (credential != NULL) {
    this->credential = new Credential(credential);
  }
}

void UserEntry::setBlacklist(Blacklist* blacklist)
{
  if (blacklist != NULL) {
    this->blacklist = new Blacklist(blacklist);
  }
}

}; // namespace Nymble
