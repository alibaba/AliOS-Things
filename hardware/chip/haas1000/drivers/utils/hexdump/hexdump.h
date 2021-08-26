/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

void dump_buffer(const void *buf, size_t len);

void dump_buffer_imm(const void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif

