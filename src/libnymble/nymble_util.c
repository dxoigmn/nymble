#include "nymble_util.h"

void printbytes(u_char *bytes, u_int size) {
  int i;
  
  for (i = 0; i < size; i++) {
    fprintf(stderr, "%02x ", *(bytes + i));
  }
}

u_int hash(u_char *buffer, u_char *value, u_int size) {
	SHA256_CTX ctx;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, value, size);
  SHA256_Final(buffer, &ctx);
  
  return DIGEST_SIZE;
}

u_int random_bytes(u_char *buffer, u_int size) {
  RAND_bytes(buffer, size);
  
  return 1;
}

/*
 Wow, the PSS stuff seems to be in my native version of openssl... but we can use
 the following if we need to drop the version dependency on openssl.
 
 I'm not sure how extensive the "minimal support for PSS" mentioned in the changelog
 between 0.9.7g and 0.9.7h is, since it was added "mainly for FIPS compliance and is
 not fully integrated yet"
*/

static const unsigned char zeroes[] = {0, 0, 0, 0, 0, 0, 0, 0};

void EMSA_PSS_encode(u_char *em, u_char *message, u_int message_len) {
  u_char m_hash[DIGEST_SIZE];
  u_char mask[SIGNATURE_SIZE - DIGEST_SIZE - 1];
  
  hash(m_hash, message, message_len);
  
  // We're maximizing the salt length
  u_int salt_len = SIGNATURE_SIZE - DIGEST_SIZE - 2;
  
  em[0] = 1;
  u_char *salt = em + 1;
  random_bytes(salt, salt_len);
  
  u_char *H = em + (SIGNATURE_SIZE - DIGEST_SIZE - 1);

  SHA256_CTX ctx;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, zeroes, sizeof(zeroes));
	SHA256_Update(&ctx, m_hash, sizeof(m_hash));
  SHA256_Update(&ctx, salt, salt_len);
  SHA256_Final(H, &ctx);
  
  PKCS1_MGF1(mask, sizeof(mask), H, DIGEST_SIZE, EVP_sha256());
  
  int i;
  for (i = 0; i < sizeof(mask); i++) {
    em[i] ^= mask[i];
  }
  
  // Set leftmost bits to 0
  em[0] &= 0x7f;
  
  // Stick the magic number on
  em[SIGNATURE_SIZE - 1] = 0xbc;
}

u_int EMSA_PSS_verify(u_char *em, u_char *message, u_int message_len) {
  u_char m_hash[DIGEST_SIZE];
  u_char computed_hash[DIGEST_SIZE];
  
  u_char mask[SIGNATURE_SIZE - DIGEST_SIZE - 1];
  
  if (em[SIGNATURE_SIZE - 1] != 0xbc) {
    return 0;
  }

  u_int salt_len = SIGNATURE_SIZE - DIGEST_SIZE - 2;
  
  u_char *H = em + (SIGNATURE_SIZE - DIGEST_SIZE - 1);
  
  PKCS1_MGF1(mask, sizeof(mask), H, DIGEST_SIZE, EVP_sha256());


  // We operate on the mask to avoid corrupting the input em
  u_char *salt = mask + 1;    
  int i;
  for (i = 0; i < sizeof(mask); i++) {
    mask[i] ^= em[i];
  }
  
  mask[0] &= 0x7f;
  
  if (mask[0] != 1) {
    return 0;
  }
  
  hash(m_hash, message, message_len);
  
  SHA256_CTX ctx;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, zeroes, sizeof(zeroes));
	SHA256_Update(&ctx, m_hash, sizeof(m_hash));
  SHA256_Update(&ctx, salt, salt_len);
  SHA256_Final(computed_hash, &ctx);
  
  if (memcmp(H, computed_hash, DIGEST_SIZE) != 0) {
    return 0;
  }
  
  return 1;
}

