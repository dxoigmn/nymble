#ifndef __NYMBLE_H__
#define __NYMBLE_H__

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
#define TRAPDOORENC_SIZE    (CIPHER_BLOCK_SIZE + DIGEST_SIZE + DIGEST_SIZE + CIPHER_BLOCK_SIZE)

typedef unsigned char u_char;
typedef unsigned int  u_int;

class Nymble
{
  public:
    u_int cur_link_window;
    u_int cur_time_period;
    
    Nymble();
    
    void setLinkWindow(u_int link_window);
    u_int getLinkWindow();
    
    void setTimePeriod(u_int time_period);
    u_int getTimePeriod();
    
    static void digest(u_char *out, u_char *in, u_int size);
    static void random_bytes(u_char *out, u_int size);
    static void hexencode(char *out, u_char *in, u_int size);
    static void hexdecode(u_char *out, char *in, u_int size);
};

#endif
