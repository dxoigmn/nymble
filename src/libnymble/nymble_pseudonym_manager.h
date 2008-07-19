#ifndef __PSEUDONYM_MANAGER_H__
#define __PSEUDONYM_MANAGER_H__

#include "nymble_util.h"
#include "nymble_sexp.h"

typedef struct {
  u_char  hmac_key_np[DIGEST_SIZE];
  u_char  keyedhash_key_p[DIGEST_SIZE];
} pm_t;

pm_t* pm_initialize(u_char *hmac_key_np);
pseudonym_t* pm_pseudonym_create(pm_t *pm, u_char *user_id, u_int link_window);

void pm_free(pm_t *pm);

#endif
