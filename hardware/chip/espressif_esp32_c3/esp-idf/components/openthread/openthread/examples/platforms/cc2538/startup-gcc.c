/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements gcc-specific startup code for the cc2538.
 */

#include <stdint.h>
#include <string.h>

#include "cc2538-reg.h"

extern uint8_t _ldata;
extern uint8_t _data;
extern uint8_t _edata;
extern uint8_t _bss;
extern uint8_t _ebss;
extern uint8_t _init_array;
extern uint8_t _einit_array;

__extension__ typedef int __guard __attribute__((mode(__DI__)));

int __cxa_guard_acquire(__guard *g)
{
    return !*(char *)(g);
}

void __cxa_guard_release(__guard *g)
{
    *(char *)g = 1;
}

void __cxa_guard_abort(__guard *g)
{
    (void)g;
}

void __cxa_pure_virtual(void)
{
    while (1)
        ;
}

void IntDefaultHandler(void);
void ResetHandler(void);

extern void SysTick_Handler(void);
extern void UART0IntHandler(void);
extern void RFCoreRxTxIntHandler(void);
extern void RFCoreErrIntHandler(void);
extern void main(void);

static uint64_t stack[640] __attribute__((section(".stack")));

__attribute__((section(".vectors"), used)) void (*const vectors[])(void) = {
    (void (*)(void))((unsigned long)stack + sizeof(stack)), // Initial Stack Pointer
    ResetHandler,                                           // 1 The reset handler
    ResetHandler,                                           // 2 The NMI handler
    IntDefaultHandler,                                      // 3 The hard fault handler
    IntDefaultHandler,                                      // 4 The MPU fault handler
    IntDefaultHandler,                                      // 5 The bus fault handler
    IntDefaultHandler,                                      // 6 The usage fault handler
    0,                                                      // 7 Reserved
    0,                                                      // 8 Reserved
    0,                                                      // 9 Reserved
    0,                                                      // 10 Reserved
    IntDefaultHandler,                                      // 11 SVCall handler
    IntDefaultHandler,                                      // 12 Debug monitor handler
    0,                                                      // 13 Reserved
    IntDefaultHandler,                                      // 14 The PendSV handler
    SysTick_Handler,                                        // 15 The SysTick handler
    IntDefaultHandler,                                      // 16 GPIO Port A
    IntDefaultHandler,                                      // 17 GPIO Port B
    IntDefaultHandler,                                      // 18 GPIO Port C
    IntDefaultHandler,                                      // 19 GPIO Port D
    0,                                                      // 20 none
    UART0IntHandler,                                        // 21 UART0 Rx and Tx
    IntDefaultHandler,                                      // 22 UART1 Rx and Tx
    IntDefaultHandler,                                      // 23 SSI0 Rx and Tx
    IntDefaultHandler,                                      // 24 I2C Master and Slave
    0,                                                      // 25 Reserved
    0,                                                      // 26 Reserved
    0,                                                      // 27 Reserved
    0,                                                      // 28 Reserved
    0,                                                      // 29 Reserved
    IntDefaultHandler,                                      // 30 ADC Sequence 0
    0,                                                      // 31 Reserved
    0,                                                      // 32 Reserved
    0,                                                      // 33 Reserved
    IntDefaultHandler,                                      // 34 Watchdog timer, timer 0
    IntDefaultHandler,                                      // 35 Timer 0 subtimer A
    IntDefaultHandler,                                      // 36 Timer 0 subtimer B
    IntDefaultHandler,                                      // 37 Timer 1 subtimer A
    IntDefaultHandler,                                      // 38 Timer 1 subtimer B
    IntDefaultHandler,                                      // 39 Timer 2 subtimer A
    IntDefaultHandler,                                      // 40 Timer 2 subtimer B
    IntDefaultHandler,                                      // 41 Analog Comparator 0
    RFCoreRxTxIntHandler,                                   // 42 RFCore Rx/Tx
    RFCoreErrIntHandler,                                    // 43 RFCore Error
    IntDefaultHandler,                                      // 44 IcePick
    IntDefaultHandler,                                      // 45 FLASH Control
    IntDefaultHandler,                                      // 46 AES
    IntDefaultHandler,                                      // 47 PKA
    IntDefaultHandler,                                      // 48 Sleep Timer
    IntDefaultHandler,                                      // 49 MacTimer
    IntDefaultHandler,                                      // 50 SSI1 Rx and Tx
    IntDefaultHandler,                                      // 51 Timer 3 subtimer A
    IntDefaultHandler,                                      // 52 Timer 3 subtimer B
    0,                                                      // 53 Reserved
    0,                                                      // 54 Reserved
    0,                                                      // 55 Reserved
    0,                                                      // 56 Reserved
    0,                                                      // 57 Reserved
    0,                                                      // 58 Reserved
    0,                                                      // 59 Reserved
    IntDefaultHandler,                                      // 60 USB 2538
    0,                                                      // 61 Reserved
    IntDefaultHandler,                                      // 62 uDMA
    IntDefaultHandler,                                      // 63 uDMA Error
};

