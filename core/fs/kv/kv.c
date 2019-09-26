/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "fs/kv_api.h"

#include "kv_conf.h"
#include "kv_adapt.h"
#include "kv_types.h"

static kv_mgr_t g_kv_mgr;

static void kv_gc_task(void *arg);

kv_item_t *kv_item_traverse(item_func func, uint8_t blk_idx, const char *key);

extern void kv_register_cli_command(void);

/******************************************************/
/****************** Internal Interface ****************/
/******************************************************/

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
    kv_start_task("kv_gc", kv_gc_task, NULL, KV_TASK_STACK_SIZE);
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
            kv_free(item->store);
        }
        kv_free(item);
    }
}

/**
 * @brief Set block state bit
 *
 * @param[in]  blk_idx   the position of the block index
 * @param[in]  state the state which want to set
 *
 * @return 0 on success, otherwise will be failed
 */
static int32_t kv_set_block_state(uint8_t blk_idx, uint8_t state)
{
    int32_t ret;

    uint32_t pos = (blk_idx << KV_BLOCK_SIZE_BITS) + KV_STATE_OFFSET;

    ret = kv_flash_write(pos, &state, 1);
    if (ret != KV_OK) {
        return ret;
    }

    g_kv_mgr.block_info[blk_idx].state = state;

    return KV_OK;
}

/**
 * @brief Set item state bit
 *
 * @param[in]  pos   the position of the item
 * @param[in]  state the state which want to set
 *
 * @return 0 on success, otherwise will be failed
 */
static int32_t kv_set_item_state(kv_size_t pos, uint8_t state)
{
    return kv_flash_write(pos + KV_STATE_OFFSET, &state, 1);
}

/**
 * @brief Format block header
 *
 * @param[in]  blk_idx the index of the block
 *
 * @return 0 on success, otherwise will be failed
 */
