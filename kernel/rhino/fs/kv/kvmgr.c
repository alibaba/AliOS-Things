/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "kvmgr.h"
#include "kv_defs.h"

static kv_mgr_t g_kv_mgr;
static void kv_gc_task(void *arg);

/**
 * @brief CRC-8: the poly is 0x31 (x^8 + x^5 + x^4 + 1)
 *
 * @param[in] buf       the buffer which is need calculate crc
 * @param[in] length    the length of the buffer
 *
 * @return the crc value
 */
static uint8_t calc_crc8(uint8_t *buf, uint16_t length)
{
    uint8_t crc = 0x00;
    uint8_t i = 0;

    while (length--) {
        crc ^= *buf++;
        for (i = 8; i > 0; i--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Trigger garbage collection process
 *
 * @return none
 */
static void kv_trigger_gc(void)
{
    if (g_kv_mgr.gc_trigger != 0) {
        return;
    }

    g_kv_mgr.gc_waiter  = 0;
    g_kv_mgr.gc_trigger = 1;
    kv_os_task_start("kv_gc", kv_gc_task, NULL, KV_GC_TASK_SIZE);
}

/**
 * @brief Free item resource
 * 
 * @param[in] item pointer to item
 *
 * @return none
 */
static void kv_item_free(kv_item_t *item)
{
    if (item) {
        if (item->store) {
            kv_os_free(item->store);
        }
        kv_os_free(item);
    }
}

/**
 * @brief Set block/item state bit
 * 
 * @param[in]  pos   the position of the state bit
 * @param[in]  state the state which want to set
 * 
 * @return 0 on success, otherwise will be failed
 */
static int kv_state_set(kv_size_t pos, uint8_t state)
{
    return kv_os_partition_write(pos + KV_STATE_OFF, &state, 1);
}

/**
 * @brief Format block header
 * 
 * @param[in]  blk_idx the index of the block
 * 
 * @return 0 on success, otherwise will be failed
 */
static int kv_block_format(uint8_t blk_idx)
{
    block_hdr_t hdr;
    kv_size_t pos = (blk_idx << BLK_BITS);

    memset(&hdr, 0, sizeof(block_hdr_t));
    hdr.magic = BLK_MAGIC_NUM;

    if (!kv_os_partition_erase(pos, BLK_SIZE)) {
        hdr.state = BLK_STATE_CLEAN;
    } else {
        return RES_FLASH_EARSE_ERR;
    }

    if (kv_os_partition_write(pos, &hdr, BLK_HDR_SIZE) != RES_OK) {
        return RES_FLASH_WRITE_ERR;
    }

    g_kv_mgr.block_info[blk_idx].state = BLK_STATE_CLEAN;
    g_kv_mgr.block_info[blk_idx].space = BLK_SIZE - BLK_HDR_SIZE;
    g_kv_mgr.clean_blk_nums++;

    return RES_OK;
}

/**
 * @brief Calculcate the item wtite position
 * 
 * @param[in]  len  the item length
 * 
 * @return write position, 0 means there is no space to store
 */
static kv_size_t kv_calc_position(uint16_t len)
{
    uint8_t blk_idx;
    block_info_t *blk_info;

#if BLK_NUMS > KV_GC_RESERVED + 1
    uint8_t i;
#endif

    blk_idx  = (g_kv_mgr.write_pos) >> BLK_BITS;
    blk_info = &(g_kv_mgr.block_info[blk_idx]);

    if (blk_info->space > len) {
        if (((blk_info->space - len) < ITEM_MAX_LEN) && \
            (g_kv_mgr.clean_blk_nums <= KV_GC_RESERVED)) {
            kv_trigger_gc();
        }
        return g_kv_mgr.write_pos;
    }

#if BLK_NUMS > KV_GC_RESERVED + 1
    for (i = blk_idx + 1; i != blk_idx; i++) {
        if (i == BLK_NUMS) {
            i = 0;
        }

        blk_info = &(g_kv_mgr.block_info[i]);
        if ((blk_info->space) > len) {
            g_kv_mgr.write_pos = (i << BLK_BITS) + BLK_SIZE - blk_info->space;
            if (blk_info->state == BLK_STATE_CLEAN) {
                if (kv_state_set((i << BLK_BITS), BLK_STATE_USED) != RES_OK) {
                    return 0;
                }
                blk_info->state = BLK_STATE_USED;
                g_kv_mgr.clean_blk_nums--;
            }
            return g_kv_mgr.write_pos;
        }
    }
#endif

    kv_trigger_gc();
    return 0;
}

/**
 * @brief Key-value pair item delete
 * 
 * @param[in]  item pointer to the key-value pair need to deleted
 * @param[in]  flag the flag indicate item delete request is from del or update
 * 
 * @return 0 on success, otherwise is failed
 */
static int kv_item_del(kv_item_t *item, int flag)
{
    uint8_t i;
    kv_size_t off;
    item_hdr_t hdr;
    int res = RES_OK;
    char *ori_key = NULL;
    char *new_key = NULL;

    if (flag == KV_DELETE_REMOVE) {
        off = item->pos;
    } else if (flag == KV_UPDATE_REMOVE) {
        off = item->hdr.origin_off;
        memset(&hdr, 0, ITEM_HDR_SIZE);
        if (kv_os_partition_read(off, &hdr, ITEM_HDR_SIZE) != RES_OK) {
            return RES_FLASH_READ_ERR;
        }

        if ((hdr.magic != ITEM_MAGIC_NUM) || \
            (hdr.state != ITEM_STATE_NORMAL) || \
            (hdr.key_len != item->hdr.key_len)) {
            return RES_OK;
        }

        ori_key = (char *)kv_os_malloc(hdr.key_len);
        if (!ori_key) {
            return RES_MALLOC_FAILED;
        }

        new_key = (char *)kv_os_malloc(hdr.key_len);
        if (!new_key) {
            kv_os_free(ori_key);
            return RES_MALLOC_FAILED;
        }

        kv_os_partition_read(off + ITEM_HDR_SIZE, ori_key, hdr.key_len);
        kv_os_partition_read(item->pos + ITEM_HDR_SIZE, new_key, hdr.key_len);
        if (memcmp(ori_key, new_key, hdr.key_len) != 0) {
            kv_os_free(ori_key);
            kv_os_free(new_key);
            return RES_OK;
        }

        kv_os_free(ori_key);
        kv_os_free(new_key);
    } else {
        return RES_INVALID_PARAM;
    }

    if ((res = kv_state_set(off, ITEM_STATE_DELETE)) != RES_OK) {
        return res;
    }

    i = off >> BLK_BITS;
    if (g_kv_mgr.block_info[i].state == BLK_STATE_USED) {
        res = kv_state_set((off & BLK_OFF_MASK), BLK_STATE_DIRTY);
        if (res != RES_OK) {
            return res;
        }

        g_kv_mgr.block_info[i].state = BLK_STATE_DIRTY;
    }

    return res;
}

/**
 * @brief recovery callback function when polling the block
 * 
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 * 
 * @return RES_CONT: continue to polling, otherwise is failed
 */
static int __item_recovery_cb(kv_item_t *item, const char *key)
{
    int res;
    char *p = (char *)kv_os_malloc(item->len);
    if (!p) {
        return RES_MALLOC_FAILED;
    }

    res = kv_os_partition_read(item->pos + ITEM_HDR_SIZE, p, item->len);
    if (res != RES_OK) {
        kv_os_free(p);
        return RES_FLASH_READ_ERR;
    }

    if (item->hdr.crc == calc_crc8((uint8_t *)p, item->len)) {
        if ((item->hdr.origin_off != 0) && \
            (item->pos != item->hdr.origin_off)) {
            kv_item_del(item, KV_UPDATE_REMOVE);
        }
    } else {
        kv_item_del(item, KV_DELETE_REMOVE);
    }

    kv_os_free(p);
    return RES_CONT;
}

/**
 * @brief find callback function when polling the block
 * 
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 * 
 * @return RES_OK: find the item; 
 *         RES_CONT: continue to polling, 
 *         otherwise is failed
 */
static int __item_find_cb(kv_item_t *item, const char *key)
{
    int res;

    if (item->hdr.key_len != strlen(key)) {
        return RES_CONT;
    }

    item->store = (char *)kv_os_malloc(item->hdr.key_len + item->hdr.val_len);
    if (!item->store) {
        return RES_MALLOC_FAILED;
    }

    res = kv_os_partition_read(item->pos + ITEM_HDR_SIZE, item->store, 
                               item->len);
    if (res != RES_OK) {
        return RES_FLASH_READ_ERR;
    }

    if (memcmp(item->store, key, strlen(key)) == 0) {
        return RES_OK;
    }

    return RES_CONT;
}

/**
 * @brief GC callback function when polling the block
 * 
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 * 
 * @return RES_CONT: continue to polling, otherwise is failed
 */
static int __item_gc_cb(kv_item_t *item, const char *key)
{
    char *p;
    int res;
    uint8_t idx;
    uint16_t len;

    len = KV_ALIGN(ITEM_HDR_SIZE + item->len);

    p = (char *)kv_os_malloc(len);
    if (!p) {
        return RES_MALLOC_FAILED;
    }

    if (kv_os_partition_read(item->pos, p, len) != RES_OK) {
        res = RES_FLASH_READ_ERR;
        goto err;
    }

    if (kv_os_partition_write(g_kv_mgr.write_pos, p, len) != RES_OK) {
        res = RES_FLASH_WRITE_ERR;
        goto err;
    }

    g_kv_mgr.write_pos += len;
    idx = (g_kv_mgr.write_pos) >> BLK_BITS;
    g_kv_mgr.block_info[idx].space -= len;
    res = RES_CONT;

err:
    kv_os_free(p);
    return res;
}

/**
 * @brief Group delete callback function when polling the block
 * 
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 * 
 * @return RES_CONT: continue to polling, otherwise is failed
 */
static int __item_del_by_prefix_cb(kv_item_t *item, const char *prefix)
{
    char *key = NULL;

    if (item->hdr.key_len < strlen(prefix)) {
        return RES_CONT;
    }

    key = (char *)kv_os_malloc(item->hdr.key_len + 1);
    if (!key) {
        return RES_MALLOC_FAILED;
    }

    memset(key, 0, item->hdr.key_len + 1);
    kv_os_partition_read(item->pos + ITEM_HDR_SIZE, key, item->hdr.key_len);

    if (strncmp(key, prefix, strlen(prefix)) == 0) {
        kv_item_del(item, KV_DELETE_REMOVE);
    }

    kv_os_free(key);
    return RES_CONT;
}

/**
 * @brief polling flash block
 * 
 * @param[in]  func    pointer to callback function
 * @param[in]  blk_idx the block index
 * @param[in]  key     pointer to the key
 * 
 * @return the key-value item or NULL.
 */
static kv_item_t *kv_item_traverse(item_func func, uint8_t blk_idx, 
                                   const char *key)
{
    int res;
    uint16_t len;
    kv_size_t pos;
    kv_size_t end;
    kv_item_t *item;
    item_hdr_t *hdr;

    pos = (blk_idx << BLK_BITS) + BLK_HDR_SIZE;
    end = (blk_idx << BLK_BITS) + BLK_SIZE;
    len = 0;

    do {
        item = (kv_item_t *)kv_os_malloc(sizeof(kv_item_t));
        if (!item) {
            return NULL;
        }

        memset(item, 0, sizeof(kv_item_t));
        hdr = &(item->hdr);

        if (kv_os_partition_read(pos, hdr, ITEM_HDR_SIZE) != RES_OK) {
            kv_item_free(item);
            return NULL;
        }

        if (hdr->magic != ITEM_MAGIC_NUM) {
            if ((hdr->magic == 0xFF) && (hdr->state == 0xFF)) {
                kv_item_free(item);
                break;
            }
            hdr->val_len = 0xFFFF;
        }

        if ((hdr->val_len > ITEM_MAX_VAL_LEN) || \
            (hdr->key_len > ITEM_MAX_KEY_LEN) || \
            (hdr->val_len == 0) || (hdr->key_len == 0)) {

            pos += ITEM_HDR_SIZE;
            kv_item_free(item);

            if (g_kv_mgr.block_info[blk_idx].state == BLK_STATE_USED) {
                kv_state_set((blk_idx << BLK_BITS), BLK_STATE_DIRTY);
                g_kv_mgr.block_info[blk_idx].state = BLK_STATE_DIRTY;
            }
            continue;
        }

        len = KV_ALIGN(ITEM_HDR_SIZE + hdr->key_len + hdr->val_len);

        if (hdr->state == ITEM_STATE_NORMAL) {
            item->pos = pos;
            item->len = hdr->key_len + hdr->val_len;

            res = func(item, key);
            if (res == RES_OK) {
                return item;
            } else if (res != RES_CONT) {
                kv_item_free(item);
                return NULL;
            }
        } else {
            if (g_kv_mgr.block_info[blk_idx].state == BLK_STATE_USED) {
                kv_state_set((blk_idx << BLK_BITS), BLK_STATE_DIRTY);
                g_kv_mgr.block_info[blk_idx].state = BLK_STATE_DIRTY;
            }
        }

        kv_item_free(item);
        pos += len;
    } while (end > (pos + ITEM_HDR_SIZE));

    if (end > pos) {
        g_kv_mgr.block_info[blk_idx].space = end - pos;
    } else {
        g_kv_mgr.block_info[blk_idx].space = ITEM_HDR_SIZE;
    }

    return NULL;
}

/**
 * @brief Get key-value pair item
 * 
 * @param[in]  key pointer to the item key
 * 
 * @return key-value item or NULL
 */
static kv_item_t *kv_item_get(const char *key)
{
    uint8_t i;
    kv_item_t *item = NULL;

    for (i = 0; i < BLK_NUMS; i++) {
        if (g_kv_mgr.block_info[i].state != BLK_STATE_CLEAN) {
            item = kv_item_traverse(__item_find_cb, i, key);
            if (item != NULL) {
                return item;
            }
        }
    }

    return NULL;
}

/**
 * @brief Store key-value item
 * 
 * @param[in]  key        pointer to the item key
 * @param[in]  val        pointer to the item value
 * @param[in]  len        the length of the value 
 * @param[in]  origin_off the old item position offset
 * 
 * @return 0 on success, otherwise is failed
 */
static int kv_item_store(const char *key, const void *val, int len, 
                         kv_size_t origin_off)
{
    char *p;
    uint8_t idx;
    kv_size_t pos;
    item_hdr_t hdr;
    kv_store_t store;

    hdr.magic       = ITEM_MAGIC_NUM;
    hdr.state       = ITEM_STATE_NORMAL;
    hdr.key_len     = strlen(key);
    hdr.val_len     = len;
    hdr.origin_off  = origin_off;

    store.len = KV_ALIGN(ITEM_HDR_SIZE + hdr.key_len + hdr.val_len);

    store.p = (char *)kv_os_malloc(store.len);
    if (!store.p) {
        return RES_MALLOC_FAILED;
    }

    memset(store.p, 0, store.len);
    p = store.p + ITEM_HDR_SIZE;
    memcpy(p, key, hdr.key_len);
    p += hdr.key_len;
    memcpy(p, val, hdr.val_len);
    p -= hdr.key_len;
    hdr.crc = calc_crc8((uint8_t *)p, hdr.key_len + hdr.val_len);
    memcpy(store.p, &hdr, ITEM_HDR_SIZE);

    pos = kv_calc_position(store.len);
    if (pos > 0) {
        store.res = kv_os_partition_write(pos, store.p, store.len);
        if (store.res == RES_OK) {
            g_kv_mgr.write_pos = pos + store.len;
            idx = (uint8_t)(g_kv_mgr.write_pos >> BLK_BITS);
            g_kv_mgr.block_info[idx].space -= store.len;
        }
    } else {
        store.res = RES_NO_SPACE;
    }

    if (store.p) {
        kv_os_free(store.p);
    }
    return store.res;
}

/**
 * @brief Update the item value
 * 
 * @param[in]  item pointer to the key-value item
 * @param[in]  key  pointer to the item key
 * @param[in]  val  pointer to the item value
 * @param[in]  len  the length of the item value
 * 
 * @return 0 on success, otherwise is failed
 */
static int kv_item_update(kv_item_t *item, const char *key, const void *val,
                          int len)
{
    int res;

    if (item->hdr.val_len == len) {
        if (!memcmp(item->store + item->hdr.key_len, val, len)) {
            return RES_OK;
        }
    }

    res = kv_item_store(key, val, len, item->pos);
    if (res != RES_OK) {
        return res;
    }

    res = kv_item_del(item, KV_DELETE_REMOVE);
    return res;
}

/**
 * @brief Initialize the KV module
 * 
 * @return  0 on success, otherwise is failed
 */
static int kv_init(void)
{
    uint8_t i, next;
    block_hdr_t hdr;
    int res, nums = 0;
    uint8_t unclean[BLK_NUMS] = {0};

    for (i = 0; i < BLK_NUMS; i++) {
        memset(&hdr, 0, BLK_HDR_SIZE);
        kv_os_partition_read((i << BLK_BITS), &hdr, BLK_HDR_SIZE);
        if (hdr.magic == BLK_MAGIC_NUM) {
            if (INVALID_BLK_STATE(hdr.state)) {
                if ((res = kv_block_format(i)) != RES_OK) {
                    return res;
                } else {
                    continue;
                }
            }

            g_kv_mgr.block_info[i].state = hdr.state;
            kv_item_traverse(__item_recovery_cb, i, NULL);
            if (hdr.state == BLK_STATE_CLEAN) {
                if (g_kv_mgr.block_info[i].space != (BLK_SIZE - BLK_HDR_SIZE)) {
                    unclean[nums] = i;
                    nums++;
                } else {
                    g_kv_mgr.clean_blk_nums++;
                }
            }
        } else {
            if ((res = kv_block_format(i)) != RES_OK) {
                return res;
            }
        }
    }

    while (nums > 0) {
        i = unclean[nums - 1];
        if (g_kv_mgr.clean_blk_nums >= KV_GC_RESERVED) {
            res = kv_state_set((i << BLK_BITS), BLK_STATE_DIRTY);
            if (res != RES_OK) {
                return res;
            }
            g_kv_mgr.block_info[i].state = BLK_STATE_DIRTY;
        } else {
            if ((res = kv_block_format(i)) != RES_OK) {
                return res;
            }
        }
        nums--;
    }

    if (g_kv_mgr.clean_blk_nums == 0) {
        if ((res = kv_block_format(0)) != RES_OK) {
            return res;
        }
    }

    if (g_kv_mgr.clean_blk_nums == BLK_NUMS) {
        g_kv_mgr.write_pos = BLK_HDR_SIZE;
        if (!kv_state_set((g_kv_mgr.write_pos & BLK_OFF_MASK), BLK_STATE_USED)) {
            g_kv_mgr.block_info[0].state = BLK_STATE_USED;
            g_kv_mgr.clean_blk_nums--;
        }
    } else {
        for (i = 0; i < BLK_NUMS; i++) {
            if ((g_kv_mgr.block_info[i].state == BLK_STATE_USED) || \
                (g_kv_mgr.block_info[i].state == BLK_STATE_DIRTY)) {
                next = ((i + 1) == BLK_NUMS) ? 0 : (i + 1);
                if (g_kv_mgr.block_info[next].state == BLK_STATE_CLEAN) {
                    g_kv_mgr.write_pos = (i << BLK_BITS) + BLK_SIZE - \
                                        g_kv_mgr.block_info[i].space;
                    break;
                }
            }
        }
    }

    return RES_OK;
}

/**
 * Garbage collection task
 * 
 * @param[in] arg pointer to the argument
 *
 * @return none
 */
static void kv_gc_task(void *arg)
{
    int res;
    kv_size_t origin_pos;
    uint8_t i, gc_idx, gc_copy = 0;

    if ((res = kv_os_mutex_lock(&(g_kv_mgr.mutex), KV_WAIT_FOREVER)) != RES_OK) {
        goto exit;
    }

    origin_pos = g_kv_mgr.write_pos;
    if (g_kv_mgr.clean_blk_nums == 0) {
        goto exit;
    }

    for (gc_idx = 0; gc_idx < BLK_NUMS; gc_idx++) {
        if (g_kv_mgr.block_info[gc_idx].state == BLK_STATE_CLEAN) {
            g_kv_mgr.write_pos = (gc_idx << BLK_BITS) + BLK_HDR_SIZE;
            break;
        }
    }

    if (gc_idx == BLK_NUMS) {
        goto exit;
    }

    i = (origin_pos >> BLK_BITS) + 1;
    while (1) {
        if (i == BLK_NUMS) {
            i = 0;
        }

        if (g_kv_mgr.block_info[i].state == BLK_STATE_DIRTY) {
            kv_item_traverse(__item_gc_cb, i, NULL);

            gc_copy = 1;

            if (kv_block_format(i) != RES_OK) {
                goto exit;
            }

            kv_state_set((g_kv_mgr.write_pos & BLK_OFF_MASK), BLK_STATE_USED);
            g_kv_mgr.block_info[gc_idx].state = BLK_STATE_USED;
            g_kv_mgr.clean_blk_nums--;
            break;
        }

        if (i == (origin_pos >> BLK_BITS)) {
            break;
        }
        i++;
    }

    if (gc_copy == 0) {
        g_kv_mgr.write_pos = origin_pos;
    }

exit:
    g_kv_mgr.gc_trigger = 0;
    kv_os_mutex_unlock(&(g_kv_mgr.mutex));
    if (g_kv_mgr.gc_waiter > 0) {
        kv_os_sem_give_all(&(g_kv_mgr.gc_sem));
    }

    kv_os_task_del();
}

int aos_kv_del(const char *key)
{
    int res;
    kv_item_t *item;

    if ((res = kv_os_mutex_lock(&(g_kv_mgr.mutex), KV_WAIT_FOREVER)) != RES_OK) {
        return RES_MUTEX_ERR;
    }

    item = kv_item_get(key);
    if (!item) {
        kv_os_mutex_unlock(&(g_kv_mgr.mutex));
        return RES_ITEM_NOT_FOUND;
    }

    res = kv_item_del(item, KV_DELETE_REMOVE);
    kv_item_free(item);
    kv_os_mutex_unlock(&(g_kv_mgr.mutex));
    return res;
}

int aos_kv_del_by_prefix(const char *prefix)
{
    int i, res;

    if ((res = kv_os_mutex_lock(&(g_kv_mgr.mutex), KV_WAIT_FOREVER)) != RES_OK) {
        return RES_MUTEX_ERR;
    }

    for (i = 0; i < BLK_NUMS; i++) {
        kv_item_traverse(__item_del_by_prefix_cb, i, prefix);
    }

    kv_os_mutex_unlock(&(g_kv_mgr.mutex));
    return RES_OK;
}

int aos_kv_set(const char *key, const void *val, int len, int sync)
{
    int res;
    kv_item_t *item;

    if (!key || !val || (len <= 0) || (strlen(key) > ITEM_MAX_KEY_LEN) || \
        (len > ITEM_MAX_VAL_LEN)) {
        return RES_INVALID_PARAM;
    }

    if (g_kv_mgr.gc_trigger != 0) {
        g_kv_mgr.gc_waiter++;
        kv_os_sem_take(&(g_kv_mgr.gc_sem), KV_WAIT_FOREVER);
    }

    if ((res = kv_os_mutex_lock(&(g_kv_mgr.mutex), KV_WAIT_FOREVER)) != RES_OK) {
        return RES_MUTEX_ERR;
    }

    item = kv_item_get(key);
    if (item != NULL) {
        res = kv_item_update(item, key, val, len);
        kv_item_free(item);
    } else {
        res = kv_item_store(key, val, len, 0);
    }

    kv_os_mutex_unlock(&(g_kv_mgr.mutex));
    return res;
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int res;
    kv_item_t *item = NULL;

    if (!key || !buffer || !buffer_len || (*buffer_len <= 0)) {
        return RES_INVALID_PARAM;
    }

    if ((res = kv_os_mutex_lock(&(g_kv_mgr.mutex), KV_WAIT_FOREVER)) != RES_OK) {
        return RES_MUTEX_ERR;
    }

    item = kv_item_get(key);

    kv_os_mutex_unlock(&(g_kv_mgr.mutex));

    if (!item) {
        return RES_ITEM_NOT_FOUND;
    }

    if (*buffer_len < item->hdr.val_len) {
        *buffer_len = item->hdr.val_len;
        kv_item_free(item);
        return RES_NO_SPACE;
    } else {
        memcpy(buffer, (item->store + item->hdr.key_len), item->hdr.val_len);
        *buffer_len = item->hdr.val_len;
    }

    kv_item_free(item);
    return RES_OK;
}

/* CLI Support */
#ifdef CONFIG_AOS_CLI
#include "aos/cli.h"

static int __item_print_cb(kv_item_t *item, const char *key)
{
    kv_size_t off;
    char *p_key = NULL;
    char *p_val = NULL;

    p_key = (char *)kv_os_malloc(item->hdr.key_len + 1);
    if (!p_key) {
        return RES_MALLOC_FAILED;
    }

    memset(p_key, 0, item->hdr.key_len + 1);
    off = item->pos + ITEM_HDR_SIZE;
    kv_os_partition_read(off, p_key, item->hdr.key_len);

    p_val = (char *)kv_os_malloc(item->hdr.val_len + 1);
    if (!p_val) {
        kv_os_free(p_key);
        return RES_MALLOC_FAILED;
    }

    memset(p_val, 0, item->hdr.val_len + 1);
    off = item->pos + ITEM_HDR_SIZE + item->hdr.key_len;
    kv_os_partition_read(off, p_val, item->hdr.val_len);

    aos_cli_printf("%s = %s\r\n", p_key, p_val);
    kv_os_free(p_key);
    kv_os_free(p_val);

    return RES_CONT;
}

static void handle_kv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    int i, res = RES_OK;
    char *buffer = NULL;
    int len = ITEM_MAX_VAL_LEN;

    if (strcmp(rtype, "set") == RES_OK) {
        if (argc != 4) {
            return;
        }

        res = aos_kv_set(argv[2], argv[3], strlen(argv[3]), 1);
        if (res != RES_OK) {
            aos_cli_printf("cli set kv failed %d.\r\n", res);
        }
    } else if (strcmp(rtype, "get") == RES_OK) {
        if (argc != 3) {
            return;
        }

        buffer = (char *)kv_os_malloc(ITEM_MAX_VAL_LEN);
        if (!buffer) {
            aos_cli_printf("there is no space\r\n");
            return;
        }

        memset(buffer, 0, ITEM_MAX_VAL_LEN);
        res = aos_kv_get(argv[2], buffer, &len);
        if (res != 0) {
            aos_cli_printf("cli: no paired kv\r\n");
        } else {
            aos_cli_printf("value is %s\r\n", buffer);
        }

        if (buffer) {
            kv_os_free(buffer);
        }
    } else if (strcmp(rtype, "del") == RES_OK) {
        if (argc != 3) {
            return;
        }

        res = aos_kv_del(argv[2]);
        if (res != RES_OK) {
            aos_cli_printf("cli kv del failed %d\r\n", res);
        }
    } else if (strcmp(rtype, "list") == RES_OK) {
        for (i = 0; i < BLK_NUMS; i++) {
            kv_item_traverse(__item_print_cb, i, NULL);
        }
    }
    return;
}

static struct cli_command ncmd = {
    "kv", "kv [set key value | get key | del key | list]", handle_kv_cmd
};
#endif

int aos_kv_init(void)
{
    int res;
    uint8_t blk_idx;

    if (g_kv_mgr.inited) {
        return RES_OK;
    }

    if (BLK_NUMS <= KV_GC_RESERVED) {
        return RES_INVALID_PARAM;
    }

    memset(&g_kv_mgr, 0, sizeof(kv_mgr_t));

    if ((res = kv_os_mutex_create(&(g_kv_mgr.mutex), "KV")) != RES_OK) {
        return RES_MUTEX_ERR;
    }

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&ncmd);
#endif

    if ((res = kv_init()) != RES_OK) {
        return res;
    }

    if ((res = kv_os_sem_create(&(g_kv_mgr.gc_sem), "KV", 0)) != RES_OK) {
        return RES_SEM_ERR;
    }

    g_kv_mgr.inited = 1;

    blk_idx = (g_kv_mgr.write_pos >> BLK_BITS);
    if (((g_kv_mgr.block_info[blk_idx].space) < ITEM_MAX_LEN) && \
         (g_kv_mgr.clean_blk_nums < KV_GC_RESERVED + 1)) {
        kv_trigger_gc();
    }

    return RES_OK;
}

void aos_kv_deinit(void)
{
    g_kv_mgr.inited = 0;
    kv_os_sem_del(&(g_kv_mgr.gc_sem));
    kv_os_mutex_del(&(g_kv_mgr.mutex));
}
