/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include "amp_kv.h"
#include "fibocom.h"
#if defined(CONFIG_FIBOCOM_ALIOS)
#include "infra_defs.h"
#include "osi_log.h"
#include "vfs.h"
#endif

#define TABLE_COL_SIZE    (384)
#define TABLE_ROW_SIZE    (2)

#define ITEM_MAX_KEY_LEN     128 /* The max key length for key-value item */
#define ITEM_MAX_VAL_LEN     512 /* The max value length for key-value item */
#define ITEM_MAX_LEN         sizeof(kv_item_t)

#define KV_FILE_NAME         "device_key.bin"

#if defined(CONFIG_FIBOCOM_ALIOS)
#define kv_err OSI_PRINTFI
#else
#define kv_err(...)               do{printf(__VA_ARGS__);printf("\r\n");}while(0)
#endif

typedef struct kv {
    char key[ITEM_MAX_KEY_LEN];
    uint8_t value[ITEM_MAX_VAL_LEN];
    int value_len;
} kv_item_t;

typedef struct kv_file_s {
    const char *filename;
    pthread_mutex_t lock;
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
        free(kv);
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
    kv = malloc(read_size);
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
            free(kv);
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
        free(kv);
        return -1;
    }
    free(kv);
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
    kv = malloc(read_size);
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
    kv = malloc(read_size);
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
    struct stat st;
    int ret = 0;
    int offset;
	
#if defined(CONFIG_FIBOCOM_ALIOS)
	int fd = vfs_open(filename, O_RDONLY);
#else
    int fd = open(filename, O_RDONLY);
