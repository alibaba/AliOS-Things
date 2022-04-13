/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"

#ifdef AMP_KV_ENABLE
#include "amp_errno.h"
#include "amp_system.h"
#include "kv_api.h"

static int amp_kv_to_res(int res)
{
    switch (res) {
        case KV_OK:
            return 0;
        case KV_ERR_NO_SPACE:
            return -ENOSPC;
        case KV_ERR_INVALID_PARAM:
            return -EINVAL;
        case KV_ERR_MALLOC_FAILED:
            return -ENOMEM;
        case KV_ERR_NOT_FOUND:
            return -ENOENT;
        case KV_ERR_FLASH_READ:
        case KV_ERR_FLASH_WRITE:
        case KV_ERR_FLASH_ERASE:
            return -EIO;
        case KV_ERR_NOT_SUPPORT:
            return -ENOSYS;
        default:
            return -EBUSY;
    }
}

#else

#include "wrappers_defs.h"
#include "amp_fs.h"
#include "amp_kv.h"

#define TABLE_COL_SIZE    (384)
#define TABLE_ROW_SIZE    (2)

#define ITEM_MAX_KEY_LEN     128 /* The max key length for key-value item */
#define ITEM_MAX_VAL_LEN     512 /* The max value length for key-value item */
#define ITEM_MAX_LEN         sizeof(kv_item_t)

#define KV_FILE_NAME         "U:/amp_kv.bin"

#define kv_err(...)               do{HAL_Printf("amp_kv.c@%d: ", __LINE__);HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)

typedef struct kv {
    char key[ITEM_MAX_KEY_LEN];
    uint8_t value[ITEM_MAX_VAL_LEN];
    int value_len;
} kv_item_t;

typedef struct kv_file_s {
    const char *filename;
    void *lock;
} kv_file_t;

static int kv_get(const char *key, void *value, int *value_len);
static int kv_set(const char *key, void *value, int value_len);
static int kv_del(const char *key);
static unsigned int hash_gen(const char *key);
static int hash_table_put(kv_file_t *file, const  char *key, void *value, int value_len);
static int hash_table_get(kv_file_t *file, const char *key, void *value, int *len);
static int hash_table_rm(kv_file_t *file,  const  char *key);
static kv_file_t *kv_open(const char *filename);
static int read_kv_item(const char *filename, void *buf, int location);
static int write_kv_item(const char *filename, void *data, int location);

static void free_kv(struct kv *kv)
{
    if (kv) {
        kv->value_len = 0;
        HAL_Free(kv);
    }
}

static unsigned int hash_gen(const char *key)
{
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + hash + *key++;
    }
    return hash % TABLE_COL_SIZE;
}

/* insert or update a value indexed by key */
static int hash_table_put(kv_file_t *file, const  char *key, void *value, int value_len)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    kv_item_t *p;
    if (!file || !file->filename ||  !key || !value  || value_len <= 0) {
        kv_err("paras err");
        return -1;
    }

    value_len = value_len > ITEM_MAX_VAL_LEN ? ITEM_MAX_VAL_LEN : value_len;
    i = hash_gen(key);
    kv_err("hash i= %d", i);
    read_size = ITEM_MAX_LEN * TABLE_ROW_SIZE;
    kv = HAL_Malloc(read_size);
    if (kv == NULL) {
        kv_err("malloc kv err");
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        kv_err("read kv err");
        free_kv(kv);
        return -1;
    }
    p = &kv[j];

    while (p && p->value_len) { /* if key is already stroed, update its value */

        if (strcmp(p->key, key) == 0) {
            memset(p->value, 0, ITEM_MAX_VAL_LEN);
            memcpy(p->value, value, value_len);
            p->value_len = value_len;
            break;
        }

        if (++j == TABLE_ROW_SIZE) {
            kv_err("hash row full");
            HAL_Free(kv);
            return -1;
        }
        p = &kv[j];
    }

    p = &kv[j];
    if (p && !p->value_len) {/* if key has not been stored, then add it */
        //p->next = NULL;
        strncpy(p->key, key, ITEM_MAX_KEY_LEN - 1);
        memcpy(p->value, value, value_len);
        p->value_len = value_len;
    }

    if (write_kv_item(file->filename, kv, i) < 0) {
        kv_err("write_kv_item err");
        HAL_Free(kv);
        return -1;
    }
    HAL_Free(kv);
    return 0;
}

