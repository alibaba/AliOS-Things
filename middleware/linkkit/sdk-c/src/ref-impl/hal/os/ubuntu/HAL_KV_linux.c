#if defined(HAL_KV)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iotx_hal_internal.h"

#define TABLE_COL_SIZE    (384)
#define TABLE_ROW_SIZE    (2)

#define ITEM_MAX_KEY_LEN     128 /* The max key length for key-value item */
#define ITEM_MAX_VAL_LEN     512 /* The max value length for key-value item */
#define ITEM_MAX_LEN         sizeof(kv_item_t)

#define KV_FILE_NAME         "linkkit_kv.bin"

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
        hal_err("paras err");
        return -1;
    }
    
    value_len = value_len > ITEM_MAX_VAL_LEN? ITEM_MAX_VAL_LEN:value_len;
    i = hash_gen(key);
    hal_err("hash i= %d", i);
    read_size = ITEM_MAX_LEN * TABLE_ROW_SIZE;
    kv = malloc(read_size);
    if (kv == NULL) {
        hal_err("malloc kv err");
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        hal_err("read kv err");
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
            hal_err("hash row full");
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
        hal_err("write_kv_item err");
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
        hal_err("paras err");
        return -1;
    }
    
    i = hash_gen(key);

    read_size = sizeof(kv_item_t) * TABLE_ROW_SIZE;
    kv = malloc(read_size);
    if (kv == NULL) {
        hal_err("malloc kv err");
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        hal_err("read kv err");
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
    hal_err("not found");
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
    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        hal_err("open err");
        return -1;
    }
    
    if (fstat(fd, &st) < 0) {
        hal_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        hal_err("read overstep");
        close(fd);
        return -1;
    }

    offset =  location * ITEM_MAX_LEN * TABLE_ROW_SIZE;
    ret = lseek(fd, offset, SEEK_SET);
    if(ret < 0) {
        hal_err("lseek err");
        close(fd);
        return -1;
    }

    if (read(fd, buf, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        hal_err("read err");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int write_kv_item(const char *filename, void *data, int location)
{
    struct stat st;
    int offset;
    int ret;
    int fd = open(filename, O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        hal_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        hal_err("overstep st.st_size = %ld location =%d cur loc=%ld", st.st_size, location,
                (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE);
        close(fd);
        return -1;
    }

    offset = (location) * ITEM_MAX_LEN * TABLE_ROW_SIZE;
    ret = lseek(fd, offset, SEEK_SET);
    if(ret < 0) {
        hal_err("lseek err");
        close(fd);
        return -1;
    }

    if (write(fd, data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        hal_err("kv write failed");
        close(fd);
        return -1;
    }

    fsync(fd);
    close(fd);

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
    fd = open(hash_kv->filename, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        return -1;
    }

    for (i = 0; i < TABLE_COL_SIZE ; i++) {

        if (write(fd, init_data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN *
            TABLE_ROW_SIZE) { /* 3 = '{}' + null terminator */
            hal_err("write err");
            close(fd);
            return -1;
        }
    }

    if (fsync(fd) < 0) {
        close(fd);
        return -1;
    }

    close(fd);
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
    pthread_mutex_init(&file->lock, NULL);
    pthread_mutex_lock(&file->lock);

    if (access(file->filename, F_OK) < 0) {
        /* create KV file when not exist */
        if (create_hash_file(file) < 0) {
            goto fail;
        }
    }
    pthread_mutex_unlock(&file->lock);
    return file;
fail:
    pthread_mutex_unlock(&file->lock);
    free(file);

    return NULL;
}

static int __kv_get(kv_file_t *file, const char *key, void *value, int *value_len)
{
    int ret;
    if (!file || !key || !value || !value_len || *value_len <= 0) {
        return -1;
    }

    pthread_mutex_lock(&file->lock);
    ret = hash_table_get(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

static int __kv_set(kv_file_t *file, const char *key, void *value, int value_len)
{
    int ret;
    if (!file || !key || !value || value_len <= 0) {
        return -1;
    }

    pthread_mutex_lock(&file->lock);
    ret = hash_table_put(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

int __kv_del(kv_file_t *file, const  char *key)
{
    int ret;
    if (!file || !key) {
        return -1;
    }

    /* remove old value if exist */
    pthread_mutex_lock(&file->lock);
    ret = hash_table_rm(file, key);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

static kv_file_t *file = NULL;
static int kv_get(const char *key, void *value, int *value_len)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            hal_err("kv_open failed");
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
            hal_err("kv_open failed");
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
            hal_err("kv_open failed");
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

#endif  /* #if defined(HAL_KV) */

