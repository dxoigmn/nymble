#ifndef __NYMBLE_CREDENTIAL_H__
#define __NYMBLE_CREDENTIAL_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_pseudonym.h"
#include "nymble_manager_entry.h"
#include "nymble_manager.h"
#include "nymble_credential.pb.h"

namespace Nymble {

class NymbleManager;
class NymbleManagerEntry;

class Credential : public Tickets
{
  u_char nymble0[DIGEST_SIZE];
  
  void seedTrapdoor(NymbleManager* nm, NymbleManagerEntry *entry, u_char *pseudonym);
  
  public:
    Credential();
    Credential(Credential* credential);
    Credential(Marshal::Credential* credential);
    Credential(NymbleManager* nm, NymbleManagerEntry* entry, Pseudonym* pseudonym, u_int time_periods);
    ~Credential();
    
    u_char* getNymble0();
    
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Credential* unmarshal(u_char* bytes, u_int size);
};

}; // namespace Nymble

#endif
