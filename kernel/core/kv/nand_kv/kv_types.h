/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef KV_TYPES_H
#define KV_TYPES_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "kv_conf.h"
#include "kv_util.h"

/* "kblk" */
#define KV_BLOCK_HDR_MAGIC (('k' << 24) | ('b' << 16) | ('l') << 8 | 'k')
/* "kpge" */
#define KV_PAGE_HDR_MAGIC (('k' << 24) | ('p' << 16) | ('g') << 8 | 'e')

#if (KV_BLOCK_SIZE_BITS >= 16) || (KV_TOTAL_SIZE >= 0x10000)
    typedef uint32_t kv_size_t; /* If more than 64KB */
#else
    typedef uint16_t kv_size_t;
#endif

#ifdef _WIN32
    #define KVMAGR_PKD
#else
    #define KVMAGR_PKD __attribute__((packed))
#endif

#ifdef _WIN32
    #pragma pack(push, 1)
#endif

/**
 * Flash block header description
 *
 * magic: The magic number of block, i.e. 0xcdcdcdcd
 * ver: The version number of the block, also indicate the erase times.
 *
 */
typedef struct _block_header_t {
    uint32_t magic;
    uint32_t rev;
} KVMAGR_PKD block_hdr_t;

/**
 * Flash page header.
 *
 * magic:       The magic number of used page, i.e. 0xabababab.
 *
 */
typedef struct _page_header_t {
    uint32_t   magic;
} KVMAGR_PKD page_hdr_t;

/**
 * KV item header header.
 *
 * magic:       The magic number of used page, i.e. 0xabababab.
 *
 */
typedef struct _item_header_t {
    uint16_t   nlen; /* name length */
    uint16_t   vlen; /* value length */
} KVMAGR_PKD item_hdr_t;

/**
 * Defination of block information
 *
 * KV_BLOCK_OFF_MASK:    the mask of the block offset in kv store
 * KV_BLOCK_HDR_SIZE:    the block header size
 *
 */
#define KV_BLOCK_OFF_MASK  ~(KV_BLOCK_SIZE - 1)
#define KV_BLOCK_HDR_SIZE  sizeof(block_hdr_t)

/**
 * Defination of page information
 *
 * KV_PAGE_PER_BLOCK_NUMS: numbers of page inside a block
 * KV_PAGE_OFF_MASK:       the mask of the page offset in kv store
 * KV_PAGE_HDR_SIZE:       the page header size
 *
 */
#define KV_PAGE_PER_BLOCK_NUMS  (KV_BLOCK_SIZE / KV_PAGE_SIZE)
#define KV_PAGE_OFF_MASK        ~(KV_PAGE_SIZE - 1)
#define KV_PAGE_HDR_SIZE        sizeof(page_hdr_t)

/**
 * Defination of item information
 *
 * KV_ITEM_HDR_SIZE: the item header size
 * KV_ITEM_MAX_LEN:  the max length of the item
 *
 */
#define KV_ITEM_HDR_SIZE sizeof(item_hdr_t)
#define KV_ITEM_MAX_LEN  (KV_ITEM_HDR_SIZE + KV_MAX_KEY_LEN + KV_MAX_VAL_LEN)

#define KV_PAGE_BITMAP_BYTES ((KV_PAGE_PER_BLOCK_NUMS + 7) >> 3)

/**
 * Flash page page tailer.
 *
 * bitmap:       the bitmap information of page state inside a block.
 * crc:          the crc value of the whole page (excluding crc iteself).
 *
 */
typedef struct _page_tailer_t {
    uint8_t   bitmap[KV_PAGE_BITMAP_BYTES];
    uint32_t  crc;
} KVMAGR_PKD page_tailer_t;

#ifdef _WIN32
    #pragma pack(pop)
#endif

/**
 * Block information struct for management
 *
 * rev:     the revision number
 * used:    the used pages.
 * bitmap:  the bitmap of the page in current block.
 * pagebuf: the buffer of the content to write to page.
 * pageoff: the offset of the buffer to write to.
 *
 */
typedef struct _block_info_t {
    uint16_t  blkidx; /* the index number of the block */
    uint32_t  rev; /* revision number */
    uint16_t  used; /* used pages */
    uint8_t   bitmap[KV_PAGE_BITMAP_BYTES]; /* current bitmap, always read from the last valid page */
    uint8_t   pwbuf[KV_PAGE_SIZE]; /* page write buffer */
    uint16_t  pwoff; /* page write offset */
    uint8_t   prbuf[KV_PAGE_SIZE]; /* page read buffer */
} block_info_t;

/**
 * item information (length and position in flash).
 * Usually used during kv item traverse process.
 */
typedef struct {
    uint16_t nlen;
    char *name;
    uint16_t vlen;
    void *value;
    uint16_t blkidx;
    uint16_t pageidx;
    uint32_t off;
} item_info_t;

/**
 * The function to be invoked while traversing the kv block.
 */
typedef int (*item_func)(item_info_t *item, const char *key);

#endif  /* KV_INTERNAL_H */

