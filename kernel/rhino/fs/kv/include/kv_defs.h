/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KV_DEFS_H
#define _KV_DEFS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "kv_conf.h"
#include "kv_osal.h"

/* Key-value function return code description */
#define RES_OK               0  /* Successed */
#define RES_CONT            -1  /* Loop continued */
#define RES_NO_SPACE        -2  /* The space is out of range */
#define RES_INVALID_PARAM   -3  /* The parameter is invalid */
#define RES_MALLOC_FAILED   -4  /* The os malloc error */
#define RES_ITEM_NOT_FOUND  -5  /* Could not find the key-value item */
#define RES_FLASH_READ_ERR  -6  /* The flash read operation failed */
#define RES_FLASH_WRITE_ERR -7  /* The flash write operation failed */
#define RES_FLASH_EARSE_ERR -8  /* The flash earse operation failed */
#define RES_MUTEX_ERR       -9  /* The os mutex error */
#define RES_SEM_ERR         -10 /* The os semaphose error */

#if (BLK_BITS >= 16) || (KV_TOTAL_SIZE >= 0x10000)
/* If more than 64KB */
typedef uint32_t kv_size_t;
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
    uint8_t  magic;
    uint8_t  state;
    uint8_t  crc;
    uint8_t  key_len;
    uint16_t val_len;
    kv_size_t origin_off;
} KVMAGR_PKD item_hdr_t;

#ifdef _WIN32
#pragma pack(pop)
#endif

/**
 * Defination of block information
 * 
 * BLK_SIZE:        block size, default is 4K bytes
 * BLK_NUMS:        numbers of blocks, must be bigger than KV_GC_RESERVED
 * BLK_OFF_MASK:    the mask of the block offset in kv store
 * BLK_HEADER_SIZE: The block header size, default is 4 bytes
 *
 */
#define BLK_SIZE     (1 << BLK_BITS)
#define BLK_NUMS     (KV_TOTAL_SIZE >> BLK_BITS)
#define BLK_OFF_MASK ~(BLK_SIZE - 1)
#define BLK_HDR_SIZE sizeof(block_hdr_t)

/**
 * Block state information
 *
 * BLK_STATE_USED:  block is inused and without dirty data
 * BLK_STATE_CLEAN: block is clean, ready for used
 * BLK_STATE_DIRTY: block is inused and with dirty data
 * 
 */
#define BLK_STATE_USED  0xCC
#define BLK_STATE_CLEAN 0xEE
#define BLK_STATE_DIRTY 0x44

#define INVALID_BLK_STATE(state)                                    \
    (((state) != BLK_STATE_USED) && ((state) != BLK_STATE_CLEAN) && \
     ((state) != BLK_STATE_DIRTY))

/**
 * Defination of item information
 *
 * ITEM_HEADER_SIZE: the item header size
 * ITEM_MAX_KEY_LEN: the max key length of the item, default is 128 bytes
 * ITEM_MAX_VAL_LEN: the max value length of the item, default is 512 bytes
 * ITEM_MAX_LEN:     the max length of the item
 * 
 */
#define ITEM_HDR_SIZE    sizeof(item_hdr_t)
#define ITEM_MAX_KEY_LEN 128
#define ITEM_MAX_VAL_LEN 512
#define ITEM_MAX_LEN (ITEM_HDR_SIZE + ITEM_MAX_KEY_LEN + ITEM_MAX_VAL_LEN)

/**
 * Item state information
 *
 * ITEM_STATE_NORMAL: the item is valid
 * ITEM_STATE_DELETE: the item had beed deleted
 * 
 */
#define ITEM_STATE_NORMAL 0xEE
#define ITEM_STATE_DELETE 0x00

/**
 * Defination of the key-value store information
 *
 * KV_STATE_OFF:   the offset of block/item state in header structure
 * KV_ALIGN_MASK:  the mask of the key-value store alignment
 * KV_SELF_REMOVE: the flag of item is deleted by self
 * KV_ORIG_REMOVE: the flag of item is deleted by update
 * 
 */
#define KV_STATE_OFF        1
#define KV_ALIGN_MASK       ~(sizeof(void *) - 1)
#define KV_DELETE_REMOVE    0
#define KV_UPDATE_REMOVE    1

/**
 * Garbage collection related defination
 *
 * KV_GC_RESERVED:      the reserved block for garbage collection
 * KV_GC_STACK_SIZE:    the stack size of the garbage collection task
 * 
 */
#define KV_GC_RESERVED      1
#define KV_GC_STACK_SIZE    1024

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
 * gc_sem:          semaphore for GC
 * mutex:           mutex for module sync
 * block_info[]:    the array to record the block management info
 * 
 */
typedef struct _kv_mgr_t {
    uint8_t         inited;
    uint8_t         gc_trigger;
    uint8_t         gc_waiter;
    uint8_t         clean_blk_nums;
    kv_size_t       write_pos;
    kv_sem_t        gc_sem;
    kv_mutex_t      mutex;
    block_info_t    block_info[BLK_NUMS];
} kv_mgr_t;

typedef struct _kv_store_t {
    char       *p;
    int         res;
    uint16_t    len;
} kv_store_t;

/**
 * Magic Number for block & item
 */
static const uint8_t BLK_MAGIC_NUM = 'K';
static const uint8_t ITEM_MAGIC_NUM = 'I';

/**
 * The function to be invoked while polling the used block
 */
typedef int (*item_func)(kv_item_t *item, const char *key);

#define KV_ALIGN(x) ((x + ~KV_ALIGN_MASK) & KV_ALIGN_MASK)

#ifdef __cplusplus
}
#endif

#endif
