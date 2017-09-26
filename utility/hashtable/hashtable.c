/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hashtable.h"

#define MODULE "hashtable"

/*the hashtable hander*/
typedef struct {
    int             cnt;//the count of hashtable items .
    aos_mutex_t     lock;
    ht_item_t       *item;
} ht_t;

//refs to: https://en.wikipedia.org/wiki/Jenkins_hash_function
static unsigned int _hash_func(const unsigned char *key, unsigned int length)
{
    unsigned int i = 0;
    unsigned int hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

/**
 * @brief hash table module initiation.
 *
 * @param[in] max_cnt: the count of hashtable items you want.
 *
 * @retval  hashtable hander on success, otherwise NULL will be returned
 */
void *ht_init(int max_cnt)
{
    ht_t *p = NULL;
    int len = max_cnt * sizeof(ht_item_t);

    if (max_cnt <= 0) {
        return NULL;
    }

    p = aos_malloc(sizeof(ht_t));
    if (NULL == p) {
        return NULL;
    }
    memset(p, 0, sizeof(ht_t));

    p->item = aos_malloc(len);
    if (NULL == p->item) {
        aos_free(p);
        return NULL;
    }
    memset(p->item, 0, len);
    aos_mutex_new(&p->lock);

    p->cnt = max_cnt;
    //LOGD(MODULE,"create hashtable : <malloc>%p, item: <malloc>: %p\n", p, p->item);
    return p;
}

/**
 * @brief lock the @ht hashtable .
 *
 * @param[in] ht: the hander of hashtable.
 *
 */
void ht_lock(void *ht)
{
    ht_t *p = (ht_t *)ht;
    if (p) {
        aos_mutex_lock(&p->lock, AOS_WAIT_FOREVER);
    }
}

/**
 * @brief unlock the @ht hashtable after you find/add/delete/delete_r.
 *
 * @param[in] ht: the hander of hashtable.
 */
void ht_unlock(void *ht)
{
    ht_t *p = (ht_t *)ht;
    if (p) {
        aos_mutex_unlock(&p->lock);
    }
}

static ht_item_t *_ht_find_lockless(void *ht, const void *key,
                                    unsigned int key_len)
{
    unsigned int pos = 0;
    ht_t *pt = (ht_t *)ht;

    if (!ht || !key || key_len <= 0) {
        return NULL;
    }

    pos = _hash_func((const unsigned char *)key, key_len) % pt->cnt;

    return (pt->item + pos);//users should check the pointer to get all the values.
}

/**
 * @brief find the item in the @ht whose key is @key in lockless mode, this maybe
 * more efficient.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to find.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 * @param[out]val: the memory to store the found val.
 *            size_val: the  size of returned val
 * @retval  the pointer to value on success, otherwise NULL will be returned
 */
void *ht_find_lockless(void *ht, const void *key, unsigned int key_len,
                       void *val, int *size_val)
{
    ht_item_t *p = _ht_find_lockless(ht, key, key_len);
    void *ret = NULL;

    while (p) {
        if (p->key && !memcmp(p->key, key, key_len)) {
            ret = p->val;
            break;
        }
        p = p->next;
    }

    if (ret && val && size_val) {
        memcpy(val, ret, p->size_val > *size_val ? *size_val : p->size_val);
        *size_val = p->size_val;
    }

    return ret;
}

/**
 * @brief find the item in the @ht whose key is @key .
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to find.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 * @param[out]val: the memory to store the found val.
 *            size_val: the size of returned val
 * @retval  the pointer to @ht_item_t on success, otherwise NULL will be returned
 */
void *ht_find(void *ht, const void *key, unsigned int key_len, void *val,
              int *size_val)
{
    void *ret = NULL;

    ht_lock(ht);
    ret = ht_find_lockless(ht, key, key_len, val, size_val);
    ht_unlock(ht);
    return ret;
}

/**
 * @brief add the item in the @ht whose key is @key and value is @val in lockless mode.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to add.
 *            key_len: the length of @key.
 *            val: the value of the item you want to add.
 *            size_val: the length of the @val.
 * @note: the @key and @val will be re-malloced and stored in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_add_lockless(void *ht, const void *key, unsigned int len_key,
                    const void *val, unsigned int size_val)
{
    ht_item_t *p_item = NULL;
    ht_item_t *p_tmp = NULL;
    ht_item_t *new_tb = NULL;

    if (!ht || !key || !val || len_key <= 0 || size_val <= 0) {
        return -1;
    }

    p_item = _ht_find_lockless(ht, key, len_key);

    if (!p_item->key) {
        p_item->key = aos_malloc(len_key);
        if (!p_item->key) {
            return -1;
        }
        memcpy(p_item->key, key, len_key);
        p_item->val = aos_malloc(size_val);
        if (!p_item->val) {
            aos_free(p_item->key);
            p_item->key = NULL;
            return -1;
        }
        memcpy(p_item->val, val, size_val);
        p_item->size_val = size_val;
        //LOGD(MODULE,"key: <aos_malloc>%p, val: <aos_malloc>%p\n", p_item->key, p_item->val);
        return 0;
    }

    //conflict: add to it's next item in the list.
    p_tmp = p_item;
    while (p_tmp) {
        if (NULL != p_tmp->key && !memcmp(p_tmp->key, key, len_key)) {
            //LOGD(MODULE,"repeated key , aos_free last val: %p, aos_malloc\n", p_tmp->val);
            aos_free(p_tmp->val);
            p_tmp->val = NULL;
            p_tmp->val = aos_malloc(size_val);
            if (!p_item->val) {
                aos_free(p_tmp);
                LOGE(MODULE, "failed to aos_malloc new value.\n");
                return -1;
            }
            p_item->size_val = size_val;
            memcpy(p_item->val, val, size_val);
            return 0;//repeated key , just update it
        }
        p_item = p_tmp;
        p_tmp = p_tmp->next;
    }

    new_tb = (ht_item_t *)aos_malloc(sizeof(ht_item_t));
    if (!new_tb) {
        return -1;
    }
    memset(new_tb, 0, sizeof(ht_item_t));
    new_tb->key = aos_malloc(len_key);
    if (!new_tb->key) {
        aos_free(new_tb);
        return -1;
    }
    memcpy(new_tb->key, key, len_key);
    new_tb->val = aos_malloc(size_val);
    if (!new_tb->val) {
        aos_free(new_tb->key);
        aos_free(new_tb);
        return -1;
    }
    new_tb->size_val = size_val;
    memcpy(new_tb->val, val, size_val);
    p_item->next = new_tb;

    //LOGD(MODULE,"conflict key: <aos_malloc>%p, val: <aos_malloc>%p,node: <aos_malloc>%p, before: %p\n", new_tb->key, new_tb->val, new_tb, p_item);
    return 0;
}

/**
 * @brief add the item in the @ht whose key is @key and value is @val.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to add.
 *            key_len: the length of @key.
 *            val: the value of the item you want to add.
 *            size_val: the length of the @val.
 * @note: the @key and @val will be re-malloced and stored in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_add(void *ht, const void *key, unsigned int len_key, const void *val,
           unsigned int size_val)
{
    int ret = 0;
    ht_lock(ht);
    ret = ht_add_lockless(ht, key, len_key, val, size_val);
    ht_unlock(ht);

    return ret;
}

static int _ht_del_node(void *ht_item, const void *key, unsigned int len_key)
{
    ht_item_t *p_tmp = NULL;
    ht_item_t *next = NULL;
    ht_item_t *parent = NULL; // the root node or previous node
    int flag = 0;
    int ret = -1;

    if (!ht_item) {
        return ret;
    }

    parent = (ht_item_t *)ht_item;
    while (parent) {
        if (parent->key && parent->val &&
            (!key || !memcmp(parent->key, key, len_key))) {
            next = parent->next;//store its next item befroe aos_free.

            //LOGD(MODULE,"del  key: <aos_free>%p, val: <aos_free>%p,node: %p ,<%s>current: %p, next: %p\n",
            // parent->key, parent->val,!flag ? NULL : parent, !flag ? "" : "aos_free", parent, next);
            aos_free(parent->key);
            parent->key = NULL;
            aos_free(parent->val);
            parent->val = NULL;
            ret = 0;
            if (1 == flag) {//just aos_free the added item not the root item.
                aos_free(parent);
                p_tmp->next = next;
            } else {
                p_tmp = parent;
            }

            if (key) {
                break;
            }
            parent = next;
        } else {
            p_tmp = parent;
            parent = parent->next;
        }
        flag = 1;
    }

    return ret;
}

/**
 * @brief delete the items in the @ht whose key is @key in lockless mode.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to delete.
 *            key_len: the length of @key.
 * @note: the @key and @val will be freed in the hashtable when found.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_del_lockless(void *ht, const void *key, unsigned int len_key)
{
    return _ht_del_node(_ht_find_lockless(ht, key, len_key), key, len_key);
}

/**
 * @brief delete the items in the @ht whose key is @key.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to delete.
 *            key_len: the length of @key.
 * @note: the @key and @val will be freed in the hashtable when found.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_del(void *ht, const void *key, unsigned int len_key)
{
    int ret = 0;
    ht_lock(ht);
    ret = ht_del_lockless(ht, key, len_key);
    ht_unlock(ht);

    return ret;
}

/**
 * @brief polling the hashtable @ht and invoke the inte_func @func
 *
 * @param[in] ht: the hander of hashtable.
 * @param[in] func: the deal function.
 *
 */
void ht_iterator_lockless(void *ht, iter_func func, void *extra)
{
    int i = 0;
    ht_t *pt = ht;
    ht_item_t *item = NULL;

    if (!pt || !func) {
        return;
    }

    for (i = 0; i < pt->cnt; i++) {
        item = pt->item + i;

        while (item) {
            if (item->key && item->val) {
                func(item->key, item->val, extra);
            }

            item = item->next;
        }
    }
}

/**
 * @brief delete all the items in the @ht.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @note: the @key and @val will be freed in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_clear_lockless(void *ht)
{
    ht_t *pt = ht;
    int i = 0;
    if (!ht) {
        return -1;
    }

    for (i = 0; i < pt->cnt; i++) {
        _ht_del_node(pt->item + i, NULL, 0);
    }

    return 0;
}

/**
 * @brief delete all the items in the @ht.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @note: the @key and @val will be freed in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_clear(void *ht)
{
    int ret = 0;
    ht_lock(ht);
    ret = ht_clear_lockless(ht);
    ht_unlock(ht);

    return ret;
}

/**
 * @brief delete all the items in the @ht and release memory.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @retval  0 on success, otherwise -1 will be returned
 */

int ht_destroy(void *ht)
{
    ht_t *pt = (ht_t *)ht;
    if (!pt) {
        return -1;
    }

    ht_lock(pt);

    ht_clear_lockless(pt);
    //LOGD(MODULE,"destroy hashtable: <aos_free>%p. <aos_free>%p \n", pt, pt->item);
    aos_free(pt->item);

    ht_unlock(pt);

    aos_mutex_free(&pt->lock);
    aos_free(pt);

    return 0;
}

