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
  u_char  ctxt[CTXT_SIZE];
  u_char  mac_n[DIGEST_SIZE];
  u_char  mac_ns[DIGEST_SIZE];
  
  public:
    Ticket();
    Ticket(u_char* nymble);
    Ticket(Ticket* ticket);
    Ticket(Marshal::Ticket* ticket);
    
    u_char* getNymble();
    u_char* getMacNS();
    
    void hmac(u_char* mac_key, u_char* sid, u_int t, u_int w, bool include_mac_n = false, u_char* out = NULL);
    void encrypt(u_char* enc_key, u_char* nymble, u_char* seed);
    void decrypt(u_char* enc_key, u_char* nymble, u_char* seed);
    
    bool verify(u_char* mac_key, u_char* sid, u_int t, u_int w);
    
    Complaint* createComplaint(u_int time_period);
    
    void marshal(Marshal::Ticket* out);
    u_int marshal(u_char* out = NULL, u_int size = 0);
    static Ticket* unmarshal(u_char* bytes, u_int size);
    
    static void computeNymble(u_char *seed, u_char *out);
    static void evolveSeed(u_char* seed, u_int delta, u_char *out);
};

}; // namespace Nymble

#endif
