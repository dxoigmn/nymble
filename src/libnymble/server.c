#include "server.h"

void server_free(server_t *server) {
  if (!server) {
    return;
  }
  
  linking_list_entry_t *ll_entry = server->linking_list;
  
  while (ll_entry) {
    linking_list_entry_t *next = ll_entry->next;
    //printf("free(%x,ll_entry)\n", (u_int)ll_entry);
    free(ll_entry);
    ll_entry = next;
  }
  
  blacklist_free(server->blacklist);
  
  //printf("free(%x,server)\n", (u_int)server);
  free(server);
}

server_t* server_initialize(u_char *server_id, u_char *hmac_key_ns, blacklist_t *blacklist) {
  server_t *server = malloc(sizeof(server_t));
  //printf("malloc(%x,server)\n", (u_int)server);
  
  memcpy(server->server_id, server_id, DIGEST_SIZE);
  memcpy(server->hmac_key_ns, hmac_key_ns, DIGEST_SIZE);
  server->finalized     = 0;
  server->linking_list  = NULL;
  server->blacklist     = malloc(sizeof(blacklist_t));
  //printf("malloc(%x,blacklist)\n", (u_int)server->blacklist);
  
  blacklist_cpy(server->blacklist, blacklist);

  return server;
}

linking_list_entry_t* server_linking_list_entry_get(server_t *server, u_char *nymble) {
  linking_list_entry_t *ll_entry = server->linking_list;
  
  while (ll_entry) {
    if (memcmp(ll_entry->nymble, nymble, DIGEST_SIZE) == 0) {
      return ll_entry;
    }
    
    ll_entry = ll_entry->next;
  }
  
  return NULL;
}
 
u_int server_ticket_verify(server_t *server, ticket_t *nymble_ticket, u_int link_window, u_int time_period) {
  u_char mac_ns[DIGEST_SIZE];
  hmac_ticket(mac_ns, server->hmac_key_ns, nymble_ticket, 1);
  
  if ((memcmp(nymble_ticket->server_id, server->server_id, DIGEST_SIZE) == 0) &&
      (nymble_ticket->time_period == time_period) &&
      (nymble_ticket->link_window == link_window) &&
      (memcmp(nymble_ticket->mac_ns, mac_ns, DIGEST_SIZE) == 0) &&
      (server_linking_list_entry_get(server, nymble_ticket->nymble)) == NULL) {
    return 1;
  } else {
    return 0;
  }
}
 
blacklist_t* server_blacklist(server_t *server) {
  if (server) {
    return server->blacklist;
  } else {
    return NULL;
  }
}
 
void server_blacklist_finalize(server_t *server) {
  server->finalized = 1;
}
 
u_int server_blacklist_finalized(server_t *server, u_int cur_time_period) {
  return server->finalized && server->blacklist->cert.time_period == cur_time_period;
}
 
void server_iterate(server_t *server, u_int time_period_delta) {
  linking_list_entry_t *ll_entry = server->linking_list;

  while (ll_entry) {
    u_char trapdoor[DIGEST_SIZE];
    u_char nymble[DIGEST_SIZE];
    
    evolve_trapdoor(trapdoor, ll_entry->trapdoor, time_period_delta);
    compute_nymble(nymble, trapdoor);
    
    memcpy(ll_entry->trapdoor, trapdoor, DIGEST_SIZE);
    memcpy(ll_entry->nymble, nymble, DIGEST_SIZE);
    ll_entry->time_period += time_period_delta;

    ll_entry = ll_entry->next;
  }
  
  server->finalized = 0;
}

void server_update_cert(server_t *server, blacklist_cert_t *blacklist_cert) {
  memcpy(&server->blacklist->cert, blacklist_cert, sizeof(blacklist_cert_t));
}
 
void server_update(server_t *server, blacklist_t *blacklist, linking_token_t *linking_tokens) {
  blacklist_free(server->blacklist);
  server->blacklist = malloc(sizeof(blacklist_t));
  //printf("malloc(%x,blacklist)\n", (u_int)server->blacklist);

  blacklist_cpy(server->blacklist, blacklist);

  linking_token_t *linking_token = linking_tokens;
  
  while (linking_token) {
    linking_list_entry_t *ll_entry = malloc(sizeof(linking_list_entry_t));
    //printf("malloc(%x,ll_entry)\n", (u_int)ll_entry);
    
    ll_entry->time_period = linking_token->time_period;
    memcpy(ll_entry->trapdoor, linking_token->trapdoor, DIGEST_SIZE);
    memcpy(ll_entry->nymble, linking_token->nymble_ticket->nymble, DIGEST_SIZE);
    ll_entry->next       = server->linking_list;
    server->linking_list = ll_entry;

    linking_token = linking_token->next;
  }
}
