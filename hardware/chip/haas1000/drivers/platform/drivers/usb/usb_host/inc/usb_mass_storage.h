/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
