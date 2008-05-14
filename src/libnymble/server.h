#ifndef __SERVER_H__
#define __SERVER_H__

#include "nymble.h"
#include "nymble_sexp.h"

typedef struct {
  u_int   time_period;
  u_char  trapdoor[DIGEST_SIZE];
  u_char  nymble[DIGEST_SIZE];
  void    *next;
} linking_list_entry_t;

typedef struct {
  u_char                server_id[DIGEST_SIZE];
  u_char                hmac_key_ns[DIGEST_SIZE];
  u_int                 finalized;
  blacklist_t           *blacklist;
  linking_list_entry_t  *linking_list;
} server_t;

server_t* server_initialize(u_char *server_id, u_char *hmac_key_ns, blacklist_t *blacklist); 
u_int server_ticket_verify(server_t *server, ticket_t *nymble_ticket, u_int link_window, u_int time_period); 
blacklist_t* server_blacklist(server_t *server); 
void server_blacklist_finalize(server_t *server); 
u_int server_blacklist_finalized(server_t *server); 
void server_iterate(server_t *server, u_int time_period_delta); 
void server_update(server_t *server, blacklist_t *blacklist, linking_token_t *linking_tokens); 
void server_update_cert(server_t *server, blacklist_cert_t *blacklist_cert);

void server_free(server_t *server);

#endif
