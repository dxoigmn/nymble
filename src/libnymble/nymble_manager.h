#ifndef __NYMBLE_MANAGER_H__
#define __NYMBLE_MANAGER_H__

#include "nymble.h"
#include "nymble_sexp.h"

typedef struct {
  u_char  server_id[DIGEST_SIZE];
  u_int   bl_last_updated;
  u_char  hmac_key_ns[DIGEST_SIZE];
  void    *next;
} nm_entry_t;

typedef struct {
  u_char      hmac_key_n[DIGEST_SIZE];
  u_char      hmac_key_np[DIGEST_SIZE];
  u_char      keyedhash_key_n[DIGEST_SIZE];
  u_char      encrypt_key_n[CIPHER_BLOCK_SIZE];
  RSA         *sign_key_n;
  nm_entry_t  *entries;
} nm_t;

nm_t* nm_initialize(u_char *hmac_key_np);
RSA* nm_verify_key(nm_t *nm);
RSA* nm_verify_private_key(nm_t *nm);
u_int nm_entry_exists(nm_t *nm, u_char *server_id);
u_char* nm_entry_add(nm_t *nm, u_char *server_id);
u_int nm_entry_update(nm_t *nm, u_char *server_id, u_int time_period);
u_int nm_pseudonym_verify(nm_t *nm, pseudonym_t *pseudonym, u_int link_window);
blacklist_t* nm_blacklist_create(nm_t *nm, u_char *server_id, u_int time_period, u_int link_window);
u_int nm_blacklist_cert_compute(nm_t *nm, blacklist_cert_t *bl_cert, u_char *bl_hash, u_int time_period);
u_int nm_blacklist_verify(nm_t *nm, blacklist_t *blacklist, u_char *server_id, u_int link_window);
u_int nm_blacklist_cert_verify(nm_t *nm, blacklist_cert_t *blacklist_cert, u_char *server_id, u_int link_window);
blacklist_t* nm_blacklist_update(nm_t *nm, blacklist_t *blacklist, ticket_t *complaints, u_int time_period, u_int link_window);
credential_t* nm_credential_create(nm_t *nm, pseudonym_t *pseudonym, u_char *server_id, u_int link_window, u_int time_periods);
linking_token_t* nm_tokens_create(nm_t *nm, u_char *server_id, blacklist_t *blacklist, ticket_t *complaints, u_int time_period, u_int link_window);

void nm_free(nm_t *nm);

#endif
