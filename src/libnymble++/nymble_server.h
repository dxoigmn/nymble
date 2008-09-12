#ifndef __NYMBLE_SERVER_H__
#define __NYMBLE_SERVER_H__

#include "nymble.h"
#include "nymble_token.h"
#include "nymble_blacklist.h"

namespace Nymble {

typedef std::vector<Token*> Tokens;

class Server : public Nymble
{
  u_char  server_id[DIGEST_SIZE];
  u_char  hmac_key_ns[DIGEST_SIZE];
  bool    finalized;
  
  Blacklist*  blacklist;
  Tokens* tokens;
  
  public:
    Server();
    ~Server();
    
    u_char* getServerId();
    void setServerId(u_char* server_id);
    
    void setTimePeriod(u_int time_period);
    void setHmacKeyNS(u_char* hmac_key_ns);
    
    bool isFinalized();
    
    Blacklist* getBlacklist();
    void setBlacklist(Blacklist* blacklist);
    
    void addTokens(Tokens* tokens);
    
    bool verifyTicket(Ticket* ticket);
};

}

#endif
