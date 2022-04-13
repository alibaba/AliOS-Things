#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <aos/hal/flash.h>
#include <aos/hal/nand_flash.h>
#include "kv_api.h"
#include "kv_types.h"
#include "kv_util.h"

static block_info_t g_kv_mngr;
static kv_lock_t g_kv_lock;
static bool kv_inited = false;

/* kv item alignment */
#define ITEMALIGNMENT 4

/* all data on flash adopts little-endian */
#define BUF2UINT32(p) kv_fromle32(*(uint32_t *)(p))
#define BUF2UINT16(p) kv_fromle16(*(uint16_t *)(p))
#define UINT322BUF(p, n) *(uint32_t *)(p) = kv_tole32(n)
#define UINT162BUF(p, n) *(uint16_t *)(p) = kv_tole16(n)

item_info_t *kv_item_traverse(item_func func, const char *key);

static int __item_print_cb(item_info_t *item, const char *key)
{
    printf("%s = %s\r\n", item->name, (char *)(item->value));
    return KV_LOOP_CONTINUE;
}

void kv_dump_items()
{
    item_info_t *item;
    printf("\n===Dump all kv items===\n");
    item = kv_item_traverse(__item_print_cb, NULL);
    if (item) {
        aos_free(item);
    }
}

static bool block_is_bad(uint32_t blk)
{
    bool ret;
    ret = hal_flash_isbad((hal_partition_t)KV_PARTITION, blk * KV_BLOCK_SIZE);
    KV_TRACE("%s: block %d checked, ret: %d", __func__, blk, ret);
    return ret;
}

static bool block_mark_bad(uint32_t blk)
{
    bool ret;
    ret = hal_flash_markbad((hal_partition_t)KV_PARTITION, blk * KV_BLOCK_SIZE);
    KV_TRACE("%s marking block %d, ret: %d", __func__, blk, ret);
    return ret;
}

int32_t kv_flash_read(uint32_t blk, uint32_t page, uint32_t offset, void *outbuf, uint32_t buflen)
{
    uint32_t off = blk * KV_BLOCK_SIZE + page * KV_PAGE_SIZE + offset, ret;
    ret = hal_flash_read((hal_partition_t)KV_PARTITION, &off, outbuf, buflen);
    return ret;
}

#ifdef CONFIG_KV_WRITE_IMMEDIATE_VERIFY
static uint8_t _page_rbuf[KV_PAGE_SIZE];
static int kv_flash_verify_page(uint32_t blk, uint32_t page)
{
    int ret;
    uint16_t idx = 0;
    uint32_t crc_read, crc_calc;

    /* block general verify */
    KV_ASSERT(!block_is_bad(blk));

    /* verify page */
    /* read page content out. */
    memset(_page_rbuf, 0, sizeof(_page_rbuf));
    ret = kv_flash_read(blk, page, 0, _page_rbuf, sizeof(_page_rbuf));
    if (ret) {
        KV_ERROR("%s %d failed to read page %d, ret: %d", \
                 __func__, __LINE__, page, ret);
        return -1;
    }

    /* verify block/page header magic */
    if (page == 0) {
        KV_ASSERT(BUF2UINT32(_page_rbuf) == KV_BLOCK_HDR_MAGIC);
        idx += 8;
    } else {
        KV_ASSERT(BUF2UINT32(_page_rbuf) == KV_PAGE_HDR_MAGIC);
        idx += 4;
    }

    /* verify crc */
    crc_calc = kv_crc32(0, _page_rbuf, sizeof(_page_rbuf) - 4);
    crc_read = BUF2UINT32(_page_rbuf + sizeof(_page_rbuf) - 4);
    KV_ASSERT(crc_read == crc_calc);

    /* verify items on page */
    uint16_t nlen, vlen;
    while (idx < ((sizeof(_page_rbuf) - 4 - KV_PAGE_BITMAP_BYTES))) {
        /* verify item length info */
        nlen = BUF2UINT16(_page_rbuf + idx);
        vlen = BUF2UINT16(_page_rbuf + idx + 2);
        KV_ASSERT(nlen <= KV_MAX_KEY_LEN);
        KV_ASSERT(vlen <= KV_MAX_VAL_LEN);

        /* items end */
        if (nlen == 0 && vlen == 0) {
            break;
        }

        /* continue with next item */
        idx = (uint16_t)kv_alignup((uint32_t)idx + \
                                   4 + nlen + vlen, ITEMALIGNMENT);
    }

    return 0;
}
#endif

/* NAND write in page unint */
static int32_t kv_flash_page_write(uint32_t blk, uint32_t page, void *buf, uint32_t len)
{
    int32_t ret;
    uint32_t off;

    if (len > KV_PAGE_SIZE) {
        KV_ERROR("Write len too big (no more than one page) to block %d.", blk);
        return -1;
    }

    kv_lock(&g_kv_lock);

    off = blk * KV_BLOCK_SIZE + page * KV_PAGE_SIZE;
    ret = hal_flash_write((hal_partition_t)KV_PARTITION, &off, buf, len);
    if (off != blk * KV_BLOCK_SIZE + page * KV_PAGE_SIZE + len) {
        KV_WARN("KV flash write not equal to expected: %d vs. %d",
                blk * KV_BLOCK_SIZE + page * KV_PAGE_SIZE + len, off);
    }

#ifdef CONFIG_KV_WRITE_IMMEDIATE_VERIFY
    if (ret == 0) {
        KV_ASSERT(kv_flash_verify_page(blk, page) == 0);
    }
#endif

    kv_unlock(&g_kv_lock);
    return ret;
}

/* NAND erase, in block unit */
int32_t kv_flash_block_erase(uint32_t blk)
{
    int32_t ret;
    ret = hal_flash_erase((hal_partition_t)KV_PARTITION, blk * KV_BLOCK_SIZE, KV_BLOCK_SIZE);
    return ret;
}

void kv_dump_block_headers()
{
    int i, ret;
    uint8_t page_data[KV_PAGE_SIZE];

    printf("%s Block header page contents:", __func__);
    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        /* read block header */
        memset(page_data, 0, KV_PAGE_SIZE);
        ret = kv_flash_read(i, 0, 0, page_data, KV_PAGE_SIZE);
        if (ret != 0) {
            KV_ERROR("%s %d failed to read block %d with ret: %d, "\
                     "skip it", __func__, __LINE__, i, ret);
            continue;
        }

        printf("===>Block %d:\r\n", i);
        for (int _cnt = 0; _cnt < KV_PAGE_SIZE; _cnt++) {
            if ((_cnt != 0) && (_cnt % 32 == 0)) {
                printf("\r\n");
            }
            printf("%02x ", page_data[_cnt]);
        }

        printf("\r\n");
    }
}

static bool find_last_active_block(bool in_good, uint16_t *active, uint32_t *rev)
{
    int i, ret;
    bool activeblk_found = false;
    block_hdr_t bhdr = {0};
    uint16_t blkactive = 0;
    uint32_t revmax = 0;
    uint8_t *page_data;

    page_data = (uint8_t *)aos_malloc(KV_PAGE_SIZE);
    if (!page_data) {
        KV_ERROR("%s failed to malloc", __func__);
        return -1;
    }

#ifdef KV_YES_TRACE
    kv_dump_block_headers();
#endif

    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        /* skip bad block */
        if (block_is_bad(i) == in_good) {
            continue;
        }

        /* read block header */
        memset(page_data, 0, KV_PAGE_SIZE);
        ret = kv_flash_read(i, 0, 0, page_data, KV_PAGE_SIZE);
        if (ret != 0) {
            KV_ERROR("%s %d failed to read block %d with ret: %d, "\
                     "skip it", __func__, __LINE__, i, ret);
            continue;
        }

        bhdr.magic = BUF2UINT32(page_data);
        bhdr.rev = BUF2UINT32(page_data + 4);
        /* skip not active block */
        if (bhdr.magic != (uint32_t)KV_BLOCK_HDR_MAGIC) {
            KV_TRACE("%s Block magic check failed for block %d", __func__, i);
            continue;
        }

        /**
         * examine crc on first page, just in case
         * unexpected power-off last time.
         */
        uint32_t crc = kv_crc32(0, page_data, KV_PAGE_SIZE - 4);
        uint32_t crc_read = BUF2UINT32(page_data + KV_PAGE_SIZE - 4);
        if (crc != crc_read) {
            KV_WARN("Looks like an unexpected power-off on "\
                    "block %d, try next.", i);
            continue;
        }

        /* first active block found */
        if (!activeblk_found) {
            KV_TRACE("%s First active block %d found, rev %d", __func__, i, bhdr.rev);
            activeblk_found = true;
            blkactive = i;
            revmax = bhdr.rev;
            continue;
        }

        /* more blocks found, pick up the most active one */
        if (kv_scmp(bhdr.rev, revmax) > 0) {
            blkactive = i;
            revmax = bhdr.rev;
            KV_TRACE("%s higher version of active block %d found, rev %d", __func__, i, bhdr.rev);
        }
    }

    if (activeblk_found) {
        *active = blkactive;
        *rev = revmax;
    }

    if (page_data) {
        aos_free(page_data);
    }
    KV_TRACE("%s Active block %d found, rev %d", __func__, *active, *rev);

    return activeblk_found;
}

