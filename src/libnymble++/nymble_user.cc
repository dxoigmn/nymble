#include "nymble_user.h"

namespace Nymble {

User::User()
{
  this->pnym = NULL;
  this->verify_key_n = NULL;
}

User::~User()
{
  RSA_free(this->verify_key_n);
  
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    delete *entry;
  }
  
  delete this->pnym;
}

void User::setPseudonym(Pseudonym* pnym)
{
  this->pnym = new Pseudonym(pnym);
}

Pseudonym* User::getPseudonym()
{
  return this->pnym;
}

void User::readVerifyKey(char* verify_key_path)
{
  FILE* verify_key = fopen(verify_key_path, "r");
  
  this->verify_key_n  = PEM_read_RSA_PUBKEY(verify_key, NULL, NULL, NULL);
  
  fclose(verify_key);
}

UserEntry* User::findOrCreateEntry(u_char* server_id)
{
  for (UserEntries::iterator entry = this->begin(); entry != this->end(); ++entry) {
    if (memcmp((*entry)->getServerId(), server_id, DIGEST_SIZE) == 0) {
      return *entry;
    }
  }
  
  UserEntry* entry = new UserEntry(server_id);
  
  this->push_back(entry);
  
  return entry;
}

bool User::verifyBlacklist(Blacklist* blacklist)
{
  return blacklist->verify(this->verify_key_n, this->cur_link_window, this->cur_time_period);
}

}; // namespace Nymble
