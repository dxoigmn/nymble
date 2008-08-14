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
#include <vector>
#include <json/json.h>

#define DIGEST_SIZE         SHA256_DIGEST_LENGTH
#define CIPHER_BLOCK_SIZE   AES_BLOCK_SIZE
#define SIGNATURE_SIZE      256
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
    
    static void digest(u_char* in, u_int size, u_char* out);
    static void random_bytes(u_int size, u_char* out);
    static u_int hexencode(u_char* in, u_int size, char* out = NULL);
    static u_int hexdecode(char* in, u_char* out = NULL);
    
    static void json_marshal_int(struct json_object* json, char* field, u_int value);
    static void json_unmarshal_int(struct json_object* json, char* field, u_int* value);
    
    static void json_marshal_str(struct json_object* json, char* field, u_char* value, u_int len);
    static void json_unmarshal_str(struct json_object* json, char* field, u_char* value, u_int len);
};

#endif
