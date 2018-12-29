/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_RINGBUF_H
#define K_MM_RINGBUF_H

#define RINGBUF_TYPE_FIX              0
#define RINGBUF_TYPE_DYN              1

#define RINGBUF_LEN_MAX_SIZE          3
#define RINGBUF_LEN_MASK_ONEBIT       0x80 //1000 0000
#define RINGBUF_LEN_MASK_TWOBIT       0xC0 //1100 0000
#define RINGBUF_LEN_MASK_CLEAN_TWOBIT 0x3f //0011 1111


#define RINGBUF_LEN_VLE_2BYTES        0x80 //1000 0000
#define RINGBUF_LEN_VLE_3BYTES        0xC0 //1100 0000

#define RINGBUF_LEN_1BYTE_MAXVALUE    0x7f //0111 1111; 127
#define RINGBUF_LEN_2BYTES_MAXVALUE   0x3fff // 16383
#define RINGBUF_LEN_3BYTES_MAXVALUE   0x3fffff //4194303


#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

typedef struct {
    uint8_t *buf;
    uint8_t *end;
    uint8_t *head;
    uint8_t *tail;
    size_t   freesize;
    size_t   type;
    size_t   blk_size;
} k_ringbuf_t;

#define COMPRESS_LEN(x) ((x) <= RINGBUF_LEN_1BYTE_MAXVALUE ? 1: (x) <= RINGBUF_LEN_2BYTES_MAXVALUE ? 2: \
                        (x) <= RINGBUF_LEN_3BYTES_MAXVALUE ? 3 : RHINO_INV_PARAM)

#endif
