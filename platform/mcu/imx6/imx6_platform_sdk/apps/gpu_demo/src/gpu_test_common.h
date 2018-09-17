/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GPU_TEST_COMMON_H__
#define __GPU_TEST_COMMON_H__

#include "sdk.h"
#include "soc_memory_map.h"

typedef unsigned int WORD;

#define CMD_BUFFER_ADDR    0x16500000
#define IDX_BUFFER_ADDR    0x13000000

#define CALLCMD_BUFFER_ADDR    0x13300000
#define RETURNCMD_BUFFER_ADDR    0x13600000

#define VERTEX_BUFFER_ADDR 0x12200000
#define TEX_BUFFER_ADDR    0x14000000

#define DEPTH_BUFFER_ADDR  0x16000000
#define FRAME_BUFFER_ADDR  0x15000000

#define TILE_STATUS_BUFFERC_ADDR 0x17000000
#define TILE_STATUS_BUFFERZ_ADDR 0x17500000
#define TILE_STATUS_BUFFERHZ_ADDR 0x17a00000

#define HZ_BUFFER_ADDR  0x86800000

#define LCD0_FRAME_BUFFER_ADDR 0x18000000
#define LCD1_FRAME_BUFFER_ADDR 0x18800000
#define RESOLVE_BUFFER_ADDR 0x19000000

#endif