static int32_t kv_block_format(uint8_t blk_idx)
{
    block_hdr_t hdr;

    kv_size_t pos = (blk_idx << KV_BLOCK_SIZE_BITS);

    memset(&hdr, 0, sizeof(block_hdr_t));
    hdr.magic = KV_BLOCK_MAGIC_NUM;

    if (!kv_flash_erase(pos, KV_BLOCK_SIZE)) {
        hdr.state = KV_BLOCK_STATE_CLEAN;
    } else {
        return KV_ERR_FLASH_ERASE;
    }

    if (kv_flash_write(pos, &hdr, KV_BLOCK_HDR_SIZE) != KV_OK) {
        return KV_ERR_FLASH_WRITE;
    }

    g_kv_mgr.block_info[blk_idx].state = KV_BLOCK_STATE_CLEAN;
    g_kv_mgr.block_info[blk_idx].space = KV_BLOCK_SIZE - KV_BLOCK_HDR_SIZE;
    g_kv_mgr.clean_blk_nums++;

    return KV_OK;
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
    uint8_t       blk_idx;
    block_info_t *blk_info;

#if KV_BLOCK_NUMS > KV_RESERVED_BLOCKS + 1
    uint8_t  i;
    uint32_t blk_start;
#endif

    blk_idx  = (g_kv_mgr.write_pos) >> KV_BLOCK_SIZE_BITS;
    blk_info = &(g_kv_mgr.block_info[blk_idx]);

    if (blk_info->space > len) {
        if (((blk_info->space - len) < KV_ITEM_MAX_LEN) && \
            (g_kv_mgr.clean_blk_nums <= KV_RESERVED_BLOCKS)) {
            kv_trigger_gc();
        }
        return g_kv_mgr.write_pos;
    }

#if KV_BLOCK_NUMS > KV_RESERVED_BLOCKS + 1
    for (i = blk_idx + 1; i != blk_idx; i++) {
        if (i == KV_BLOCK_NUMS) {
            i = 0;
        }

        blk_info = &(g_kv_mgr.block_info[i]);
        if ((blk_info->space) > len) {
            blk_start = (i << KV_BLOCK_SIZE_BITS);
            g_kv_mgr.write_pos = blk_start + KV_BLOCK_SIZE - blk_info->space;

            if (blk_info->state == KV_BLOCK_STATE_CLEAN) {
                if (kv_set_block_state(i, KV_BLOCK_STATE_USED) != KV_OK) {
                    return 0;
                }
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
static int32_t kv_item_del(kv_item_t *item, int flag)
{
    uint8_t    i;
    kv_size_t  off;
    item_hdr_t hdr;

    int   res = KV_OK;
    char *ori = NULL;
    char *new = NULL;

    if (flag == KV_DELETE_FLAG) {
        off = item->pos;
    } else if (flag == KV_UPDATE_FLAG) {
        off = item->hdr.origin_off;
        memset(&hdr, 0, KV_ITEM_HDR_SIZE);
        if (kv_flash_read(off, &hdr, KV_ITEM_HDR_SIZE) != KV_OK) {
            return KV_ERR_FLASH_READ;
        }

        if ((hdr.magic != KV_ITEM_MAGIC_NUM) || \
            (hdr.state != KV_ITEM_STATE_NORMAL) || \
            (hdr.key_len != item->hdr.key_len)) {
            return KV_OK;
        }

        ori = (char *)kv_malloc(hdr.key_len);
        if (!ori) {
            return KV_ERR_MALLOC_FAILED;
        }

        new = (char *)kv_malloc(hdr.key_len);
        if (!new) {
            kv_free(ori);
            return KV_ERR_MALLOC_FAILED;
        }

        kv_flash_read(off + KV_ITEM_HDR_SIZE, ori, hdr.key_len);
        kv_flash_read(item->pos + KV_ITEM_HDR_SIZE, new, hdr.key_len);
        if (memcmp(ori, new, hdr.key_len) != 0) {
            kv_free(ori);
            kv_free(new);
            return KV_OK;
        }

        kv_free(ori);
        kv_free(new);
    } else {
        return KV_ERR_INVALID_PARAM;
    }

    if ((res = kv_set_item_state(off, KV_ITEM_STATE_DELETE)) != KV_OK) {
        return res;
    }

    i = off >> KV_BLOCK_SIZE_BITS;
    if (g_kv_mgr.block_info[i].state == KV_BLOCK_STATE_USED) {
        res = kv_set_block_state(i, KV_BLOCK_STATE_DIRTY);
    }

    return res;
}

/**
 * @brief recovery callback function when polling the block
 *
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 *
 * @return KV_LOOP_CONTINUE: continue to polling, otherwise is failed
 */
static int32_t __item_recovery_cb(kv_item_t *item, const char *key)
{
    int32_t res;

    char *p = (char *)kv_malloc(item->len);
    if (!p) {
        return KV_ERR_MALLOC_FAILED;
    }

    res = kv_flash_read(item->pos + KV_ITEM_HDR_SIZE, p, item->len);
    if (res != KV_OK) {
        kv_free(p);
        return KV_ERR_FLASH_READ;
    }

    if (item->hdr.crc == calc_crc8((uint8_t *)p, item->len)) {
        if ((item->hdr.origin_off != 0) && \
            (item->pos != item->hdr.origin_off)) {
            kv_item_del(item, KV_UPDATE_FLAG);
        }
    } else {
        kv_item_del(item, KV_DELETE_FLAG);
    }

    kv_free(p);
    return KV_LOOP_CONTINUE;
}

/**
 * @brief find callback function when polling the block
 *
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 *
 * @return KV_OK: find the item;
 *         KV_LOOP_CONTINUE: continue to polling,
 *         otherwise is failed
 */
static int32_t __item_find_cb(kv_item_t *item, const char *key)
{
    int32_t res;

    if (item->hdr.key_len != strlen(key)) {
        return KV_LOOP_CONTINUE;
    }

    item->store = (char *)kv_malloc(item->hdr.key_len + item->hdr.val_len);
    if (!item->store) {
        return KV_ERR_MALLOC_FAILED;
    }

    res = kv_flash_read(item->pos + KV_ITEM_HDR_SIZE, item->store,
                               item->len);
    if (res != KV_OK) {
        return KV_ERR_FLASH_READ;
    }

    if (memcmp(item->store, key, strlen(key)) == 0) {
        return KV_OK;
    }

    return KV_LOOP_CONTINUE;
}

/**
 * @brief GC callback function when polling the block
 *
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 *
 * @return KV_LOOP_CONTINUE: continue to polling, otherwise is failed
 */
static int32_t __item_gc_cb(kv_item_t *item, const char *key)
{
    char     *p;
    int32_t   res;
    uint8_t   idx;
    uint16_t  len;

    len = KV_ALIGN(KV_ITEM_HDR_SIZE + item->len);

    p = (char *)kv_malloc(len);
    if (!p) {
        return KV_ERR_MALLOC_FAILED;
    }

    if (kv_flash_read(item->pos, p, len) != KV_OK) {
        res = KV_ERR_FLASH_READ;
        goto err;
    }

    if (kv_flash_write(g_kv_mgr.write_pos, p, len) != KV_OK) {
        res = KV_ERR_FLASH_WRITE;
        goto err;
    }

    g_kv_mgr.write_pos += len;
    idx = (g_kv_mgr.write_pos) >> KV_BLOCK_SIZE_BITS;
    g_kv_mgr.block_info[idx].space -= len;
    res = KV_LOOP_CONTINUE;

err:
    kv_free(p);

    return res;
}

/**
 * @brief Group delete callback function when polling the block
 *
 * @param[in]  item pointer to the key-value pair item
 * @param[in]  key  pointer to the key
 *
 * @return KV_LOOP_CONTINUE: continue to polling, otherwise is failed
 */
static int32_t __item_del_by_prefix_cb(kv_item_t *item, const char *prefix)
{
    char *key = NULL;

    if (item->hdr.key_len < strlen(prefix)) {
        return KV_LOOP_CONTINUE;
    }

    key = (char *)kv_malloc(item->hdr.key_len + 1);
    if (!key) {
        return KV_ERR_MALLOC_FAILED;
    }

    memset(key, 0, item->hdr.key_len + 1);
    kv_flash_read(item->pos + KV_ITEM_HDR_SIZE, key, item->hdr.key_len);

    if (strncmp(key, prefix, strlen(prefix)) == 0) {
        kv_item_del(item, KV_DELETE_FLAG);
    }

    kv_free(key);

    return KV_LOOP_CONTINUE;
}

/**
 * @brief Search key-value pair item
 *
 * @param[in]  key pointer to the item key
 *
 * @return key-value item or NULL
 */
static kv_item_t *kv_item_search(const char *key)
{
    uint8_t i;

    kv_item_t *item = NULL;

    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        if (g_kv_mgr.block_info[i].state != KV_BLOCK_STATE_CLEAN) {
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
static int32_t kv_item_store(const char *key, const void *val, int len,
                             kv_size_t origin_off)
{
    char       *p;
    uint8_t     idx;
    kv_size_t   pos;
    item_hdr_t  hdr;
    kv_store_t  store;

    hdr.magic      = KV_ITEM_MAGIC_NUM;
    hdr.state      = KV_ITEM_STATE_NORMAL;
    hdr.key_len    = strlen(key);
    hdr.val_len    = len;
    hdr.origin_off = origin_off;

    store.len = KV_ALIGN(KV_ITEM_HDR_SIZE + hdr.key_len + hdr.val_len);

    store.p = (char *)kv_malloc(store.len);
    if (!store.p) {
        return KV_ERR_MALLOC_FAILED;
    }

    memset(store.p, 0, store.len);
    p = store.p + KV_ITEM_HDR_SIZE;

    memcpy(p, key, hdr.key_len);
    p += hdr.key_len;

    memcpy(p, val, hdr.val_len);
    p -= hdr.key_len;

    hdr.crc = calc_crc8((uint8_t *)p, hdr.key_len + hdr.val_len);
    memcpy(store.p, &hdr, KV_ITEM_HDR_SIZE);

    pos = kv_calc_position(store.len);
    if (pos > 0) {
        store.res = kv_flash_write(pos, store.p, store.len);
        if (store.res == KV_OK) {
            g_kv_mgr.write_pos = pos + store.len;
            idx = (uint8_t)(g_kv_mgr.write_pos >> KV_BLOCK_SIZE_BITS);
            g_kv_mgr.block_info[idx].space -= store.len;
        }
    } else {
        store.res = KV_ERR_NO_SPACE;
    }

    if (store.p) {
        kv_free(store.p);
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
static int32_t kv_item_update(kv_item_t *item, const char *key, const void *val,
                              int32_t len)
{
    int res;

    if (item->hdr.val_len == len) {
        if (!memcmp(item->store + item->hdr.key_len, val, len)) {
            return KV_OK;
        }
    }

    res = kv_item_store(key, val, len, item->pos);
    if (res != KV_OK) {
        return res;
    }

    res = kv_item_del(item, KV_DELETE_FLAG);

    return res;
}

/**
 * @brief Initialize the KV module
 *
 * @return  0 on success, otherwise is failed
 */
static int32_t kv_init_internal(void)
{
    uint8_t     i;
    uint8_t     next;
    block_hdr_t hdr;

    int32_t res  = KV_OK;
    int32_t nums = 0;

    uint8_t unclean[KV_BLOCK_NUMS] = {0};

    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        memset(&hdr, 0, KV_BLOCK_HDR_SIZE);
        kv_flash_read((i << KV_BLOCK_SIZE_BITS), &hdr, KV_BLOCK_HDR_SIZE);
        if (hdr.magic == KV_BLOCK_MAGIC_NUM) {
            if (INVALID_BLK_STATE(hdr.state)) {
                if ((res = kv_block_format(i)) != KV_OK) {
                    return res;
                } else {
                    continue;
                }
            }

            g_kv_mgr.block_info[i].state = hdr.state;
            kv_item_traverse(__item_recovery_cb, i, NULL);
            if (hdr.state == KV_BLOCK_STATE_CLEAN) {
                if (g_kv_mgr.block_info[i].space != (KV_BLOCK_SIZE - KV_BLOCK_HDR_SIZE)) {
                    unclean[nums] = i;
                    nums++;
                } else {
                    g_kv_mgr.clean_blk_nums++;
                }
            }
        } else {
            if ((res = kv_block_format(i)) != KV_OK) {
                return res;
            }
        }
    }

    while (nums > 0) {
        i = unclean[nums - 1];
        if (g_kv_mgr.clean_blk_nums >= KV_RESERVED_BLOCKS) {
            res = kv_set_block_state(i, KV_BLOCK_STATE_DIRTY);
            if (res != KV_OK) {
                return res;
            }
        } else {
            if ((res = kv_block_format(i)) != KV_OK) {
                return res;
            }
        }
        nums--;
    }

    if (g_kv_mgr.clean_blk_nums == 0) {
        if ((res = kv_block_format(0)) != KV_OK) {
            return res;
        }
    }

    if (g_kv_mgr.clean_blk_nums == KV_BLOCK_NUMS) {
        g_kv_mgr.write_pos = KV_BLOCK_HDR_SIZE;
        i = (uint8_t)(g_kv_mgr.write_pos & KV_BLOCK_OFF_MASK);

        res = kv_set_block_state(i, KV_BLOCK_STATE_USED);
        if (res != KV_OK) {
            return res;
        }
        g_kv_mgr.clean_blk_nums--;
    } else {
        for (i = 0; i < KV_BLOCK_NUMS; i++) {
            if ((g_kv_mgr.block_info[i].state == KV_BLOCK_STATE_USED) || \
                (g_kv_mgr.block_info[i].state == KV_BLOCK_STATE_DIRTY)) {
                next = ((i + 1) == KV_BLOCK_NUMS) ? 0 : (i + 1);
                if (g_kv_mgr.block_info[next].state == KV_BLOCK_STATE_CLEAN) {
                    g_kv_mgr.write_pos = (i << KV_BLOCK_SIZE_BITS) + \
                                         KV_BLOCK_SIZE - \
                                         g_kv_mgr.block_info[i].space;
                    break;
                }
            }
        }
    }

    return KV_OK;
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
    kv_size_t origin_pos;
    uint8_t   i;
    uint8_t   gc_idx;

    uint8_t gc_copy = 0;

    if ((kv_lock(g_kv_mgr.lock)) != KV_OK) {
        goto exit;
    }

    origin_pos = g_kv_mgr.write_pos;
    if (g_kv_mgr.clean_blk_nums == 0) {
        goto exit;
    }

    for (gc_idx = 0; gc_idx < KV_BLOCK_NUMS; gc_idx++) {
        if (g_kv_mgr.block_info[gc_idx].state == KV_BLOCK_STATE_CLEAN) {
            g_kv_mgr.write_pos = (gc_idx << KV_BLOCK_SIZE_BITS) + KV_BLOCK_HDR_SIZE;
            break;
        }
    }

    if (gc_idx == KV_BLOCK_NUMS) {
        goto exit;
    }

    i = (origin_pos >> KV_BLOCK_SIZE_BITS) + 1;
    while (1) {
        if (i == KV_BLOCK_NUMS) {
            i = 0;
        }

        if (g_kv_mgr.block_info[i].state == KV_BLOCK_STATE_DIRTY) {
            kv_item_traverse(__item_gc_cb, i, NULL);

            gc_copy = 1;

            if (kv_block_format(i) != KV_OK) {
                goto exit;
            }

            kv_set_block_state(gc_idx, KV_BLOCK_STATE_USED);
            g_kv_mgr.clean_blk_nums--;
            break;
        }

        if (i == (origin_pos >> KV_BLOCK_SIZE_BITS)) {
            break;
        }
        i++;
    }

    if (gc_copy == 0) {
        g_kv_mgr.write_pos = origin_pos;
    }

exit:
    g_kv_mgr.gc_trigger = 0;
    kv_unlock(g_kv_mgr.lock);
    if (g_kv_mgr.gc_waiter > 0) {
        kv_sem_post_all(g_kv_mgr.gc_sem);
    }

    kv_delete_task();
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
kv_item_t *kv_item_traverse(item_func func, uint8_t blk_idx, const char *key)
{
    int32_t     res;
    uint16_t    len;
    kv_size_t   pos;
    kv_size_t   end;
    kv_item_t  *item;
    item_hdr_t *hdr;

    pos = (blk_idx << KV_BLOCK_SIZE_BITS) + KV_BLOCK_HDR_SIZE;
    end = (blk_idx << KV_BLOCK_SIZE_BITS) + KV_BLOCK_SIZE;
    len = 0;

    do {
        item = (kv_item_t *)kv_malloc(sizeof(kv_item_t));
        if (!item) {
            return NULL;
        }

        memset(item, 0, sizeof(kv_item_t));
        hdr = &(item->hdr);

        if (kv_flash_read(pos, hdr, KV_ITEM_HDR_SIZE) != KV_OK) {
            kv_item_free(item);
            return NULL;
        }

        if (hdr->magic != KV_ITEM_MAGIC_NUM) {
            if ((hdr->magic == 0xFF) && (hdr->state == 0xFF)) {
                kv_item_free(item);
                break;
            }
            hdr->val_len = 0xFFFF;
        }

        if ((hdr->val_len > KV_MAX_VAL_LEN) || \
            (hdr->key_len > KV_MAX_KEY_LEN) || \
            (hdr->val_len == 0) || (hdr->key_len == 0)) {

            pos += KV_ITEM_HDR_SIZE;
            kv_item_free(item);

            if (g_kv_mgr.block_info[blk_idx].state == KV_BLOCK_STATE_USED) {
                kv_set_block_state(blk_idx, KV_BLOCK_STATE_DIRTY);
            }
            continue;
        }

        len = KV_ALIGN(KV_ITEM_HDR_SIZE + hdr->key_len + hdr->val_len);

        if (hdr->state == KV_ITEM_STATE_NORMAL) {
            item->pos = pos;
            item->len = hdr->key_len + hdr->val_len;

            res = func(item, key);
            if (res == KV_OK) {
                return item;
            } else if (res != KV_LOOP_CONTINUE) {
                kv_item_free(item);
                return NULL;
            }
        } else {
            if (g_kv_mgr.block_info[blk_idx].state == KV_BLOCK_STATE_USED) {
                kv_set_block_state(blk_idx, KV_BLOCK_STATE_DIRTY);
            }
        }

        kv_item_free(item);
        pos += len;
    } while (end > (pos + KV_ITEM_HDR_SIZE));

    if (end > pos) {
        g_kv_mgr.block_info[blk_idx].space = end - pos;
    } else {
        g_kv_mgr.block_info[blk_idx].space = KV_ITEM_HDR_SIZE;
    }

    return NULL;
}

/******************************************************/
/****************** Public Interface ******************/
/******************************************************/

int32_t kv_init(void)
{
    int32_t res;
    uint8_t blk_idx;

    if (g_kv_mgr.inited) {
        return KV_OK;
    }

    if (KV_BLOCK_NUMS <= KV_RESERVED_BLOCKS) {
        return KV_ERR_INVALID_PARAM;
    }

    memset(&g_kv_mgr, 0, sizeof(kv_mgr_t));

    g_kv_mgr.lock = kv_lock_create();
    if (g_kv_mgr.lock == NULL) {
        return KV_ERR_OS_LOCK;
    }

    g_kv_mgr.gc_sem = kv_sem_create();
    if (g_kv_mgr.gc_sem == NULL) {
        return KV_ERR_OS_SEM;
    }

    if ((res = kv_init_internal()) != KV_OK) {
        return res;
    }

    g_kv_mgr.inited = 1;

    blk_idx = (g_kv_mgr.write_pos >> KV_BLOCK_SIZE_BITS);
    if (((g_kv_mgr.block_info[blk_idx].space) < KV_ITEM_MAX_LEN) && \
         (g_kv_mgr.clean_blk_nums < KV_RESERVED_BLOCKS + 1)) {
        kv_trigger_gc();
    }

    kv_register_cli_command();

    return KV_OK;
}

void kv_deinit(void)
{
    g_kv_mgr.inited = 0;

    kv_sem_free(g_kv_mgr.gc_sem);

    kv_lock_free(g_kv_mgr.lock);
}

int32_t kv_item_set(const char *key, const void *val, int32_t len)
{
    int32_t res;

    kv_item_t *item = NULL;

#if (KV_SECURE_SUPPORT) && (KV_SECURE_LEVEL > 1)
    return kv_item_secure_set(key, val, len);
#endif

    if (!key || !val || (len <= 0) || (strlen(key) > KV_MAX_KEY_LEN) || \
        (len > KV_MAX_VAL_LEN)) {
        return KV_ERR_INVALID_PARAM;
    }

    if (g_kv_mgr.gc_trigger != 0) {
        g_kv_mgr.gc_waiter++;
        kv_sem_wait(g_kv_mgr.gc_sem);
    }

    if ((res = kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    item = kv_item_search(key);
    if (item != NULL) {
        res = kv_item_update(item, key, val, len);
        kv_item_free(item);
    } else {
        res = kv_item_store(key, val, len, 0);
    }

    kv_unlock(g_kv_mgr.lock);

    return res;
}

int32_t kv_item_get(const char *key, void *buffer, int32_t *buffer_len)
{
    kv_item_t *item = NULL;

#if (KV_SECURE_SUPPORT) && (KV_SECURE_LEVEL > 1)
    return kv_item_secure_get(key, buffer, buffer_len);
#endif

    if (!key || !buffer || !buffer_len || (*buffer_len <= 0)) {
        return KV_ERR_INVALID_PARAM;
    }

    if ((kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    item = kv_item_search(key);

    kv_unlock(g_kv_mgr.lock);

    if (!item) {
        return KV_ERR_NOT_FOUND;
    }

    if (*buffer_len < item->hdr.val_len) {
        *buffer_len = item->hdr.val_len;
        kv_item_free(item);
        return KV_ERR_NO_SPACE;
    } else {
        memcpy(buffer, (item->store + item->hdr.key_len), item->hdr.val_len);
        *buffer_len = item->hdr.val_len;
    }

    kv_item_free(item);

    return KV_OK;
}

int32_t kv_item_delete(const char *key)
{
    int32_t res;

    kv_item_t *item = NULL;

    if ((res = kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    item = kv_item_search(key);
    if (!item) {
        kv_unlock(g_kv_mgr.lock);
        return KV_ERR_NOT_FOUND;
    }

    res = kv_item_del(item, KV_DELETE_FLAG);
    kv_item_free(item);
    kv_unlock(g_kv_mgr.lock);

    return res;
}

int32_t kv_item_delete_by_prefix(const char *prefix)
{
    int32_t i;

    if ((kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        kv_item_traverse(__item_del_by_prefix_cb, i, prefix);
    }

    kv_unlock(g_kv_mgr.lock);

    return KV_OK;
}

#if (KV_SECURE_SUPPORT)

int32_t kv_item_secure_set(const char *key, const void *val, int32_t len)
{
    int32_t res;

    uint8_t   *data = NULL;
    kv_item_t *item = NULL;

    if (!key || !val || (len <= 0) || (strlen(key) > KV_MAX_KEY_LEN) || \
        (len > KV_MAX_VAL_LEN)) {
        return KV_ERR_INVALID_PARAM;
    }

    if (g_kv_mgr.gc_trigger != 0) {
        g_kv_mgr.gc_waiter++;
        kv_sem_wait(g_kv_mgr.gc_sem);
    }

    if ((res = kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    data = (uint8_t *)kv_malloc(len);
    if (data == NULL) {
        return KV_ERR_NO_SPACE;
    }

    memset(data, 0, len);

    res = kv_secure_encrypt((uint8_t *)val, data, len);
    if (res != KV_OK) {
        kv_free(data);
        data = NULL;

        return KV_ERR_ENCRYPT;
    }

    item = kv_item_search(key);
    if (item != NULL) {
        res = kv_item_update(item, key, data, len);
        kv_item_free(item);

    } else {
        res = kv_item_store(key, data, len, 0);
    }

    kv_unlock(g_kv_mgr.lock);

    kv_free(data);
    data = NULL;

    return res;
}

int32_t kv_item_secure_get(const char *key, void *buffer, int32_t *buffer_len)
{
    int32_t res;

    uint8_t   *data = NULL;
    kv_item_t *item = NULL;

    if (!key || !buffer || !buffer_len || (*buffer_len <= 0)) {
        return KV_ERR_INVALID_PARAM;
    }

    if ((res = kv_lock(g_kv_mgr.lock)) != KV_OK) {
        return KV_ERR_OS_LOCK;
    }

    item = kv_item_search(key);

    kv_unlock(g_kv_mgr.lock);

    if (!item) {
        return KV_ERR_NOT_FOUND;
    }

    if (*buffer_len < item->hdr.val_len) {
        *buffer_len = item->hdr.val_len;
        kv_item_free(item);
        return KV_ERR_NO_SPACE;
    } else {
        data = (uint8_t *)kv_malloc(item->hdr.val_len);
        if (data == NULL) {
            return KV_ERR_NO_SPACE;
        }

        memset(data, 0, item->hdr.val_len);

        res = kv_secure_decrypt((uint8_t *)(item->store + item->hdr.key_len), data, item->hdr.val_len);
        if (res != KV_OK) {
            kv_free(data);
            data = NULL;

            return KV_ERR_DECRYPT;
        }

        memcpy(buffer, data, item->hdr.val_len);
        *buffer_len = item->hdr.val_len;
    }

    kv_item_free(item);

    kv_free(data);
    data = NULL;

    return KV_OK;
}

#else
int32_t kv_item_secure_set(const char *key, const void *val, int32_t len)
{
    return KV_ERR_NOT_SUPPORT;
}

int32_t kv_item_secure_get(const char *key, void *buffer, int32_t *buffer_len)
{
    return KV_ERR_NOT_SUPPORT;
}
#endif

