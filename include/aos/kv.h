/**
 * @file kv.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_KV_H
#define AOS_KV_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_kv KV
 *  KV AOS API: Data storing service based on key-value paring.
 *
 *  @{
 */

/**
 * Init the kv module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int aos_kv_init(void);

/**
 * Deinit the kv module.
 *
 * @retrun none
 */
void aos_kv_deinit(void);

/**
 * Add a new KV pair.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  value  the value of the KV pair.
 * @param[in]  len    the length of the value.
 * @param[in]  sync   save the KV pair to flash right now (should always be 1).
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_set(const char *key, const void *value, int len, int sync);

/**
 * Get the KV pair's value stored in buffer by its key.
 *
 * @note: the buffer_len should not be smaller than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_get(const char *key, void *buffer, int *buffer_len);

/**
 * Delete the KV pair by its key.
 *
 * @param[in]  key  the key of the KV pair to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_del(const char *key);

/**
 * Delete the KV pair by its prefix.
 *
 * @param[in]  prefix  the prefix of the kv pair which is need to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_del_by_prefix(const char *prefix);

/**
 * Add a new KV pair with encryption.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  value  the value of the KV pair.
 * @param[in]  len    the length of the value.
 * @param[in]  sync   save the KV pair to flash right now (should always be 1).
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_secure_set(const char *key, const void *value, int len, int sync);

/**
 * Get the KV pair's value stored in buffer by its key with decrypt.
 *
 * @note: the buffer_len should be larger than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int aos_kv_secure_get(const char *key, void *buffer, int *buffer_len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_KV_H */

