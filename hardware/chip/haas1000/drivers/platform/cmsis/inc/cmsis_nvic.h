/* mbed Microcontroller Library
 * CMSIS-style functionality to support dynamic vectors
 *******************************************************************************
 * Copyright (c) 2011 ARM Limited. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of ARM Limited nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */

#ifndef MBED_CMSIS_NVIC_H
#define MBED_CMSIS_NVIC_H

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*NVIC_DEFAULT_FAULT_HANDLER_T)(void);

void NVIC_DisableAllIRQs(void);

void NVIC_InitVectors(void);

void NVIC_SetDefaultFaultHandler(NVIC_DEFAULT_FAULT_HANDLER_T handler);

void NVIC_SetDefaultFaultHandler_cp(NVIC_DEFAULT_FAULT_HANDLER_T handler);

IRQn_Type NVIC_GetCurrentActiveIRQ(void);

void NVIC_PowerDownSleep(uint32_t *buf, uint32_t cnt);

void NVIC_PowerDownWakeup(uint32_t *buf, uint32_t cnt);

#ifdef __ARM_ARCH_ISA_ARM
enum EXCEPTION_ID_T {
    EXCEPTION_NONE = -1,
    EXCEPTION_UNDEF = -2,
    EXCEPTION_SVC = -3,
    EXCEPTION_PABT = -4,
    EXCEPTION_DABT = -5,
    EXCEPTION_HYP = -6,
    EXCEPTION_IRQ = -7,
    EXCEPTION_FIQ = -8,
};

struct FAULT_REGS_T {
    uint32_t r[16];
    uint32_t spsr;
};

struct UNDEF_FAULT_INFO_T {
    enum EXCEPTION_ID_T id;
    uint32_t opcode;
    uint32_t state;
};

struct SVC_FAULT_INFO_T {
    enum EXCEPTION_ID_T id;
    uint32_t svc_num;
};

struct PABT_FAULT_INFO_T {
    enum EXCEPTION_ID_T id;
    uint32_t IFSR;
    uint32_t IFAR;
};

struct DABT_FAULT_INFO_T {
    enum EXCEPTION_ID_T id;
    uint32_t DFSR;
    uint32_t DFAR;
};

typedef void (*GIC_FAULT_DUMP_HANDLER_T)(const uint32_t *regs, const uint32_t *extra, uint32_t extra_len);

void GIC_DisableAllIRQs(void);

void GIC_InitVectors(void);

void GIC_SetFaultDumpHandler(GIC_FAULT_DUMP_HANDLER_T handler);

IRQn_Type IRQ_GetCurrentActiveIRQ(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