/* get a value indexed by key */
static int hash_table_get(kv_file_t *file, const char *key, void *value, int *len)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    struct kv *p;
    if (!file || !file->filename || !key || !value || !len  || *len <= 0) {
        kv_err("paras err");
        return -1;
    }

    i = hash_gen(key);

    read_size = sizeof(kv_item_t) * TABLE_ROW_SIZE;
    kv = HAL_Malloc(read_size);
    if (kv == NULL) {
        kv_err("malloc kv err");
        return -1;
    }
    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        kv_err("read kv err");
        free_kv(kv);
        return -1;
    }
    // struct kv *p = ht->table[i];
    p = &kv[j];

    while (p && p->value_len) {
        if (strcmp(key, p->key) == 0) {
            *len = p->value_len < *len ? p->value_len : *len;
            memcpy(value, p->value, *len);
            free_kv(kv);
            return 0;
        }
        if (++j == TABLE_ROW_SIZE) {
            break;
        }
        p = &kv[j];
    }
    free_kv(kv);
    kv_err("not found");
    return -1;
}

/* remove a value indexed by key */
static int hash_table_rm(kv_file_t *file,  const  char *key)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    struct kv *p;
    if (!file || !file->filename ||  !key) {
        return -1;
    }
    i = hash_gen(key) % TABLE_COL_SIZE;
    read_size = sizeof(kv_item_t) * TABLE_ROW_SIZE;
    kv = HAL_Malloc(read_size);
    if (kv == NULL) {
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        free_kv(kv);
        return -1;
    }

    p = &kv[j];

    while (p && p->value_len) {
        if (strcmp(key, p->key) == 0) {
            memset(p, 0, ITEM_MAX_LEN);
        }
        if (++j == TABLE_ROW_SIZE) {
            break;
        }
        p = &kv[j];
    }

    if (write_kv_item(file->filename, kv, i) < 0) {
        free_kv(kv);
        return -1;
    }
    free_kv(kv);
    return 0;
}

static int read_kv_item(const char *filename, void *buf, int location)
{
    struct hal_stat st;
	int32_t cur_pos = 0;
    int ret = 0;
    int offset;
    void *fp;

    fp = HAL_Fopen(filename, "r");
    if (fp == NULL) {
        kv_err("open err");
        return -1;
    }

    memset(&st, 0, sizeof(st));
    if (HAL_stat(filename, &st) < 0) {
        kv_err("fstat err");
        HAL_Fclose(fp);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("read overstep");
        HAL_Fclose(fp);
        return -1;
    }


    offset = location * ITEM_MAX_LEN * TABLE_ROW_SIZE;
    ret = HAL_Fseek(fp, offset, HAL_SEEK_SET, &cur_pos);
    if (ret < 0) {
        kv_err("lseek err %d", ret);
        HAL_Fclose(fp);
        return -1;
    }

    ret = HAL_Fread(buf, ITEM_MAX_LEN * TABLE_ROW_SIZE, 1, fp);
    if (ret < 0) {
        kv_err("read err %d", ret);
        HAL_Fclose(fp);
        return -1;
    }

    HAL_Fclose(fp);
    return 0;
}

