/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_H_
#define _SST_H_

#include <stdint.h>

#define SST_SUCCESS                       (0x00000000)
#define SST_ERROR_GENERIC                 (0xFFFF0000)
#define SST_ERROR_ACCESS_DENIED           (0xFFFF0001)
#define SST_ERROR_ITEM_NOT_FOUND          (0xFFFF0002)
#define SST_ERROR_BAD_PARAMETERS          (0xFFFF0003)
#define SST_ERROR_OUT_OF_MEMORY           (0xFFFF0004)
#define SST_ERROR_STORAGE_NO_SPACE        (0xFFFF0006)
#define SST_ERROR_STORAGE_NOT_AVAILABLE   (0xFFFF0007)
#define SST_ERROR_BUSY                    (0xFFFF0008)
#define SST_ERROR_OVERFLOW                (0xFFFF0009)
#define SST_ERROR_SHORT_BUFFER            (0xFFFF000A)
#define SST_ERROR_ACCESS_CONFLICT         (0xFFFF000B)
#define SST_ERROR_OBJ_ID                  (0xFFFF000C)
#define SST_ERROR_OBJ_CORRUPT             (0xFFFF000D)
#define SST_ERROR_OBJ_RECOVER             (0xFFFF000E)

#define  SST_MIGRATION_KEY_LEN    16

#define SST_TYPE_PASSWORD   (0)
#define SST_TYPE_KEY        (1)
#define SST_TYPE_CERT       (2)
#define SST_TYPE_USERDATA   (3)
#define SST_TYPE_NONE       (0xFFFFFFFF)

uint32_t sst_init(void);

uint32_t sst_add_item(const char *name, const uint8_t *data,
                      uint32_t data_len, uint32_t type, uint32_t flag);

uint32_t sst_get_item(const char *name, uint8_t *data, uint32_t *data_len, uint32_t *type);

uint32_t sst_delete_item(const char *name);

void sst_mig_set_file_path(const char *path);

uint32_t sst_mig_get_file(const char *name, uint8_t *key, uint32_t *key_len);

uint32_t sst_mig_store_file(const char *name, uint8_t *key, uint32_t key_len);

uint32_t sst_encrypt_data(uint8_t *data, uint32_t data_len,
                          const char *file_path, uint8_t *key, uint32_t *key_len);

uint32_t sst_decrypt_data(const char *name, uint8_t *key, uint32_t key_len,
                          uint8_t *data, uint32_t *data_len);

#endif /* _SST_H_ */