void blacklist_cpy(blacklist_t *out, blacklist_t *in) {
  memcpy(out->server_id, in->server_id, sizeof(out->server_id));
  out->link_window = in->link_window;
  
  // Simply copying cert was crashing for some reason. TODO: find out why, and simplify this
  memcpy(out->cert.bl_hash, in->cert.bl_hash, sizeof(out->cert.bl_hash));
  out->cert.time_period = in->cert.time_period;
  memcpy(out->cert.bmac_n, in->cert.bmac_n, sizeof(out->cert.bmac_n));
  memcpy(out->cert.sig, in->cert.sig, sizeof(out->cert.sig));

  out->nymbles = NULL;
  
  nymblelist_t *in_nymble = in->nymbles;
  nymblelist_t *tail      = NULL;
  
  while (in_nymble) {
    nymblelist_t *nymble = malloc(sizeof(nymblelist_t));
    memcpy(nymble->nymble, in_nymble->nymble, DIGEST_SIZE);
    nymble->next = NULL;

    if (out->nymbles) {
      tail->next = nymble;
    } else {
      out->nymbles = nymble;
    }

    tail = nymble;

    in_nymble = in_nymble->next;
  }
  
}

void blacklist_free(blacklist_t *blacklist) {
  if (!blacklist) {
    return;
  }

  nymblelist_t *nymble = blacklist->nymbles;

  while (nymble) {
    nymblelist_t *next_nymble = nymble->next;
    free(nymble);
    nymble = next_nymble;
  }

  free(blacklist);
}

void ticket_cpy(ticket_t *out, ticket_t *in) {
  memcpy(out->server_id, in->server_id, sizeof(out->server_id));
  out->time_period = in->time_period;
  out->link_window = in->link_window;
  memcpy(out->nymble, in->nymble, sizeof(out->nymble));
  memcpy(out->trapdoorenc, in->trapdoorenc, sizeof(out->trapdoorenc));
  memcpy(out->mac_n, in->mac_n, sizeof(out->mac_n));
  memcpy(out->mac_ns, in->mac_ns, sizeof(out->mac_ns));
}

void ticket_free(ticket_t *ticket) {
  if (!ticket) {
    return;
  }
  
  free(ticket);
}

void credential_cpy(credential_t *out, credential_t *in, u_int time_periods) {
  memcpy(out->seed, in->seed, sizeof(out->seed));
  
  out->tickets = malloc(sizeof(ticket_t) * (time_periods + 1));
  
  int i;
  
  for (i = 1; i <= time_periods; i++) {
    ticket_cpy(&out->tickets[i], &in->tickets[i]);
  }
}

void credential_free(credential_t *credential) {
  if (!credential) {
    return;
  }
  
  free(credential->tickets);
  free(credential);
}

void token_free(linking_token_t *linking_token) {
  if (!linking_token) {
    return;
  }
  
  ticket_free(linking_token->nymble_ticket);
  free(linking_token);
}

void bl_hash(u_char *buffer, blacklist_t *blacklist) {
  SHA256_CTX ctx;

  SHA256_Init(&ctx);

  SHA256_Update(&ctx, blacklist->server_id, sizeof(blacklist->server_id));
  SHA256_Update(&ctx, &blacklist->link_window, sizeof(blacklist->link_window));

  nymblelist_t *nymble = blacklist->nymbles;
  
  while (nymble) {
    SHA256_Update(&ctx, nymble->nymble, DIGEST_SIZE);
    nymble = nymble->next;
  }
	
  SHA256_Final(buffer, &ctx);
}

u_int bl_verify(RSA *rsa, blacklist_t *blacklist) {
  u_char  bl_hashed[DIGEST_SIZE];

  bl_hash(bl_hashed, blacklist);
  
  // Check that the cert matches this blacklist
  if (memcmp(bl_hashed, blacklist->cert.bl_hash, DIGEST_SIZE)) {
    return 0;
  }

  // Verify the cert itself
  return bl_cert_verify(rsa, &blacklist->cert);
}

