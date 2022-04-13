/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_system.h
 * @brief:   vendor system apis defination
 * @author:  
 * @date:    2019/12/09
 * @version: 1.0
 */
#ifndef _TG_VENDOR_SYSTEM_H_
#define _TG_VENDOR_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the ID of the CPU chip.
 *
 * @param[in]  buffer where to store the ID of the chip
 * @param[in]  buf_len length of the buffer size
 *
 * @return  the real length in byte of chip ID on success, 0 on failure.
 */
int tg_get_chipid(char *buffer, int buf_len);

/**
 * Add a new KV pair.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  value  the value of the KV pair.
 * @param[in]  len    the length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int tg_kv_set(const char *key, const void *value, int len);

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
int tg_kv_get(const char *key, void *buffer, int *buffer_len);

/**
 * reboot system.
 */
void tg_reboot(void);

int tg_get_mm_free(void);


#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_SYSTEM_H_ */
