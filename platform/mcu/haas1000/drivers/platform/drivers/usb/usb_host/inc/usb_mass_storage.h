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

void usb_mass_storage_set_read_ep(uint8_t ep);

void usb_mass_storage_set_write_ep(uint8_t ep);

void usb_mass_storage_set_itf_num(uint8_t num);

#endif // USB_MASS_STORAGE_H
