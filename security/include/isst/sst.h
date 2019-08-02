/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_H_
#define _SST_H_

#include <stdint.h>

//sst error code
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
#define SST_ERROR_NOT_SUPPORTED           (0xFFFF000F)

#define  SST_MIGRATION_KEY_LEN    16

//the type of the item
#define SST_TYPE_PASSWORD   (0)
#define SST_TYPE_KEY        (1)
#define SST_TYPE_CERT       (2)
#define SST_TYPE_USERDATA   (3)
#define SST_TYPE_NONE       (0xFFFFFFFF)

/**
 * @brief init the sst module
 *
 * @return see sst error code.
 */
uint32_t sst_init(void);

/**
 * @brief sst store item.
 *
 * @param[in]  name       the name of the item.
 * @param[in]  data       the data stored to sst.
 * @param[in]  data_len   the length of the data.
 * @param[in]  type       the type of item.
 * @param[in]  overwrite  1: if item exist, update
 *                        0: if item exist, return SST_ERROR_ACCESS_CONFLICT
 *
 * @return see sst error code.
 */
uint32_t sst_add_item(const char *name, const uint8_t *data,
                      uint32_t data_len, uint32_t type, uint32_t overwrite);
/**
 * @brief sst get item.
 *
 * @param[in]      name       the name of the item.
 * @param[out]      data       the data stored to sst.
 * @param[in_out]  data_len   the length of the data.
 * @param[out]     type       the type of item.
 *
 * @return see sst error code.
 */
uint32_t sst_get_item(const char *name, uint8_t *data, uint32_t *data_len, uint32_t *type);

/**
 * @brief sst delete item.
 *
 * @param[in]      name       the name of the item.
 *
 * @return see sst error code.
 */
uint32_t sst_delete_item(const char *name);

/**
 * @brief sst cleanup.
 */
void sst_cleanup(void);


/********************************************************************/
/*                                                                  */
/*         for data migration, only supported for linux             */
/*                                                                  */
/********************************************************************/

/**
 * @brief sst set migration file path.
 *
 * @param[in]      name       the path of migration file.
 */
void sst_mig_set_file_path(const char *path);

/**
 * @brief sst get migration file.
 *
 * @param[in]      name       the name of the item.
 * @param[out]     key        the key used to encrypt item value.
 * @param[in_out]  key_len    the length of the key.
 *
 * @return see sst error code.
 */
uint32_t sst_mig_get_file(const char *name, uint8_t *key, uint32_t *key_len);

/**
 * @brief sst get sst file based on migration file.
 *
 * @param[in]      name       the name of the item.
 * @param[in]      key        the key used to decrypt data in migration file.
 * @param[in]      key_len    the length of the key.
 *
 * @return see sst error code.
 */
uint32_t sst_mig_store_file(const char *name, uint8_t *key, uint32_t key_len);

/**
 * @brief generate key to encrypt data and store the ciphertext to file.
 *
 * @param[in]       data       the data to be encrypted.
 * @param[in]       data_len   the length of the data.
 * @param[in]       file_path  the path of the file to store the encrypted data.
 * @param[out]      key        key used to encrypt the data.
 * @param[in_out]   key_len    the length of the key.
 *
 * @return see sst error code.
 */
uint32_t sst_encrypt_data(uint8_t *data, uint32_t data_len,
                          const char *file_path, uint8_t *key, uint32_t *key_len);
/**
 * @brief get the plaintext from file
 *
 * @param[in]       name       the path of the file.
 * @param[in]       key        the key used to decrypt data in file.
 * @param[in]       key_len    the length of the key.
 * @param[out]      data       the plaintext.
 * @param[in_out]   data_len   the length of the data.
 *
 * @return see sst error code.
 */
uint32_t sst_decrypt_data(const char *name, uint8_t *key, uint32_t key_len,
                          uint8_t *data, uint32_t *data_len);


#endif /* _SST_H_ */

