/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
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
 * @file texture.c
 * @brief Texture fucntions..
 *
 * @ingroup diag_enet
 */

#include "gpu_test_common.h"
#include "sdk_gpu_utilities.h"
#include "texture5.h"

static void loadTexture(void)
{
    int i;
    for (i = 0; i < sizeof(tex5) / sizeof(tex5[0]); i++) {
        Poke(TEX_BUFFER_ADDR + (i << 2), tex5[i]);
    }
}

static void loadTex_vtx_idx(void)
{
    int i;
    for (i = 0; i < sizeof(vtxData_tex5) / sizeof(vtxData_tex5[0]); i++) {
        Poke(VERTEX_BUFFER_ADDR + (i << 2), vtxData_tex5[i]);
    }
    for (i = 0; i < sizeof(idxData_tex5) / sizeof(idxData_tex5[0]); i++) {
        Poke(IDX_BUFFER_ADDR + (i << 2), idxData_tex5[i]);
    }
}

void runTexture(unsigned long phys, int SuperSample)
{
    unsigned long write_addr = phys;

    loadTexture();
    loadTex_vtx_idx();

    // LOAD STATES
    //switch to 3D mode (in case you were in 2D)
    Poke(write_addr, 0x8010E00);
    write_addr += 4;
    Poke(write_addr, 0x0);
    write_addr += 4;

    Poke(write_addr, 0x08010193);
    write_addr += 4;
    Poke(write_addr, VERTEX_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x08010194);
    write_addr += 4;
    Poke(write_addr, 0x00000010);
    write_addr += 4;
    Poke(write_addr, 0x08010180);
    write_addr += 4;
    Poke(write_addr, 0x10000088);
    write_addr += 4;
    Poke(write_addr, 0x08010500);
    write_addr += 4;
    if (depth_format == 2) {
        Poke(write_addr, 0x04001301);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x04011311);
        write_addr += 4;

    }
    Poke(write_addr, 0x08020518);
    write_addr += 4;
    Poke(write_addr, FRAME_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr,
         FRAME_BUFFER_ADDR + pixel_format * ((width + 63) & (~63)) * ((height / 2 + 63) & (~63)));
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x08020520);
    write_addr += 4;
    Poke(write_addr, DEPTH_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr,
         DEPTH_BUFFER_ADDR + depth_format * ((width + 63) & (~63)) * ((height / 2 + 63) & (~63)));
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x08010501);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010502);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x08010503);
    write_addr += 4;
    if (depth_format == 2) {
        Poke(write_addr, 0x477fff00);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x4b7fffff);
        write_addr += 4;

    }
    Poke(write_addr, 0x08010504);
    write_addr += 4;
    Poke(write_addr, DEPTH_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x08010505);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, (((width + 63) & (~63)) * depth_format * 2));
        write_addr += 4;

    } else {
        Poke(write_addr, (((width + 63) & (~63)) * depth_format));
        write_addr += 4;
    }
    Poke(write_addr, 0x08010506);
    write_addr += 4;
    Poke(write_addr, 0x00070007);
    write_addr += 4;
    Poke(write_addr, 0x0801050b);
    write_addr += 4;
    if (pixel_format == 2) {
        Poke(write_addr, 0x00100F04);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x00100F06);
        write_addr += 4;
    }
    if (SuperSample != 0) {
        Poke(write_addr, 0x08010e06);
        write_addr += 4;
        Poke(write_addr, 0x000000f2);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x08010e06);
        write_addr += 4;
        Poke(write_addr, 0x00000000);
        write_addr += 4;
    }
    Poke(write_addr, 0x0801050c);
    write_addr += 4;
    Poke(write_addr, FRAME_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x0801050d);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, (((width + 63) & (~63)) * pixel_format * 2));
        write_addr += 4;

    } else {
        Poke(write_addr, (((width + 63) & (~63)) * pixel_format));
        write_addr += 4;
    }
    Poke(write_addr, 0x08010300);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010301);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010302);
    write_addr += 4;
    Poke(write_addr, new_width.u);
    write_addr += 4;
    Poke(write_addr, 0x08010303);
    write_addr += 4;
    Poke(write_addr, new_height.u);
    write_addr += 4;
    Poke(write_addr, 0x08010380);
    write_addr += 4;
    Poke(write_addr, 0x00000004);
    write_addr += 4;
    Poke(write_addr, 0x08010515);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010516);
    write_addr += 4;
    Poke(write_addr, 0x0048a000);
    write_addr += 4;
    Poke(write_addr, 0x08010508);
    write_addr += 4;
    Poke(write_addr, 0x00000070);
    write_addr += 4;
    Poke(write_addr, 0x0801050a);
    write_addr += 4;
    Poke(write_addr, 0x00100010);
    write_addr += 4;
    Poke(write_addr, 0x08010304);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010305);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010306);
    write_addr += 4;
    Poke(write_addr, 0x00000001);
    write_addr += 4;
    Poke(write_addr, 0x0801028a);
    write_addr += 4;
    Poke(write_addr, 0x00000011);
    write_addr += 4;
    Poke(write_addr, 0x08010e05);
    write_addr += 4;
    Poke(write_addr, 0x00000001);
    write_addr += 4;
    Poke(write_addr, 0x08010404);
    write_addr += 4;
    Poke(write_addr, 0x00000002);
    write_addr += 4;
    Poke(write_addr, 0x08010283);
    write_addr += 4;
    Poke(write_addr, new_half_width.u);
    write_addr += 4;
    Poke(write_addr, 0x08010280);
    write_addr += 4;
    Poke(write_addr, new_half_width.u);
    write_addr += 4;

    Poke(write_addr, 0x08010284);
    write_addr += 4;
    Poke(write_addr, new_half_height.u);
    write_addr += 4;

    Poke(write_addr, 0x08010281);
    write_addr += 4;
    Poke(write_addr, neg_new_half_height.u);
    write_addr += 4;

    if (SuperSample != 0) {
        Poke(write_addr, 0x08010381);
        write_addr += 4;
        Poke(write_addr, 0x4ee44ee4);
        write_addr += 4;
        Poke(write_addr, 0x08010384);
        write_addr += 4;
        Poke(write_addr, 0xeaa26e26);
        write_addr += 4;
        Poke(write_addr, 0x08040390);
        write_addr += 4;
        Poke(write_addr, 0x4a6e2600);
        write_addr += 4;
        Poke(write_addr, 0x00000000);
        write_addr += 4;
        Poke(write_addr, 0x00000000);
        write_addr += 4;
        Poke(write_addr, 0x00000000);
        write_addr += 4;
        Poke(write_addr, 0xffffffff);
        write_addr += 4;
        Poke(write_addr, 0x08010385);
        write_addr += 4;
        Poke(write_addr, 0xe8828e28);
        write_addr += 4;
        Poke(write_addr, 0x08040394);
        write_addr += 4;
        Poke(write_addr, 0x4c812020);
        write_addr += 4;
        Poke(write_addr, 0x04101c04);
        write_addr += 4;
        Poke(write_addr, 0x0c4c8400);
        write_addr += 4;
        Poke(write_addr, 0x0001101c);
        write_addr += 4;
        Poke(write_addr, 0xffffffff);
        write_addr += 4;
        Poke(write_addr, 0x08010386);
        write_addr += 4;
        Poke(write_addr, 0xe552be2b);
        write_addr += 4;
        Poke(write_addr, 0x08040398);
        write_addr += 4;
        Poke(write_addr, 0x03902500);
        write_addr += 4;
        Poke(write_addr, 0x7906500a);
        write_addr += 4;
        Poke(write_addr, 0x90a500a0);
        write_addr += 4;
        Poke(write_addr, 0x0e500a0b);
        write_addr += 4;
        Poke(write_addr, 0xffffffff);
        write_addr += 4;
        Poke(write_addr, 0x08010387);
        write_addr += 4;
        Poke(write_addr, 0xc334dc4d);
        write_addr += 4;
        Poke(write_addr, 0x0804039c);
        write_addr += 4;
        Poke(write_addr, 0x510a3015);
        write_addr += 4;
        Poke(write_addr, 0x59086002);
        write_addr += 4;
        Poke(write_addr, 0x75101515);
        write_addr += 4;
        Poke(write_addr, 0x10a30151);
        write_addr += 4;
        Poke(write_addr, 0xffffffff);
        write_addr += 4;

    }
    Poke(write_addr, 0x08010285);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010282);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x08010500);
    write_addr += 4;
    Poke(write_addr, 0xfffdffff);
    write_addr += 4;
    Poke(write_addr, 0x08010193);
    write_addr += 4;
    Poke(write_addr, VERTEX_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x08010191);
    write_addr += 4;
    Poke(write_addr, IDX_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x08010192);
    write_addr += 4;
    Poke(write_addr, 0x00000001);
    write_addr += 4;
    Poke(write_addr, 0x08010287);
    write_addr += 4;
    Poke(write_addr, 0x3f000000);
    write_addr += 4;
    Poke(write_addr, 0x08041420);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xc0600000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08041424);
    write_addr += 4;
    Poke(write_addr, 0x3f666666);
    write_addr += 4;
    Poke(write_addr, 0x3f666666);
    write_addr += 4;
    Poke(write_addr, 0x3f666666);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010202);
    write_addr += 4;
    Poke(write_addr, 0x00001e01);
    write_addr += 4;
    Poke(write_addr, 0x08010208);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010201);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x08010e07);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010e08);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010402);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, 0x00001e02);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x00001e01);
        write_addr += 4;
    }
    Poke(write_addr, 0x08010401);
    write_addr += 4;
    Poke(write_addr, 0x00000001);
    write_addr += 4;
    Poke(write_addr, 0x08010204);
    write_addr += 4;
    Poke(write_addr, 0x8ff3a001);
    write_addr += 4;
    Poke(write_addr, 0x08281000);
    write_addr += 4;
    Poke(write_addr, 0x07841003);
    write_addr += 4;
    Poke(write_addr, 0x39000f20);
    write_addr += 4;
    Poke(write_addr, 0x01c804c0);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x00811006);
    write_addr += 4;
    Poke(write_addr, 0x39004f20);
    write_addr += 4;
    Poke(write_addr, 0x01c80240);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x01011006);
    write_addr += 4;
    Poke(write_addr, 0x39004f20);
    write_addr += 4;
    Poke(write_addr, 0x01c802c0);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x02011006);
    write_addr += 4;
    Poke(write_addr, 0x39004f20);
    write_addr += 4;
    Poke(write_addr, 0x01c80340);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x04011006);
    write_addr += 4;
    Poke(write_addr, 0x39004f20);
    write_addr += 4;
    Poke(write_addr, 0x01c803c0);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x07841005);
    write_addr += 4;
    Poke(write_addr, 0x39000f20);
    write_addr += 4;
    Poke(write_addr, 0x01c80040);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x0784100d);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000048);
    write_addr += 4;
    Poke(write_addr, 0x07841003);
    write_addr += 4;
    Poke(write_addr, 0x39000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000240);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x07821009);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00390048);
    write_addr += 4;
    Poke(write_addr, 0x07831001);
    write_addr += 4;
    Poke(write_addr, 0x39008f20);
    write_addr += 4;
    Poke(write_addr, 0x00000058);
    write_addr += 4;
    Poke(write_addr, 0x00790008);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010200);
    write_addr += 4;
    Poke(write_addr, 0x0000000a);
    write_addr += 4;
    Poke(write_addr, 0x08010203);
    write_addr += 4;
    Poke(write_addr, 0x00000005);
    write_addr += 4;
    Poke(write_addr, 0x0801020c);
    write_addr += 4;
    Poke(write_addr, 0x003f053f);
    write_addr += 4;
    Poke(write_addr, 0x08010216);
    write_addr += 4;
    Poke(write_addr, 0x40001005);
    write_addr += 4;
    Poke(write_addr, 0x08010900);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR);
    write_addr += 4;
    Poke(write_addr, 0x08010910);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x180000);
    write_addr += 4;
    Poke(write_addr, 0x08010920);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1e0000);
    write_addr += 4;
    Poke(write_addr, 0x08010930);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1f8000);
    write_addr += 4;
    Poke(write_addr, 0x08010940);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1fe000);
    write_addr += 4;
    Poke(write_addr, 0x08010950);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1ff800);
    write_addr += 4;
    Poke(write_addr, 0x08010960);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1ffe00);
    write_addr += 4;
    Poke(write_addr, 0x08010970);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x1fff80);
    write_addr += 4;
    Poke(write_addr, 0x08010980);
    write_addr += 4;
    Poke(write_addr, TEX_BUFFER_ADDR + 0x200100);
    write_addr += 4;
    Poke(write_addr, 0x08010800);
    write_addr += 4;
    Poke(write_addr, 0x0000f555);
    write_addr += 4;
    Poke(write_addr, 0x08010810);
    write_addr += 4;
    Poke(write_addr, 0x01000100);
    write_addr += 4;
    Poke(write_addr, 0x08010820);
    write_addr += 4;
    Poke(write_addr, 0x00040100);
    write_addr += 4;
    Poke(write_addr, 0x08010830);
    write_addr += 4;
    Poke(write_addr, 0x00000200);
    write_addr += 4;
    if (HZ) {
        Poke(write_addr, 0x08010500);
        write_addr += 4;

        Poke(write_addr, 0xFFFCFFFF);
        write_addr += 4;
        Poke(write_addr, 0x8010515);
        write_addr += 4;

        Poke(write_addr,
             (HZ == 2) ? ((depth_format == 2) ? 0x5 : 0x7) : ((depth_format == 2) ? 0x9 : 0xB));
        write_addr += 4;
        Poke(write_addr, 0x8010516);
        write_addr += 4;

        Poke(write_addr, HZ_BUFFER_ADDR);
        write_addr += 4;
    }
    Poke(write_addr, 0x08101410);
    write_addr += 4;
    Poke(write_addr, 0x3fe7c3b5);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x401a8279);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x3f814afd);
    write_addr += 4;
    Poke(write_addr, 0x40219dbc);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x40600000);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x08041c00);
    write_addr += 4;
    Poke(write_addr, 0x40000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010e07);
    write_addr += 4;
    Poke(write_addr, 0x00000006);
    write_addr += 4;
    Poke(write_addr, 0x08010e08);
    write_addr += 4;
    Poke(write_addr, 0x00000033);
    write_addr += 4;
    Poke(write_addr, 0x08020e0a);
    write_addr += 4;
    Poke(write_addr, 0x00000bae);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x08010402);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, 0x00001e04);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x00001e03);
        write_addr += 4;
    }
    Poke(write_addr, 0x08010401);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x08010204);
    write_addr += 4;
    Poke(write_addr, 0x00030201);
    write_addr += 4;
    Poke(write_addr, 0x08281800);
    write_addr += 4;
    Poke(write_addr, 0x03841005);
    write_addr += 4;
    Poke(write_addr, 0x29001f20);
    write_addr += 4;
    Poke(write_addr, 0x014800c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x0384100d);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000048);
    write_addr += 4;
    Poke(write_addr, 0x03841003);
    write_addr += 4;
    Poke(write_addr, 0x29001f20);
    write_addr += 4;
    Poke(write_addr, 0x00000240);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x03851005);
    write_addr += 4;
    Poke(write_addr, 0x29002f20);
    write_addr += 4;
    Poke(write_addr, 0x01480140);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x0385100d);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000058);
    write_addr += 4;
    Poke(write_addr, 0x03851003);
    write_addr += 4;
    Poke(write_addr, 0x29002f20);
    write_addr += 4;
    Poke(write_addr, 0x000002c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x04041005);
    write_addr += 4;
    Poke(write_addr, 0x29004f20);
    write_addr += 4;
    Poke(write_addr, 0x014802c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x03841003);
    write_addr += 4;
    Poke(write_addr, 0x29004f20);
    write_addr += 4;
    Poke(write_addr, 0x01fe0240);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x03851002);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x01480250);
    write_addr += 4;
    Poke(write_addr, 0x00690058);
    write_addr += 4;
    Poke(write_addr, 0x07831018);
    write_addr += 4;
    Poke(write_addr, 0x39005f20);
    write_addr += 4;
    Poke(write_addr, 0x00000040);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010400);
    write_addr += 4;
    Poke(write_addr, 0x0000000a);
    write_addr += 4;
    Poke(write_addr, 0x08010403);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, 0x7);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x6);
        write_addr += 4;
    }
    Poke(write_addr, 0x08031420);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xc0600000);
    write_addr += 4;
    Poke(write_addr, 0x08031424);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x3f800000);
    write_addr += 4;
    Poke(write_addr, 0x08031c20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xc0600000);
    write_addr += 4;
    Poke(write_addr, 0x0801028c);
    write_addr += 4;
    Poke(write_addr, 0x00000200);
    write_addr += 4;
    Poke(write_addr, 0x08010201);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x0801028d);
    write_addr += 4;
    Poke(write_addr, 0xe009aaff);
    write_addr += 4;
    Poke(write_addr, 0x30000000);
    write_addr += 4;
    Poke(write_addr, 0x00000004);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x000011fa);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010e07);
    write_addr += 4;
    Poke(write_addr, 0x00000004);
    write_addr += 4;
    Poke(write_addr, 0x08010e08);
    write_addr += 4;
    Poke(write_addr, 0x00000003);
    write_addr += 4;
    Poke(write_addr, 0x08020e0a);
    write_addr += 4;
    Poke(write_addr, 0x0000002e);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xffffffff);
    write_addr += 4;
    Poke(write_addr, 0x08010402);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, 0x00001e03);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x00001e02);
        write_addr += 4;
    }
    Poke(write_addr, 0x08010401);
    write_addr += 4;
    Poke(write_addr, 0x00000002);
    write_addr += 4;
    Poke(write_addr, 0x08010204);
    write_addr += 4;
    Poke(write_addr, 0xedb00201);
    write_addr += 4;
    Poke(write_addr, 0x08101800);
    write_addr += 4;
    Poke(write_addr, 0x03831005);
    write_addr += 4;
    Poke(write_addr, 0x29001f20);
    write_addr += 4;
    Poke(write_addr, 0x014800c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x0383100d);
    write_addr += 4;
    Poke(write_addr, 0x00000f20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000038);
    write_addr += 4;
    Poke(write_addr, 0x03831003);
    write_addr += 4;
    Poke(write_addr, 0x29001f20);
    write_addr += 4;
    Poke(write_addr, 0x000001c0);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x07821018);
    write_addr += 4;
    Poke(write_addr, 0x29003f20);
    write_addr += 4;
    Poke(write_addr, 0x00000040);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010400);
    write_addr += 4;
    Poke(write_addr, 0x00000004);
    write_addr += 4;
    Poke(write_addr, 0x08010403);
    write_addr += 4;
    if (SuperSample != 0) {
        Poke(write_addr, 0x5);
        write_addr += 4;
    } else {
        Poke(write_addr, 0x4);
        write_addr += 4;
    }
    Poke(write_addr, 0x08031420);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xc0600000);
    write_addr += 4;
    Poke(write_addr, 0x08031424);
    write_addr += 4;
    Poke(write_addr, 0x41200000);
    write_addr += 4;
    Poke(write_addr, 0x41200000);
    write_addr += 4;
    Poke(write_addr, 0x41200000);
    write_addr += 4;
    Poke(write_addr, 0x08031c20);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0xc0600000);
    write_addr += 4;
    Poke(write_addr, 0x0801028c);
    write_addr += 4;
    Poke(write_addr, 0x00000100);
    write_addr += 4;
    Poke(write_addr, 0x08010201);
    write_addr += 4;
    Poke(write_addr, 0x00000002);
    write_addr += 4;
    Poke(write_addr, 0x0801028d);
    write_addr += 4;
    Poke(write_addr, 0xe009a9ff);
    write_addr += 4;
    Poke(write_addr, 0x30000000);
    write_addr += 4;
    Poke(write_addr, 0x00000004);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x000011fa);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;
    Poke(write_addr, 0x08010e03);
    write_addr += 4;
    Poke(write_addr, 0x0000001f);
    write_addr += 4;

    Poke(write_addr, 0x08010e02);
    write_addr += 4;
    Poke(write_addr, 0x00000701);
    write_addr += 4;
    Poke(write_addr, 0x48000000);
    write_addr += 4;
    Poke(write_addr, 0x00000701);
    write_addr += 4;

    Poke(write_addr, 0x10000000);
    write_addr += 4;
    Poke(write_addr, 0x00000000);
    write_addr += 4;

    start(phys);
}