static bool page_is_blank(uint32_t blk, uint32_t page)
{
    int ret, i = 0;
    uint8_t data[32] = {0};

    ret = kv_flash_read(blk, page, 0, data, sizeof(data));
    if (ret != 0) {
        KV_ERROR("%s failed to read flash, ret: %d", __func__, ret);
        return false;
    }

    while (i < 32) {
        if (data[i++] != 0xff) {
            return false;
        }
    }

    return true;
}

static inline bool is_valid_page(uint8_t *bitmap, uint32_t pageidx)
{
    return *(bitmap + (pageidx >> 3)) & (1 << (pageidx % 8));
}

static inline void set_bitmap(uint8_t *bitmap, uint32_t pageidx)
{
    *(bitmap + (pageidx >> 3)) |= (1 << (pageidx % 8));
}

static inline void clear_bitmap(uint8_t *bitmap, uint32_t pageidx)
{
    *(bitmap + (pageidx >> 3)) &= ~(1 << (pageidx % 8));
}

static int format_kv()
{
    int first_good = -1, i;
    int32_t ret;
    uint32_t crc;

    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        if (block_is_bad(i)) {
            continue;
        }

        ret = kv_flash_block_erase(i);
        if (ret) {
            KV_WARN("%s block %d is bad, mark it now.", __func__, i);
            hal_flash_markbad((hal_partition_t)KV_PARTITION, i * KV_BLOCK_SIZE);
        } else {
            if (first_good < 0) {
                first_good = i;
            }
        }
    }

    if (first_good < 0) {
        KV_ERROR("%s seems no good block in kv partition now.", __func__);
        return -1;
    }

    /* update page magic and revision */
    memset(&g_kv_mngr, 0, sizeof(g_kv_mngr));
    UINT322BUF(g_kv_mngr.pwbuf, KV_BLOCK_HDR_MAGIC);
    UINT322BUF(g_kv_mngr.pwbuf + 4, 0);

    /* bitmap update */
    set_bitmap(g_kv_mngr.bitmap, 0);
    memcpy(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4 - \
           KV_PAGE_BITMAP_BYTES, g_kv_mngr.bitmap, \
           KV_PAGE_BITMAP_BYTES);

    /* crc */
    crc = kv_crc32(0, g_kv_mngr.pwbuf, KV_PAGE_SIZE - 4);
    UINT322BUF(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4, crc);

    /* flush onto flash */
    for (i = 0; i < KV_BLOCK_NUMS; i++) {
        if (block_is_bad(i)) {
            continue;
        }

        ret = kv_flash_page_write(i, 0, \
                                  g_kv_mngr.pwbuf, KV_PAGE_SIZE);
        if (ret) {
            /* if write fail, assume a bad block. */
            KV_ERROR("%s %d failed to write page %d, ret: %d", \
                     __func__, __LINE__, i, ret);
            block_mark_bad(i);
            continue;
        }

        break;
    }

    /* if failed to write a block, reset kv manager and return failure code */
    if (i >= KV_BLOCK_NUMS) {
        memset(&g_kv_mngr, 0, sizeof(g_kv_mngr));
        KV_ERROR("%s failed.", __func__);
        return -1;
    }

    /* update kv manager */
    g_kv_mngr.used++;
    g_kv_mngr.blkidx = i;
    g_kv_mngr.rev = 0;

    /* reset write buffer state in kv manager */
    g_kv_mngr.pwoff = 0;
    memset(g_kv_mngr.pwbuf, 0, sizeof(g_kv_mngr.pwbuf));

    return 0;
}

typedef enum {
    KV_REBUILD_NORMAL,
    KV_REBUILD_DELETING,
    KV_REBUILD_SETTING,
    KV_REBUILD_MAX
} kv_rebuild_op_t;

