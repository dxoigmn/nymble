#include "nymble_util.h"

namespace Nymble {

void digest(u_char* in, u_int size, u_char* out) {
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, in, size);
  SHA256_Final(out, &ctx);
}

void random_bytes(u_int size, u_char* out) {
  RAND_bytes(out, size);
}

u_int hexencode(u_char* in, u_int size, char* out)
{
  if (out) {
    for (u_int i = 0; i < size; i++) {
      sprintf(out + i*2, "%02x", (u_char) in[i]);
    }

    out[size*2] = 0;
  }

  return size*2 + 1;
}

u_int hexdecode(char* in, u_char* out)
{
  u_int len = strlen(in) / 2;

  if (out) {
    for (u_int i = 0; i < len; i++) {
      sscanf(in + i*2, "%02x", (u_int*) (out + i));
    }
  }

  return len;
}

void json_marshal_int(struct json_object* json, char* field, u_int value)
{
  json_object_object_add(json, field, json_object_new_int(value));
}

void json_unmarshal_int(struct json_object* json, char* field, u_int* value)
{
  *value = json_object_get_int(json_object_object_get(json, field));
}

void json_marshal_str(struct json_object* json, char* field, u_char* value, u_int len)
{
  u_int encoded_len = hexencode(value, len);
  char encoded[encoded_len];

  hexencode(value, len, encoded);

  json_object_object_add(json, field, json_object_new_string(encoded));
}

void json_unmarshal_str(struct json_object* json, char* field, u_char* value, u_int len)
{
  char* encoded = json_object_get_string(json_object_object_get(json, field));
  u_int decoded_len = hexdecode(encoded);
  u_char decoded[decoded_len];

  hexdecode(encoded, decoded);

  if (decoded_len == len) {
    memcpy(value, decoded, len);
  } else {
    fprintf(stderr, "Couldn't unmarshal field %s because due to unexpected length %d (expected %d)", field, decoded_len, len);
  }
}

}; // namespace Nymble