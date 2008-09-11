#include "nymble_user_entry.h"

namespace Nymble {

UserEntry::UserEntry()
{
  
}

UserEntry::UserEntry(u_char* server_id)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);
  
  this->blacklist = NULL;
  this->credential = NULL;
}

u_char* UserEntry::getServerId()
{
  return this->server_id;
}

bool UserEntry::isBlacklisted()
{
  bool blacklisted = false;
  
  if (this->blacklist == NULL || this->credential == NULL) {
    return false;
  }
  
  u_char* nymble0 = this->credential->getNymble0();
  
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
  
  if (time_period > this->credential->size()) {
    return NULL;
  }
  
  return this->credential->at(time_period - 1);
}

void UserEntry::setCredential(Credential* credential)
{
  this->credential = new Credential(credential);
}

void UserEntry::setBlacklist(Blacklist* blacklist)
{
  this->blacklist = new Blacklist(blacklist);
}

}; // namespace Nymble
