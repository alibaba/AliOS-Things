/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HOBBIT_PARTITION_H
#define HOBBIT_PARTITION_H

#define PART_ADDR_BOOTLOADER_MTB        (0x10000000)
#define PART_ADDR_END_BOOTLOADER_MTB    (0x100001FF)
#define PART_SIZE_BOOTLOADER_MTB        (PART_ADDR_END_BOOTLOADER_MTB - PART_ADDR_BOOTLOADER_MTB + 1)

#define PART_ADDR_BOOTLOADER            (0x10000200)
#define PART_ADDR_END_BOOTLOADER        (0x100057FF)
#define PART_SIZE_BOOTLOADER            (PART_ADDR_END_BOOTLOADER - PART_ADDR_BOOTLOADER + 1)

#define PART_ADDR_TEE                   (0x10005800)
#define PART_ADDR_END_TEE               (0x1000A7FF)
#define PART_SIZE_TEE                   (PART_ADDR_END_TEE - PART_ADDR_TEE + 1)

#define PART_ADDR_FACTORYSETTINGS       (0x1000A800)
#define PART_ADDR_END_FACTORYSETTINGS   (0x1000ABFF)
#define PART_SIZE_FACTORYSETTINGS       (PART_ADDR_END_FACTORYSETTINGS - PART_ADDR_FACTORYSETTINGS + 1)

#define PART_ADDR_IMAGES_MTB            (0x1000AC00)
#define PART_ADDR_END_IMAGES_MTB        (0x1000BBFF)
#define PART_SIZE_IMAGES_MTB            (PART_ADDR_END_IMAGES_MTB - PART_ADDR_IMAGES_MTB + 1)

#define PART_ADDR_REE                   (0x1000BC00)
#define PART_ADDR_END_REE               (0x100313FF)
#define PART_SIZE_REE                   (PART_ADDR_END_REE - PART_ADDR_REE + 1)

#define PART_ADDR_FOTA                  (0x10031400)
#define PART_ADDR_END_FOTA              (0x1003E7FF)
#define PART_SIZE_FOTA                  (PART_ADDR_END_FOTA - PART_ADDR_FOTA + 1)

#define PART_ADDR_KV                    (0x1003E800)
#define PART_ADDR_END_KV                (0x1003EFFF)
#define PART_SIZE_KV                    (PART_ADDR_END_KV - PART_ADDR_KV + 1)

#define PART_ADDR_OTP                   (0x4003F800)
#define PART_ADDR_END_OTP               (0x4003FFFF)
#define PART_SIZE_OTP                   (PART_ADDR_END_OTP - PART_ADDR_OTP + 1)

#define LOWPOWEOR_ADDR                  (0x1003F7F0)

#endif /* HOBBIT_PARTITION_H */
