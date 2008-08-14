#ifndef __NYMBLE_UTIL_H__
#define __NYMBLE_UTIL_H__

#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <string.h>

namespace Nymble {

typedef unsigned char u_char;
typedef unsigned int  u_int;

void digest(u_char* in, u_int size, u_char* out);
void random_bytes(u_int size, u_char* out);
u_int hexencode(u_char* in, u_int size, char* out = NULL);
u_int hexdecode(char* in, u_char* out = NULL);

}; // namespace Nymble

#endif