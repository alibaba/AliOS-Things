/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_OSA_H_
#define _SST_OSA_H_

#include <stdlib.h>
#include <string.h>
#include "ls_osa.h"

#define SST_VERSION 0x01000100

#define sst_memcpy memcpy
#define sst_memset memset
#define sst_memcmp memcmp
#define sst_strlen strlen
#define sst_strcpy strcpy
#define sst_strncpy strncpy

#define INIT_MIG_MAGIC(a) \
        do { \
            a[0] = 'a'; \
            a[1] = 'M'; \
            a[2] = 'i'; \
            a[3] = 'g'; \
        } while(0);


#define IS_SST_MIG_MAGIC_VALID(_m)  (('a' == _m[0]) &&  \
                                    ('M' == _m[1]) &&   \
                                    ('i' == _m[2]) &&   \
                                    ('g' == _m[3]))


/* the head size=48 Bytes,in front of the data*/
typedef struct _sst_mig_head {
    uint8_t     magic[4];
    uint32_t    version;
    uint32_t    type;
    uint32_t    data_size;
    uint8_t     pt_hash[32];    /*plaintext data hash:sha256*/
} sst_mig_head;

uint64_t sst_current_raw_time(void);

uint32_t sst_store_obj(const char *name, void *file_data, uint32_t file_len, uint32_t flag);

uint32_t sst_get_obj(const char *name, void **pp_data, uint32_t *p_file_len);

uint32_t sst_delete_obj(const char *name);

uint32_t sst_hal_init();

void sst_hal_deinit();

void sst_set_mig_obj_path(const char *path);

//store data in .mig file
uint32_t sst_get_mig_obj(const char *name, uint8_t *key, uint32_t *key_len);

//store data in sst file
uint32_t sst_store_mig_obj(const char *name, uint8_t *key, uint32_t key_len);

uint32_t sst_migration_enc_data(const char *name,
                                uint8_t *data_in,
                                uint32_t data_size,
                                uint8_t *key,
                                uint32_t *key_len);

uint32_t sst_migration_dec_data(const char *name,
                                uint8_t *data_out,
                                uint32_t *data_size,
                                uint8_t *key,
                                uint32_t key_len
                               );

#endif /* _SST_OSA_H_ */
