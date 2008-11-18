#ifndef __NYMBLE_MANAGER_H__
#define __NYMBLE_MANAGER_H__

#include "nymble.h"
#include "nymble_manager_entry.h"

#include "nymble_server_state.pb.h"
#include "nymble_blacklist.pb.h"
#include "nymble_blacklist_cert.pb.h"
#include "nymble_pseudonym.pb.h"
#include "nymble_credential.pb.h"
#include "nymble_ticket.pb.h"
#include "nymble_complaint.pb.h"
#include "nymble_complaints.pb.h"
#include "nymble_token.pb.h"
#include "nymble_seeds.pb.h"

namespace Nymble {

typedef std::vector<NymbleManagerEntry*> NymbleManagerEntries;

class NymbleManager : public Nymble
{
  std::string mac_key_n;
  std::string mac_key_np;
  std::string seed_key_n;
  std::string enc_key_n;
  RSA* sign_key_n;
  NymbleManagerEntries entries;
  
  NymbleManagerEntry* findServer(std::string sid);
  void computeNymble(std::string seed, std::string* nymble);
  void evolveSeed(std::string seed, int delta, std::string* seed_out);
  bool signBlacklist(std::string sid, std::string target, Blacklist* blist, BlacklistCert* cert);
  bool verifyPseudonym(Pseudonym pseudonym);
  bool verifyTicket(std::string sid, Ticket ticket);
  bool verifyBlacklist(std::string sid, Blacklist blist, BlacklistCert cert);
  
  public:
    NymbleManager();
    ~NymbleManager();
    
    std::string getMacKeyNP();
    bool writeVerifyKeyN(std::string path);
    
    bool createCredential(std::string sid, Pseudonym pnym, Credential* cred);
    bool registerServer(std::string sid, ServerState* server_state);
    bool computeBlacklistUpdate(std::string sid, Blacklist blist, Complaints clist, Blacklist* blist_out, BlacklistCert* cert_out);
    bool computeTokens(u_int t_prime, Blacklist blist, Complaints clist, Seeds* seeds);
};

}; // namespace Nymble

#endif