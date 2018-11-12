#ifndef OTA_RSA_VERIFY_H
#define OTA_RSA_VERIFY_H
#include "ota_rsa_param.h"

typedef enum {
    OTA_SIGN_OFF = 0,
    OTA_SIGN_ON
} OTA_SIGN_E;

typedef struct {
    char sign_enable;
    unsigned char sign_value[256];
}ota_sign_t;

int ota_verify_download_rsa_sign(unsigned char* sign_dat, const char* src_hash_dat, OTA_HASH_E src_hash_method);
#endif
