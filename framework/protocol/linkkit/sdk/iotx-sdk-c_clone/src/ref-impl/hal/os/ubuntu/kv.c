/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/stat.h>
#include "iotx_hal_internal.h"
#include "kv.h"
#include "cJSON.h"
#include "base64.h"

struct kv_file_s {
    char filename[128];
    cJSON *json_root;

    pthread_mutex_t lock;
};

/*
 * update KV file atomically:
 *   step 1. save data in temporary file
 *   step 2. rename temporary file to the orignal one
 */
static int kv_sync(kv_file_t *file)
{
    char *json = cJSON_Print(file->json_root);
    if (!json)
        return -1;

    /* create temporary file in the same directory as orignal KV file */
    char fullpath[128] = {0};
    strncpy(fullpath, file->filename, sizeof(fullpath) - 1);

    char *dname = dirname(fullpath);
    char *template = "/tmpfile.XXXXXX";

    int pathlen = strlen(dname) + strlen(template) + 1;
    if (pathlen > sizeof(fullpath)) {
        free(json);
        return -1;
    }

    if (dname == fullpath) {    /* see dirname man-page for more detail */
        strcat(fullpath, template);
    } else {
        strcpy(fullpath, dname);
        strcat(fullpath, template);
    }

    int tmpfd = mkstemp(fullpath);
    if (tmpfd < 0) {
        hal_err("kv_sync open");
        free(json);
        return -1;
    }

    /* write json data into temporary file */
    int len = strlen(json) + 1;
    if (write(tmpfd, json, len) != len) {
        hal_err("kv_sync write");
        close(tmpfd);
        free(json);
        return -1;
    }

    fsync(tmpfd);
    close(tmpfd);
    free(json);

    /* save KV file atomically */
    if (rename(fullpath, file->filename) < 0) {
        hal_err("rename");
        return -1;
    }

    return 0;
}

static char *read_file(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return NULL;

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return NULL;
    }

    char *buf = malloc(st.st_size);
    if (!buf) {
        close(fd);
        return NULL;
    }

    if (read(fd, buf, st.st_size) != st.st_size) {
        free(buf);
        close(fd);
        return NULL;
    }

    close(fd);

    return buf;
}

static int create_json_file(char *filename)
{
    int fd = open(filename, O_CREAT | O_RDWR, 0644);
    if (fd < 0)
        return -1;

    if (write(fd, "{}", 3) != 3) {  /* 3 = '{}' + null terminator */
        close(fd);
        return -1;
    }

    if (fsync(fd) < 0) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

kv_file_t *kv_open(char *filename)
{
    kv_file_t *file = malloc(sizeof(kv_file_t));
    if (!file)
        return NULL;
    memset(file, 0, sizeof(kv_file_t));

    if (strlen(filename) > sizeof(file->filename) - 1) {
        hal_err("filename %s is too long\n", filename);
        goto fail;
    }

    strncpy(file->filename, filename, sizeof(file->filename) - 1);

    if (access(file->filename, F_OK) < 0) {
        /* create KV file when not exist */
        if (create_json_file(file->filename) < 0)
            goto fail;
    }

    char *json = read_file(filename);
    if (!json)
        goto fail;

    file->json_root = cJSON_Parse(json);
    if (!file->json_root) {
        free(json);
        goto fail;
    }

    pthread_mutex_init(&file->lock, NULL);

    free(json);

    return file;

fail:
    if (file->json_root)
        cJSON_Delete(file->json_root);
    free(file);

    return NULL;
}

int kv_close(kv_file_t *file)
{
    if (!file)
        return -1;

    pthread_mutex_destroy(&file->lock);

    if (file->json_root)
        cJSON_Delete(file->json_root);
    free(file);

    return 0;
}

int kv_get(kv_file_t *file, char *key, char *value, int value_len)
{
    if (!file || !file->json_root || !key || !value || value_len <= 0)
        return -1;

    pthread_mutex_lock(&file->lock);

    cJSON *obj = cJSON_GetObjectItem(file->json_root, key);
    if (!obj) {
        pthread_mutex_unlock(&file->lock);
        return -1;
    }

    strncpy(value, obj->valuestring, value_len - 1);
    value[value_len - 1] = '\0';

    pthread_mutex_unlock(&file->lock);

    return 0;
}

int kv_set(kv_file_t *file, char *key, char *value)
{
    if (!file || !file->json_root || !key || !value)
        return -1;

    pthread_mutex_lock(&file->lock);
    /* remove old value if exist */
    cJSON_DeleteItemFromObject(file->json_root, key);
    cJSON_AddItemToObject(file->json_root, key, cJSON_CreateString(value));

    int ret = kv_sync(file);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

int kv_del(kv_file_t *file, char *key)
{
    if (!file || !file->json_root || !key)
        return -1;

    /* remove old value if exist */
    pthread_mutex_lock(&file->lock);
    cJSON_DeleteItemFromObject(file->json_root, key);
    int ret = kv_sync(file);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

int kv_set_blob(kv_file_t *file, char *key, void *value, int value_len)
{
    int encoded_len = ABase64_EncodeLen(value_len);

    char *encoded = malloc(encoded_len);
    if (!encoded)
        return -1;

    ABase64_Encode(value, value_len, encoded, encoded_len);

    int ret = kv_set(file, key, encoded);

    free(encoded);

    return ret;
}

int kv_get_blob(kv_file_t *file, char *key, void *value, int *value_len)
{
    if (!file || !file->json_root || !key || !value || !value_len || *value_len <= 0)
        return -1;

    int ret = kv_get(file, key, value, *value_len);
    if (ret < 0)
        return ret;

    int decode_len = ABase64_DecodeLen(value);

    uint8_t *decoded = malloc(decode_len);
    if (!decoded)
        return -1;

    if (ABase64_Decode(value, decoded, decode_len) < 0) {
        free(decoded);
        return -1;
    }

    if (decode_len > *value_len) {
        free(decoded);
        return -1;
    }

    memcpy(value, decoded, decode_len);
    free(decoded);

    *value_len = decode_len;

    return 0;
}
