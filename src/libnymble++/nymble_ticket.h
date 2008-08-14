#ifndef __NYMBLE_TICKET_H__
#define __NYMBLE_TICKET_H__

#include <openssl/aes.h>
#include <openssl/hmac.h>

#include "nymble.h"

namespace Nymble {

class Ticket;
typedef std::vector<Ticket*> Tickets;

class Ticket
{
  u_int   link_window;
  u_int   time_period;
  u_char  server_id[DIGEST_SIZE];
  u_char  nymble[DIGEST_SIZE];
  u_char  trapdoorenc[TRAPDOORENC_SIZE];
  u_char  mac_n[DIGEST_SIZE];
  u_char  mac_ns[DIGEST_SIZE];
  
  
  public:
    Ticket();
    Ticket(u_int link_window, u_int time_period, u_char* server_id, u_char* nymble);
    Ticket(Ticket* ticket);
    
    u_int getLinkWindow();
    u_int getTimePeriod();
    u_char* getServerId();
    u_char* getNymble();
    u_char* getMacNS();
    
    void hmac(u_char* hmac_key, bool include_mac_n = false, u_char* out = NULL);
    void encrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym);
    void decrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym);
    
    void marshal(json_object* out);
    u_int marshal(char* out = NULL);
    static void unmarshal(char* bytes, Ticket* out);
    static void unmarshal(json_object* ticket, Ticket* out);
    
    static void computeNymble(u_char *trapdoor, u_char *out);
    static void evolveTrapdoor(u_char* trapdoor, u_int delta, u_char *out);
};

}; // namespace Nymble

#endif
