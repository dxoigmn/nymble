#include "nymble_user_entry.h"

UserEntry::UserEntry(u_char* server_id, Blacklist* blacklist, Credential* credential)
{
  memcpy(this->server_id, server_id, DIGEST_SIZE);
  this->blacklist = new Blacklist(blacklist);
  this->credential = new Credential(credential);
}

u_char* UserEntry::getServerId()
{
  return this->server_id;
}

Credential* UserEntry::getCredential()
{
  return this->credential;
}