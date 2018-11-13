/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_VERIFY_H_
#define OTA_VERIFY_H_
#include <stdint.h>
#include "ota_rsa_param.h"
#include "ota_service.h"

#define AOS_SINGLE_TAG (0xabababab)
#define AOS_KERNEL_TAG (0xcdcdcdcd)
#define AOS_APP_TAG    (0xefefefef)

typedef struct
{
    OTA_HASH_E hash_method;
    char       hash_value[65];
} ota_hash_t;

typedef struct
{
    OTA_HASH_E hash_method;
    int         ctx_size;
    void       *ctx_hash;
} ota_hash_param_t;

typedef struct
{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[16];
    unsigned char  image_reserver[2];
    unsigned short image_crc16;
} ota_image_t;

void         ota_save_state(int breakpoint, ota_hash_param_t *hash_ctx);
void         ota_free_hash_ctx(void);
int          ota_get_break_point(void);
int          ota_malloc_hash_ctx(OTA_HASH_E type);
int          ota_check_hash(OTA_HASH_E hash_type, char* hash);
int          ota_set_break_point(int offset);
int          ota_breakpoint_is_valid(void);
int          ota_get_last_hash_ctx(ota_hash_param_t *hash_ctx);
int          ota_set_cur_hash_ctx(ota_hash_param_t *hash_ctx);
int          ota_get_last_hash(char *value);
int          ota_set_cur_hash(char *value);
int          ota_verify_hash_value(ota_hash_t last_hash, ota_hash_t cur_hash);
ota_hash_param_t *ota_get_hash_ctx(void);
unsigned char    *ota_get_identity_image_md5_strvalue(void);
int      ota_check_image(void);
#endif