static int rebuild_kv(kv_rebuild_op_t type, const char *key, \
                      const void *val, int32_t vallen)
{
    int i, ret = 0;
    block_info_t *new_mngr;
    uint8_t *buf_1st, *p;
    uint32_t off_1st, crc;
    item_hdr_t ihdr;
    uint32_t new_off;
    bool is_the_set_item, set_item_saved = false;
    uint16_t vlen;

    KV_TRACE("%s entry, blkidx - %d, used - %d, rev - %d, type: %d", __func__,
             g_kv_mngr.blkidx, g_kv_mngr.used, g_kv_mngr.rev, type);

    if (type >= KV_REBUILD_MAX) {
        return -1;
    } else {
        if (!key) {
            return -1;
        }
        if ((type == KV_REBUILD_SETTING) && \
            (!val || (vallen <= 0))) {
            return -1;
        }
    }

    new_mngr = (block_info_t *)aos_malloc(sizeof(block_info_t));
    if (!new_mngr) {
        KV_ERROR("%s %d failed malloc", __func__, __LINE__);
        return -1;
    }

    /**
     * buffer used for first page store, need flush at the end,
     * so we allocate a saperate buffer.
     */
    buf_1st = (uint8_t *)aos_malloc(KV_PAGE_SIZE);
    if (!buf_1st) {
        aos_free(new_mngr);
        KV_ERROR("%s %d failed malloc", __func__, __LINE__);
        return -1;
    }

    /* lock kv manager from now on */
    //kv_lock(&g_kv_lock);

    int j;
    bool become_bad;
    uint32_t old_blkidx = g_kv_mngr.blkidx;
    /* go find a active block, search starting from current active block */
    for (i = (old_blkidx + 1); i != old_blkidx; i++) {
        /* roll back */
        if (i >= KV_BLOCK_NUMS) {
            i = 0;
            /* deal with the case: old_blkidx == 0 */
            if (i == old_blkidx) {
                KV_ERROR("%s failed to find free block, too many "\
                         "bad blocks inside kv partition!", __func__);
                ret = -1;
                goto end;
            }
        }

        if (block_is_bad(i)) {
            continue;
        }

        /* now we find a valid block */
        KV_TRACE("%s a good block %d found, will erase it", __func__, i);
        if (kv_flash_block_erase(i)) {
            /* if erase failed, go find next block. */
            KV_WARN("fail to erase block %i, continue with next one", i);
            block_mark_bad(i);
            continue;
        }

        memset(new_mngr, 0, sizeof(block_info_t));
        /* always assume 1st page used and valid during rebuild. */
        new_mngr->used = 1;
        new_mngr->blkidx = i;
        new_mngr->rev = g_kv_mngr.rev + 1;
        set_bitmap(new_mngr->bitmap, 0);

        become_bad = false;
        new_mngr->pwoff = 4; /* new write */
        memset(new_mngr->pwbuf, 0, sizeof(new_mngr->pwbuf));

        /* initialize first page, which have block header included */
        off_1st = 0;
        memset(buf_1st, 0, KV_PAGE_SIZE);
        UINT322BUF(buf_1st, KV_BLOCK_HDR_MAGIC);
        UINT322BUF(buf_1st + 4, new_mngr->rev);
        off_1st += 8;

        /* move valid pages to new block, need ##COMPACTING##. */
        for (j = 0; j < KV_PAGE_PER_BLOCK_NUMS; j++) {
            if (!is_valid_page(g_kv_mngr.bitmap, j)) {
                continue;
            }

            /* got a valid page, read content out. */
            memset(new_mngr->prbuf, 0, KV_PAGE_SIZE);
            ret = kv_flash_read(g_kv_mngr.blkidx, j, 0, \
                                new_mngr->prbuf, KV_PAGE_SIZE);
            if (ret) {
                KV_ERROR("%s %d failed to read page %d, ret: %d", \
                         __func__, __LINE__, j, ret);
                goto end;
            }

            /* we trust bitmap hereafter, so no crc checking */
            if (j == 0) {
                KV_ASSERT(BUF2UINT32(new_mngr->prbuf) == KV_BLOCK_HDR_MAGIC);
                p = new_mngr->prbuf + 8;
            } else {
                KV_ASSERT(BUF2UINT32(new_mngr->prbuf) == KV_PAGE_HDR_MAGIC);
                p = new_mngr->prbuf + 4;
            }

            while (p < (new_mngr->prbuf + KV_PAGE_SIZE - \
                        4 - KV_PAGE_BITMAP_BYTES)) {
                ihdr.nlen = BUF2UINT16(p);
                ihdr.vlen = BUF2UINT16(p + 2);
                vlen = ihdr.vlen;

                if (ihdr.nlen > KV_MAX_KEY_LEN || vlen > KV_MAX_VAL_LEN) {
                    KV_ERROR("%s %d something was wrong, invalid length: %d %d "\
                             "on page %d", __func__, __LINE__, ihdr.nlen, vlen, j);
                    KV_ASSERT(1);
                }

                KV_ASSERT(ihdr.nlen <= KV_MAX_KEY_LEN || vlen <= KV_MAX_VAL_LEN);

                /* if len is 0, this is the end of items on the page. */
                if (ihdr.nlen == 0 || ihdr.vlen == 0) {
                    break;
                }

                /* if rebuild during delete process, just ignore the key. */
                /* take care of "xxx" vs. "xxxyy" cases. */
                if ((type == KV_REBUILD_DELETING) && \
                    (strlen(key) + 1 == ihdr.nlen) && \
                    (strncmp(key, (const char *)(p + 4), ihdr.nlen - 1) == 0)) {
                    p = (uint8_t *)kv_alignup((uint32_t)p + 4 + \
                                              ihdr.nlen + ihdr.vlen, ITEMALIGNMENT);
                    KV_TRACE("%s match to-delete item %s found, will skip it", __func__, p + 4);
                    continue;
                }

                /* if rebuild during kv set process, save the new value */
                is_the_set_item = false;
                if ((type == KV_REBUILD_SETTING) && \
                    (strlen(key) + 1 == ihdr.nlen) && \
                    (strncmp(key, (const char *)(p + 4), ihdr.nlen - 1) == 0)) {
                    is_the_set_item = true;
                    vlen = vallen;
                    KV_TRACE("%s match to-set item %s found, old vlen: %d, new vlen: %d",
                             __func__, p + 4, ihdr.vlen, vallen);
                }

                /* first try if this item fitting first page */
                new_off = kv_alignup((uint32_t)off_1st + 4 + ihdr.nlen + \
                                     vlen, ITEMALIGNMENT);
                if (new_off <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES)) {
                    if (is_the_set_item) {
                        /* copy the len and name fields */
                        memcpy(buf_1st + off_1st, p, 4 + ihdr.nlen);
                        /* update the value length */
                        UINT162BUF(buf_1st + off_1st + 2, vlen);
                        /* copy the value field */
                        memcpy(buf_1st + off_1st + 4 + ihdr.nlen, val, vlen);
                        set_item_saved = true;
                    } else {
                        memcpy(buf_1st + off_1st, p, 4 + ihdr.nlen + vlen);
                    }

                    KV_TRACE("%s valid item %s copied to first page", __func__, p + 4);

                    off_1st = new_off;
                    p = (uint8_t *)kv_alignup((uint32_t)p + 4 + \
                                              ihdr.nlen + ihdr.vlen, ITEMALIGNMENT);
                    continue;
                }

                /* first page not fit, copy to normal buffer */
                new_off = kv_alignup((uint32_t)(new_mngr->pwoff) + 4 + ihdr.nlen + \
                                     vlen, ITEMALIGNMENT);
                /* if normal page full, flush it. */
                if (new_off <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES)) {
                    /* still free space on the page, copy the item. */
                    if (is_the_set_item) {
                        /* copy the len and name fields */
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff, p, 4 + ihdr.nlen);
                        /* update the value length */
                        UINT162BUF(new_mngr->pwbuf + new_mngr->pwoff + 2, vlen);
                        /* copy the value field */
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff + 4 + ihdr.nlen, val, vlen);
                        set_item_saved = true;
                    } else {
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff, p, \
                               4 + ihdr.nlen + vlen);
                    }

                    KV_TRACE("%s valid item %s copied to page %d", __func__, p + 4, i);

                    new_mngr->pwoff = new_off;
                    p = (uint8_t *)kv_alignup((uint32_t)p + 4 + \
                                              ihdr.nlen + ihdr.vlen, ITEMALIGNMENT);
                    continue;
                } else {
                    /* page almost full, update bitmap and crc, and flush it first. */
                    /* update page magic */
                    UINT322BUF(new_mngr->pwbuf, KV_PAGE_HDR_MAGIC);

                    /* bitmap update */
                    set_bitmap(new_mngr->bitmap, new_mngr->used);
                    memcpy(new_mngr->pwbuf + KV_PAGE_SIZE - 4 - \
                           KV_PAGE_BITMAP_BYTES, new_mngr->bitmap, \
                           KV_PAGE_BITMAP_BYTES);

                    /* crc */
                    uint32_t crc = kv_crc32(0, new_mngr->pwbuf, KV_PAGE_SIZE - 4);
                    UINT322BUF(new_mngr->pwbuf + KV_PAGE_SIZE - 4, crc);

                    /* flush onto flash */
                    KV_TRACE("%s page is full, flusing to flash: blk - %d, used - %d",
                             __func__, i, new_mngr->used);
                    ret = kv_flash_page_write(i, new_mngr->used, \
                                              new_mngr->pwbuf, KV_PAGE_SIZE);
                    if (ret) {
                        /* if write fail, assume a bad block. */
                        KV_ERROR("%s %d failed to write page %d, ret: %d", \
                                 __func__, __LINE__, j, ret);
                        block_mark_bad(i);
                        become_bad = true;
                        break;
                    }

                    new_mngr->used++;
                    if (new_mngr->used >= KV_PAGE_PER_BLOCK_NUMS) {
                        KV_ERROR("%s KV space is full! Pages are exhausted "\
                                 "but there are still items not copied.", __func__);
                        ret = -1;
                        goto end;
                    }

                    /* prepare for a new page write */
                    new_mngr->pwoff = 4;
                    memset(new_mngr->pwbuf, 0, sizeof(new_mngr->pwbuf));

                    /* save the item. */
                    new_off = kv_alignup((uint32_t)(new_mngr->pwoff) + 4 + ihdr.nlen + \
                                         vlen, ITEMALIGNMENT);
                    if (is_the_set_item) {
                        /* copy the len and name fields */
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff, p, 4 + ihdr.nlen);
                        /* update the value length */
                        UINT162BUF(new_mngr->pwbuf + new_mngr->pwoff + 2, vlen);
                        /* copy the value field */
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff + 4 + ihdr.nlen, val, vlen);
                        set_item_saved = true;
                    } else {
                        memcpy(new_mngr->pwbuf + new_mngr->pwoff, p, \
                               4 + ihdr.nlen + vlen);
                    }

                    KV_TRACE("%s item %s copied to page %d", __func__, p + 4, i);

                    /* update the to-page's index */
                    new_mngr->pwoff = new_off;

                    /* update from-page's item pointer */
                    p = (uint8_t *)kv_alignup((uint32_t)p + 4 + \
                                              ihdr.nlen + ihdr.vlen, ITEMALIGNMENT);
                    continue;
                }
            }

            /* from-page content finished, continue with next page */
            if (ihdr.nlen == 0 || ihdr.vlen == 0) {
                continue;
            }

            /* bad block encountered, go find next good block and rebuild again */
            if (become_bad) {
                KV_WARN("%s bad block %d encountered", __func__, i);
                break;
            }
        }

        /* if bad block found when writing, go find next good block */
        if (become_bad) {
            continue;
        }

        /* if the to-set item not copied yet, then copy it. */
        uint16_t toset_nlen = strlen(key) + 1, toset_vlen = vallen;
        if (type == KV_REBUILD_SETTING && !set_item_saved) {
            /* try first page first */
            new_off = kv_alignup((uint32_t)off_1st + 4 + toset_nlen + \
                                 toset_vlen, ITEMALIGNMENT);
            if (new_off <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES)) {
                /* first page is still available */
                /* copy the len and name fields */
                UINT162BUF(buf_1st + off_1st, strlen(key) + 1);
                off_1st += 2;
                UINT162BUF(buf_1st + off_1st, vallen);
                off_1st += 2;
                /* copy the name */
                strncpy((char *)(buf_1st + off_1st), key, strlen(key));
                off_1st += strlen(key) + 1;
                /* copy the value field */
                memcpy(buf_1st + off_1st, val, vallen);
                off_1st += vallen;
                KV_TRACE("%s to-set item %s copied to the first page", __func__, key);
            } else {
                /* first page not fit, copy to normal buffer */
                new_off = kv_alignup((uint32_t)(new_mngr->pwoff) + 4 + \
                                     toset_nlen + toset_vlen, ITEMALIGNMENT);
                /* if current page full, flush it first. */
                if (new_off > (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES)) {
                    UINT322BUF(new_mngr->pwbuf, KV_PAGE_HDR_MAGIC);

                    /* bitmap update */
                    set_bitmap(new_mngr->bitmap, new_mngr->used);
                    memcpy(new_mngr->pwbuf + KV_PAGE_SIZE - 4 - \
                           KV_PAGE_BITMAP_BYTES, new_mngr->bitmap, \
                           KV_PAGE_BITMAP_BYTES);

                    /* crc */
                    uint32_t crc = kv_crc32(0, new_mngr->pwbuf, KV_PAGE_SIZE - 4);
                    UINT322BUF(new_mngr->pwbuf + KV_PAGE_SIZE - 4, crc);

                    /* flush onto flash */
                    KV_TRACE("%s %d page is full, flushing to flash: blk - %d, used - %d",
                             __func__, __LINE__, i, new_mngr->used);
                    ret = kv_flash_page_write(i, new_mngr->used, \
                                              new_mngr->pwbuf, KV_PAGE_SIZE);
                    if (ret) {
                        /* if write fail, assume a bad block. */
                        KV_ERROR("%s %d failed to write page %d, ret: %d", \
                                 __func__, __LINE__, new_mngr->used, ret);
                        block_mark_bad(i);
                        continue;
                    }

                    new_mngr->used++;
                    if (new_mngr->used >= KV_PAGE_PER_BLOCK_NUMS) {
                        KV_ERROR("%s KV space is full! Pages are exhausted "\
                                 "but there are still items not copied.", __func__);
                        ret = -1;
                        goto end;
                    }

                    /* prepare for a new page write */
                    new_mngr->pwoff = 4;
                    memset(new_mngr->pwbuf, 0, sizeof(new_mngr->pwbuf));
                }

                KV_ASSERT(new_mngr->used < KV_PAGE_PER_BLOCK_NUMS);

                /* save the to-set item. */
                /* copy the len and name fields */
                UINT162BUF(new_mngr->pwbuf + new_mngr->pwoff, strlen(key) + 1);
                new_mngr->pwoff += 2;
                UINT162BUF(new_mngr->pwbuf + new_mngr->pwoff, vallen);
                new_mngr->pwoff += 2;
                /* copy the name */
                strncpy((char *)new_mngr->pwbuf + new_mngr->pwoff, key, strlen(key));
                new_mngr->pwoff += strlen(key) + 1;
                /* copy the value field */
                memcpy(new_mngr->pwbuf + new_mngr->pwoff, val, vallen);
                new_mngr->pwoff += vallen;
                KV_TRACE("%s to-set item %s copied to page %d", __func__, key, i);
            }
        }

        /**
         * if anything left in new kv manager page
         * write buffer, flush onto flash.
         */
        if (new_mngr->pwoff > 4) {
            /* update page magic */
            UINT322BUF(new_mngr->pwbuf, KV_PAGE_HDR_MAGIC);

            /* bitmap update */
            set_bitmap(new_mngr->bitmap, new_mngr->used);
            memcpy(new_mngr->pwbuf + KV_PAGE_SIZE - 4 - \
                   KV_PAGE_BITMAP_BYTES, new_mngr->bitmap, \
                   KV_PAGE_BITMAP_BYTES);

            /* crc */
            crc = kv_crc32(0, new_mngr->pwbuf, KV_PAGE_SIZE - 4);
            UINT322BUF(new_mngr->pwbuf + KV_PAGE_SIZE - 4, crc);

            /* flush onto flash */
            KV_TRACE("%s writing to flash: blk - %d, used - %d",
                     __func__, i, new_mngr->used);
            ret = kv_flash_page_write(i, new_mngr->used, \
                                      new_mngr->pwbuf, KV_PAGE_SIZE);
            if (ret) {
                /* if write fail, assume a bad block. */
                KV_ERROR("%s %d failed to write page %d, ret: %d", \
                         __func__, __LINE__, j, ret);
                block_mark_bad(i);
                continue;
            }

            new_mngr->used++;

            /* prepare for a new page write */
            new_mngr->pwoff = 0;
            memset(new_mngr->pwbuf, 0, sizeof(new_mngr->pwbuf));
        }

        /* now all pages, except for the first page, copied. */
        /* 1st page is with revision number, so should be copied at the end. */
        /* magic, revision number, bitmap in memory already updated before here. */
        /* bitmap to update to flash */
        memcpy(buf_1st + KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES,
               new_mngr->bitmap, KV_PAGE_BITMAP_BYTES);

        /* crc update */
        crc = kv_crc32(0, buf_1st, KV_PAGE_SIZE - 4);
        UINT322BUF(buf_1st + KV_PAGE_SIZE - 4, crc);

        KV_TRACE("%s first page writing to flash: blk - %d, used - %d",
                 __func__, i, new_mngr->used);
        ret = kv_flash_page_write(i, 0, buf_1st, KV_PAGE_SIZE);
        if (ret) {
            KV_ERROR("%s %d failed to write page 0, ret: %d", \
                     __func__, __LINE__, ret);
            block_mark_bad(i);
            continue;
        }

        /* eventhing is fine now, switch kv manager to the new one */
        memcpy(&g_kv_mngr, new_mngr, sizeof(g_kv_mngr));

        break;
    }

    if (i == old_blkidx) {
        KV_ERROR("%s failed to find free block, too many "\
                 "bad blocks inside kv partition!", __func__);
        ret = -1;
        goto end;
    }

