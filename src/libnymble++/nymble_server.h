#ifndef __NYMBLE_SERVER_H__
#define __NYMBLE_SERVER_H__

#include "nymble.h"
#include "nymble_token.h"
#include "nymble_blacklist.h"

namespace Nymble {

typedef std::vector<Token*> Tokens;

class Server : public Nymble
{
  u_char  sid[DIGEST_SIZE];
  u_char  mac_key_ns[DIGEST_SIZE];
  bool    finalized;
  
  Blacklist*  blacklist;
  Tokens* tokens;
  
  public:
    Server();
    ~Server();
    
    u_char* getServerId();
    void setServerId(u_char* sid);
    
    void setTimePeriod(u_int time_period);
    void setMacKeyNS(u_char* mac_key_ns);
    
    bool isFinalized();
    
    Blacklist* getBlacklist();
    void setBlacklist(Blacklist* blacklist);
    
    void addTokens(Tokens* tokens);
    
    bool verifyTicket(Ticket* ticket);
};

}

#endif
