/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __KV_H__
#define __KV_H__

typedef struct kv_file_s kv_file_t;

kv_file_t *kv_open(char *filename);
int kv_close(kv_file_t *file);

int kv_get(kv_file_t *file, char *key, char *value, int value_len);
int kv_set(kv_file_t *file, char *key, char *value);

int kv_set_blob(kv_file_t *file, char *key, void *value, int  value_len);
int kv_get_blob(kv_file_t *file, char *key, void *value, int *value_len);

int kv_del(kv_file_t *file, char *key);

#endif /* __KV_H__ */
