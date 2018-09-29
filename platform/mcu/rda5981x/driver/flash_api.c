/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
 *
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

#include "flash_api.h"
#include "flash_data.h"
//#include "mbed_critical.h"
#include "critical.h"

// This file is automagically generated

// This is a flash algo binary blob. It is PIC (position independent code) that should be stored in RAM
static uint32_t FLASH_ALGO[] = {
    0x4df0e92d, 0x4180f04f, 0x68082400, 0xea322203, 0xbf1c62d0, 0x50c0f040, 0x49cc6008, 0xf0106808,
    0xd1100f40, 0x00c0f040, 0x03086008, 0x68486088, 0x0001f040, 0x20006048, 0x28081c40, 0x6848d3fc,
    0x0f01f010, 0x4dc2d1fb, 0x0835f04f, 0x8000f8c5, 0x1c402000, 0xd3fc2808, 0xf01068e8, 0xd1fb0f01,
    0xf0106928, 0xd17b0f02, 0x6c704eba, 0x007cf420, 0x1010f440, 0x68b06470, 0x2040f440, 0x209f60b0,
    0x20006028, 0x28081c40, 0x68e8d3fc, 0x0f01f010, 0x6928d1fb, 0x69286929, 0x2102b2ca, 0xf04f2705,
    0xf04f0a00, 0x2a400b01, 0xb2c0d123, 0xd9202815, 0xf36f6868, 0xf4402010, 0x60687080, 0xf00060a9,
    0x2031f92b, 0x20006028, 0x28081c40, 0x68e8d3fc, 0x0f01f010, 0x602fd1fb, 0x1c402000, 0xd3fc2808,
    0xf01068e8, 0xd1fb0f01, 0xf0106928, 0xd1f20f01, 0x6868e021, 0x2010f36f, 0x7000f440, 0xf8c56068,
    0x60a9a008, 0xf908f000, 0xb000f8c5, 0x1c402000, 0xd3fc2808, 0xf01068e8, 0xd1fb0f01, 0x2000602f,
    0x28081c40, 0x68e8d3fc, 0x0f01f010, 0x6928d1fb, 0x0f01f010, 0x4a88d1f2, 0x444a2100, 0x8000f8c5,
    0x1c402000, 0xd3fc2808, 0xf01068e8, 0xd1fb0f01, 0x1c496928, 0x6f80f5b1, 0x2401d303, 0xa000f882,
    0xf010e00d, 0xd0e90f02, 0x6968b94c, 0x0001f040, 0x6c706168, 0x00d8f440, 0xf8826470, 0x4620b000,
    0x8df0e8bd, 0x47702000, 0x47702000, 0xf000b510, 0x496ff8c3, 0x60082060, 0x1c402000, 0xd3fc2808,
    0xf01068c8, 0xd1fb0f01, 0x600a2205, 0x1c402000, 0xd3fc2808, 0xf01068c8, 0xd1fb0f01, 0xf0106908,
    0xd1f20f01, 0x68484961, 0x0001f040, 0x20006048, 0x28081c40, 0x6848d3fc, 0x0001f010, 0xbd10d1fb,
    0x4604b510, 0xf898f000, 0x20204959, 0x2004ea40, 0x20006008, 0x28081c40, 0x68c8d3fc, 0x0f01f010,
    0x2205d1fb, 0x2000600a, 0x28081c40, 0x68c8d3fc, 0x0f01f010, 0x6908d1fb, 0x0f01f010, 0x494bd1f2,
    0xf0406848, 0x60480001, 0x1c402000, 0xd3fc2808, 0xf0106848, 0xd1fb0001, 0xe92dbd10, 0xf02005f0,
    0x4845437c, 0x78004448, 0xbf142800, 0x24022432, 0x2c11ea5f, 0x68684d3e, 0x2010f36f, 0x3080f440,
    0xf04f6068, 0xbf1c0100, 0x26052706, 0xf04fd042, 0x20000800, 0xa000f812, 0xa008f8c5, 0x28041c40,
    0x1d12dbf8, 0x0801f108, 0x0f40f1b8, 0x602fdbf1, 0x1c402000, 0xd3fc2808, 0xf01068e8, 0xd1fb0f01,
    0x2000602e, 0x28081c40, 0x68e8d3fc, 0x0f01f010, 0x6928d1fb, 0x0f02f010, 0xea44d0f2, 0x60282003,
    0xf5032000, 0x1c407380, 0xd3fc2808, 0xf01068e8, 0xd1fb0f01, 0x2000602e, 0x28081c40, 0x68e8d3fc,
    0x0f01f010, 0x6928d1fb, 0x0f01f010, 0x1c49d1f2, 0xd3bc4561, 0x68484915, 0x0001f040, 0x20006048,
    0x28081c40, 0x6848d3fc, 0x0001f010, 0xe8bdd1fb, 0x477005f0, 0x47704408, 0x2006490d, 0x20006008,
    0x28081c40, 0x68c8d3fc, 0x0f01f010, 0x2205d1fb, 0x2000600a, 0x28081c40, 0x68c8d3fc, 0x0f01f010,
    0x6908d1fb, 0x0f02f010, 0x4770d0f2, 0x40014000, 0x17fff000, 0x40001000, 0x00000004, 0x00000000,
    0x00000000
};

static const flash_algo_t flash_algo_config = {
    .init = 0x1,
    .uninit = 0x185,
    .erase_sector = 0x1e1,
    .program_page = 0x23b,
    .static_base = 0x35c,
    .algo_blob = FLASH_ALGO
};

static const sector_info_t sectors_info[] = {
    {0x18000000, 0x1000},
};

static const flash_target_config_t flash_target_config = {
    .page_size  = 0x100,
    .flash_start = 0x18000000,
    .flash_size = 0x400000,
    .sectors = sectors_info,
    .sector_info_count = sizeof(sectors_info) / sizeof(sector_info_t)
};

void flash_set_target_config(flash_t *obj)
{
    obj->flash_algo = &flash_algo_config;
    obj->target_config = &flash_target_config;
}
