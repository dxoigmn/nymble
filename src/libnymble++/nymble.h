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
#define SIGNATURE_SIZE      128
#define TRAPDOORENC_SIZE    (CIPHER_BLOCK_SIZE + DIGEST_SIZE + DIGEST_SIZE + CIPHER_BLOCK_SIZE)

#define JSON_MARSHALL_INT(OBJ,FIELD) \
        json_object_object_add(json_##OBJ, #FIELD, json_object_new_int(this->FIELD))

#define JSON_UNMARSHALL_INT(OBJ,FIELD) \
        OBJ->FIELD = json_object_get_int(json_object_object_get(json_##OBJ, #FIELD))

#define JSON_MARSHALL_STR(OBJ,FIELD,SIZE) \
        json_object_object_add(json_##OBJ, #FIELD, json_object_new_string(Nymble::hexencode(this->FIELD, SIZE)))

#define JSON_UNMARSHALL_STR(OBJ,FIELD,SIZE) \
        memcpy(OBJ->FIELD, Nymble::hexdecode(json_object_get_string(json_object_object_get(json_##OBJ, #FIELD))), SIZE);

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
    static char* hexencode(u_char *in, u_int size);
    static u_char* hexdecode(char *in, u_int* size = NULL);
};

#endif