static int write_kv_item(const char *filename, void *data, int location)
{
    struct hal_stat st;
	uint32_t cur_pos = 0;
    int offset;
    int ret;
    void *fp;

    fp = HAL_Fopen(filename, "r+");
    if (fp == NULL) {
		kv_err("%s: open %s fail\n", __func__, filename);
        return -1;
    }

    memset(&st, 0, sizeof(st));
    if (HAL_stat(filename, &st) < 0) {
        kv_err("fstat err");
        HAL_Fclose(fp);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("overstep st.st_size = %d location =%d cur loc=%d",
               (int)st.st_size,
               (int)location,
               (int)((location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE));
        HAL_Fclose(fp);
        return -1;
    }

    offset = (location) * ITEM_MAX_LEN * TABLE_ROW_SIZE;

    ret = HAL_Fseek(fp, offset, HAL_SEEK_SET, &cur_pos);
    if (ret < 0) {
        kv_err("lseek err %d", ret);
        HAL_Fclose(fp);
        return -1;
    }

    ret = HAL_Fwrite(data, ITEM_MAX_LEN * TABLE_ROW_SIZE, 1, fp);
    if (ret < 0) {
        kv_err("kv write err %d", ret);
        HAL_Fclose(fp);
        return -1;
    }

    HAL_Fclose(fp);
    return 0;
}

static int create_hash_file(kv_file_t *hash_kv)
{
    int i;
    void *fp;
    char init_data[ITEM_MAX_LEN * TABLE_ROW_SIZE] = {0};
    int ret;

    if (hash_kv == NULL) {
        return -1;
    }
	fp = HAL_Fopen(hash_kv->filename, "r");
	if (fp) {
		HAL_Fclose(fp);
		return 0;
	}

    fp = HAL_Fopen(hash_kv->filename, "w+");
    if (fp == NULL) {
		kv_err("create hash file fail");
        return -1;
    }

    for (i = 0; i < TABLE_COL_SIZE ; i++) {
        ret = HAL_Fwrite(init_data, ITEM_MAX_LEN * TABLE_ROW_SIZE, 1, fp);
        if (ret < 0) { /* 3 = '{}' + null terminator */
            kv_err("write hash file fail %d", ret);
            HAL_Fclose(fp);
            return -1;
        }
    }

    HAL_Fclose(fp);
    kv_err("kv file create ok\n");
    return 0;
}


static kv_file_t *kv_open(const char *filename)
{
    kv_file_t *file;
    struct hal_stat st;

    file = HAL_Malloc(sizeof(kv_file_t));
    if (!file) {
        return NULL;
    }
    memset(file, 0, sizeof(kv_file_t));

    file->filename = filename;
    file->lock = HAL_MutexCreate();
    if (!file->lock) {
        HAL_Free(file);
        return NULL;
    }

    memset(&st, 0, sizeof(st));
    if (HAL_stat(file->filename, &st) < 0 || !HAL_S_ISREG(st.st_mode)) {
        /* create KV file when not exist */
        if (create_hash_file(file) < 0) {
            HAL_MutexDestroy(file->lock);
            HAL_Free(file);
			return NULL;
        }
    }

    return file;
}

static int __kv_get(kv_file_t *file, const char *key, void *value, int *value_len)
{
    int ret;

    if (!file || !key || !value || !value_len || *value_len <= 0) {
        return -1;
    }
    HAL_MutexLock(file->lock);
    ret = hash_table_get(file, key, value, value_len);
    HAL_MutexUnlock(file->lock);

    return ret;
}

static int __kv_set(kv_file_t *file, const char *key, void *value, int value_len)
{
    int ret;
    if (!file || !key || !value || value_len <= 0) {
		kv_err("args invalid\n");
        return -1;
    }

    HAL_MutexLock(file->lock);
    ret = hash_table_put(file, key, value, value_len);
    HAL_MutexUnlock(file->lock);

    return ret;
}

int __kv_del(kv_file_t *file, const  char *key)
{
    int ret;
    if (!file || !key) {
        return -1;
    }

    /* remove old value if exist */
    HAL_MutexLock(file->lock);
    ret = hash_table_rm(file, key);
    HAL_MutexUnlock(file->lock);

    return ret;
}

static kv_file_t *file = NULL;

static int kv_get(const char *key, void *value, int *value_len)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }
    return __kv_get(file, key, value, value_len);
}

static int kv_set(const char *key, void *value, int value_len)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }

    return __kv_set(file, key, value, value_len);
}

static int kv_del(const char *key)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }

    return __kv_del(file, key);
}

#endif

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
#ifdef AMP_KV_ENABLE
    return amp_kv_to_res(kv_item_set(key, val, len));
#else
    return kv_set(key, (void *)val, len);
#endif
}

int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
#ifdef AMP_KV_ENABLE
    return amp_kv_to_res(kv_item_get(key, val, buffer_len));
#else
    return kv_get(key, val, buffer_len);
#endif
}

int HAL_Kv_Del(const char *key)
{
#ifdef AMP_KV_ENABLE
    return amp_kv_to_res(kv_item_delete(key));
#else
    return kv_del(key);
#endif
}