end:
    KV_TRACE("%s finish: newblk - %d, newused - %d", __func__,
             g_kv_mngr.blkidx, g_kv_mngr.used, g_kv_mngr.rev);
    KV_TRACE("%s exit", __func__);
    //kv_unlock(&g_kv_lock);
    if (buf_1st) {
        aos_free(buf_1st);
    }
    if (new_mngr) {
        aos_free(new_mngr);
    }

    return ret;
}

static bool is_key_on_page(uint8_t *start, uint8_t *end,
                           const char *key, uint32_t *pos)
{
    if (!start || !end || !key || !pos) {
        return false;
    }

    uint8_t *p = start;
    item_hdr_t ihdr;
    while (p < end) {
        ihdr.nlen = BUF2UINT16(p);
        ihdr.vlen = BUF2UINT16(p + 2);

        /* reach the end of items */
        if (ihdr.nlen == 0 || ihdr.vlen == 0) {
            break;
        }

        if ((strlen(key) + 1 == ihdr.nlen) && \
            (strncmp(key, (const char *)(p + sizeof(ihdr.nlen) + \
                                         sizeof(ihdr.vlen)), ihdr.nlen) == 0)) {
            *pos = p - start;
            KV_TRACE("%s key %s is found existing.", __func__, key);
            return true;
        }

        p = (uint8_t *)kv_alignup((uint32_t)p + sizeof(ihdr.nlen) \
                                  + sizeof(ihdr.vlen) + ihdr.nlen + ihdr.vlen, ITEMALIGNMENT);
    }

    return false;
}

