#ifndef __NYMBLE_MANAGER_H__
#define __NYMBLE_MANAGER_H__

#include <openssl/pem.h>

#include "nymble.h"
#include "nymble_blacklist.h"
#include "nymble_manager_entry.h"
#include "nymble_pseudonym.h"
#include "nymble_ticket.h"
#include "nymble_complaint.h"
#include "nymble_token.h"

namespace Nymble {

class Blacklist;
class NymbleManagerEntry;
class Credential;

typedef std::vector<NymbleManagerEntry*> NymbleManagerEntries;
typedef std::vector<Token*> Tokens;
typedef std::vector<u_char*> Nymbles;

class NymbleManager : public Nymble
{
  u_char  mac_key_n[DIGEST_SIZE];
  u_char  mac_key_np[DIGEST_SIZE];
  u_char  seed_key_n[DIGEST_SIZE];
  u_char  enc_key_n[CIPHER_BLOCK_SIZE];
  RSA*    sign_key_n;
  
  NymbleManagerEntries* entries;
  
  bool userIsBlacklisted(Nymbles* nymbles, u_char* nymble0);
  NymbleManagerEntry* findServer(u_char *server_id);
  
  public:
    NymbleManager();
    ~NymbleManager();
    
    u_char* getMacKeyN();
    u_char* getEncryptKeyN();
    
    void setMacKeyNP(u_char* mac_key_np);
    
    void readSignKey(char* sign_key_path);
    
    u_char* addServer(u_char* server_id);
    bool verifyPseudonym(Pseudonym* pseudonym);
    bool verifyBlacklist(u_char* server_id, Blacklist* blacklist);
    Blacklist* createBlacklist(u_char* server_id);
    Blacklist* updateBlacklist(u_char* server_id, Blacklist* blacklist, Complaints* complaints);
    Tokens* createTokens(u_char* server_id, Blacklist* blacklist, Complaints* complaints);
    Credential* createCredential(u_char* server_id, Pseudonym* pseudonym, u_int time_periods);
    bool verifyTicket(Ticket* ticket, u_char* sid);
    
    void seed(NymbleManagerEntry *entry, u_char *pseudonym, u_char *out);
};

}; // namespace Nymble

#endif