void IntDefaultHandler(void)
{
    while (1)
        ;
}

// clang-format off

#define FLASH_CCA_BOOTLDR_CFG_DISABLE               0xEFFFFFFF ///< Disable backdoor function
#define FLASH_CCA_BOOTLDR_CFG_ENABLE                0xF0FFFFFF ///< Enable backdoor function
#define FLASH_CCA_BOOTLDR_CFG_ACTIVE_HIGH           0x08000000 ///< Selected pin on pad A active high
#define FLASH_CCA_BOOTLDR_CFG_PORT_A_PIN_M          0x07000000 ///< Selected pin on pad A mask
#define FLASH_CCA_BOOTLDR_CFG_PORT_A_PIN_S          24         ///< Selected pin on pad A shift
#define FLASH_CCA_IMAGE_VALID                       0x00000000 ///< Indicates valid image in flash

#define FLASH_CCA_CONF_BOOTLDR_BACKDOOR_PORT_A_PIN  3      ///< Select Button on SmartRF06 Eval Board

// clang-format on

typedef struct
{
    uint32_t ui32BootldrCfg;
    uint32_t ui32ImageValid;
    uint32_t ui32ImageVectorAddr;
    uint8_t  ui8lock[32];
} flash_cca_lock_page_t;

__attribute__((__section__(".flashcca"), used)) const flash_cca_lock_page_t flash_cca_lock_page = {
    FLASH_CCA_BOOTLDR_CFG_ENABLE | (FLASH_CCA_CONF_BOOTLDR_BACKDOOR_PORT_A_PIN << FLASH_CCA_BOOTLDR_CFG_PORT_A_PIN_S),
    FLASH_CCA_IMAGE_VALID,
    (uint32_t)&vectors,
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

typedef void (*init_fn_t)(void);

void ResetHandler(void)
{
    HWREG(SYS_CTRL_EMUOVR) = 0xFF;

    // configure clocks
    HWREG(SYS_CTRL_CLOCK_CTRL) |= SYS_CTRL_CLOCK_CTRL_AMP_DET;
    HWREG(SYS_CTRL_CLOCK_CTRL) = SYS_CTRL_SYSDIV_32MHZ;

    // alternate map
    HWREG(SYS_CTRL_I_MAP) |= SYS_CTRL_I_MAP_ALTMAP;

    // copy the data segment initializers from flash to SRAM
    memcpy(&_data, &_ldata, &_edata - &_data);

    // zero-fill the bss segment
    memset(&_bss, 0, &_ebss - &_bss);

    // C++ runtime initialization (BSS, Data, relocation, etc.)
    init_fn_t *fp;

    for (fp = (init_fn_t *)&_init_array; fp < (init_fn_t *)&_einit_array; fp++)
    {
        (*fp)();
    }

    // call the application's entry point
    main();

    // end here if main() returns
    while (1)
        ;
}
