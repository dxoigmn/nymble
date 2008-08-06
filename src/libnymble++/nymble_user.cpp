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

void User::readVerifyKey(char* verify_key_path)
{
  FILE* verify_key = fopen(verify_key_path, "r");
  
  this->verify_key_n  = PEM_read_RSA_PUBKEY(verify_key, NULL, NULL, NULL);
  
  fclose(verify_key);
}

u_char* User::addBlacklist(Blacklist* blacklist)
{
  if (!blacklist->verify(this->verify_key_n, this->cur_link_window, this->cur_time_period)) {
    return NULL;
  }
  
  UserEntry* entry = this->findEntry(blacklist->getServerId());
  
  if (entry == NULL) {
    entry = new UserEntry(blacklist);
    this->push_back(entry);
  } else {
    entry->setBlacklist(blacklist);
  }
  
  return entry->getServerId();
}

bool User::addCredential(Credential* credential)
{
  Ticket* ticket = credential->front();
  UserEntry* entry = this->findEntry(ticket->getServerId());
  
  if (entry == NULL) {
    return false;
  }
  
  entry->setCredential(credential);
  
  return true;
}

Ticket* User::getTicket(u_char* server_id)
{
  UserEntry* entry = this->findEntry(server_id);
  
  if (entry == NULL || entry->isBlacklisted()) {
    return NULL;
  }
  
  return entry->getTicket(this->cur_time_period);
}

UserEntry* User::findEntry(u_char* server_id)
{
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    if (memcmp((*entry)->getServerId(), server_id, DIGEST_SIZE) == 0) {
      return *entry;
    }
  }
  
  return NULL;
}
