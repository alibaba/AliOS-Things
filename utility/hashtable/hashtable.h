/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/aos.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

typedef struct hash_item {
    void *key;
    void *val;
    int size_val;
    struct hash_item *next;
} ht_item_t;


/**
 * @brief hash table module initiation.
 *
 * @param[in] max_cnt: the count of hashtable items you want.
 *
 * @retval  hashtable hander on success, otherwise NULL will be returned
 */
void *ht_init(int max_cnt);

/**
 * @brief lock the @ht hashtable. you may invoke this interface when you
 * find/add/delete hashtable in lockless mode.
 *
 * @param[in] ht: the hander of hashtable.
 *
 */
void ht_lock(void *ht);

/**
 * @brief unlock the @ht hashtable.
 *
 * @param[in] ht: the hander of hashtable.
 */
void ht_unlock(void *ht);

/**
 * @brief find the item in the @ht whose key is @key in lockless mode, this maybe
 * more efficient.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to find.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 * @param[out]val: the memory to store the found val.
 *            size_val: the size of returned val
 * @retval  the pointer to value on success, otherwise NULL will be returned
 */
void *ht_find_lockless(void *ht, const void  *key, unsigned int key_len,
                       void *val, int *size_val);

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
void *ht_find(void *ht, const void  *key, unsigned int key_len, void *val,
              int *size_val);

/**
 * @brief add the item in the @ht whose key is @key and value is @val in lockless
 * mode, this maybe more efficient.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to add.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 *            val: the value of the item you want to add.
 *            size_val: the length of the @val, if @val is string, @size_val must be strlen(val)+1.
 * @note: the @key and @val will be re-malloced and stored in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_add_lockless(void *ht, const void *key, unsigned int len_key,
                    const void *val, unsigned int size_val);

/**
 * @brief add the item in the @ht whose key is @key and value is @val.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to add.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 *            val: the value of the item you want to add.
 *            size_val: the length of the @val, if @val is string, @size_val must be strlen(val)+1.
 * @note: the @key and @val will be re-malloced and stored in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_add(void *ht, const void *key, unsigned int len_key, const void *val,
           unsigned int size_val);

/**
 * @brief delete the items in the @ht whose key is @key in lockless mode.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to delete.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 * @note: the @key and @val will be freed in the hashtable when found.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_del_lockless(void *ht, const void *key, unsigned int len_key);

/**
 * @brief delete the items in the @ht whose key is @key.
 *
 * @param[in] ht: the hander of hashtable.
 *            key: the key of the item you want to delete.
 *            key_len: the length of @key, if @key is string, @key_len must be strlen(key)+1.
 * @note: the @key and @val will be freed in the hashtable when found.
 * @note: the @key and @val will be freed in the hashtable when found, this interface delete all the items whose key
 * is matched, compared to @ht_del_strict.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_del(void *ht, const void  *key, unsigned int len_key);

/*the function to be invoked while polling the hashtable*/
typedef void *(*iter_func)(void *key, void *val, void *extra);

/**
 * @brief polling the hashtable @ht and invoke the inte_func @func
 *
 * @param[in] ht: the hander of hashtable.
 * @param[in] func: the deal function.
 *
 */
void ht_iterator_lockless(void *ht, iter_func func, void *extra);

/**
 * @brief delete all the items in the @ht in lockless mode.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @note: the @key and @val will be freed in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_clear_lockless(void *ht);

/**
 * @brief delete all the items in the @ht.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @note: the @key and @val will be freed in the hashtable.
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_clear(void *ht);

/**
 * @brief delete all the items in the @ht and release memory.
 *
 * @param[in] ht: the hander of hashtable.
 *
 * @retval  0 on success, otherwise -1 will be returned
 */
int ht_destroy(void *ht);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

