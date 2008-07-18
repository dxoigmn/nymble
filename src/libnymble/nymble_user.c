#include "nymble_user.h"

void user_free(user_t *user) {
  if (!user) {
    return;
  }
  
  user_entry_t *user_entry = user->entries;
  
  while (user_entry) {
    user_entry_t *next = user_entry->next;
    
    credential_free(user_entry->credential);
    blacklist_free(user_entry->blacklist);
    free(user_entry);
    
    user_entry = next;
  }
  
  RSA_free(user->verify_key_n);
  free(user);
}

user_t* user_initialize(pseudonym_t *pseudonym, RSA *verify_key_n) {
  user_t *user = malloc(sizeof(user_t));
  memcpy(user->pseudonym->pseudonym, pseudonym->pseudonym, DIGEST_SIZE);
  memcpy(user->pseudonym->mac_np, pseudonym->mac_np, DIGEST_SIZE);
  user->verify_key_n  = verify_key_n;
  user->entries       = NULL;

  return user;
}
 
pseudonym_t* user_pseudonym(user_t *user) {
  return user->pseudonym;
}

void user_entry_initialize(user_t *user, u_char *server_id, credential_t *credential, u_int time_periods) {
  user_entry_t *user_entry = malloc(sizeof(user_entry_t));
  memcpy(user_entry->server_id, server_id, DIGEST_SIZE);
  user_entry->link_window         = 0;
  user_entry->blacklist           = NULL;
  user_entry->blacklisted         = 0;
  user_entry->last_authenticated  = 0;
  user_entry->credential          = malloc(sizeof(credential_t));
  credential_cpy(user_entry->credential, credential, time_periods);
  user_entry->next  = user->entries;
  user->entries     = user_entry;
}

user_entry_t* user_entry_get(user_t *user, u_char *server_id) {
  user_entry_t *user_entry = user->entries;
  
  while (user_entry) {
    if (memcmp(user_entry->server_id, server_id, DIGEST_SIZE) == 0) {
      return user_entry;
    }
    
    user_entry = user_entry->next;
  }
  
  return NULL;
}

u_int user_entry_exists(user_t *user, u_char *server_id) {
  if (user_entry_get(user, server_id)) {
    return 1;
  } else {
    return 0;
  }
}
 
u_int user_blacklist_update(user_t *user, u_char *server_id, blacklist_t *blacklist, u_int link_window, u_int time_period) {
  if (!bl_check_integrity(blacklist, server_id, user->verify_key_n) ||
      !bl_check_freshness(blacklist, time_period, link_window)) {
    return 0;
  }

  user_entry_t *user_entry = user_entry_get(user, server_id);

  if (user_entry == NULL) {
    return 0;
  }

  user_entry->blacklisted = 0;
  user_entry->link_window = link_window;
  user_entry->blacklist   = malloc(sizeof(blacklist_t));
  blacklist_cpy(user_entry->blacklist, blacklist);
  
  return 1;
}
 
u_int user_blacklist_check(user_t *user, u_char *server_id) {
  u_char nymble0[DIGEST_SIZE];  
  user_entry_t *user_entry  = user_entry_get(user, server_id);
  credential_t *credential  = user_entry->credential;
  blacklist_t *blacklist    = user_entry->blacklist;
  u_int flag                = 0;

  if (blacklist == NULL) {
    return 0;
  }
  
  compute_nymble(nymble0, credential->seed);
  
  nymblelist_t *nymble = blacklist->nymbles;
  
  while (nymble) {
    if (memcmp(nymble->nymble, nymble0, DIGEST_SIZE) == 0) {
      flag = 1;
    }

    nymble = nymble->next;
  }
  
  return flag;
}
 
ticket_t* user_credential_get(user_t *user, u_char *server_id, u_int time_period) {
  user_entry_t *user_entry = user_entry_get(user, server_id);

  if (user_entry == NULL) {
    return NULL;
  }
  
  return &(user_entry->credential->tickets[time_period]);
}
