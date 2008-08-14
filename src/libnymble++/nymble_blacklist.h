#ifndef __NYMBLE_BLACKLIST_H__
#define __NYMBLE_BLACKLIST_H__

#include "nymble.h"
#include "nymble_credential.h"

typedef std::vector<u_char*> Nymbles;

class Blacklist : public Nymbles
{
  u_char  server_id[DIGEST_SIZE];
  u_int   link_window;
  u_int   time_period;
  u_char  bl_hash[DIGEST_SIZE];
  u_char  bmac_n[DIGEST_SIZE];
  u_char  sig[SIGNATURE_SIZE];
  
  public:
    Blacklist();
    Blacklist(Blacklist* blacklist);
    Blacklist(u_char* server_id, u_int link_window, u_int time_period);
    ~Blacklist();
    
    u_char* getServerId();
    u_char* getHash();
    u_char* getHmac();
    u_int getLinkWindow();
    u_int getTimePeriod();
    
    void hash(u_char* out = NULL);
    void hmac(u_char* hmac_key_n, u_char* out = NULL);
    void sign(RSA* sign_key_n, u_char* out = NULL);
    
    bool verify(RSA* verify_key_n, u_int link_window, u_int time_period);
    
    u_int marshal(char* out = NULL);
    static void unmarshal(char* bytes, Blacklist* out);
};

#endif
