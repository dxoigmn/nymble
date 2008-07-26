#ifndef __NYMBLE_SERVER_H__
#define __NYMBLE_SERVER_H__

#include "nymble.h"
#include "nymble_linking_token.h"
#include "nymble_blacklist.h"

typedef std::vector<LinkingToken*> LinkingTokens;

class Server : public Nymble
{
  public:
    u_char  server_id[DIGEST_SIZE];
    u_char  hmac_key_ns[DIGEST_SIZE];
    bool    finalized;
    
    Blacklist*  blacklist;
    LinkingTokens* linking_tokens;
    
    Server();
    ~Server();
    
    u_char* getServerId();
    void setServerId(u_char* server_id);
    
    void setHmacKeyNS(u_char* hmac_key_ns);
    
    Blacklist* getBlacklist();
    void setBlacklist(Blacklist* blacklist);
    
    bool verifyTicket(Ticket* ticket);
};

#endif
