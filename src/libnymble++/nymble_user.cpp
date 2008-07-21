#include "nymble_user.h"

User::User()
{
  this->pseudonym = NULL;
  this->verify_key_n = NULL;
}

User::~User()
{
  RSA_free(this->verify_key_n);
  
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    delete *entry;
  }
  
  delete this->pseudonym;
}

void User::setPseudonym(Pseudonym* pseudonym)
{
  this->pseudonym = new Pseudonym(pseudonym);
}

Pseudonym* User::getPseudonym()
{
  return this->pseudonym;
}

void User::setVerifyKeyN(u_char** verify_key_n, u_int verify_key_n_len)
{
  this->verify_key_n = d2i_RSAPublicKey(NULL, (const u_char**)verify_key_n, verify_key_n_len);
}

UserEntry* User::addServer(u_char* server_id, Blacklist* blacklist, Credential* credential)
{
  UserEntry* entry = new UserEntry(server_id, blacklist, credential);
  
  this->push_back(entry);
  
  return entry;
}

UserEntry* User::findServer(u_char* server_id)
{
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    if (memcmp((*entry)->getServerId(), server_id, DIGEST_SIZE) == 0) {
      return *entry;
    }
  }
  
  return NULL;
}

Ticket* User::getTicket(u_char* server_id)
{
  UserEntry* entry = findServer(server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  for (Tickets::iterator ticket = entry->getCredential()->begin(); ticket != entry->getCredential()->end(); ++ticket) {
    if ((*ticket)->getTimePeriod() == this->cur_time_period) {
      return *ticket;
    }
  }
  
  return NULL;
}