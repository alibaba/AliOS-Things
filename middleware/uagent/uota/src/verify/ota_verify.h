/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_VERIFY_H_
#define OTA_VERIFY_H_
#include <stdint.h>
#include "ota_manifest.h"
#include "ota_transport.h"

#define AOS_SINGLE_TAG (0xabababab)
#define AOS_KERNEL_TAG (0xcdcdcdcd)
#define AOS_APP_TAG    (0xefefefef)

typedef struct
{
    ota_hash_type_t hash_method;
    char            hash_value[65];
} ota_hash_params;

typedef struct
{
    ota_hash_type_t hash_method;
    int         ctx_size;
    void       *ctx_hash;
} ota_hash_ctx_params;

typedef struct
{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[16];
    unsigned char  image_reserver[2];
    unsigned short image_crc16;
} ota_image_info;


void     ota_free_global_context(void);
void     ota_save_state(uint32_t breakpoint, ota_hash_ctx_params *hash_ctx);
uint32_t ota_get_update_breakpoint(void);
int      ota_malloc_global_context(ota_hash_type_t type);
int      ota_check_hash_value(ota_hash_params *download_hash);
int      ota_set_update_breakpoint(uint32_t offset);
int      ota_get_last_hash_context(ota_hash_ctx_params *hash_ctx);
int      ota_set_cur_hash_context(ota_hash_ctx_params *hash_ctx);
int      ota_get_last_hash(char *value);
int      ota_set_cur_hash(char *value);
int ota_verify_hash_value(ota_hash_params last_hash, ota_hash_params cur_hash);
ota_hash_ctx_params *ota_get_global_hash_context(void);
unsigned char       *ota_get_identity_image_md5_strvalue(void);
int                  ota_check_image(ota_read_cb_t read_fuc);
#endif
