/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_COMMON_H__
#define __UVOICE_COMMON_H__


typedef enum {
    MEDIA_TYPE_UNKNOWN = 0,
    MEDIA_TYPE_FILE,
    MEDIA_TYPE_HTTP,
    MEDIA_TYPE_FLASH,
    MEDIA_TYPE_MEM,
    MEDIA_TYPE_STREAM,
    MEDIA_TYPE_COUNT,
} media_type_t;

typedef struct {
    int rate;
    int channels;
    int bits;
    int frames;
} media_pcminfo_t;

typedef struct {
    uint8_t *buffer;
    uint8_t *buffer_end;
    uint8_t *rd_ptr;
    uint8_t *wr_ptr;
    int32_t free_size;
    int32_t dirty_size;
} uvoice_ringbuff_t;


#endif /* __UVOICE_COMMON_H__ */
