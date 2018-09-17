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

/*!
 * @file sdk_gpu_utilities.c
 * @brief Driver of the i.MX GPU.
 *
 * @ingroup diag_enet
 */

#include "sdk_gpu_utilities.h"
#include "gpu_test_common.h"

//#define DOUBLE_BUFFER
//#define USE_FAST_CLEAR 1
//#define MULTIPLE_BUFFER
//#define DEBUG_MODE

int frame_no = 0;
extern volatile int gpu_new_frame;
extern void ipu_channel_buf_ready(int32_t ipu_index, int32_t channel, int32_t buf);

/********* UTILITY FUNCTIONS *********/
unsigned long ReadReg(unsigned int addr)
{
    return *(unsigned long *)(REG_BASE + (addr << 2));
}

void WriteReg(unsigned int addr, unsigned long data)
{
    *(unsigned long *)(REG_BASE + (addr << 2)) = data;
}

void WriteDebugReg(unsigned int offset, unsigned long data)
{
    *(unsigned long *)(DEBUG_REG_BASE + (offset)) = data;
}

void Poke(unsigned long addr, unsigned long data)
{
    *(unsigned long *)addr = data;
}

unsigned long ReadMem(unsigned long addr)
{
    return *(unsigned long *)addr;
}

/********* Resolve CLEAR *********/
void clearTWOD(unsigned long phys, unsigned long target_address, unsigned long target_data)
{
    unsigned long write_addr = phys;

    //Switch to 2D
    Poke(write_addr, 0x8010E00);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    //set the mask
    Poke(write_addr, 0x8010581);
    write_addr += 4;

    if ((target_address == DEPTH_BUFFER_ADDR) && (depth_format == 2)) {
        Poke(write_addr, 0x0404);
        write_addr += 4;

    } else if ((target_address == FRAME_BUFFER_ADDR) && (pixel_format == 2)) {
        Poke(write_addr, 0x0404);
        write_addr += 4;

    } else {
        Poke(write_addr, 0x0606);
        write_addr += 4;
    }
    //set the target data - fist half

    Poke(write_addr, 0x8010590);
    write_addr += 4;

    Poke(write_addr, target_data);
    write_addr += 4;

    Poke(write_addr, 0x080205b8);
    write_addr += 4;

    Poke(write_addr, target_address);
    write_addr += 4;

    Poke(write_addr, target_address + width * 4 * ((height / 2 + 63) & (~63)));
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x080205c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, ((height / 2 + 63) & (~63)) << 16);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;

    //set the target address
    Poke(write_addr, 0x8010584);
    write_addr += 4;

    Poke(write_addr, target_address);
    write_addr += 4;

    //set the stride
    Poke(write_addr, 0x8010585);
    write_addr += 4;

    if ((target_address == DEPTH_BUFFER_ADDR)) {
        Poke(write_addr, ((width + 63) & (~63)) * depth_format);
        write_addr += 4;
    } else if ((target_address == FRAME_BUFFER_ADDR)) {
        Poke(write_addr, ((width + 63) & (~63)) * pixel_format);
        write_addr += 4;
    } else {
        Poke(write_addr, width * depth_format);
    }
    //set rotation
    Poke(write_addr, 0x801058c);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x801058d);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x801058f);
    write_addr += 4;

    Poke(write_addr, 0x1000f);
    write_addr += 4;
    //set the clipping plane Bottom/Left

    Poke(write_addr, 0x8010588);
    write_addr += 4;

    Poke(write_addr, (((height / 2 + 63) & (~63)) << 16) | ((width + 63) & (~63)));
    write_addr += 4;

    //set the ROP   
    Poke(write_addr, 0x8010580);
    write_addr += 4;

    Poke(write_addr, 0x1);
    write_addr += 4;

    //Semaphore and stall
    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;
    Poke(write_addr, 0x801058f);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;
    //CMDEND

    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void clearTWODMSAA(unsigned long phys, unsigned long target_address, unsigned long target_data)
{
    unsigned long write_addr = phys;

    //Switch to 2D
    Poke(write_addr, 0x8010E00);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    //set the mask
    Poke(write_addr, 0x8010581);
    write_addr += 4;

    if ((target_address == DEPTH_BUFFER_ADDR) && (depth_format == 2)) {
        Poke(write_addr, 0x0404);
        write_addr += 4;
    } else if ((target_address == FRAME_BUFFER_ADDR) && (pixel_format == 2)) {
        Poke(write_addr, 0x0404);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x0606);
        write_addr += 4;
    }

    Poke(write_addr, 0x8010590);
    write_addr += 4;

    Poke(write_addr, target_data);
    write_addr += 4;

    Poke(write_addr, 0x080205b8);
    write_addr += 4;

    Poke(write_addr, target_address);
    write_addr += 4;

    Poke(write_addr, target_address + ((width + 63) & (~63)) * 4 * ((height + 63) & (~63)) * 2);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x080205c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, ((height + 63) & (~63)) << 16);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;

    //set the target address
    Poke(write_addr, 0x8010584);
    write_addr += 4;

    Poke(write_addr, target_address);
    write_addr += 4;

    //set the stride
    Poke(write_addr, 0x8010585);
    write_addr += 4;

    if ((target_address == DEPTH_BUFFER_ADDR)) {
        Poke(write_addr, ((width + 63) & (~63)) * depth_format * 2);
        write_addr += 4;
    } else if ((target_address == FRAME_BUFFER_ADDR)) {
        Poke(write_addr, ((width + 63) & (~63)) * pixel_format * 2);
        write_addr += 4;
    } else {
        Poke(write_addr, width * depth_format * 2);
        write_addr += 4;
    }
    //set rotation
    Poke(write_addr, 0x801058c);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x801058d);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;

    Poke(write_addr, 0x801058f);
    write_addr += 4;

    Poke(write_addr, 0x1000f);
    write_addr += 4;

    //set the clipping plane Bottom/Left
    Poke(write_addr, 0x8010588);
    write_addr += 4;

    Poke(write_addr, ((((height + 63) & (~63))) << 16) | (((width + 63) & (~63)) * 2));
    write_addr += 4;

    //set the ROP   
    Poke(write_addr, 0x8010580);
    write_addr += 4;

    Poke(write_addr, 0x1);
    write_addr += 4;

    //Semaphore and stall
    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND
    Poke(write_addr, 0x801058f);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

/********* CLEAR BUFFERS *********/

void clearFrameBuffer(unsigned long phys, unsigned long colour)
{
    unsigned long write_addr = phys;

    if (USE_FAST_CLEAR) {
        fastClear(phys, colour);
    } else {
        clearTWOD(phys, FRAME_BUFFER_ADDR, 0xdeadbeef);

        clearTWOD(phys, DEPTH_BUFFER_ADDR, 0xffffffff);

        clearTWOD(phys, HZ_BUFFER_ADDR, 0xffffffff);
    }

    //Switch to 3D
    Poke(write_addr, 0x8010E00);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    Poke(write_addr, 0x8010380);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;
    //Semaphore and stall

    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void clearFrameBufferMSAA(unsigned long phys)
{
    unsigned long write_addr = phys;

    if (USE_FAST_CLEAR) {
        fastClearMSAA(phys);
    } else {
        clearTWODMSAA(phys, FRAME_BUFFER_ADDR, 0xdeadbeef);

        clearTWODMSAA(phys, DEPTH_BUFFER_ADDR, 0xffffffff);

        clearTWODMSAA(phys, HZ_BUFFER_ADDR, 0xffffffff);
    }

    //Switch to 3D
    Poke(write_addr, 0x8010E00);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    //Semaphore and stall
    Poke(write_addr, 0x8010380);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;
    //Semaphore and stall
    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void clearBuffers(unsigned long phys, int clearMode)
{
    int i;

    if (USE_FAST_CLEAR) {
        printf("Fast Clearing\n");

        fastClear(phys, 0xdeadbeef);
    } else {
        if (clearMode == 0) {
            printf("Direct Clearing\n");

            for (i = 0; i < 0x50000; i++) {
                Poke(DEPTH_BUFFER_ADDR + i * 4, 0xffffffff);

                Poke(FRAME_BUFFER_ADDR + i * 4, 0xdeadbeef);
            }
        } else if (clearMode == 1) {
            unsigned long write_addr = phys;

            clearTWOD(phys, FRAME_BUFFER_ADDR, 0xdeadbeef);

            clearTWOD(phys, DEPTH_BUFFER_ADDR, 0xffffffff);
            clearTWOD(phys, HZ_BUFFER_ADDR, 0xffffffff);

            printf("2D Clearing\n");

            //Switch to 3D
            Poke(write_addr, 0x8010E00);
            write_addr += 4;

            Poke(write_addr, 0x0);
            write_addr += 4;

            //Semaphore and stall
            Poke(write_addr, 0x08010E02);
            write_addr += 4;

            Poke(write_addr, 0x00000701);
            write_addr += 4;

            Poke(write_addr, 0x48000000);
            write_addr += 4;

            Poke(write_addr, 0x00000701);
            write_addr += 4;

            //CMDEND
            Poke(write_addr, 0x10000000);
            write_addr += 4;

            Poke(write_addr, 0x0);
            write_addr += 4;

            start(phys);
        }
    }
}

/************* START *************/

void fastClear(unsigned long phys, unsigned long colour)
{
    unsigned long write_addr = phys;

    int i = 0;

    for (i = 0; i < (((width + 63) & (~63)) * ((height + 63) & (~63)) / 16) * 2 / 8; i++) {

        Poke(TILE_STATUS_BUFFERC_ADDR + (i << 2), 0x55555555);

        Poke(TILE_STATUS_BUFFERZ_ADDR + (i << 2), 0x55555555);
        Poke(TILE_STATUS_BUFFERHZ_ADDR + (i << 2), 0x55555555);
    }

    Poke(write_addr, 0x8010595);
    write_addr += 4;

    if (ENABLEZC) {
        if ((pixel_format == 2) & (depth_format == 2)) {
            Poke(write_addr, 0x4f);
            write_addr += 4;    // Enable color and depth fast clear.
        } else if (pixel_format == 2) {
            Poke(write_addr, 0x47);
            write_addr += 4;
        } else if (depth_format == 2) {
            Poke(write_addr, 0x4b);
            write_addr += 4;
        } else {
            Poke(write_addr, 0x43);
            write_addr += 4;
        }
    } else {
        if ((pixel_format == 2) & (depth_format == 2)) {
            Poke(write_addr, 0xf);
            write_addr += 4;    // Enable color and depth fast clear.
        } else if (pixel_format == 2) {
            Poke(write_addr, 0x7);
            write_addr += 4;
        } else if (depth_format == 2) {
            Poke(write_addr, 0xb);
            write_addr += 4;
        } else {
            Poke(write_addr, 0x3);
            write_addr += 4;
        }
    }

    Poke(write_addr, 0x8010596);
    write_addr += 4;

    Poke(write_addr, TILE_STATUS_BUFFERC_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x8010597);
    write_addr += 4;

    Poke(write_addr, FRAME_BUFFER_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x8010598);
    write_addr += 4;

    Poke(write_addr, colour);
    write_addr += 4;

    Poke(write_addr, 0x8010599);
    write_addr += 4;

    Poke(write_addr, TILE_STATUS_BUFFERZ_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x801059A);
    write_addr += 4;

    Poke(write_addr, DEPTH_BUFFER_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x801059B);
    write_addr += 4;

    Poke(write_addr, 0xFFFFFFFF);
    write_addr += 4;

    if (HZ) {
        Poke(write_addr, 0x8010595);
        write_addr += 4;
        if (ENABLEZC) {
            if ((pixel_format == 2) & (depth_format == 2)) {
                Poke(write_addr, 0x104f);
                write_addr += 4;    // Enable color and depth fast clear.
            } else if (pixel_format == 2) {
                Poke(write_addr, 0x1047);
                write_addr += 4;
            } else if (depth_format == 2) {
                Poke(write_addr, 0x104b);
                write_addr += 4;
            } else {
                Poke(write_addr, 0x1043);
                write_addr += 4;
            }
        } else {
            if ((pixel_format == 2) & (depth_format == 2)) {
                Poke(write_addr, 0x100f);
                write_addr += 4;    // Enable color and depth fast clear.
            } else if (pixel_format == 2) {
                Poke(write_addr, 0x1007);
                write_addr += 4;
            } else if (depth_format == 2) {
                Poke(write_addr, 0x100b);
                write_addr += 4;
            } else {
                Poke(write_addr, 0x1003);
                write_addr += 4;
            }
        }

        Poke(write_addr, 0x80105A9);
        write_addr += 4;

        Poke(write_addr, TILE_STATUS_BUFFERHZ_ADDR);
        write_addr += 4;

        Poke(write_addr, 0x8010516);
        write_addr += 4;

        Poke(write_addr, HZ_BUFFER_ADDR);
        write_addr += 4;

        Poke(write_addr, 0x80105AA);
        write_addr += 4;

        // Poke(write_addr, (HZ == 2)?((depth_format == 2)? 0x5 : 0x7 ) :((depth_format == 2)? 0x9 : 0xB)); write_addr+=4;
        Poke(write_addr, 0xFFFFFFFF);
        write_addr += 4;
    }

    Poke(write_addr, 0x8010851);
    write_addr += 4;

    Poke(write_addr, 0x1);
    write_addr += 4;

    Poke(write_addr, 0x8010594);
    write_addr += 4;

    Poke(write_addr, 0x71);
    write_addr += 4;

    // Semaphore and stall until idle
    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void fastClearMSAA(unsigned long phys)
{
    unsigned long write_addr = phys;
    int i = 0;

    for (i = 0; i < (((width + 63) & (~63)) * ((height + 63) & (~63)) * 2 * 2 / 16) * 2 / 8 / 4;
         i++) {
        Poke(TILE_STATUS_BUFFERC_ADDR + (i << 2), 0x55555555);
        Poke(TILE_STATUS_BUFFERZ_ADDR + (i << 2), 0x55555555);
        if (HZ) {
            Poke(TILE_STATUS_BUFFERHZ_ADDR + (i << 2), 0x55555555);
        }

    }

    Poke(write_addr, 0x8010595);
    write_addr += 4;
    if (ENABLEZC) {
        if ((pixel_format == 2) & (depth_format == 2)) {
            Poke(write_addr, 0x104f);
            write_addr += 4;    // Enable color and depth fast clear.
        } else if (pixel_format == 2) {
            Poke(write_addr, 0x1047);
            write_addr += 4;
        } else if (depth_format == 2) {
            Poke(write_addr, 0x104b);
            write_addr += 4;
        } else {
            Poke(write_addr, 0x1043);
            write_addr += 4;
        }
    } else {
        if ((pixel_format == 2) & (depth_format == 2)) {
            Poke(write_addr, 0x100f);
            write_addr += 4;    // Enable color and depth fast clear.
        } else if (pixel_format == 2) {
            Poke(write_addr, 0x1007);
            write_addr += 4;
        } else if (depth_format == 2) {
            Poke(write_addr, 0x100b);
            write_addr += 4;
        } else {
            Poke(write_addr, 0x1003);
            write_addr += 4;
        }
    }

    if (HZ) {
        Poke(write_addr, 0x08010500);
        write_addr += 4;

        Poke(write_addr, 0xFFFCFFFF);
        write_addr += 4;

        Poke(write_addr, 0x80105A9);
        write_addr += 4;

        Poke(write_addr, TILE_STATUS_BUFFERHZ_ADDR);
        write_addr += 4;

        Poke(write_addr, 0x8010516);
        write_addr += 4;

        Poke(write_addr, HZ_BUFFER_ADDR);
        write_addr += 4;

        Poke(write_addr, 0x80105AA);
        write_addr += 4;

        // Poke(write_addr, (HZ == 2)?((depth_format == 2)? 0x5 : 0x7 ) :((depth_format == 2)? 0x9 : 0xB)); write_addr+=4;
        Poke(write_addr, 0xFFFFFFFF);
        write_addr += 4;
    }

    Poke(write_addr, 0x8010596);
    write_addr += 4;

    Poke(write_addr, TILE_STATUS_BUFFERC_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x8010597);
    write_addr += 4;

    Poke(write_addr, FRAME_BUFFER_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x8010598);
    write_addr += 4;

    Poke(write_addr, 0xDEADBEEF);
    write_addr += 4;

    Poke(write_addr, 0x8010599);
    write_addr += 4;

    Poke(write_addr, TILE_STATUS_BUFFERZ_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x801059A);
    write_addr += 4;

    Poke(write_addr, DEPTH_BUFFER_ADDR);
    write_addr += 4;

    Poke(write_addr, 0x801059B);
    write_addr += 4;

    Poke(write_addr, 0xFFFFFFFF);
    write_addr += 4;

    Poke(write_addr, 0x8010851);
    write_addr += 4;

    Poke(write_addr, 0x1);
    write_addr += 4;
    Poke(write_addr, 0x8010594);
    write_addr += 4;

    Poke(write_addr, 0x71);
    write_addr += 4;

    // Semaphore and stall until idle
    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void disableFastClear(unsigned long phys)
{

    unsigned long write_addr = phys;

    Poke(write_addr, 0x8010595);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);
}

void start(unsigned long phys)
{
    unsigned long i;
    int idle;
    unsigned long write_addr = phys;

    WriteReg(0x195, phys);      //16'h0153
    WriteReg(0x196, 0xffffffff);    //16'h0154

    for (i = 0; i < 1000000; i++) {
        idle = ReadReg(0x1);
        if (!(idle ^ 0x7FFFFFFF))
            break;
    }

    Poke(write_addr, 0x8010594);
    write_addr += 4;

    Poke(write_addr, 0x71);
    write_addr += 4;
    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    WriteReg(0x195, phys);      //16'h0153

    WriteReg(0x196, 0xffffffff);    //16'h0154

    for (i = 0; i < 1000000; i++) {
        idle = ReadReg(0x1);
        if (!(idle ^ 0x7FFFFFFF))
            break;
    }
}

/*********** DO RESOLVE **********/

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
               int EdgeDetect, int SuperSample, int SrcSuperTile, int DstSuperTile, int sRGB_en)
{
    unsigned long write_addr = phys;

    unsigned long config_value;

    int srcFormat = (SrcColorBpp == 2) ? 0x4 : 0x6;

    int dstFormat = sRGB_en ? ((DestColorBpp == 2) ? 0x4 : 0x5) : (DestColorBpp == 2) ? 0x4 : 0x6;

    config_value = (0 << 28) |  //AQ_RS_CONFIG_RS_SRC_DEPTH_TILE_ENABLE = DepthTiled
        (0 << 22) |             //AQ_RS_CONFIG_DEPTH_FORMAT = Z24
        (0 << 21) |             //AQ_RS_CONFIG_RS_EDGE_DETECT_CMETHOD = DISABLE
        (0 << 20) |             //AQ_RS_CONFIG_RS_EDGE_DETECT_SMETHOD = DISABLE
        (0 << 18) |             //AQ_RS_CONFIG_RS_EDGE_DETECT_LINE_CHECK = DISABLE
        (0 << 17) |             //AQ_RS_CONFIG_RS_EDGE_DETECT_ZMETHOD = DIFF2
        (0 << 16) |             //AQ_RS_CONFIG_RS_EDGE_DETECT_ENABLE = EdgeDetect
        (DestTiled << 14) |     //AQ_RS_CONFIG_RS_EDGE_DETECT_ENABLE = DestTiled
        (dstFormat << 8) |      //AQ_RS_CONFIG_RS_DEST_FORMAT = A8R8G8B8
        (SrcTiled << 7) |       //AQ_RS_CONFIG_RS_SRC_TILE_ENABLE = SrcTiled
        (SuperSample << 5) |    //Super Sample = DISABLE
        (srcFormat);            //AQ_RS_CONFIG_RS_SRC_FORMAT = A8R8G8B8

    if (sRGB_en) {
        config_value = config_value | (1 << 13);
    }
    //Switch to 3D

    Poke(write_addr, 0x8010E00);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    //Dummy Write

    Poke(write_addr, 0xffff);
    write_addr += 4;

    Poke(write_addr, 0);
    write_addr += 4;

    //Src Address

    Poke(write_addr, 0x8010582);
    write_addr += 4;

    Poke(write_addr, SrcAddress);
    write_addr += 4;

    //Src Stride

    Poke(write_addr, 0x8010583);
    write_addr += 4;

    if (SrcSuperTile) {
        Poke(write_addr,
             ((Width + 63) & (~63)) * SrcColorBpp * (SrcTiled ? 4 : 1) * (SuperSample ==
                                                                          0 ? 1 : 2) | SrcSuperTile
             << 31 | 1 << 30);
        write_addr += 4;
    }

    else {
        Poke(write_addr,
             Width * SrcColorBpp * (SrcTiled ? 4 : 1) * (SuperSample ==
                                                         0 ? 1 : 2) | SrcSuperTile << 31 | 1 << 30);
        write_addr += 4;

    }

    //Dest Address

    Poke(write_addr, 0x8010584);
    write_addr += 4;

    Poke(write_addr, DestAddress);
    write_addr += 4;

    Poke(write_addr, 0x080205b0);
    write_addr += 4;

    Poke(write_addr, SrcAddress + 0x0);
    write_addr += 4;

    Poke(write_addr,
         SrcAddress + ((Width + 63) & (~63)) * SrcColorBpp * (SuperSample ==
                                                              3 ? 2 : 1) * ((Height / 2 *
                                                                             (SuperSample ==
                                                                              3 ? 2 : 1) +
                                                                             63) & (~63)));
    write_addr += 4;

    Poke(write_addr, 0xffffffff);
    write_addr += 4;

    Poke(write_addr, 0x080205b8);
    write_addr += 4;

    Poke(write_addr, DestAddress);
    write_addr += 4;

    Poke(write_addr, DestAddress + Width * DestColorBpp * ((Height / 2 + 63) & (~63)));
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x080205c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, (((Height / 2 * (SuperSample == 3 ? 2 : 1) + 63)) & (~63)) << 16);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;

    //Dest Stride

    Poke(write_addr, 0x8010585);
    write_addr += 4;

    //Poke(write_addr, Width*DestColorBpp*(DestTiled?4:1)); write_addr += 4;

    Poke(write_addr, Width * DestColorBpp * (DestTiled ? 4 : 1) | DstSuperTile << 31);
    write_addr += 4;
    //Depth Address
    // Width and Height

    Poke(write_addr, 0x8010588);
    write_addr += 4;

    Poke(write_addr,
         (((Height / 2 * (SuperSample == 3 ? 2 : 1) + 63) & (~63)) << 16) | (Width * (SuperSample ==
                                                                                      3 ? 2 : 1)));
    write_addr += 4;

    //Threshold
    // Dither Table

    Poke(write_addr, 0x801058c);
    write_addr += 4;

    //Poke(write_addr, 0xffffffff); write_addr += 4;

    Poke(write_addr, 0x6E4CA280);
    write_addr += 4;

    // Dither Table 2

    Poke(write_addr, 0x801058d);
    write_addr += 4;

    //Poke(write_addr, 0xffffffff); write_addr += 4;

    Poke(write_addr, 0x5D7F91B3);
    write_addr += 4;
    //Poke(write_addr, 0x801058e); write_addr += 4;

    //Poke(write_addr, 0xffffffff); write_addr += 4;

    //Poke(write_addr, 0x4); write_addr += 4;

    // Config

    Poke(write_addr, 0x8010581);
    write_addr += 4;

    Poke(write_addr, config_value);
    write_addr += 4;

    //Poke(write_addr, 0x801058f); write_addr += 4;

    Poke(write_addr, 0x8010590);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    //Trigger

    Poke(write_addr, 0x8010580);
    write_addr += 4;

    Poke(write_addr, 0xbeebbeeb);
    write_addr += 4;

    // Semaphore and stall until idle

    Poke(write_addr, 0x08010E02);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x48000000);
    write_addr += 4;

    Poke(write_addr, 0x00000701);
    write_addr += 4;

    //CMDEND

    Poke(write_addr, 0x10000000);
    write_addr += 4;

    Poke(write_addr, 0x0);
    write_addr += 4;

    start(phys);

}

/*********** CALL RESOLVE **********/

void CallResolve(unsigned long phys, int SuperSample, int SrcSuperTile, int DstSuperTile)
{

    static unsigned long lcd_buffer_addr = LCD0_FRAME_BUFFER_ADDR;

    if (frame_no == 0)

        lcd_buffer_addr = LCD0_FRAME_BUFFER_ADDR;
    else {

#ifdef DOUBLE_BUFFER

        if (lcd_buffer_addr == LCD0_FRAME_BUFFER_ADDR) {

            lcd_buffer_addr = LCD1_FRAME_BUFFER_ADDR;

        }

        else {

            lcd_buffer_addr = LCD0_FRAME_BUFFER_ADDR;

        }
#endif

    }

#ifdef MULTIPLE_BUFFER

    if (lcd_buffer_addr == LCD0_FRAME_BUFFER_ADDR) {
        lcd_buffer_addr = LCD1_FRAME_BUFFER_ADDR;
    } else {
        lcd_buffer_addr -= width * height * lcd_pixel_format;
    }

#endif

    //printf("Rendering Frame no: %d at address 0x%x\n",frame_no,lcd_buffer_addr);

    {
        unsigned long src_addr = FRAME_BUFFER_ADDR;
        unsigned long dest_addr = lcd_buffer_addr;
        unsigned long depth_addr = DEPTH_BUFFER_ADDR;

#ifdef USE_FAST_CLEAR
        {
            int loop = height / 64;
            int rest_loop = height - (64 * loop);
            for (i = 0; i < loop; i++) {
                DoResolve(phys, //Cmd Buffer address      
                          src_addr, //SrcAddress,        
                          dest_addr,    //DestAddress,
                          depth_addr,   //DepthAddress,
                          width,    //Width,
                          64,   //Height, 
                          pixel_format, //SrcColorBpp, 
                          lcd_pixel_format, //DestColorBpp,
                          depth_format, //DepthBpp, 
                          1,    //SrcTiled,
                          1,    //DepthTiled,
                          0,    //DestTiled, 
                          5,    //DetectThresh,
                          0,    //FilterOffFrontZ,
                          0,    //EdgeDetect5
                          0,    //SuperSample
                          SrcSuperTile, DstSuperTile, 0);
                src_addr = src_addr + width * pixel_format * 64;
                dest_addr = dest_addr + width * lcd_pixel_format * 64;
                depth_addr = depth_addr + width * depth_format * 64;
            }
            DoResolve(phys,     //Cmd Buffer address      
                      src_addr, //SrcAddress,        
                      dest_addr,    //DestAddress,
                      depth_addr,   //DepthAddress,
                      width,    //Width,
                      rest_loop,    //Height, 
                      pixel_format, //SrcColorBpp, 
                      lcd_pixel_format, //DestColorBpp,
                      depth_format, //DepthBpp, 
                      1,        //SrcTiled,
                      1,        //DepthTiled,
                      0,        //DestTiled, 
                      5,        //DetectThresh,
                      0,        //FilterOffFrontZ,
                      0,        //EdgeDetect5
                      0,        //SuperSample
                      SrcSuperTile, DstSuperTile, 0);
        }
#else

        DoResolve(phys,         //Cmd Buffer address      
                  src_addr,     //SrcAddress,        
                  dest_addr,    //DestAddress,
                  depth_addr,   //DepthAddress,
                  width,        //Width,
                  height,       //Height, 
                  pixel_format, //SrcColorBpp, 
                  lcd_pixel_format, //DestColorBpp,
                  depth_format, //DepthBpp, 
                  1,            //SrcTiled,
                  1,            //DepthTiled,
                  0,            //DestTiled, 
                  5,            //DetectThresh,
                  0,            //FilterOffFrontZ,
                  0,            //EdgeDetect5
                  0,            //SuperSample
                  SrcSuperTile, DstSuperTile, 0);

#endif
    }

    frame_no++;

    //while(!gpu_new_frame){}
    gpu_new_frame = 0;

    //Update IPU buffer pointer HERE!!!
    if (lcd_buffer_addr == LCD0_FRAME_BUFFER_ADDR) {
		ipu_channel_buf_ready(2, 27, 0);
    } else {
		ipu_channel_buf_ready(2, 27, 0);
    }
}
