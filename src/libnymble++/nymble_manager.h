#ifndef __NYMBLE_MANAGER_H__
#define __NYMBLE_MANAGER_H__

#include "nymble.h"
#include "nymble_manager_entry.h"
#include "nymble_server.h"

#include "nymble_blacklist.pb.h"
#include "nymble_blacklist_cert.pb.h"
#include "nymble_pseudonym.pb.h"
#include "nymble_ticket.pb.h"
#include "nymble_complaint.pb.h"
#include "nymble_token.pb.h"

namespace Nymble {

typedef std::vector<NymbleManagerEntry*> NymbleManagerEntries;
typedef std::vector<std::string> Seeds;
typedef std::vector<Complaint*> Complaints;

class NymbleManager : public Nymble
{
  std::string mac_key_n;
  std::string mac_key_np;
  std::string seed_key_n;
  std::string enc_key_n;
  RSA sign_key_n;
  NymbleManagerEntries entries;
  
  NymbleManagerEntry* findServer(std::string sid);
  
  public:
    NymbleManager();
    ~NymbleManager();
    
    std::string getMacKeyNP();
    
    bool verifyPseudonym(Pseudonym pseudonym);
    bool createCredential(std::string sid, Pseudonym pseudonym, Credential& credential);
    bool verifyTicket(std::string sid, Ticket ticket);
    bool signBlacklist(std::string sid, std::string target, Blacklist blist, BlacklistCert& cert);
    bool verifyBlacklist(std::string sid, Blacklist blist, BlacklistCert cert);
    bool registerServer(std::string sid, Server& server);
    bool computeBlacklistUpdate(std::string sid, Blacklist blist, Complaints clist, Blacklist& blist_out, BlacklistCert& cert_out);
    bool computeTokens(u_int t_prime, Blacklist* blist, Complaints clist, Seeds& seeds);
};

}; // namespace Nymble

#endif