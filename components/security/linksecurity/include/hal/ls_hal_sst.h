/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef _LS_HAL_SST_H_
#define _LS_HAL_SST_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* sst hal error code */
#define SST_HAL_SUCCESS                   (0x00000000)
#define SST_HAL_ERROR_GENERIC             (0xFFFF0000)
#define SST_HAL_ERROR_ITEM_NOT_FOUND      (0xFFFF0001)
#define SST_HAL_ERROR_STORAGE_NO_SPACE    (0xffff0002)
#define SST_HAL_ERROR_BAD_PARAMETERS      (0xffff0003)
#define SST_HAL_ERROR_OUT_OF_MEMORY       (0xffff0004)
#define SST_HAL_ERROR_SHORT_BUFFER        (0xffff0005)
#define SST_HAL_ERROR_NOT_SUPPORTED       (0xffff0006)

/**
 * @brief init the kv module
 *
 * @note: if no need to init return SST_HAL_SUCCESS
 *
 * @return see sst hal error code.
 */
uint32_t ls_hal_kv_init(void);

/**
 * @brief deinit the kv module
 *
 * @retrun none
 */
void ls_hal_kv_deinit(void);

/**
 * Add a new KV pair.
 *
 * @note: if KV exist, update KV
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  value  the value of the KV pair.
 * @param[in]  len    the length of the value.
 *
 * @return see sst hal error code.
 */
uint32_t ls_hal_kv_set(const char *key, const void *value, uint32_t len);

/**
 * Get the KV pair's value stored in buffer by its key.
 *
 * @note: if buffer is NULL and *buffer_len is 0, return SST_HAL_ERROR_SHORT_BUFFER
 *        and set *buffer_len to the real length of the value
 * @note: if key not exist return SST_HAL_ERROR_ITEM_NOT_FOUND
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return see sst hal error code.
 */
uint32_t ls_hal_kv_get(const char *key, void *buffer, uint32_t *buffer_len);

/**
 * Delete the KV pair by its key.
 *
 * @param[in] key the key of the KV pair to delete.
 *
 * @return see sst hal error code.
 */
uint32_t ls_hal_kv_del(const char *key);

#ifdef __cplusplus
}
#endif

#endif /* _LS_HAL_SST_H_ */

