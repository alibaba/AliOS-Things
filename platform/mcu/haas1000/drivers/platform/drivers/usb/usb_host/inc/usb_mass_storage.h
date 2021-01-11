/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef USB_MASS_STORAGE_H
#define USB_MASS_STORAGE_H

#include "plat_types.h"

#define USB_MASS_STORAGE_BLOCK_SIZE  512


int32_t usb_mass_storage_init(int32_t device);

int32_t usb_mass_storage_get_capacity(int32_t device,
                                      uint32_t* block_count,
                                      uint32_t* blockSize);

int32_t usb_mass_storage_read(int32_t device,
                                     uint32_t block_addr,
                                     uint32_t block_count,
                                     uint8_t* dst,
                                     u32 blockSize);

int32_t usb_mass_storage_write(int32_t device,
                                     uint32_t block_addr,
                                     uint32_t block_count,
                                     uint8_t* dst,
                                     u32 blockSize);

#endif // USB_MASS_STORAGE_H