/**
 * @brief Initialize the kv module
 *
 * @retrun 0 on success, otherwise will be failed.
 *
 */
int32_t kv_init(void)
{
    bool need_rebuild = false;
    uint16_t blkidx;
    uint32_t rev;

    if (kv_inited) {
        KV_WARN("%s kv already initialized", __func__);
        return -1;
    }

    KV_TRACE("%s entry", __func__);

    memset(&g_kv_mngr, 0, sizeof(g_kv_mngr));
    kv_lock_create(&g_kv_lock);

    /* 1st scan in good blocks, get valid block and page info. */
    if (!find_last_active_block(true, &blkidx, &rev)) {
        KV_TRACE("%s failed to find last active block in good", __func__);
        /* if no valid found in good blocks, do 2nd scan in bad blocks */
        if (!find_last_active_block(false, &blkidx, &rev)) {
            KV_WARN("%s no last active block found, will format kv partition.", __func__);
            if (format_kv()) {
                KV_ERROR("%s format failed", __func__);
                return -1;
            } else {
                goto done;
            }
        } else {
            KV_TRACE("%s last active block found in bad", __func__);
            need_rebuild = true;
        }
    }

    /* last active block found */
    g_kv_mngr.blkidx = blkidx;
    g_kv_mngr.rev = rev;

    KV_TRACE("%s last active block found: blkidx %d rev %d", __func__, blkidx, rev);

    /* scan the last active block */
    int i;
    uint8_t *_rbuf = (uint8_t *)aos_malloc(KV_PAGE_SIZE);
    if (!_rbuf) {
        KV_ERROR("%s failed to malloc", __func__);
        return -1;
    }

    bool blankpg = false;
    for (i = 1; i < KV_PAGE_PER_BLOCK_NUMS; i++) {
        blankpg = page_is_blank(blkidx, i);

        /* find the start of the free pages, or last page */
        if (!blankpg && (i < (KV_PAGE_PER_BLOCK_NUMS - 1))) {
            continue;
        }

        /* blank or last page found, continue to find the last valid page. */
        if ((i >= (KV_PAGE_PER_BLOCK_NUMS - 1)) && !blankpg) {
            i = KV_PAGE_PER_BLOCK_NUMS - 1;
        } else {
            i--;
        }

        g_kv_mngr.used = i + 1;
        KV_TRACE("%s used: %d", __func__, g_kv_mngr.used);

        /* go find last valid page, and bitmap */
        uint32_t crc, crc_read;
        while (i >= 0) {
            memset(_rbuf, 0, KV_PAGE_SIZE);
            kv_flash_read(blkidx, i, 0, _rbuf, KV_PAGE_SIZE);
            crc = kv_crc32(0, _rbuf, KV_PAGE_SIZE - 4);
            crc_read = BUF2UINT32(&(_rbuf[KV_PAGE_SIZE - 4]));
            if (crc == crc_read) {
                break;
            }

            KV_TRACE("%s page %d crc check failed", __func__, i);
            i--;
        }

        /* no valid page found */
        if (i < 0) {
            KV_ERROR("%s fail to find a valid page in active block.", __func__);
            return -1;
        }

        KV_TRACE("%s last valid page %d", __func__, i);

        /* read bitmap from page */
        memcpy(g_kv_mngr.bitmap, _rbuf + KV_PAGE_SIZE - 4 - \
               KV_PAGE_BITMAP_BYTES, KV_PAGE_BITMAP_BYTES);

        /**
         * attention: need update bitmap, just in case some
         * proged but not finshed page during last powe-off.
         */
        i++;
        while (i < g_kv_mngr.used) {
            clear_bitmap(g_kv_mngr.bitmap, i);
            KV_TRACE("%s page %d bitmap cleared", __func__, i);
            i++;
        }

        break;
    }

    /* rebuild KV, i.e. move valid data to new block */
    if (need_rebuild) {
        KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                 __func__, __LINE__, g_kv_mngr.used);
        if (rebuild_kv(KV_REBUILD_NORMAL, NULL, NULL, 0)) {
            KV_ERROR("%s failed to rebuild KV", __func__);
            return -1;
        }
    }

done:
    kv_inited = true;
#ifdef KV_YES_TRACE
    kv_dump_items();
#endif
    KV_TRACE("%s exit", __func__);

    return 0;
}

/**
 * @brief De-initialize the kv module
 *
 * @retrun none
 */
void kv_deinit(void)
{
    kv_lock_destory(&g_kv_lock);
    memset(&g_kv_mngr, 0, sizeof(g_kv_mngr));
    KV_TRACE("%s exit", __func__);

    return;
}

