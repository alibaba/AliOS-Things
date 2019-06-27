/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef NBPATCH_H_
#define NBPATCH_H_

int ota_nbpatch_main(void);
int ota_patch_read(unsigned long src, unsigned char *buffer, unsigned long pos, unsigned long size);
int ota_patch_write(unsigned long dst, unsigned char *buffer, unsigned long pos, unsigned long size);
int ota_patch_new_data(unsigned char *buf, long size);
int ota_load_new_data(unsigned long dst, long size);
unsigned short ota_patch_crc(void *addr, long len);
#endif /* NBPATCH_H_ */
