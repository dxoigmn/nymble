#ifndef __NYMBLE_TICKET_H__
#define __NYMBLE_TICKET_H__

#include <openssl/aes.h>
#include <openssl/hmac.h>

#include "nymble.h"
#include "nymble_complaint.h"
#include "nymble_ticket.pb.h"

namespace Nymble {

class Complaint;
class Ticket;
typedef std::vector<Ticket*> Tickets;

class Ticket
{
  u_char  nymble[DIGEST_SIZE];
  u_char  trapdoorenc[TRAPDOORENC_SIZE];
  u_char  mac_n[DIGEST_SIZE];
  u_char  mac_ns[DIGEST_SIZE];
  
  public:
    Ticket();
    Ticket(u_char* nymble);
    Ticket(Ticket* ticket);
    Ticket(Marshal::Ticket* ticket);
    
    u_char* getNymble();
    u_char* getMacNS();
    
    void hmac(u_char* hmac_key, u_char* server_id, u_int link_window, u_int time_period, bool include_mac_n = false, u_char* out = NULL);
    void encrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym);
    void decrypt(u_char* encrypt_key_n, u_char* trapdoor, u_char* pseudonym);
    
    Complaint* createComplaint(u_int time_period);
    
    void marshal(Marshal::Ticket* out);
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Ticket* unmarshal(u_char* bytes, u_int size);
    
    static void computeNymble(u_char *trapdoor, u_char *out);
    static void evolveTrapdoor(u_char* trapdoor, u_int delta, u_char *out);
};

}; // namespace Nymble

#endif
