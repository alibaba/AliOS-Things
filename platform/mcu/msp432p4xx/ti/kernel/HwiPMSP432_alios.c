/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== HwiP_freertos.c ========
 *  TODO is this the correct license?
 *
 *  Writing an RTOS safe ISR for FreeRTOS is very dependent on the
 *  microcontroller and tool chain port of FreeRTOS being used. Refer to
 *  the documentation page and demo application for the RTOS port being used.
 */

#include <stdbool.h>
#include <k_api.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/devices/DeviceFamily.h>

/* Driver lib includes */
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>

#define MAX_INTERRUPTS 256


/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK  (0xFFUL)

/*
 *  ======== HwiP_DispatchEntry ========
 */
typedef struct HwiP_DispatchEntry {
    HwiP_Fxn entry;
    uintptr_t arg;
} HwiP_DispatchEntry;

HwiP_DispatchEntry HwiP_dispatchTable[MAX_INTERRUPTS] = {{(HwiP_Fxn)0, 0}};

/*
 *  ======== HwiP_disable ========
 */
uintptr_t HwiP_disable(void)
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    return cpsr;
}

/*
 *  ======== HwiP_restore ========
 */
void HwiP_restore(uintptr_t key)
{
    CPSR_ALLOC();
    cpsr = key;
    RHINO_CPU_INTRPT_ENABLE();
}

#ifndef xdc_target__isaCompatible_28

typedef struct HwiP_NVIC {
    uint32_t RES_00;
    uint32_t ICTR;
    uint32_t RES_08;
    uint32_t RES_0C;
    uint32_t STCSR;
    uint32_t STRVR;
    uint32_t STCVR;
    uint32_t STCALIB;
    uint32_t RES_20[56];
    uint32_t ISER[8];
    uint32_t RES_120[24];
    uint32_t ICER[8];
    uint32_t RES_1A0[24];
    uint32_t ISPR[8];
    uint32_t RES_220[24];
    uint32_t ICPR[8];
    uint32_t RES_2A0[24];
    uint32_t IABR[8];
    uint32_t RES_320[56];
    uint8_t IPR[240];
    uint32_t RES_4F0[516];
    uint32_t CPUIDBR;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint8_t SHPR[12];
    uint32_t SHCSR;
    uint8_t MMFSR;
    uint8_t BFSR;
    uint16_t UFSR;
    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t MMAR;
    uint32_t BFAR;
    uint32_t AFSR;
    uint32_t PFR0;
    uint32_t PFR1;
    uint32_t DFR0;
    uint32_t AFR0;
    uint32_t MMFR0;
    uint32_t MMFR1;
    uint32_t MMFR2;
    uint32_t MMFR3;
    uint32_t ISAR0;
    uint32_t ISAR1;
    uint32_t ISAR2;
    uint32_t ISAR3;
    uint32_t ISAR4;
    uint32_t RES_D74[5];
    uint32_t CPACR;
    uint32_t RES_D8C[93];
    uint32_t STI;
    uint32_t RES_F04[12];
    uint32_t FPCCR;
    uint32_t FPCAR;
    uint32_t FPDSCR;
    uint32_t MVFR0;
    uint32_t MVFR1;
    uint32_t RES_F48[34];
    uint32_t PID4;
    uint32_t PID5;
    uint32_t PID6;
    uint32_t PID7;
    uint32_t PID0;
    uint32_t PID1;
    uint32_t PID2;
    uint32_t PID3;
    uint32_t CID0;
    uint32_t CID1;
    uint32_t CID2;
    uint32_t CID3;
} HwiP_NVIC;

static volatile HwiP_NVIC *HwiP_nvic = (HwiP_NVIC *)0xE000E000;

/*
 *  ======== HwiP_clearInterrupt ========
 */
void HwiP_clearInterrupt(int interruptNum)
{
    // TODO: Should driverlib functions be prefixed with MAP_?
    Interrupt_unpendInterrupt((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_delete ========
 */
void HwiP_delete(HwiP_Handle handle)
{
    Interrupt_disableInterrupt((int)handle);
    Interrupt_unregisterInterrupt((int)handle);
}

/*
 *  ======== HwiP_disableInterrupt ========
 */
void HwiP_disableInterrupt(int interruptNum)
{
    Interrupt_disableInterrupt(interruptNum);
}

/*
 *  ======== HwiP_dispatch ========
 */
void HwiP_dispatch(void)
{
    uint32_t intNum;
    HwiP_DispatchEntry hwi;

    /* Determine which interrupt has fired */
    intNum = HwiP_nvic->ICSR & 0x000000ff;
    hwi = HwiP_dispatchTable[intNum];
    if (hwi.entry) {
        krhino_intrpt_enter();
        (hwi.entry)(hwi.arg);
        krhino_intrpt_exit();
    }
}

/*
 *  ======== HwiP_enableInterrupt ========
 */
void HwiP_enableInterrupt(int interruptNum)
{
    Interrupt_enableInterrupt(interruptNum);
}

/*
 *  ======== HwiP_create ========
 */
HwiP_Handle HwiP_create(int interruptNum,
                        HwiP_Fxn hwiFxn,
                        HwiP_Params *params)
{
    HwiP_Params defaultParams;

    if (params == NULL) {
        params = &defaultParams;
        HwiP_Params_init(&defaultParams);
    }

    HwiP_dispatchTable[interruptNum].entry = hwiFxn;
    HwiP_dispatchTable[interruptNum].arg = params->arg;

    // TODO: Should driverlib functions be prefixed with MAP_?
    Interrupt_registerInterrupt(interruptNum, (void(*)(void))HwiP_dispatch);
    Interrupt_setPriority(interruptNum, params->priority);

    if (params->enableInt) {
        Interrupt_enableInterrupt(interruptNum);
    }

    return ((HwiP_Handle)interruptNum);
}

/*
 *  ======== HwiP_inISR ========
 */
bool HwiP_inISR(void)
{
    /*for now don't know how to support*/
    bool stat = false;
#if 0
    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        /* Not currently in an ISR */
        stat = false;
    }
    else {
        stat = true;
    }
#endif
    return (stat);
}

/*
 *  ======== HwiP_Params_init ========
 */
void HwiP_Params_init(HwiP_Params *params)
{
    params->arg = 0;
    params->priority = ~0;
    params->enableInt = true;
}

/*
 *  ======== HwiP_plug ========
 */
void HwiP_plug(int interruptNum, void *fxn)
{
    Interrupt_registerInterrupt(interruptNum, (void(*)(void))fxn);
}

/*
 *  ======== HwiP_post ========
 */
void HwiP_post(int interruptNum)
{
    if (interruptNum >= 16) {
        HwiP_nvic->STI = interruptNum - 16;
    }
}

/*
 *  ======== HwiP_setFunc ========
 */
void HwiP_setFunc(HwiP_Handle hwiP, HwiP_Fxn fxn, uintptr_t arg)
{
    int interruptNum = (int)hwiP;

    uintptr_t key = HwiP_disable();

    HwiP_dispatchTable[interruptNum].entry = fxn;
    HwiP_dispatchTable[interruptNum].arg = arg;

    HwiP_restore(key);
}

#endif

