#include "nymble_user_entry.h"

namespace Nymble {

UserEntry::UserEntry(std::string sid, Credential cred)
{
  this->sid = sid;
  this->credential.CopyFrom(cred);
  this->has_been_silent = true;
}

std::string UserEntry::getServerId()
{
  return this->sid;
}

Credential* UserEntry::getCredential()
{
  return &this->credential;
}

}; // namespace Nymble
