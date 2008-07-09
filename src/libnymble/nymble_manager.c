#include "nymble_manager.h"

void nm_free(nm_t *nm) {
  if (!nm) {
    return;
  }
  
  nm_entry_t *nm_entry = nm->entries;
  
  while (nm_entry) {
    nm_entry_t *next = nm_entry->next;
    free(nm_entry);
    nm_entry = next;
  }
  
  RSA_free(nm->sign_key_n);
  free(nm);
}

nm_t* nm_initialize(u_char *hmac_key_np) {
  nm_t  *nm   = malloc(sizeof(nm_t));
  RSA   *rsa  = RSA_generate_key(SIGNATURE_SIZE * 8, 65537, NULL, NULL);
  
  random_bytes(nm->hmac_key_n, sizeof(nm->hmac_key_n));
  memcpy(nm->hmac_key_np, hmac_key_np, sizeof(nm->hmac_key_np));
  random_bytes(nm->keyedhash_key_n, sizeof(nm->keyedhash_key_n));
  random_bytes(nm->encrypt_key_n, sizeof(nm->encrypt_key_n));
  nm->sign_key_n    = rsa;
  nm->entries = NULL;

  return nm;
}
 
RSA* nm_verify_key(nm_t *nm) {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSA_PUBKEY(bio, nm->sign_key_n);
  RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
  BIO_free(bio);
  
  return rsa;
}

RSA *nm_verify_private_key(nm_t *nm) {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSAPrivateKey(bio, nm->sign_key_n, NULL, NULL, 0, NULL, NULL);
  RSA *rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
  BIO_free(bio);
  
  return rsa;
}

/* internal function */
nm_entry_t* nm_entry_get(nm_t *nm, u_char *server_id) {
  nm_entry_t *entry = nm->entries;

  while (entry) {
    if (memcmp(server_id, entry->server_id, sizeof(entry->server_id)) == 0) {
      return entry;
    }

    entry = entry->next;
  }
  
  return NULL;
}

u_int nm_entry_exists(nm_t *nm, u_char *server_id) {
  nm_entry_t *entry = nm_entry_get(nm, server_id);
  
  if (entry) {
    return 1;
  }
  
  return 0;
}
 
u_char* nm_entry_add(nm_t *nm, u_char *server_id) {
  nm_entry_t  *nm_entry  = malloc(sizeof(nm_entry_t));
  memcpy(nm_entry->server_id, server_id, sizeof(nm_entry->server_id));
  random_bytes(nm_entry->hmac_key_ns, sizeof(nm_entry->hmac_key_ns));
  nm_entry->bl_last_updated = 1;
  nm_entry->next            = nm->entries;
  nm->entries               = nm_entry;
  
  return nm_entry->hmac_key_ns;
}
 
u_int nm_entry_update(nm_t *nm, u_char *server_id, u_int time_period) {
  nm_entry_t *entry = nm_entry_get(nm, server_id);
  
  if (!entry) {
    return 0;
  }
  
  entry->bl_last_updated = time_period;
    
  return 1;
}
 
u_int nm_pseudonym_verify(nm_t *nm, pseudonym_t *pseudonym, u_int link_window) {
  u_char mac[DIGEST_SIZE];
  HMAC_CTX ctx;

  HMAC_Init(&ctx, nm->hmac_key_np, DIGEST_SIZE, EVP_sha256());
  HMAC_Update(&ctx, pseudonym->pseudonym, DIGEST_SIZE);
  HMAC_Update(&ctx, (u_char *)&link_window, sizeof(link_window));
  HMAC_Final(&ctx, mac, NULL);

  if (memcmp(mac, pseudonym->mac_np, DIGEST_SIZE) == 0) {
    return 1;
  }
  
  return 0;
}
 
blacklist_t* nm_blacklist_create(nm_t *nm, u_char *server_id, u_int time_period, u_int link_window) {
  u_char hashed[DIGEST_SIZE];
  
  blacklist_t *blacklist = malloc(sizeof(blacklist_t));
  memcpy(blacklist->server_id, server_id, DIGEST_SIZE);
  blacklist->link_window  = link_window;
  blacklist->nymbles      = NULL;
  
  bl_hash(hashed, blacklist);

  nm_blacklist_cert_compute(nm, &blacklist->cert, hashed, time_period);

  return blacklist;
}

