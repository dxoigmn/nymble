#include "nymble.h"

Nymble::Nymble()
{
  setLinkWindow(0);
  setTimePeriod(1);
}

void Nymble::setLinkWindow(u_int link_window)
{
  this->cur_link_window = link_window;
}

u_int Nymble::getLinkWindow()
{
  return this->cur_link_window;
}

void Nymble::setTimePeriod(u_int time_period)
{
  this->cur_time_period = time_period;
}

u_int Nymble::getTimePeriod()
{
  return this->cur_time_period;
}

void Nymble::digest(u_char* in, u_int size, u_char* out) {
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, in, size);
  SHA256_Final(out, &ctx);
}

void Nymble::random_bytes(u_int size, u_char* out) {
  RAND_bytes(out, size);
}

u_int Nymble::hexencode(u_char* in, u_int size, char* out)
{
  if (out) {
    for (u_int i = 0; i < size; i++) {
      sprintf(out + i*2, "%02x", (u_char) in[i]);
    }
    
    out[size*2] = 0;
  }
  
  return size*2 + 1;
}

u_int Nymble::hexdecode(char* in, u_char* out)
{
  u_int len = strlen(in) / 2;
  
  if (out) {
    for (u_int i = 0; i < len; i++) {
      sscanf(in + i*2, "%02x", (u_int*) (out + i));
    }
  }
  
  return len;
}

void Nymble::json_marshall_int(struct json_object* json, char* field, u_int value)
{
  json_object_object_add(json, field, json_object_new_int(value));
}

void Nymble::json_unmarshall_int(struct json_object* json, char* field, u_int* value)
{
  *value = json_object_get_int(json_object_object_get(json, field));
}

void Nymble::json_marshall_str(struct json_object* json, char* field, u_char* value, u_int len)
{
  u_int encoded_len = Nymble::hexencode(value, len);
  char encoded[encoded_len];
  
  Nymble::hexencode(value, len, encoded);
  
  json_object_object_add(json, field, json_object_new_string(encoded));
}

void Nymble::json_unmarshall_str(struct json_object* json, char* field, u_char* value, u_int len)
{
  char* encoded = json_object_get_string(json_object_object_get(json, field));
  u_int decoded_len = Nymble::hexdecode(encoded);
  u_char decoded[decoded_len];
  
  Nymble::hexdecode(encoded, decoded);
  
  if (decoded_len == len) {
    memcpy(value, decoded, len);
  } else {
    fprintf(stderr, "Couldn't unmarshall field %s because due to unexpected length %d (expected %d)", field, decoded_len, len);
  }
}