#ifndef OTA_RSA_VERIFY_H
#define OTA_RSA_VERIFY_H
#include "ota_rsa_param.h"

typedef enum {
    OTA_SIGN_OFF = 0,
    OTA_SIGN_ON
} OTA_SIGN_T;

typedef struct {
    char sign_enable;
    unsigned char sign_value[256];
}ota_sign_params;

int ota_verify_download_rsa_sign(unsigned char* sign_dat, const char* src_hash_dat, ota_hash_type_t src_hash_method);

#endif
