/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef _LS_HAL_SST_H_
#define _LS_HAL_SST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SST_HAL_SUCCESS                   (0x00000000)
#define SST_HAL_ERROR_GENERIC             (0xFFFF0000)
#define SST_HAL_ERROR_ITEM_NOT_FOUND      (0xFFFF0001)
#define SST_HAL_ERROR_STORAGE_NO_SPACE    (0xffff0002)
#define SST_HAL_ERROR_BAD_PARAMETERS      (0xffff0003)
#define SST_HAL_ERROR_OUT_OF_MEMORY       (0xffff0004)

/**
 * @brief init the kv module
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int ls_hal_kv_init(void);

/**
 * @brief deinit the kv module
 *
 * @retrun none
 */
void ls_hal_kv_deinit(void);

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
int ls_hal_kv_set(const char *key, const void *value, int len, int sync);

/**
 * Get the KV pair's value stored in buffer by its key.
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
int ls_hal_kv_get(const char *key, void *buffer, int *buffer_len);

/**
 * Delete the KV pair by its key.
 *
 * @param[in]  key  the key of the KV pair to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int ls_hal_kv_del(const char *key);

#ifdef __cplusplus
}
#endif

#endif /* _LS_HAL_SST_H_ */

