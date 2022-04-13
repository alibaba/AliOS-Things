/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* AMP System Abstract Layer */
#ifndef AMP_KV_H
#define AMP_KV_H

/**
 * Init the kv module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int HAL_Kv_Init(void);

/**
 * Deinit the kv module.
 *
 * @retrun none
 */
void HAL_Kv_DeInit(void);

/**
 * Add a new KV pair.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  val    the value of the KV pair.
 * @param[in]  len    the length of the value.
 * @param[in]  sync   save the KV pair to flash right now (should always be 1).
 *
 * @return  0 on success, negative error on failure.
 */
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);

/**
 * Get the KV pair's value stored in buffer by its key.
 *
 * @note: the buffer_len should be larger than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     val         the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);

/**
 * Delete the KV pair by its key.
 *
 * @param[in]  key  the key of the KV pair to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int HAL_Kv_Del(const char *key);


#endif /* AMP_KV_H */
