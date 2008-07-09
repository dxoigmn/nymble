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

void printbytes(u_char *bytes, u_int size);

#define PB(desc, data, size) fprintf(stderr, "%s", (desc)); printbytes((data), (size)); fprintf(stderr, "\n");

u_int hash(u_char *buffer, u_char *value, u_int size);
u_int random_bytes(u_char *buffer, u_int size);

#endif
