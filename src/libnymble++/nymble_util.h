#ifndef __NYMBLE_UTIL_H__
#define __NYMBLE_UTIL_H__

#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <string.h>

#include <json/json.h>

namespace Nymble {

typedef unsigned char u_char;
typedef unsigned int  u_int;

void digest(u_char* in, u_int size, u_char* out);
void random_bytes(u_int size, u_char* out);
u_int hexencode(u_char* in, u_int size, char* out = NULL);
u_int hexdecode(char* in, u_char* out = NULL);

void json_marshal_int(struct json_object* json, char* field, u_int value);
void json_unmarshal_int(struct json_object* json, char* field, u_int* value);

void json_marshal_str(struct json_object* json, char* field, u_char* value, u_int len);
void json_unmarshal_str(struct json_object* json, char* field, u_char* value, u_int len);

}; // namespace Nymble

#endif