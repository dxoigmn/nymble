#include "util.h"

void printbytes(u_char *bytes, u_int size) {
  int i;
  
  for (i = 0; i < size; i++) {
    printf("%02x ", *(bytes + i));
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