u_int bl_cert_verify(RSA *rsa, blacklist_cert_t *blacklist_cert) {
  u_char  cert_hashed[DIGEST_SIZE];
  u_char  pad_buf[SIGNATURE_SIZE];
  
  SHA256_CTX sha_ctx;

  SHA256_Init(&sha_ctx);

  SHA256_Update(&sha_ctx, blacklist_cert->bl_hash, sizeof(blacklist_cert->bl_hash));
  SHA256_Update(&sha_ctx, (u_char *)&blacklist_cert->time_period, sizeof(blacklist_cert->time_period));
	
  SHA256_Final(cert_hashed, &sha_ctx);
  
  RSA_public_decrypt(sizeof(blacklist_cert->sig), blacklist_cert->sig, pad_buf, rsa, RSA_NO_PADDING);	
  return RSA_verify_PKCS1_PSS(rsa, cert_hashed, EVP_sha256(), pad_buf, -2);
}

u_int nm_bl_check_integrity(blacklist_t *blacklist, u_char *server_id, u_char *hmac_key_n) {
  u_char hashed[DIGEST_SIZE];
  
  bl_hash(hashed, blacklist);
  
  // Make sure the blacklist is for this server
  if (memcmp(server_id, blacklist->server_id, DIGEST_SIZE) != 0) {
    return 0;
  }
  
  // Make sure that the blacklist cert is for this blacklist
  if (memcmp(hashed, blacklist->cert.bl_hash, DIGEST_SIZE) != 0) {
    return 0;
  }
  
  // Check that the cert itself is correct
  return nm_bl_cert_check_integrity(&blacklist->cert, server_id, hmac_key_n);
}

u_int nm_bl_cert_check_integrity(blacklist_cert_t *blacklist_cert, u_char *server_id, u_char *hmac_key_n) {
  u_char buffer[DIGEST_SIZE];
  
  HMAC_CTX ctx;

  HMAC_Init(&ctx, hmac_key_n, DIGEST_SIZE, EVP_sha256());
  
  HMAC_Update(&ctx, blacklist_cert->bl_hash, sizeof(blacklist_cert->bl_hash));
  HMAC_Update(&ctx, (u_char *)&blacklist_cert->time_period, sizeof(blacklist_cert->time_period));
  
  HMAC_Final(&ctx, buffer, NULL);
  HMAC_CTX_cleanup(&ctx);
  
  if (memcmp(buffer, blacklist_cert->bmac_n, DIGEST_SIZE) == 0) {
    return 1;
  } else {
    return 0;
  }
}

u_int bl_check_integrity(blacklist_t *blacklist, u_char *server_id, RSA *verify_key_n) {
  if (bl_verify(verify_key_n, blacklist) &&
      memcmp(server_id, blacklist->server_id, DIGEST_SIZE) == 0) {
    return 1;
  } else {
    return 0;
  }
}

u_int bl_check_freshness(blacklist_t *blacklist, u_int time_period, u_int link_window) {
  if (bl_cert_check_freshness(&blacklist->cert, time_period) &&
      (blacklist->link_window == link_window)) {
    return 1;
  } else {
    return 0;
  }
}

u_int bl_cert_check_freshness(blacklist_cert_t *blacklist_cert, u_int time_period) {
  return blacklist_cert->time_period == time_period;
}

void seed_trapdoor(u_char *buffer, u_char *key, u_char *pseudonym, u_char *server_id, u_int link_window) {
  HMAC_CTX ctx;

  HMAC_Init(&ctx, key, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym, DIGEST_SIZE);
  HMAC_Update(&ctx, server_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&link_window, sizeof(link_window));
  HMAC_Final(&ctx, buffer, NULL);
  HMAC_CTX_cleanup(&ctx);
}

void evolve_trapdoor(u_char *buffer, u_char *trapdoor, u_int delta) {
  memcpy(buffer, trapdoor, DIGEST_SIZE);

  int i;
  for (i = 0; i < delta; i++) {
    SHA256_CTX ctx;
    u_char out[DIGEST_SIZE];
    u_char nonce[1] = "f";
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, nonce, sizeof(nonce));
    SHA256_Update(&ctx, buffer, DIGEST_SIZE);
    SHA256_Final(out, &ctx);

    memcpy(buffer, out, DIGEST_SIZE);
  }
}

void compute_nymble(u_char *buffer, u_char *trapdoor) {
  u_char nonce[] = "g";

  SHA256_CTX ctx;

  SHA256_Init(&ctx);
  SHA256_Update(&ctx, nonce, sizeof(nonce));
  SHA256_Update(&ctx, trapdoor, DIGEST_SIZE);
  SHA256_Final(buffer, &ctx);
}

void encrypt_trapdoor(u_char *buffer, u_char *key, u_char *trapdoor, pseudonym_t *pseudonym) {
  u_char in[DIGEST_SIZE * 2];
  
  memcpy(in, trapdoor, DIGEST_SIZE);
  memcpy(in + DIGEST_SIZE, pseudonym->pseudonym, DIGEST_SIZE);
  
  u_char iv[CIPHER_BLOCK_SIZE];
  
  AES_KEY xkey;
  
  /* iv must be separate from buffer as the AES function messes with it as it works */
  random_bytes(iv, CIPHER_BLOCK_SIZE);
  memcpy(buffer, iv, CIPHER_BLOCK_SIZE);

  AES_set_encrypt_key(key, CIPHER_BLOCK_SIZE * 8, &xkey);
  AES_cbc_encrypt(in, buffer + CIPHER_BLOCK_SIZE, sizeof(in), &xkey, iv, AES_ENCRYPT);
}

void decrypt_trapdoor(u_char *trapdoor, u_char *pseudonym, u_char *key, u_char *value, u_int size) {
  u_char buffer[CIPHER_BLOCK_SIZE * 4]; // This is awfully big...
  u_char iv[CIPHER_BLOCK_SIZE];

  AES_KEY xkey;

  memcpy(iv, value, CIPHER_BLOCK_SIZE);

  AES_set_decrypt_key(key, CIPHER_BLOCK_SIZE * 8, &xkey);
  AES_cbc_encrypt(value + CIPHER_BLOCK_SIZE, buffer, size - CIPHER_BLOCK_SIZE, &xkey, iv, AES_DECRYPT);
  
  if (trapdoor) {
    memcpy(trapdoor, buffer, DIGEST_SIZE);
  }
  
  if (pseudonym) {
    memcpy(pseudonym, buffer + DIGEST_SIZE, DIGEST_SIZE);
  } 
}

void hmac_ticket(u_char *buffer, u_char *key, ticket_t *ticket, u_int include_hmac) {
  HMAC_CTX ctx;

  HMAC_Init(&ctx, key, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, ticket->server_id, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&ticket->link_window, sizeof(ticket->link_window));
  HMAC_Update(&ctx, (u_char *)&ticket->time_period, sizeof(ticket->time_period));
  HMAC_Update(&ctx, ticket->nymble, DIGEST_SIZE);
  HMAC_Update(&ctx, ticket->trapdoorenc, sizeof(ticket->trapdoorenc));
  
  if (include_hmac) {
    HMAC_Update(&ctx, ticket->mac_n, DIGEST_SIZE);    
  }
  
  HMAC_Final(&ctx, buffer, NULL);
  HMAC_CTX_cleanup(&ctx);  
}

u_int user_already_blacklisted(nymblelist_t *bl_nymbles, u_char *nymble0) {
  u_int flag = 0;
  
  while (bl_nymbles) {
    if (memcmp(bl_nymbles->nymble, nymble0, DIGEST_SIZE) == 0) {
      flag = 1;
    }
  
    bl_nymbles = bl_nymbles->next;
  }
  
  return flag;
}
