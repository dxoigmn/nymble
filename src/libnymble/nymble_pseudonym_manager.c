#include "nymble_pseudonym_manager.h"

void pm_free(pm_t *pm) {
  if (!pm) {
    return;
  }
  
  free(pm);
}

pm_t* pm_initialize(u_char *hmac_key_np) {
  pm_t *pm = malloc(sizeof(pm_t));
  memcpy(pm->hmac_key_np, hmac_key_np, sizeof(pm->hmac_key_np));
  random_bytes(pm->keyedhash_key_p, sizeof(pm->keyedhash_key_p));
  
  return pm;
}

pseudonym_t* pm_pseudonym_create(pm_t *pm, u_char *user_id, u_int link_window) {
  pseudonym_t *pseudonym = malloc(sizeof(pseudonym_t));
  HMAC_CTX ctx;
  
  HMAC_Init(&ctx, pm->keyedhash_key_p, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, user_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&link_window, sizeof(link_window));
  HMAC_Final(&ctx, pseudonym->pseudonym, NULL);

  HMAC_Init(&ctx, pm->hmac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym->pseudonym, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&link_window, sizeof(link_window));
  HMAC_Final(&ctx, pseudonym->mac_np, NULL);

  return pseudonym;
}
