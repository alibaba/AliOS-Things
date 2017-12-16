/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lite-utils_internal.h"
#include "json_parser.h"

char *LITE_json_value_of(char *key, char *src)
{
    char       *value = NULL;
    int         value_len = -1;
    char       *ret = NULL;

    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    int         key_len;
    char       *src_iter;

    src_iter = src;
    key_iter = key;

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_next = LITE_malloc(key_len + 1);
            strncpy(key_next, key_iter, key_len);
            key_next[key_len] = '\0';
            value = json_get_value_by_name(src_iter, strlen(src_iter), key_next, &value_len, 0);

            if (value == NULL) {
                LITE_free(key_next);
                return NULL;
            }

            src_iter = value;
            key_iter = delim + 1;
            LITE_free(key_next);
        }
    } while (delim);

    value = json_get_value_by_name(src_iter, strlen(src_iter), key_iter, &value_len, 0);
    if (NULL == value) {
        return NULL;
    }
    ret = LITE_malloc((value_len + 1) * sizeof(char));
    if (NULL == ret) {
        return NULL;
    }
    LITE_snprintf(ret, value_len + 1, "%s", value);
    return ret;
}

void LITE_json_keys_release(list_head_t *keylist)
{
    json_key_t         *pos, *tmp;

    list_for_each_entry_safe(pos, tmp, keylist, json_key_t, list) {
        if (pos->key) {
            LITE_free(pos->key);
        }
        list_del(&pos->list);
        LITE_free(pos);
    }
}

list_head_t *LITE_json_keys_of(char *src, char *prefix)
{
    static              LIST_HEAD(keylist);

    char    *pos = 0, *key = 0, *val = 0;
    int     klen = 0, vlen = 0, vtype = 0;

    if (src == NULL || prefix == NULL) {
        return NULL;
    }

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    json_object_for_each_kv(src, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {

            json_key_t     *entry = NULL;

            entry = LITE_malloc(sizeof(json_key_t));
            memset(entry, 0, sizeof(json_key_t));
            entry->key = LITE_format_string("%s%.*s", prefix, klen, key);
            list_add_tail(&entry->list, &keylist);

            if (JOBJECT == vtype) {
                char *iter_val = LITE_format_string("%.*s", vlen, val);
                char *iter_pre = LITE_format_string("%s%.*s.", prefix, klen, key);
                LITE_json_keys_of(iter_val, iter_pre);
                LITE_free(iter_val);
                LITE_free(iter_pre);
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = LITE_malloc(sizeof(json_key_t));
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;
}
