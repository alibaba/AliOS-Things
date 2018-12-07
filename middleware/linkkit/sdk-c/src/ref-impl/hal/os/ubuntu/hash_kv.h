#ifndef __HASH_KV_H__
#define __HASH_KV_H__

#ifdef __cplusplus
extern "C" {
#endif


int kv_get(const char *key, void *value, int *value_len);
int kv_set(const char *key, void *value, int value_len);
int kv_del(const char *key);

#ifdef __cplusplus
}
#endif

#endif
