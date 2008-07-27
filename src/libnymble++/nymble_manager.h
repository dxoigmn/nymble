#ifndef __NYMBLE_MANAGER_H__
#define __NYMBLE_MANAGER_H__

#include "nymble.h"
#include "nymble_manager_entry.h"
#include "nymble_blacklist.h"
#include "nymble_pseudonym.h"
#include "nymble_linking_token.h"

typedef std::vector<NymbleManagerEntry*> NymbleManagerEntries;
typedef std::vector<LinkingToken*> LinkingTokens;

class NymbleManager : public Nymble
{
  u_char  hmac_key_n[DIGEST_SIZE];
  u_char  hmac_key_np[DIGEST_SIZE];
  u_char  keyedhash_key_n[DIGEST_SIZE];
  u_char  encrypt_key_n[CIPHER_BLOCK_SIZE];
  RSA*    sign_key_n;
  
  NymbleManagerEntries* entries;
  
  void signCredential(Credential* credential, Pseudonym* pseudonym, u_char* seed);
  void encryptTrapdoor(Ticket* ticket, Pseudonym* pseudonym, u_char *seed);
  void decryptTrapdoor(u_char *trapdoor, u_char *pseudonym, Ticket* ticket);
  void seedTrapdoor(u_char *out, NymbleManagerEntry *entry, u_char *pseudonym);
  bool userIsBlacklisted(Nymbles* nymbles, u_char* nymble0);
  NymbleManagerEntry* findServer(u_char *server_id);
  
  public:
    NymbleManager();
    NymbleManager(u_char* hmac_key_np);
    ~NymbleManager();
    
    void setHmacKeyNP(u_char* hmac_key_np);
    
    u_int getSignKeyN(u_char** out = NULL);
    u_int getVerifyKeyN(u_char** out = NULL);
    
    u_char* addServer(u_char* server_id);
    bool verifyPseudonym(Pseudonym* pseudonym);
    bool verifyBlacklist(u_char* server_id, Blacklist* blacklist);
    Blacklist* createBlacklist(u_char* server_id);
    Blacklist* updateBlacklist(u_char* server_id, Blacklist* blacklist, Tickets* complaints);
    LinkingTokens* createLinkingTokens(u_char* server_id, Blacklist* blacklist, Tickets* complaints);
    Credential* createCredential(u_char* server_id, Pseudonym* pseudonym, u_int time_periods);
};

#endif