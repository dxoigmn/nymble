#ifndef __NYMBLE_CREDENTIAL_H__
#define __NYMBLE_CREDENTIAL_H__

#include "nymble.h"
#include "nymble_ticket.h"
#include "nymble_pseudonym.h"
#include "nymble_manager_entry.h"
#include "nymble_manager.h"

class NymbleManager;
class NymbleManagerEntry;

class Credential : public Tickets
{
  u_char  seed[DIGEST_SIZE];
  
  void seedTrapdoor(NymbleManager* nm, NymbleManagerEntry *entry, u_char *pseudonym);
  
  public:
    Credential();
    Credential(Credential* credential);
    Credential(NymbleManager* nm, NymbleManagerEntry* entry, Pseudonym* pseudonym, u_int time_periods);
    ~Credential();
    
    u_char* getSeed();
    
    u_int marshal(char* out = NULL);
    static void unmarshal(char* bytes, Credential* out);
};

#endif
