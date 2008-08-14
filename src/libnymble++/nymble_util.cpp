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

}; // namespace Nymble