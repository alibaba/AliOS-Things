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
 * magic: The magic number of block
 * state: The state of the block
 *
 */
typedef struct _block_header_t
{
    uint8_t magic;
    uint8_t state;
    uint8_t reserved[2];
} KVMAGR_PKD block_hdr_t;

/**
 * Key-value item header description
 *
 * magic:       The magic number of key-value item
 * state:       The state of key-value item
 * crc:         The crc-8 value of key-value item
 * key_len:     The length of the key
 * val_len:     The length of the value
 * origin_off:  The origin item offset
 *
 */
typedef struct _item_header_t
{
    uint8_t   magic;
    uint8_t   state;
    uint8_t   crc;
    uint8_t   key_len;
    uint16_t  val_len;
    kv_size_t origin_off;
} KVMAGR_PKD item_hdr_t;

#ifdef _WIN32
#pragma pack(pop)
#endif

/**
 * Defination of block information
 *
 * KV_BLOCK_SIZE:        block size, default is 4K bytes
 * KV_BLOCK_NUMS:        numbers of blocks, must be bigger than KV_GC_RESERVED
 * KV_BLOCK_OFF_MASK:    the mask of the block offset in kv store
 * KV_BLOCK_HDR_SIZE:    The block header size, default is 4 bytes
 *
 */
#define KV_BLOCK_SIZE      (1 << KV_BLOCK_SIZE_BITS)
#define KV_BLOCK_NUMS      (KV_TOTAL_SIZE >> KV_BLOCK_SIZE_BITS)
#define KV_BLOCK_OFF_MASK ~(KV_BLOCK_SIZE - 1)
#define KV_BLOCK_HDR_SIZE  sizeof(block_hdr_t)

/**
 * Block state information
 *
 * KV_BLOCK_STATE_USED:  block is inused and without dirty data
 * KV_BLOCK_STATE_CLEAN: block is clean, ready for used
 * KV_BLOCK_STATE_DIRTY: block is inused and with dirty data
 *
 */
#define KV_BLOCK_STATE_USED  0xCC
#define KV_BLOCK_STATE_CLEAN 0xEE
#define KV_BLOCK_STATE_DIRTY 0x44

#define INVALID_BLK_STATE(state) (((state) != KV_BLOCK_STATE_USED) &&  \
                                  ((state) != KV_BLOCK_STATE_CLEAN) && \
                                  ((state) != KV_BLOCK_STATE_DIRTY))

/**
 * Defination of item information
 *
 * KV_ITEM_HDR_SIZE: the item header size
 * KV_ITEM_MAX_LEN:  the max length of the item
 *
 */
#define KV_ITEM_HDR_SIZE sizeof(item_hdr_t)
#define KV_ITEM_MAX_LEN  (KV_ITEM_HDR_SIZE + KV_MAX_KEY_LEN + KV_MAX_VAL_LEN)

/**
 * Item state information
 *
 * KV_ITEM_STATE_NORMAL: the item is valid
 * KV_ITEM_STATE_DELETE: the item had beed deleted
 *
 */
#define KV_ITEM_STATE_NORMAL 0xEE
#define KV_ITEM_STATE_DELETE 0x00

/**
 * Defination of the key-value store information
 *
 * KV_STATE_OFFSET: the offset of block/item state in header structure
 * KV_ALIGN_MASK:   the mask of the key-value store alignment
 * KV_DELETE_FLAG:  the flag of item is deleted by self
 * KV_UPDATE_FLAG:  the flag of item is deleted by update
 *
 */
#define KV_STATE_OFFSET  1
#define KV_ALIGN_MASK   ~(sizeof(void *) - 1)
#define KV_DELETE_FLAG   0
#define KV_UPDATE_FLAG   1

/**
 * Garbage collection related defination
 *
 * KV_RESERVED_BLOCKS: the reserved block for garbage collection
 *
 */
#define KV_RESERVED_BLOCKS 1

/**
 * Key-value item descriptor defination
 *
 * hdr:     the header of the item
 * store:   the store buffer for key-value pair
 * len:     the length of the buffer
 * pos:     the store position of the key-value item
 *
 */
typedef struct _kv_item_t {
    item_hdr_t  hdr;
    char       *store;
    uint16_t    len;
    kv_size_t   pos;
} kv_item_t;

/**
 * Block information struct for management
 *
 * space:   free space in current block
 * state:   the state of current block
 *
 */
typedef struct _block_info_t {
    kv_size_t space;
    uint8_t   state;
} block_info_t;

/**
 * Key-value module management struct
 *
 * inited:          the flag to indicate the module is inited
 * gc_trigger:      the flag to indicate GC is triggered
 * gc_waiter:       numbers of the task wait for GC finished
 * clean_blk_nums:  numbers of the clean block
 * write_pos:       current write position
 * block_info[]:    the array to record the block management info
 *
 */
typedef struct _kv_mgr_t {
    uint8_t       inited;
    uint8_t       gc_trigger;
    uint8_t       gc_waiter;
    uint8_t       clean_blk_nums;
    kv_size_t     write_pos;
    void         *gc_sem;
    void         *lock;
    block_info_t  block_info[KV_BLOCK_NUMS];
} kv_mgr_t;

typedef struct _kv_store_t {
    char     *p;
    int       res;
    uint16_t  len;
} kv_store_t;

/**
 * Magic Number for block & item
 */
static const uint8_t KV_BLOCK_MAGIC_NUM = 'K';
static const uint8_t KV_ITEM_MAGIC_NUM  = 'I';

/**
 * The function to be invoked while polling the used block
 */
typedef int32_t (*item_func)(kv_item_t *item, const char *key);

#define KV_ALIGN(x) ((x + ~KV_ALIGN_MASK) & KV_ALIGN_MASK)

#endif  /* KV_INTERNAL_H */

