/**
 * ant_aos/kv.h
 *
 * KV�洢���ʽӿ�
 */
#ifndef __ANT_KV_H__
#define __ANT_KV_H__

#include "ant_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif
    
typedef void* kv_handle;

void      ant_kv_init(void);

kv_handle ant_kv_open(void);

ant_s32   ant_kv_get_string(void *h, const char *key, char *value, ant_u32 *len);

ant_s32   ant_kv_set_string(void *h, const char *key, char *value);

ant_s32   ant_kv_get_u32(void *h, const char *key, ant_u32 *value);

ant_s32   ant_kv_set_u32(void *h, const char *key, ant_u32 value);

ant_s32   ant_kv_erase(void *h, const char *key);

void      ant_kv_close(kv_handle h);

void *ant_iot_kv_open(const char *block_name);

int ant_iot_kv_get(const void *h, const char *key, void **value, size_t *len);

int ant_iot_kv_set(const void *h, const char *key, const void *value, size_t len);

int ant_iot_kv_delete(const void *h, const char *key);

void ant_iot_kv_close(const void *handle);

ant_s32 ant_kv_erase_iot_data(void);

ant_s32 ant_kv_erase_ant_data(void);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif

