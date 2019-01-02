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

#ifndef __GPU_UTILITY_H__
#define __GPU_UTILITY_H__

#include "soc_memory_map.h"
#include "ipu_reg_def.h"

#define REG_BASE GPU_3D_BASE_ADDR
#define DEBUG_REG_BASE GPU_3D_BASE_ADDR

extern int width;
extern int height;
extern int pixel_format;        //2 - 16 bit; 4 - 32 bit
extern int lcd_pixel_format;    //2 - 16 bit; 4 - 32 bit
extern int depth_format;        //2 - 16 bit; 4 - 32 bit

extern int SAVE_BMP, ENABLEZC, HZ;
extern int USE_FAST_CLEAR;

typedef union {
    float f;
    unsigned int u;
} floatint;

extern float f;

extern floatint new_width;
extern floatint new_height;
extern floatint new_half_width;
extern floatint new_half_height;
extern floatint neg_new_half_height;

extern volatile int gpu_new_frame;

unsigned long ReadReg(unsigned int addr);
void WriteReg(unsigned int addr, unsigned long data);
void Poke(unsigned long addr, unsigned long data);
void clearFrameBufferMSAA(unsigned long phys);
void clearFrameBuffer(unsigned long phys, unsigned long colour);
void clearBuffers(unsigned long phys, int use2DClear);
void fastClear(unsigned long phys, unsigned long colour);
void disableFastClear(unsigned long phys);
void fastClearMSAA(unsigned long phys);
void start(unsigned long phys);
void DoResolve(unsigned long phys,
               unsigned long SrcAddress,
               unsigned long DestAddress,
               unsigned long DepthAddress,
               int Width,
               int Height,
               int SrcColorBpp,
               int DestColorBpp,
               int DepthBpp,
               int SrcTiled,
               int DepthTiled,
               int DestTiled,
               int DetectThresh,
               int FilterOffFrontZ,
               int EdgeDetect, int SuperSample, int SrcSuperTile, int DstSuperTile, int sRGB_en);
void CallResolve(unsigned long phys, int SuperSample, int SrcSuperTile, int DstSuperTile);
void clearTWOD(unsigned long phys, unsigned long target_address, unsigned long target_data);

#endif
