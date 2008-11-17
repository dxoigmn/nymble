#include "nymble_util.h"

namespace Nymble {

void digest(std::string in, std::string* out)
{
  char buffer[DIGEST_SIZE];
  
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, in.c_str(), in.size());
  SHA256_Final((u_char*)buffer, &ctx);
  
  *out = std::string(buffer, sizeof(buffer));
}

void random_bytes(size_t size, std::string* out)
{
  char buffer[size];
  
  RAND_bytes((u_char*)buffer, size);
  
  *out = std::string(buffer, sizeof(buffer));
}

void hexencode(std::string in, std::string* out)
{
  char buffer[] = "00";
  
  for (size_t i = 0; i < in.size(); i++) {
    sprintf(buffer, "%02x", (u_char)in[i]);
    *out += buffer;
  }
}

void hexdecode(std::string in, std::string* out)
{
  char buffer;
  
  for (size_t i = 0; i < in.size(); i += 2) {
    sscanf(in.substr(i, 2).c_str(), "%02x", (u_int*)&buffer);
    *out += buffer;
  }
}

}; // namespace Nymble