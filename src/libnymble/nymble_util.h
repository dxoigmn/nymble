#ifndef __UTIL_H__
#define __UTIL_H__

#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <string.h>

#define DIGEST_SIZE         SHA256_DIGEST_LENGTH
#define CIPHER_BLOCK_SIZE   AES_BLOCK_SIZE
#define SIGNATURE_SIZE      128

typedef unsigned char u_char;
typedef unsigned int  u_int;

typedef struct {
  u_char  pseudonym[DIGEST_SIZE];
  u_char  mac_np[DIGEST_SIZE];
} pseudonym_t;

#define TRAPDOORENC_SIZE (CIPHER_BLOCK_SIZE + DIGEST_SIZE + DIGEST_SIZE + CIPHER_BLOCK_SIZE)

typedef struct {
  u_char  server_id[DIGEST_SIZE];
  u_int   time_period;
  u_int   link_window;
  u_char  nymble[DIGEST_SIZE];
  // iv + hash + hash + final, beware (hash should be rounded up to nearest CIPHER_BLOCK_SIZE to be correct)
  u_char  trapdoorenc[TRAPDOORENC_SIZE];
  u_char  mac_n[DIGEST_SIZE];
  u_char  mac_ns[DIGEST_SIZE];
  void    *next;
} ticket_t;

typedef struct {
  u_char    seed[DIGEST_SIZE];
  ticket_t  *tickets;
} credential_t;

typedef struct nymblelist {
  u_char             nymble[DIGEST_SIZE];
  struct nymblelist  *next;
} nymblelist_t;

typedef struct blacklist_cert {
  u_char bl_hash[DIGEST_SIZE];
  u_int  time_period;
  u_char bmac_n[DIGEST_SIZE];
  u_char sig[SIGNATURE_SIZE];
} blacklist_cert_t;

typedef struct {
  u_char            server_id[DIGEST_SIZE];
  u_int             link_window;
  blacklist_cert_t  cert;
  nymblelist_t      *nymbles;
} blacklist_t;

typedef struct {
  u_char    server_id[DIGEST_SIZE];
  u_int     link_window;
  u_int     time_period;
  u_char    trapdoor[DIGEST_SIZE];
  ticket_t  *nymble_ticket;
  void      *next;
} linking_token_t;

void blacklist_cpy(blacklist_t *out, blacklist_t *in);
void blacklist_free(blacklist_t *blacklist);
void ticket_cpy(ticket_t *out, ticket_t *in);
void ticket_free(ticket_t *ticket);
void credential_cpy(credential_t *out, credential_t *in, u_int time_periods);
void credential_free(credential_t *credential);
void token_free(linking_token_t *linking_token);

void bl_hmac(u_char *buffer, u_char *key, blacklist_t *blacklist);
void bl_hash(u_char *buffer, blacklist_t *blacklist);
void bl_sign(u_char *buffer, RSA *rsa, blacklist_t *blacklist);
u_int bl_verify(RSA *rsa, blacklist_t *blacklist);
u_int bl_cert_verify(RSA *rsa, blacklist_cert_t *blacklist_cert);
u_int nm_bl_check_integrity(blacklist_t *blacklist, u_char *server_id, u_char *hmac_key_n);
u_int nm_bl_cert_check_integrity(blacklist_cert_t *blacklist_cert, u_char *server_id, u_char *hmac_key_n);
u_int bl_check_integrity(blacklist_t *blacklist, u_char *server_id, RSA *verify_key_n);
u_int bl_check_freshness(blacklist_t *blacklist, u_int time_period, u_int link_window);
u_int bl_cert_check_freshness(blacklist_cert_t *blacklist_cert, u_int time_period);
void seed_trapdoor(u_char *buffer, u_char *key, u_char *pseudonym, u_char *server_id, u_int link_window);
void evolve_trapdoor(u_char *buffer, u_char *trapdoor, u_int delta);
void compute_nymble(u_char *buffer, u_char *trapdoor);
void encrypt_trapdoor(u_char *buffer, u_char *key, u_char *trapdoor, pseudonym_t *pseudonym);
void decrypt_trapdoor(u_char *trapdoor, u_char *pseudonym, u_char *key, u_char *value, u_int size);
void hmac_ticket(u_char *buffer, u_char *key, ticket_t *ticket, u_int include_hmac);
u_int user_already_blacklisted(nymblelist_t *bl_nymbles, u_char *nymble0);

u_int hash(u_char *buffer, u_char *value, u_int size);
u_int random_bytes(u_char *buffer, u_int size);

#endif
