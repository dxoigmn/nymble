#include "nymble_user_entry.h"

UserEntry::UserEntry(Blacklist* blacklist)
{
  this->setBlacklist(blacklist);
  this->credential = NULL;
}

u_char* UserEntry::getServerId()
{
  return this->blacklist->getServerId();
}

Credential* UserEntry::getCredential()
{
  return this->credential;
}

void UserEntry::setCredential(Credential* credential)
{
  if (credential != NULL) {
    this->credential = new Credential(credential);
  }
}

Blacklist* UserEntry::getBlacklist()
{
  return this->blacklist;
}

void UserEntry::setBlacklist(Blacklist* blacklist)
{
  if (blacklist != NULL) {
    this->blacklist = new Blacklist(blacklist);
  }
}