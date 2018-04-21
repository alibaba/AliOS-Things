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
#include "cmsis_nvic.h"

#define NVIC_RAM_VECTOR_ADDRESS   (RDA_IRAM_BASE)   // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (RDA_CODE_BASE)   // Initial vector position in flash

void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector) {
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    uint32_t i;

    // Copy and switch to dynamic vectors if the first time called
    if (SCB->VTOR == NVIC_FLASH_VECTOR_ADDRESS) {
        uint32_t *old_vectors = vectors;
        vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;
        for (i=0; i<NVIC_NUM_VECTORS; i++) {
            vectors[i] = old_vectors[i];
        }
#if 0
        // reuse Bootrom NMI/HardFault/MemManage/BusFault/UsageFault handler
        old_vectors = (uint32_t*)RDA_ROM_BASE;
        for (i=2; i<7; i++) {
            vectors[i] = old_vectors[i];
        }
#endif
        SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;
    }
    vectors[IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}

uint32_t NVIC_GetVector(IRQn_Type IRQn) {
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    return vectors[IRQn + NVIC_USER_IRQ_OFFSET];
}

void NVIC_Remap(void)
{
    uint32_t *vectors = (uint32_t*)SCB->VTOR;
    uint32_t i;

    // Copy and switch to dynamic vectors if the first time called
    if (SCB->VTOR == NVIC_FLASH_VECTOR_ADDRESS) {
        uint32_t *old_vectors = vectors;
        vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;
        for (i=0; i<NVIC_NUM_VECTORS; i++) {
            vectors[i] = old_vectors[i];
        }
        SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;
    }
}

struct pt_regs
{
    unsigned long uregs[18];
};

#define ARM_cpsr    uregs[7]
#define ARM_pc      uregs[6]
#define ARM_lr      uregs[5]
#define ARM_ip      uregs[4]
#define ARM_r3      uregs[3]
#define ARM_r2      uregs[2]
#define ARM_r1      uregs[1]
#define ARM_r0      uregs[0]

#define CC_V_BIT    (0x01UL << 28)
#define CC_C_BIT    (0x01UL << 29)
#define CC_Z_BIT    (0x01UL << 30)
#define CC_N_BIT    (0x01UL << 31)
#define condition_codes(regs) \
    ((regs)->ARM_cpsr & (CC_V_BIT | CC_C_BIT | CC_Z_BIT | CC_N_BIT))

typedef union
{
    struct
    {
        struct
        {
            volatile uint8_t IACCVIOL    :1;
            volatile uint8_t DACCVIOL    :1;
            volatile uint8_t             :1;
            volatile uint8_t MUNSTKERR   :1;
            volatile uint8_t MSTKERR     :1;
            volatile uint8_t MLSPERR     :1;
            volatile uint8_t             :1;
            volatile uint8_t MMARVALID   :1;
        }MFSR;

        struct
        {
            volatile uint8_t IBUSERR     :1;
            volatile uint8_t PRECISERR   :1;
            volatile uint8_t IMPRECISERR :1;
            volatile uint8_t UNSTKERR    :1;
            volatile uint8_t STKERR      :1;
            volatile uint8_t LSPERR      :1;
            volatile uint8_t             :1;
            volatile uint8_t BFARVALID   :1;
        }BFSR;

        struct
        {
            volatile uint16_t UNDEFINSTR  :1;
            volatile uint16_t INVSTATE    :1;
            volatile uint16_t INVPC       :1;
            volatile uint16_t NOCP        :1;
            volatile uint16_t             :4;
            volatile uint16_t UNALIGNED   :1;
            volatile uint16_t DIVBYZERO   :1;
            volatile uint16_t             :6;
        }UFSR;
    }BITS;
    volatile uint32_t WORD;
} SCB_CFSR_REG;

static void panic(void)
{
    mbed_error_printf("Panic...\r\n");
    while(1);
}

void stack_dump(struct pt_regs *regs){
    uint8_t i = 0;
    uint32_t *tmp = (uint32_t *)regs;

    mbed_error_printf("stack dump:\r\n");
    for (i = 0; i < 64; i++) {
        mbed_error_printf("0x%08x, ",tmp[i]);
        if ((i + 1) % 8 == 0)
            mbed_error_printf("\r\n");
    }
}

static void show_regs(struct pt_regs *regs)
{
    uint32_t flags = 0;
    SCB_CFSR_REG *pt_cFSR = ((void *)0);

    mbed_error_printf("[Hard Faults Report - all numbers in hex]\r\n");
    mbed_error_printf("R0 = 0x%08X\r\n", regs->ARM_r0);
    mbed_error_printf("R1 = 0x%08X\r\n", regs->ARM_r1);
    mbed_error_printf("R2 = 0x%08X\r\n", regs->ARM_r2);
    mbed_error_printf("R3 = 0x%08X\r\n", regs->ARM_r3);
    mbed_error_printf("R12 = 0x%08X\r\n", regs->ARM_ip);
    mbed_error_printf("LR [R14] = 0x%08X\r\n", regs->ARM_lr);
    mbed_error_printf("PC [R15] = 0x%08X\r\n", regs->ARM_pc);

    mbed_error_printf("PSR = 0x%08X\r\n", regs->ARM_cpsr);

    flags = __get_PRIMASK();
    mbed_error_printf("PRIMASK = %x, IRQ:%sable\r\n", flags, flags ? "Dis" : "En");

    flags = __get_FAULTMASK();
    mbed_error_printf("FAULTMASK = 0x%08x\r\n", flags);

    flags = __get_BASEPRI();
    mbed_error_printf("BASEPRI = 0x%08x\r\n", flags);

    flags = __get_CONTROL();
    mbed_error_printf("CONTROL = 0x%08x\r\n", flags);

    mbed_error_printf("BFAR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED38))));
    mbed_error_printf("CFSR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED28))));
    mbed_error_printf("HFSR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED2C))));
    mbed_error_printf("DFSR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED30))));
    mbed_error_printf("AFSR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED3C))));
    mbed_error_printf("MMFAR = 0x%08x\r\n", (*((volatile unsigned long *)(0xE000ED34))));
    mbed_error_printf("SCB_SHCSR = 0x%08x\r\n", SCB->SHCSR);

    mbed_error_printf("\n\n[Detail report]\r\n");

    flags = condition_codes(regs);
    mbed_error_printf("APSR Flags: %c%c%c%c\r\n",
        flags & CC_N_BIT ? 'N' : 'n',
        flags & CC_Z_BIT ? 'Z' : 'z',
        flags & CC_C_BIT ? 'C' : 'c',
        flags & CC_V_BIT ? 'V' : 'v');

    mbed_error_printf("ProcStack:0x%08x MainStack:0x%08x\r\n", __get_PSP(), __get_MSP());

    flags = SCB->CFSR;
    pt_cFSR = (SCB_CFSR_REG *)&flags;

    mbed_error_printf("\nMemory Manage Faults\r\n");
    mbed_error_printf("MM_FAULT_STAT:0x%02x\r\n", ((flags & SCB_CFSR_MEMFAULTSR_Msk) >> SCB_CFSR_MEMFAULTSR_Pos));
    mbed_error_printf("\tIACCVIOL:0x%02x \t\tDACCVIOL:0x%02x\r\n",
        pt_cFSR->BITS.MFSR.IACCVIOL, pt_cFSR->BITS.MFSR.DACCVIOL);
    mbed_error_printf("\tMUNSTKERR:0x%02x \t\tMSTKERR:0x%02x\r\n",
        pt_cFSR->BITS.MFSR.MUNSTKERR, pt_cFSR->BITS.MFSR.MSTKERR);
    mbed_error_printf("\tMLSPERR:0x%02x \t\tMMARVALID:0x%02x\r\n",
        pt_cFSR->BITS.MFSR.MLSPERR, pt_cFSR->BITS.MFSR.MMARVALID);

    mbed_error_printf("\nBus Faults\r\n");
    mbed_error_printf("BUS_FAULT_STAT:0x%02x\r\n", ((flags & SCB_CFSR_BUSFAULTSR_Msk) >> SCB_CFSR_BUSFAULTSR_Pos));
    mbed_error_printf("\tIBUSERR:0x%02x \t\tPRECISERR:0x%02x\r\n",
        pt_cFSR->BITS.BFSR.IBUSERR, pt_cFSR->BITS.BFSR.PRECISERR);
    mbed_error_printf("\tIMPRECISERR:0x%02x \tUNSTKERR:0x%02x\r\n",
        pt_cFSR->BITS.BFSR.IMPRECISERR, pt_cFSR->BITS.BFSR.UNSTKERR);
    mbed_error_printf("\tSTKERR:0x%02x \t\tLSPERR:0x%02x\r\n",
        pt_cFSR->BITS.BFSR.STKERR, pt_cFSR->BITS.BFSR.LSPERR);
    mbed_error_printf("\tBFARVALID:0x%02x\r\n", pt_cFSR->BITS.BFSR.BFARVALID);

    mbed_error_printf("\nUsage Faults\r\n");
    mbed_error_printf("USG_FAULT_STAT:0x%02x\r\n", ((flags & SCB_CFSR_USGFAULTSR_Msk) >> SCB_CFSR_USGFAULTSR_Pos));
    mbed_error_printf("\tUNDEFINSTR:0x%02x \tINVSTATE:0x%02x\r\n",
        pt_cFSR->BITS.UFSR.UNDEFINSTR, pt_cFSR->BITS.UFSR.INVSTATE);
    mbed_error_printf("\tINVPC:0x%02x \t\tNOCP:0x%02x\r\n",
        pt_cFSR->BITS.UFSR.INVPC, pt_cFSR->BITS.UFSR.NOCP);
    mbed_error_printf("\tUNALIGNED:0x%02x \t\tDIVBYZERO:0x%02x\r\n",
        pt_cFSR->BITS.UFSR.UNALIGNED, pt_cFSR->BITS.UFSR.DIVBYZERO);

    stack_dump(regs);

    mbed_error_printf("\n[Report done]\r\n");
}

void NMI_Handler(void)
{
    mbed_error_printf("#E NMI_Handler\n");
}

void HardFault_Handler_C(struct pt_regs *pt_regs)
{
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
        __BKPT(0);
    }

    mbed_error_printf("#E HardFault_Handler\n");
    show_regs(pt_regs);
    panic();
}

void MemManage_Handler_C(struct pt_regs *pt_regs)
{
    mbed_error_printf("#E MemManage_Handler\n");
    show_regs(pt_regs);
    panic();
}

void BusFault_Handler_C(struct pt_regs *pt_regs)
{
    mbed_error_printf("#E BusFault_Handler\n");
    show_regs(pt_regs);
    panic();
}

void UsageFault_Handler_C(struct pt_regs *pt_regs)
{
    mbed_error_printf("#E UsageFault_Handler\n");
    show_regs(pt_regs);
    panic();
}

#if defined (__CC_ARM)

__asm void HardFault_Handler(void)
{
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B __cpp(HardFault_Handler_C)
}

__asm void MemManage_Handler(void)
{
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B __cpp(MemManage_Handler_C)
}

__asm void BusFault_Handler(void)
{
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B __cpp(BusFault_Handler_C)
}

__asm void UsageFault_Handler(void)
{
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B __cpp(UsageFault_Handler_C)
}

#elif defined (__GNUC__)

void HardFault_Handler(void)
{
    __asm(" tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b HardFault_Handler_C\n");
}

void MemManage_Handler(void)
{
    __asm(" tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b MemManage_Handler_C\n");
}

void BusFault_Handler(void)
{
    __asm(" tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b BusFault_Handler_C\n");
}

void UsageFault_Handler(void)
{
    __asm(" tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b UsageFault_Handler_C\n");
}

#endif

