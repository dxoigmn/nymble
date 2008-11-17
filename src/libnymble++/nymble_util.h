#ifndef __NYMBLE_UTIL_H__
#define __NYMBLE_UTIL_H__

#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>

#define DIGEST_SIZE       SHA256_DIGEST_LENGTH
#define CIPHER_BLOCK_SIZE AES_BLOCK_SIZE
#define SIGNATURE_SIZE    256
#define CTXT_SIZE         (CIPHER_BLOCK_SIZE + DIGEST_SIZE + DIGEST_SIZE + CIPHER_BLOCK_SIZE)

#include <string>

namespace Nymble {

typedef unsigned char u_char;
typedef unsigned int  u_int;

void digest(std::string in, std::string* out);
void random_bytes(size_t size, std::string* out);
void hexencode(std::string in, std::string* out);
void hexdecode(std::string in, std::string* out);

}; // namespace Nymble

#endif