/**
 * Add a new KV pair.
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  val    the value of the KV pair.
 * @param[in]  len    the length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_set(const char *key, const void *val, int32_t len)
{
    int ret = 0;
    uint16_t i;
    uint8_t *p;
    uint32_t pos, crc;
    page_hdr_t phdr;
    item_hdr_t ihdr;
    char *name;
    uint8_t *value;
    bool found = false;

    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    if (!key || !val || (len <= 0)) {
        return -1;
    }

    if (strlen(key) > KV_MAX_KEY_LEN || len > KV_MAX_VAL_LEN) {
        KV_ERROR("(%d,%d) length is too big, not supported!", \
                 (int)strlen(key), (int)len);
        return -1;
    }

    kv_lock(&g_kv_lock);

    KV_TRACE("%s key: %s, vallen: %d", __func__, key, len);

again:
    /* iterate on each used page */
    KV_TRACE("%s blkmgr: blkidx - %d, used - %d", __func__,
             g_kv_mngr.blkidx, g_kv_mngr.used);
    for (i = 0; i < g_kv_mngr.used; i++) {
        if (!is_valid_page(g_kv_mngr.bitmap, i)) {
            continue;
        }

        /* got a valid page, read content */
        memset(g_kv_mngr.prbuf, 0, sizeof(g_kv_mngr.prbuf));
        if (kv_flash_read(g_kv_mngr.blkidx, i, 0, g_kv_mngr.prbuf,
                          sizeof(g_kv_mngr.prbuf))) {
            KV_ERROR("faied to read page %d on block %d", i, g_kv_mngr.blkidx);
            continue;
        }

        p = g_kv_mngr.prbuf;
        phdr.magic = BUF2UINT32(p);

        /* the whole page read now, check magic */
        if (i == 0) {
            if (phdr.magic != KV_BLOCK_HDR_MAGIC) {
                KV_ERROR("not a valid block magic number on page %d", i);
                continue;
            }

            p += sizeof(phdr.magic) + sizeof(g_kv_mngr.rev);
        } else {
            if (phdr.magic != KV_PAGE_HDR_MAGIC) {
                KV_ERROR("not a valid page magic number on page %d", i);
                continue;
            }

            p += sizeof(phdr.magic);
        }

        /* iterate over each page */
        uint8_t *item_start, *item_end;
        if (i == 0) {
            item_start = g_kv_mngr.prbuf + 8;
        } else {
            item_start = g_kv_mngr.prbuf + 4;
        }
        item_end = g_kv_mngr.prbuf + KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES;
        if (is_key_on_page(item_start, item_end, key, &pos)) {
            found = true;
            KV_TRACE("%s is setting an existing key, will do update.", __func__);
            break;
        }
    }

    /**
     * simplely fall into rebuild rocess,
     * this makes implementation simple.
     */
    if (g_kv_mngr.used >= (KV_PAGE_PER_BLOCK_NUMS - 1)) {
        /* if block already full, need rebuild kv. */
        /* TODO: make sure not exeeding kv storage limit */
        KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                 __func__, __LINE__, g_kv_mngr.used);
        ret = rebuild_kv(KV_REBUILD_SETTING, (const char *)key, val, len);
        if (ret != 0) {
            KV_ERROR("failed to rebuild kv, ret: %d", ret);
        }
        KV_TRACE("%s after rebuild, blkidx - %d, used - %d, rev - %d", __func__,
                 g_kv_mngr.blkidx, g_kv_mngr.used, g_kv_mngr.rev);
        goto end;
    } else {
        if (!found) {
            KV_TRACE("%s existing not found", __func__);

            /* if no existing found, just write a new page. */
            g_kv_mngr.pwoff = 0;
            memset(g_kv_mngr.pwbuf, 0, sizeof(g_kv_mngr.pwbuf));

            /* update page magic */
            UINT322BUF(g_kv_mngr.pwbuf, KV_PAGE_HDR_MAGIC);
            g_kv_mngr.pwoff += 4;

            /* copy the kv name and value, and length info. */
            UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, strlen(key) + 1);
            g_kv_mngr.pwoff += 2;
            UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, len);
            g_kv_mngr.pwoff += 2;
            memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, key, strlen(key) + 1);
            g_kv_mngr.pwoff += strlen(key) + 1;
            memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, val, len);
            g_kv_mngr.pwoff += len;
            KV_ASSERT(g_kv_mngr.pwoff <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES));

            /* bitmap update */
            set_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
            memcpy(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4 - \
                   KV_PAGE_BITMAP_BYTES, g_kv_mngr.bitmap, \
                   KV_PAGE_BITMAP_BYTES);

            /* crc */
            crc = kv_crc32(0, g_kv_mngr.pwbuf, KV_PAGE_SIZE - 4);
            UINT322BUF(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4, crc);

            /* flush onto flash */
            KV_TRACE("%s writing to flash: blkidx - %d, used -%d",
                     __func__, g_kv_mngr.blkidx, g_kv_mngr.used);
            ret = kv_flash_page_write(g_kv_mngr.blkidx, g_kv_mngr.used, \
                                      g_kv_mngr.pwbuf, KV_PAGE_SIZE);
            if (ret) {
                /* if write fail, assume a bad block. */
                KV_ERROR("%s %d failed to write page %d, ret: %d", \
                         __func__, __LINE__, g_kv_mngr.used, ret);
                /* change back to kv manager before rebuild */
                clear_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
                KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                         __func__, __LINE__, g_kv_mngr.used);
                ret = rebuild_kv(KV_REBUILD_NORMAL, NULL, NULL, 0);
                if (ret == 0) {
                    block_mark_bad(i);
                    goto again;
                } else {
                    KV_ERROR("failed to rebuild kv, ret: %d", ret);
                    goto end;
                }
            }

            g_kv_mngr.used++;
            g_kv_mngr.pwoff = 0;
            memset(g_kv_mngr.pwbuf, 0, sizeof(g_kv_mngr.pwbuf));
        } else {
            KV_TRACE("%s existing found", __func__);

            /* if exsiting found, write 1 or 2 pages and evict old page. */
            g_kv_mngr.pwoff = 4;
            memset(g_kv_mngr.pwbuf, 0, sizeof(g_kv_mngr.pwbuf));

            if (i == 0) {
                p = g_kv_mngr.prbuf + 8;
            } else {
                p = g_kv_mngr.prbuf + 4;
            }

            /* iterate over each item on the page, save all non related item */
            while ((p - g_kv_mngr.prbuf) < (sizeof(g_kv_mngr.prbuf) - 4 - \
                                            KV_PAGE_BITMAP_BYTES)) {
                /* ensure nlen and vlen have the tailing '\0' included */
                ihdr.nlen = BUF2UINT16(p);
                ihdr.vlen = BUF2UINT16(p + 2);
                name = (char *)p + 4;
                value = p + 4 + ihdr.nlen;

                /* if the end of read-from item, break */
                if (!ihdr.nlen && !ihdr.vlen) {
                    break;
                }

                if ((strlen(key) + 1 == ihdr.nlen) && \
                    (strncmp(name, key, strlen(key)) == 0)) {
                    if ((len == ihdr.vlen) && \
                        (memcmp(val, value, len) == 0)) {
                        /* exactly the same item found, no need to set anymore */
                        ret = 0;
                        KV_TRACE("%s exactly the same value to set for "\
                                 "key %s, will skip it.", __func__, key);
                        goto end;
                    } else {
                        /* do nothing. */
                    }
                } else {
                    memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, p, \
                           4 + ihdr.nlen + ihdr.vlen);
                    /* always update write-to index. */
                    g_kv_mngr.pwoff = kv_alignup(g_kv_mngr.pwoff + 4 + ihdr.nlen + \
                                                 ihdr.vlen, ITEMALIGNMENT);
                    KV_ASSERT(g_kv_mngr.pwoff <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES));
                }

                /* update read-from index */
                p += 4 + ihdr.nlen + ihdr.vlen;
                p = (uint8_t *)kv_alignup((uint32_t)p, ITEMALIGNMENT);
            }

            /* now all items except for the to-set one copied. */
            bool exceed_page = false;
            if ((KV_PAGE_SIZE - g_kv_mngr.pwoff - 4 - KV_PAGE_BITMAP_BYTES) >= \
                kv_alignup(4 + strlen(key) + 1 + len, ITEMALIGNMENT)) {
                KV_TRACE("%s only one page write.", __func__);
                /* good, we can save all data on current page. */
                UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, strlen(key) + 1);
                g_kv_mngr.pwoff += sizeof(ihdr.nlen);
                UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, len);
                g_kv_mngr.pwoff += sizeof(ihdr.vlen);
                memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, key, strlen(key) + 1);
                g_kv_mngr.pwoff += strlen(key) + 1;
                memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, val, len);
                g_kv_mngr.pwoff += len;
                g_kv_mngr.pwoff = kv_alignup((uint32_t)(g_kv_mngr.pwoff), ITEMALIGNMENT);
                KV_ASSERT(g_kv_mngr.pwoff <= (KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES));
            } else {
                /**
                 * well, we need one more page. The to-set item will
                 * be on the new page alone. Be careful to deal with
                 *unexpected power-off.
                 */
                KV_TRACE("%s two pages write needed.", __func__);

                exceed_page = true;

                /* page magic update */
                UINT322BUF(g_kv_mngr.pwbuf, KV_PAGE_HDR_MAGIC);

                /**
                 * bitmap update.
                 *
                 * Caution: keep i valid and current page invalid at this step.
                 *          We will invaid i and valid current page in the next
                 *          page commit. By doing like this, we are able to
                 *          the splitting a atomic operation.
                 *
                 * No action required here, just copy.
                 */
                memcpy(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES, \
                       g_kv_mngr.bitmap, KV_PAGE_BITMAP_BYTES);

                /* crc update */
                crc = kv_crc32(0, g_kv_mngr.pwbuf, KV_PAGE_SIZE - 4);
                UINT322BUF(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4, crc);

                /* commit onto flash */
                KV_TRACE("%s first page writing to flash: blkidx - %d, used -%d",
                         __func__, g_kv_mngr.blkidx, g_kv_mngr.used);
                if (kv_flash_page_write(g_kv_mngr.blkidx, g_kv_mngr.used, \
                                        g_kv_mngr.pwbuf, KV_PAGE_SIZE)) {
                    /* commit failed, need rebuild KV and mark bad block. */
                    KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                             __func__, __LINE__, g_kv_mngr.used);
                    ret = rebuild_kv(KV_REBUILD_NORMAL, NULL, NULL, 0);
                    if (ret == 0) {
                        block_mark_bad(i);
                        goto again;
                    } else {
                        KV_ERROR("%s failed to rebuild KV", __func__);
                        goto end;
                    }
                }

                g_kv_mngr.used++;

                /* clear g_kv_mngr.pwbuf to store the to-set item. */
                g_kv_mngr.pwoff = 4;
                memset(g_kv_mngr.pwbuf, 0, sizeof(g_kv_mngr.pwbuf));
                UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, strlen(key) + 1);
                g_kv_mngr.pwoff += 2;
                UINT162BUF(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, len);
                g_kv_mngr.pwoff += 2;
                memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, key, strlen(key) + 1);
                g_kv_mngr.pwoff += strlen(key) + 1;
                memcpy(g_kv_mngr.pwbuf + g_kv_mngr.pwoff, val, len);
                g_kv_mngr.pwoff += len;
            }

            /* page magic update */
            UINT322BUF(g_kv_mngr.pwbuf, KV_PAGE_HDR_MAGIC);

            /**
             * bitmap update.
             *
             * Caution: If page is slit due a longer value set
             * into an existing item, now this bitmap should invalid i
             * and valid the last page now, which has other valid items on.
             */
            clear_bitmap(g_kv_mngr.bitmap, i);
            set_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
            if (exceed_page) {
                set_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used - 1);
            }
            memcpy(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES, \
                   g_kv_mngr.bitmap, KV_PAGE_BITMAP_BYTES);

            /* crc update */
            crc = kv_crc32(0, g_kv_mngr.pwbuf, KV_PAGE_SIZE - 4);
            UINT322BUF(g_kv_mngr.pwbuf + KV_PAGE_SIZE - 4, crc);

            /* commit onto flash */
            KV_TRACE("%s last page writing to flash: blkidx - %d, used -%d",
                     __func__, g_kv_mngr.blkidx, g_kv_mngr.used);
            if (kv_flash_page_write(g_kv_mngr.blkidx, g_kv_mngr.used, \
                                    g_kv_mngr.pwbuf, KV_PAGE_SIZE)) {
                /* commit failed, need rebuild KV and mark bad block. */
                /* restore bitmap before recover */
                set_bitmap(g_kv_mngr.bitmap, i);
                clear_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
                KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                         __func__, __LINE__, g_kv_mngr.used);
                if (exceed_page) {
                    clear_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used - 1);
                    g_kv_mngr.used--;
                }
                ret = rebuild_kv(KV_REBUILD_NORMAL, NULL, NULL, 0);
                if (ret == 0) {
                    block_mark_bad(i);
                    goto again;
                } else {
                    KV_ERROR("%s failed to rebuild KV", __func__);
                    goto end;
                }
            }

            g_kv_mngr.used++;
        }
    }

