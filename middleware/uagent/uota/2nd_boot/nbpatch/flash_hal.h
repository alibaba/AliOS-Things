/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef FLASH_HAL_H_
#define FLASH_HAL_H_

#include "rec_pub.h"

void patch_flash_init(const char *file);

int patch_flash_write(int par, const unsigned char *buffer, unsigned long address, unsigned long len);

int patch_flash_read(int par, const unsigned char *buffer, unsigned long address, unsigned long len);

int patch_flash_erase(int par, unsigned long address, size_t splict_size);

uint32_t patch_flash_get_partion_length(int par);

int patch_flash_copy(int par, unsigned long dst_offset, unsigned long src_offset, size_t size);

int patch_flash_copy_par(int dst_par, int src_par, unsigned long offset, size_t size);

#endif /* FLASH_HAL_H_ */
