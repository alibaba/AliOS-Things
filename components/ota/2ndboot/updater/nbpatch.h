/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef NBPATCH_H
#define NBPATCH_H
#include "xz.h"

#define NBPATCH_VERSION  "3.1.0"

#define IN_BUF_MAX  1024
#define OUT_BUF_MAX 4096

#define DIFF_DICT_SIZE (1 << 14)
#if defined __LINUX_HOST__
#define DIFF_BLOCK_SIZE  (0x400000)
#else
#define DIFF_BLOCK_SIZE  (0x40000)
#endif
typedef struct xzReadHandler {
    unsigned char in_buf [IN_BUF_MAX];
    size_t avail_size;
    size_t read_pos;
} xzReadHandler;

long xz_read(xzReadHandler *handler, struct xz_buf *b,struct xz_dec *s, unsigned long patch, unsigned char *buf, int len);

int ota_nbpatch_main(void);
int ota_patch_show_percent(int per);
int ota_patch_read(unsigned long src, unsigned char *buffer, unsigned long pos, unsigned long size);
int ota_patch_write(unsigned long dst, unsigned char *buffer, unsigned long pos, unsigned long size);
int ota_patch_new_data(unsigned char *buf, long size);
int ota_load_new_data(unsigned long dst, long size);
unsigned short ota_patch_crc(void *addr, long len);
#endif /* NBPATCH_H_ */