u_int nm_blacklist_cert_compute(nm_t *nm, blacklist_cert_t *bl_cert, u_char *bl_hash, u_int time_period) {
  u_char  hashed[DIGEST_SIZE];
  u_char  pad_buf[SIGNATURE_SIZE];

  memcpy(bl_cert->bl_hash, bl_hash, DIGEST_SIZE);
  bl_cert->time_period = time_period;
  
  HMAC_CTX hmac_ctx;

  HMAC_Init(&hmac_ctx, nm->hmac_key_n, DIGEST_SIZE, EVP_sha256());
  
  HMAC_Update(&hmac_ctx, bl_cert->bl_hash, sizeof(bl_cert->bl_hash));
  HMAC_Update(&hmac_ctx, (u_char *)&bl_cert->time_period, sizeof(bl_cert->time_period));
  
  HMAC_Final(&hmac_ctx, bl_cert->bmac_n, NULL);
  HMAC_CTX_cleanup(&hmac_ctx);
  
  SHA256_CTX sha_ctx;

  SHA256_Init(&sha_ctx);

  SHA256_Update(&sha_ctx, bl_cert->bl_hash, sizeof(bl_cert->bl_hash));
  SHA256_Update(&sha_ctx, (u_char *)&bl_cert->time_period, sizeof(bl_cert->time_period));
	
  SHA256_Final(hashed, &sha_ctx);
    
  RSA_padding_add_PKCS1_PSS(nm->sign_key_n, pad_buf, hashed, EVP_sha256(), -2);
	RSA_private_encrypt(RSA_size(nm->sign_key_n), pad_buf, bl_cert->sig, nm->sign_key_n, RSA_NO_PADDING);
		
  return 1;
}

u_int nm_blacklist_cert_verify(nm_t *nm, blacklist_cert_t *blacklist_cert, u_char *server_id, u_int link_window) {
  nm_entry_t *entry = nm_entry_get(nm, server_id);
  
  if (!entry) {
    return 0;
  }
  
  return nm_bl_cert_check_integrity(blacklist_cert, server_id, nm->hmac_key_n) &&
          bl_cert_check_freshness(blacklist_cert, entry->bl_last_updated);
}

u_int nm_blacklist_verify(nm_t *nm, blacklist_t *blacklist, u_char *server_id, u_int link_window) {
  nm_entry_t *entry = nm_entry_get(nm, server_id);
  
  if (!entry) {
    return 0;
  }
  
  return nm_bl_check_integrity(blacklist, server_id, nm->hmac_key_n) &&
         bl_check_freshness(blacklist, entry->bl_last_updated, link_window);
}
 
blacklist_t* nm_blacklist_update(nm_t *nm, blacklist_t *blacklist, ticket_t *complaints, u_int time_period, u_int link_window) {
  u_char hashed[DIGEST_SIZE];

  blacklist_t *new_blacklist = malloc(sizeof(blacklist_t));
  memcpy(new_blacklist->server_id, blacklist->server_id, DIGEST_SIZE);
  new_blacklist->link_window  = link_window;
  new_blacklist->nymbles      = NULL;

  nymblelist_t *nymble = blacklist->nymbles;

  while (nymble) {
    nymblelist_t *new_nymble = malloc(sizeof(nymblelist_t));
    memcpy(new_nymble->nymble, nymble->nymble, DIGEST_SIZE);
    new_nymble->next = new_blacklist->nymbles;
    new_blacklist->nymbles = new_nymble;
    
    nymble = nymble->next;
  }

  ticket_t *complaint = complaints;
  
  while (complaint) {
    u_char pseudonym[DIGEST_SIZE];
    u_char seed[DIGEST_SIZE];
    u_char nymble0[DIGEST_SIZE];
    
    nymblelist_t *new_complaint = malloc(sizeof(nymblelist_t));
    
    /* Compute the nymble0 for the current complaint ticket */
    decrypt_trapdoor(NULL, pseudonym, nm->encrypt_key_n, complaint->trapdoorenc, sizeof(complaint->trapdoorenc));    
    seed_trapdoor(seed, nm->keyedhash_key_n, pseudonym, blacklist->server_id, link_window);
    compute_nymble(nymble0, seed);
    
    if (user_already_blacklisted(new_blacklist->nymbles, nymble0)) {
      random_bytes(new_complaint->nymble, DIGEST_SIZE);
    } else {
      memcpy(new_complaint->nymble, nymble0, DIGEST_SIZE);
    }
    
    new_complaint->next = new_blacklist->nymbles;
    new_blacklist->nymbles = new_complaint;
    
    complaint = complaint->next;
  }
  
  bl_hash(hashed, new_blacklist);

  nm_blacklist_cert_compute(nm, &new_blacklist->cert, hashed, time_period);
  
  return new_blacklist;
}

