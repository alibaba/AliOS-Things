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
#ifndef _COREDUMP_SECTION_H
#define _COREDUMP_SECTION_H
#define COREDUMP_SECTOR_SIZE             0x1000
#define COREDUMP_SECTOR_SIZE_MASK        0xFFF
#define COREDUMP_BUFFER_LEN              (COREDUMP_SECTOR_SIZE*2)
#define COREDUMP_NORFALSH_BUFFER_LEN     (COREDUMP_BUFFER_LEN)
#if defined(__cplusplus)
extern "C" {
#endif

void coredump_to_flash_init();
void core_dump_erase_section();
int32_t core_dump_write_large(const uint8_t* ptr,uint32_t len);
int32_t core_dump_write(const uint8_t* ptr,uint32_t len);

#if defined(__cplusplus)
}
#endif

#endif