end:
    KV_TRACE("%s exit", __func__);
    kv_unlock(&g_kv_lock);

    return ret;
}

/**
 * Get the KV pair's value stored in buffer by its key.
 *
 * @note: the buffer_len should be larger than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_get(const char *key, void *buffer, int32_t *buffer_len)
{
    int ret = -1;
    uint16_t i;
    uint8_t *p;
    page_hdr_t phdr;
    item_hdr_t ihdr;
    char *name;
    uint8_t *value;
    bool found = false;

    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    if (!key || !buffer || (*buffer_len <= 0)) {
        return -1;
    }

    kv_lock(&g_kv_lock);

    KV_TRACE("%s entry, key: %s", __func__, key);

    /* iterate on each used page */
    for (i = 0; i < g_kv_mngr.used; i++) {
        if (!is_valid_page(g_kv_mngr.bitmap, i)) {
            continue;
        }

        KV_TRACE("%s valid page %d", __func__, i);

        /* got a valid page, read content */
        memset(g_kv_mngr.prbuf, 0, sizeof(g_kv_mngr.prbuf));
        if (kv_flash_read(g_kv_mngr.blkidx, i, 0, g_kv_mngr.prbuf,
                          sizeof(g_kv_mngr.prbuf))) {
            KV_ERROR("faied to read page %d on block %d", i, g_kv_mngr.blkidx);
            continue;
        }

        p = g_kv_mngr.prbuf;
        phdr.magic = BUF2UINT32(p);

        /* the whole page read now, check magic */
        if (i == 0) {
            if (phdr.magic != KV_BLOCK_HDR_MAGIC) {
                KV_ERROR("not a valid block magic number on page %d", i);
                continue;
            }

            p += sizeof(phdr.magic) + sizeof(g_kv_mngr.rev);
        } else {
            if (phdr.magic != KV_PAGE_HDR_MAGIC) {
                KV_ERROR("not a valid page magic number on page %d", i);
                continue;
            }

            p += sizeof(phdr.magic);
        }

        /* iterate over each item */
        while ((p - g_kv_mngr.prbuf) <= sizeof(g_kv_mngr.prbuf)) {
            /* ensure nlen and vlen have the tailing '\0' included */
            ihdr.nlen = BUF2UINT16(p);
            p += sizeof(ihdr.nlen);
            ihdr.vlen = BUF2UINT16(p);
            p += sizeof(ihdr.vlen);
            name = (char *)p;
            value = p + ihdr.nlen;

            /* reach the end of items */
            if (ihdr.nlen == 0 || ihdr.vlen == 0) {
                break;
            }

            if ((strlen(key) + 1 == ihdr.nlen) && \
                (strncmp(name, key, strlen(key)) == 0)) {
                found = true;

                memcpy(buffer, value, *buffer_len);
                if (ihdr.vlen > *buffer_len) {
                    KV_WARN("buffer is too small (%d vs. %d)", *buffer_len, ihdr.vlen);
                    ret = -1;
                } else {
                    ret = 0;
                }

                *buffer_len = ihdr.vlen;
                KV_TRACE("%s item found on page %d", __func__, i);
                break;
            }

            p += ihdr.nlen + ihdr.vlen;
            p = (uint8_t *)kv_alignup((uint32_t)p, ITEMALIGNMENT);
        }

        if (found) {
            break;
        }
    }

    KV_TRACE("%s exit", __func__);
    kv_unlock(&g_kv_lock);

    return found ? 0 : ret;
}

/**
 * Delete the KV pair by its key.
 *
 * @param[in]  key  the key of the KV pair to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_delete(const char *key)
{
    int ret;
    uint16_t i;
    uint8_t *p, *q, *item_start;
    page_hdr_t phdr;
    bool found;
    uint32_t pos;

    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    if (!key) {
        return -1;
    }

    kv_lock(&g_kv_lock);

    KV_TRACE("%s entry", __func__);

    /* just in case bad block when deleting, should try again from here. */
