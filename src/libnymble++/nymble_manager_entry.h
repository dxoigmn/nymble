#ifndef __NYMBLE_MANAGER_ENTRY_H__
#define __NYMBLE_MANAGER_ENTRY_H__

#include "nymble.h"
#include "nymble_credential.h"

class NymbleManagerEntry {
  u_char  server_id[DIGEST_SIZE];
  u_int   bl_last_updated;
  u_char  hmac_key_ns[DIGEST_SIZE];
  
  public:
    NymbleManagerEntry(u_char* server_id, u_int cur_time_period);
    
    void setServerId(u_char* server_id);
    u_char* getServerId();
    
    void setLastUpdated(u_int bl_last_updated);
    u_int getLastUpdated();
    
    void setHmacKeyNS(u_char* hmac_key_ns);
    u_char* getHmacKeyNS();
    
    void signCredential(Credential* credential);
};

#endif
