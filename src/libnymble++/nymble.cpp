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

void Nymble::digest(u_char *out, u_char *in, u_int size) {
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, in, size);
  SHA256_Final(out, &ctx);
}

void Nymble::random_bytes(u_char *out, u_int size) {
  RAND_bytes(out, size);
}

void Nymble::hexencode(char *out, u_char *in, u_int size)
{
  for (u_int i = 0; i < size; i++) {
    sprintf((char*) (out + i*2), "%02x", (u_char) in[i]);
  }
}

void Nymble::hexdecode(u_char *out, char *in, u_int size)
{
  for (u_int i = 0; i < size / 2; i++) {
    sscanf((char*) (in + i*2), "%02x", (u_int*) (out + i));
  }
}