again:
    ret = 0;
    found = false;

    /* iterate on each used page */
    for (i = 0; i < g_kv_mngr.used; i++) {
        if (!is_valid_page(g_kv_mngr.bitmap, i)) {
            continue;
        }

        /* got a valid page, read content */
        memset(g_kv_mngr.prbuf, 0, sizeof(g_kv_mngr.prbuf));
        if (kv_flash_read(g_kv_mngr.blkidx, i, 0, g_kv_mngr.prbuf,
                          sizeof(g_kv_mngr.prbuf))) {
            KV_ERROR("faied to read page %d on block %d", i, g_kv_mngr.blkidx);
            continue;
        }

        phdr.magic = BUF2UINT32(g_kv_mngr.prbuf);

        /* all page read now, check magic */
        if ((i == 0 && phdr.magic != KV_BLOCK_HDR_MAGIC) || \
            (i > 0 && phdr.magic != KV_PAGE_HDR_MAGIC)) {
            KV_ERROR("not a valid magic number on page %d", i);
            continue;
        }

        p = g_kv_mngr.prbuf;
        p += (i == 0 ? 8 : 4);

#if 0
        /* iterate over each item. */
        while ((p - g_kv_mngr.prbuf) <= sizeof(g_kv_mngr.prbuf)) {
            /* caller to ensure nlen have the tailing '\0' included */
            ihdr.nlen = BUF2UINT16(p);
            p += sizeof(ihdr.nlen);
            ihdr.vlen = BUF2UINT16(p);
            p += sizeof(ihdr.vlen);
            name = (char *)p;
            value = p + ihdr.nlen;

            /* reach the end of items */
            if (ihdr.nlen == 0 || ihdr.vlen == 0) {
                break;
            }

            if (strncmp(name, key, strlen(key)) == 0) {
                found = true;
                break;
            }

            p += ihdr.nlen + ihdr.vlen;
            p = (uint8_t *)kv_alignup((uint32_t)p, ITEMALIGNMENT);
        }
#endif

        /* iterate over each page */
        uint8_t *item_end;
        item_start = p;
        item_end = g_kv_mngr.prbuf + KV_PAGE_SIZE - 4 - KV_PAGE_BITMAP_BYTES;

        if (is_key_on_page(item_start, item_end, key, &pos)) {
            found = true;
            KV_TRACE("%s find an existing key.", __func__);
            break;
        }
    }

    /* if found, delete it. */
    if (found) {
        KV_TRACE("%s item found: %d", __func__, found);

        if (g_kv_mngr.used < KV_PAGE_PER_BLOCK_NUMS) {
            KV_TRACE("%s deleting ny writing new page", __func__);

            /* there is still free pages on block, normal delete */
            //uint8_t *item_start = p - sizeof(ihdr.nlen) - sizeof(ihdr.vlen);
            item_start += pos;
            uint16_t _nlen, _vlen;
            _nlen = BUF2UINT16(item_start);
            _vlen = BUF2UINT16(item_start + 2);
            uint8_t *next_start = (uint8_t *)kv_alignup((uint32_t)item_start + 4 + _nlen + \
                                  _vlen, ITEMALIGNMENT);
            uint32_t remain_size = (uint32_t)(KV_PAGE_SIZE + \
                                              (uint32_t)g_kv_mngr.prbuf - \
                                              (uint32_t)next_start - \
                                              sizeof(page_tailer_t));

            /**
             * if it is the block header page, change magic to page
             * magic number, and remove rev field first.
             */
            if (i == 0) {
                p = g_kv_mngr.prbuf;
                UINT322BUF(p, (uint32_t)KV_PAGE_HDR_MAGIC);
                p += 4;

                q = g_kv_mngr.prbuf + 4 + 4;
                while (q < item_start) {
                    *p++ = *q++;
                }
            } else {
                p = item_start;
            }

            /* move remaining items forward */
            memcpy(p, next_start, remain_size);
            memset(p + remain_size, 0, KV_PAGE_SIZE - \
                   remain_size + (size_t)(g_kv_mngr.prbuf - p));

            /* clear outdated page bitmap */
            clear_bitmap(g_kv_mngr.bitmap, i);

            /* set new used page's bitmap */
            set_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
            memcpy(g_kv_mngr.prbuf + KV_PAGE_SIZE - sizeof(page_tailer_t), \
                   g_kv_mngr.bitmap, sizeof(g_kv_mngr.bitmap));

            /* update CRC */
            uint32_t crc = kv_crc32(0, g_kv_mngr.prbuf, KV_PAGE_SIZE - 4);
            UINT322BUF(g_kv_mngr.prbuf + KV_PAGE_SIZE - 4, crc);

            /* commit onto flash */
            KV_TRACE("%s writing flash: blkidx - %d, used - %d",
                     __func__, g_kv_mngr.blkidx, g_kv_mngr.used);
            if (kv_flash_page_write(g_kv_mngr.blkidx, g_kv_mngr.used, \
                                    g_kv_mngr.prbuf, KV_PAGE_SIZE)) {
                /* commit failed, need rebuild KV and mark bad block. */
                /* resume kv manager info before rebuild. */
                KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                         __func__, __LINE__, g_kv_mngr.used);
                /* restore bitmap */
                clear_bitmap(g_kv_mngr.bitmap, g_kv_mngr.used);
                set_bitmap(g_kv_mngr.bitmap, i);
                ret = rebuild_kv(KV_REBUILD_NORMAL, NULL, NULL, 0);
                if (ret == 0) {
                    block_mark_bad(i);
                    goto again;
                } else {
                    KV_ERROR("%s failed to rebuild KV", __func__);
                    return ret;
                }
            } else {
                /* anything else to update in kv manager? */
            }

            g_kv_mngr.used++;
        } else {
            /* block is full, delete in place. */
            KV_TRACE("%s %d, used pages: %d, will rebuild kv.", \
                     __func__, __LINE__, g_kv_mngr.used);
            ret = rebuild_kv(KV_REBUILD_DELETING, key, NULL, 0);
            KV_TRACE("%s after rebuild: blkidx - %d, used - %d",
                     __func__, g_kv_mngr.blkidx, g_kv_mngr.used);
        }
    } else {
        ret = -1;
    }

    KV_TRACE("%s entry, ret: %d", __func__, ret);
    kv_unlock(&g_kv_lock);

    /* if not found, delete should return success too. */
    return ret;
}

/**
 * Delete the KV pair by its prefix.
 *
 * @param[in]  prefix  the prefix of the kv pair which is need to delete.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_delete_by_prefix(const char *prefix)
{
    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    return 0;
}

/**
 * Add a new KV pair (secure mode).
 *
 * @param[in]  key    the key of the KV pair.
 * @param[in]  val    the value of the KV pair.
 * @param[in]  len    the length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_secure_set(const char *key, const void *val, int32_t len)
{
    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    return 0;
}

/**
 * Get the KV pair's value stored in buffer by its key. (secure mode)
 *
 * @note: the buffer_len should be larger than the real length of the value,
 *        otherwise buffer would be NULL.
 *
 * @param[in]      key         the key of the KV pair to get.
 * @param[out]     buffer      the memory to store the value.
 * @param[in-out]  buffer_len  in: the length of the input buffer.
 *                             out: the real length of the value.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t kv_item_secure_get(const char *key, void *buffer, int32_t *buffer_len)
{
    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return -1;
    }

    return 0;
}

item_info_t *kv_item_traverse(item_func func, const char *key)
{
    int ret;
    uint16_t i;
    uint8_t *p;
    page_hdr_t phdr;
    item_info_t *item;

    if (!kv_inited) {
        KV_ERROR("%s KV is not correctly initialized.", __func__);
        return NULL;
    }

    item = (item_info_t *)aos_malloc(sizeof(item_info_t));
    if (!item) {
        KV_ERROR("%s failed to allocate memory.", __func__);
        return NULL;
    }

    kv_lock(&g_kv_lock);

    /* iterate on each used page */
    for (i = 0; i < g_kv_mngr.used; i++) {
        if (!is_valid_page(g_kv_mngr.bitmap, i)) {
            continue;
        }

        /* got a valid page, read content */
        memset(g_kv_mngr.prbuf, 0, sizeof(g_kv_mngr.prbuf));
        if (kv_flash_read(g_kv_mngr.blkidx, i, 0, g_kv_mngr.prbuf,
                          sizeof(g_kv_mngr.prbuf))) {
            KV_ERROR("faied to read page %d on block %d", i, g_kv_mngr.blkidx);
            continue;
        }

        phdr.magic = BUF2UINT32(g_kv_mngr.prbuf);

        /* all page read now, check magic */
        if ((i == 0 && phdr.magic != KV_BLOCK_HDR_MAGIC) || \
            (i > 0 && phdr.magic != KV_PAGE_HDR_MAGIC)) {
            KV_ERROR("not a valid magic number on page %d", i);
            continue;
        }

        p = g_kv_mngr.prbuf;
        p += (i == 0 ? 8 : 4);

        while (p < (g_kv_mngr.prbuf + KV_PAGE_SIZE - \
                    4 - KV_PAGE_BITMAP_BYTES)) {
            memset(item, 0, sizeof(item));
            item->nlen = BUF2UINT16(p);
            item->vlen = BUF2UINT16(p + 2);

            /* reach the end of items */
            if (item->nlen == 0 || item->vlen == 0) {
                break;
            }

            item->name = (char *)p + 4;
            item->value = p + 4 + item->nlen;
            item->blkidx = g_kv_mngr.blkidx;
            item->pageidx = i;
            item->off = p - g_kv_mngr.prbuf;
            ret = func(item, key);
            if (ret == KV_OK) {
                kv_unlock(&g_kv_lock);
                /* caller to ensure memory free */
                return item;
            }
            p = (uint8_t *)kv_alignup((uint32_t)p + 4 + \
                                      item->nlen + item->vlen, ITEMALIGNMENT);
        }
    }

    kv_unlock(&g_kv_lock);
    aos_free(item);
    return NULL;
}