#endif

    if (fd < 0) {
        kv_err("open err");
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    if (vfs_fstat(fd, &st) < 0) {
#else
    if (fstat(fd, &st) < 0) {
#endif
        kv_err("fstat err");
#if defined(CONFIG_FIBOCOM_ALIOS)
		vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("read overstep");
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }

    offset =  location * ITEM_MAX_LEN * TABLE_ROW_SIZE;
#if defined(CONFIG_FIBOCOM_ALIOS)
    ret = vfs_lseek(fd, offset, SEEK_SET);
#else
	ret = lseek(fd, offset, SEEK_SET);
#endif
    if (ret < 0) {
        kv_err("lseek err");
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    if (vfs_read(fd, buf, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
#else
    if (read(fd, buf, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
#endif
        kv_err("read err");
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }

#if defined(CONFIG_FIBOCOM_ALIOS)
     vfs_close(fd);
 #else
    close(fd);
#endif
    return 0;
}

static int write_kv_item(const char *filename, void *data, int location)
{
    struct stat st;
    int offset;
    int ret;
#if defined(CONFIG_FIBOCOM_ALIOS)
    int fd = vfs_open(filename, O_WRONLY);
#else
    int fd = open(filename, O_WRONLY);
#endif
    if (fd < 0) {
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    if (vfs_fstat(fd, &st) < 0) {
#else
    if (fstat(fd, &st) < 0) {
#endif
        kv_err("fstat err");
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("overstep st.st_size = %d location =%d cur loc=%d",
               (int)st.st_size,
               (int)location,
               (int)((location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE));
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }

    offset = (location) * ITEM_MAX_LEN * TABLE_ROW_SIZE;
#if defined(CONFIG_FIBOCOM_ALIOS)
    ret = vfs_lseek(fd, offset, SEEK_SET);
#else
    ret = lseek(fd, offset, SEEK_SET);
#endif
    if (ret < 0) {
        kv_err("lseek err");
#if defined(CONFIG_FIBOCOM_ALIOS)
	    vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    if (vfs_write(fd, data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
#else
    if (write(fd, data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
#endif
        kv_err("kv write failed");
#if defined(CONFIG_FIBOCOM_ALIOS)
        vfs_close(fd);
#else
        close(fd);
#endif
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    vfs_fsync(fd);
    vfs_close(fd);
#else
    fsync(fd);
    close(fd);
#endif
    return 0;
}

static int create_hash_file(kv_file_t *hash_kv)
{
    int i;
    int fd;
    char init_data[ITEM_MAX_LEN * TABLE_ROW_SIZE] = {0};
    if (hash_kv == NULL) {
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    fd = vfs_open(hash_kv->filename, O_CREAT | O_RDWR, 0644);
#else
    fd = open(hash_kv->filename, O_CREAT | O_RDWR, 0644);
#endif
    if (fd < 0) {
        return -1;
    }

    for (i = 0; i < TABLE_COL_SIZE ; i++) {
#if defined(CONFIG_FIBOCOM_ALIOS)
        if (vfs_write(fd, init_data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN *
            TABLE_ROW_SIZE) { /* 3 = '{}' + null terminator */
#else
        if (write(fd, init_data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN *
            TABLE_ROW_SIZE) { /* 3 = '{}' + null terminator */
#endif
            kv_err("write err");
#if defined(CONFIG_FIBOCOM_ALIOS)
            vfs_close(fd);
#else
            close(fd);
#endif
            return -1;
        }
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    if (vfs_fsync(fd) < 0) {
        vfs_close(fd);
        return -1;
    }
    vfs_close(fd);	
#else
    if (fsync(fd) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
#endif
    return 0;
}


static kv_file_t *kv_open(const char *filename)
{
    kv_file_t *file = malloc(sizeof(kv_file_t));
    if (!file) {
        return NULL;
    }
    memset(file, 0, sizeof(kv_file_t));

    file->filename = filename;
#if defined(CONFIG_FIBOCOM_ALIOS)
    file->lock = HAL_MutexCreate();
    HAL_MutexLock(file->lock);

    if (vfs_file_size(file->filename) < 0) {
#else
    pthread_mutex_init(&file->lock, NULL);
    pthread_mutex_lock(&file->lock);

    if (access(file->filename, F_OK) < 0) {
#endif
        /* create KV file when not exist */
        if (create_hash_file(file) < 0) {
            goto fail;
        }
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    HAL_MutexUnlock(file->lock);
#else
    pthread_mutex_unlock(&file->lock);
#endif
    return file;
fail:
#if defined(CONFIG_FIBOCOM_ALIOS)
    HAL_MutexUnlock(file->lock);
#else
    pthread_mutex_unlock(&file->lock);
#endif
    free(file);

    return NULL;
}

static int __kv_get(kv_file_t *file, const char *key, void *value, int *value_len)
{
    int ret;

    if (!file || !key || !value || !value_len || *value_len <= 0) {
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    HAL_MutexLock(file->lock);
    ret = hash_table_get(file, key, value, value_len);
    HAL_MutexUnlock(file->lock);
#else
    pthread_mutex_lock(&file->lock);
    ret = hash_table_get(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);
#endif
    return ret;
}

static int __kv_set(kv_file_t *file, const char *key, void *value, int value_len)
{
    int ret;
    if (!file || !key || !value || value_len <= 0) {
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    HAL_MutexLock(file->lock);
    ret = hash_table_put(file, key, value, value_len);
    HAL_MutexUnlock(file->lock);
#else
    pthread_mutex_lock(&file->lock);
    ret = hash_table_put(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);
#endif

    return ret;
}

int __kv_del(kv_file_t *file, const  char *key)
{
    int ret;
    if (!file || !key) {
        return -1;
    }
#if defined(CONFIG_FIBOCOM_ALIOS)
    HAL_MutexLock(file->lock);
    ret = hash_table_rm(file, key);
    HAL_MutexUnlock(file->lock);
#else
    /* remove old value if exist */
    pthread_mutex_lock(&file->lock);
    ret = hash_table_rm(file, key);
    pthread_mutex_unlock(&file->lock);
#endif

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

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    return kv_set(key, (void *)val, len);
}

int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
    return kv_get(key, val, buffer_len);
}

int HAL_Kv_Del(const char *key)
{

    return kv_del(key);
}