credential_t* nm_credential_create(nm_t *nm, pseudonym_t *pseudonym, u_char *server_id, u_int link_window, u_int time_periods) {
  nm_entry_t *entry = nm_entry_get(nm, server_id);
  
  if (entry == NULL) {
    return NULL;
  }
  
  u_char trapdoor[time_periods + 1][DIGEST_SIZE];
    
  seed_trapdoor(trapdoor[0], nm->keyedhash_key_n, pseudonym->pseudonym, server_id, link_window);

  credential_t *credential  = malloc(sizeof(credential_t));
  credential->tickets       = malloc(sizeof(ticket_t) * (time_periods + 1));
  memcpy(credential->seed, trapdoor[0], DIGEST_SIZE);

  int i;
  
  for (i = 1; i <= time_periods; i++) {
    evolve_trapdoor(trapdoor[i], trapdoor[i - 1], 1);

    ticket_t *ticket = &credential->tickets[i];
    
    memcpy(ticket->server_id, server_id, DIGEST_SIZE);
    ticket->time_period = i;
    ticket->link_window = link_window;
    compute_nymble(ticket->nymble, trapdoor[i]);
    encrypt_trapdoor(ticket->trapdoorenc, nm->encrypt_key_n, trapdoor[i], pseudonym);
    hmac_ticket(ticket->mac_n, nm->hmac_key_n, ticket, 0);
    hmac_ticket(ticket->mac_ns, entry->hmac_key_ns, ticket, 1);
    
    if (i > 1) {
      credential->tickets[i - 1].next = ticket;
    }
        
    ticket->next = NULL;
  }

  return credential;
}

linking_token_t* nm_tokens_create(nm_t *nm, u_char *server_id, blacklist_t *blacklist, ticket_t *complaints, u_int time_period, u_int link_window) {
  linking_token_t *linking_tokens = NULL;
  u_int next_time_period          = time_period + 1;
  
  nymblelist_t *blacklisted_nymbles   = blacklist->nymbles;
  ticket_t *ticket                    = complaints;

  while (ticket) {
    u_char trapdoor_for_next_time_period[DIGEST_SIZE];

    u_char trapdoor[DIGEST_SIZE];
    u_char pseudonym[DIGEST_SIZE];
    u_char seed[DIGEST_SIZE];
    u_char nymble0[DIGEST_SIZE];
    
    /* Compute the nymble0 for the current complaint ticket */
    decrypt_trapdoor(trapdoor, pseudonym, nm->encrypt_key_n, ticket->trapdoorenc, sizeof(ticket->trapdoorenc));
    seed_trapdoor(seed, nm->keyedhash_key_n, pseudonym, server_id, link_window);
    compute_nymble(nymble0, seed);

    if (user_already_blacklisted(blacklisted_nymbles, nymble0)) {
      random_bytes(trapdoor_for_next_time_period, DIGEST_SIZE);
    } else {
      u_int delta       = next_time_period - ticket->time_period;
      
      evolve_trapdoor(trapdoor_for_next_time_period, trapdoor, delta);
      
      // FIXME: This should modify blacklisted_tickets to include new blacklisted tickets
    }

    linking_token_t *cur_linking_token = malloc(sizeof(linking_token_t));
    memcpy(cur_linking_token->server_id, server_id, DIGEST_SIZE);
    cur_linking_token->link_window = link_window;
    cur_linking_token->time_period = next_time_period;
    memcpy(cur_linking_token->trapdoor, trapdoor_for_next_time_period, DIGEST_SIZE);
    cur_linking_token->nymble_ticket = malloc(sizeof(ticket_t));
    ticket_cpy(cur_linking_token->nymble_ticket, ticket);
    
    cur_linking_token->next = linking_tokens;
    linking_tokens = cur_linking_token;

    ticket = ticket->next;
  }

  return linking_tokens;
}
