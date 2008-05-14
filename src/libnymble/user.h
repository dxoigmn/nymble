#ifndef __USER_H__
#define __USER_H__

#include "nymble.h"
#include "nymble_sexp.h"

typedef struct {
  u_char        server_id[DIGEST_SIZE];
  u_int         link_window;
  u_int         blacklisted;
  u_int         last_authenticated;
  credential_t  *credential;
  blacklist_t   *blacklist;
  void          *next;
} user_entry_t;

typedef struct {
  pseudonym_t   pseudonym[DIGEST_SIZE];
  RSA           *verify_key_n;
  user_entry_t  *entries;
} user_t;

user_t* user_initialize(pseudonym_t *pseudonym, RSA *verify_key_n); 
pseudonym_t* user_pseudonym(user_t *user);
void user_entry_initialize(user_t *user, u_char *server_id, credential_t *credential, u_int time_periods); 
u_int user_entry_exists(user_t *user, u_char *server_id); 
u_int user_blacklist_update(user_t *user, u_char *server_id, blacklist_t *blacklist, u_int link_window, u_int time_period); 
u_int user_blacklist_check(user_t *user, u_char *server_id); 
ticket_t* user_credential_get(user_t *user, u_char *server_id, u_int time_period);
user_entry_t* user_entry_get(user_t *user, u_char *server_id);

void user_free(user_t *user);

#endif
