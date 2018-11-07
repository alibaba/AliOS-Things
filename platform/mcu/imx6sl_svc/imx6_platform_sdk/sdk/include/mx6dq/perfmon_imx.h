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

#ifndef __PERFMON_IMX_H__
#define	__PERFMON_IMX_H__

#include "soc_memory_map.h"
#include "irq_numbers.h"

/* Number of performance monitors used in the i.MX6 */
#define PERFMON_INST 3

typedef enum {
    PERFMON_ID1,
    PERFMON_ID2,
    PERFMON_ID3,
} perfmon_id_e;

typedef struct {
    uint32_t base;      /* module base address */
    uint32_t irq_id;    /* ID of its interrupt */
} perfmon_param;

/* used to list the instances of performance monitors */
static const perfmon_param perfmon_list[PERFMON_INST] = {
        {PERFMON1_BASE_ADDR, IMX_INT_PERFMON1},
        {PERFMON2_BASE_ADDR, IMX_INT_PERFMON2},
        {PERFMON3_BASE_ADDR, IMX_INT_PERFMON3},
};

#endif /* __PERFMON_IMX_H__ */
