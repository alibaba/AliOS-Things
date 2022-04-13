/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __BOOT_RINGBUFFER_H__
#define __BOOT_RINGBUFFER_H__

#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	uint8_t *buffer_end;
	uint8_t *rd_ptr;
	uint8_t *wr_ptr;
	int32_t free_size;
	int32_t dirty_size;
} boot_ringbuff_t;

#endif
