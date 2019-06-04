/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_ARCH_H
#define K_ARCH_H

#include <stdint.h>
#include "k_types.h"

/* ARMCC || IAR || GNU */
#define FPU_AVL ((defined(__CC_ARM) && defined(__TARGET_FPU_VFP)) \
                || (defined(__ICCARM__) && defined(__ARMVFP__))   \
                || (defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)))

/* ARM exception IDs */
#define ARM_EXCEPT_RESET                    0x00u       /* Reset */
#define ARM_EXCEPT_UNDEF_INSTR              0x01u       /* Undefined Instruction */
#define ARM_EXCEPT_SWI                      0x02u       /* Supervisor Call */
#define ARM_EXCEPT_PREFETCH_ABORT           0x03u       /* Prefetch Abort */
#define ARM_EXCEPT_DATA_ABORT               0x04u       /* Data Abort */
#define ARM_EXCEPT_RESERVED                 0x05u       /* Not used */
#define ARM_EXCEPT_IRQ                      0x06u       /* IRQ interrupt */
#define ARM_EXCEPT_FIQ                      0x07u       /* FIQ interrupt */

/* Bits in CPSR (Current Program Status Register) */
#define CPSR_Mode_USR                       0x10
#define CPSR_Mode_FIQ                       0x11
#define CPSR_Mode_IRQ                       0x12
#define CPSR_Mode_SVC                       0x13
#define CPSR_Mode_ABT                       0x17
#define CPSR_Mode_UND                       0x1B
#define CPSR_Mode_SYS                       0x1F

#define CPSR_FIQ_DIS                        0x40
#define CPSR_IRQ_DIS                        0x80        /* Disable IRQ. */
#define CPSR_INT_DIS                        (CPSR_FIQ_DIS | CPSR_IRQ_DIS)
#define CPSR_ARM							0x00
#define CPSR_THUMB                          0x20        /* Set Thumb mode */
#define CPSR_SVC_THUMB                      (CPSR_Mode_SVC | CPSR_THUMB)
#define CPSR_SVC_ARM                        (CPSR_Mode_SVC | CPSR_ARM)
#define CPSR_USR_ARM                        (CPSR_Mode_USR | CPSR_ARM)

#define _INS_(x) #x
#define STR(x) _INS_(x)

/* Read from coprocessor 15 register */
/* Instruction Synchronization Barrier */
#define OS_ISB() \
        __asm volatile ("isb")

/* Data Memory Barrier */
#define OS_DMB() \
        __asm volatile ("dmb")

/* Data Synchronization Barrier */
#define OS_DSB() \
        __asm volatile ("dsb")

/* Get coprocessor 15 ( System control functionality ) */
#define OS_GET_CP15(op1, Rt, CRn, CRm, op2) \
	do { \
		__asm {                                            \
			mrc p15, op1, Rt, CRn, CRm, op2            \
		}                                                  \
	} while(0)


/* Set coprocessor 15 ( System control functionality ) */
#define OS_SET_CP15(op1, Rt, CRn, CRm, op2) \
	do {                                                       \
		__asm {                                            \
			mcr p15, op1, Rt, CRn, CRm, op2            \
		}                                                  \
	} while(0)

/********** CP15 c0 register summary, identification registers **********/
/* CSSELR, Cache Size Selection Register */
RHINO_INLINE uint32_t os_get_CSSELR(void)
{
    uint32_t reg;
    OS_GET_CP15(2, reg, c0, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_CSSELR(uint32_t reg)
{
    OS_SET_CP15(2, reg, c0, c0, 0);
}

/********** CP15 c1 register summary, system control registers **********/
/* SCTLR, System Control Register */
RHINO_INLINE uint32_t os_get_SCTLR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c1, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_SCTLR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c1, c0, 0);
}

/* ACTLR, Auxiliary Control Register, IMPLEMENTATION DEFINED */
RHINO_INLINE uint32_t os_get_ACTLR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c1, c0, 1);
    return reg;
}
RHINO_INLINE void os_set_ACTLR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c1, c0, 1);
}

/* CPACR, Coprocessor Access Control Register */
RHINO_INLINE uint32_t os_get_CPACR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c1, c0, 2);
    return reg;
}
RHINO_INLINE void os_set_CPACR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c1, c0, 2);
}

/********** CP15 c2 and c3 register summary, Memory protection and control registers **********/
/* TTBR0, Translation Table Base Register 0 */
RHINO_INLINE uint32_t os_get_TTBR0(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c2, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_TTBR0(uint32_t reg)
{
    OS_SET_CP15(0, reg, c2, c0, 0);
}

/* TTBCR, Translation Table Base Control Register */
RHINO_INLINE void *os_get_TTBCR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c2, c0, 2);
    return (void*)reg;
}
RHINO_INLINE void os_set_TTBCR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c2, c0, 2);
}

/* DACR, Domain Access Control Register */
RHINO_INLINE uint32_t os_get_DACR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c3, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_DACR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c3, c0, 0);
}

/********** CP15 c5 and c6 register summary, Memory system fault registers **********/
/* DFSR, Data Fault Status Register */
RHINO_INLINE uint32_t os_get_DFSR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c5, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_DFSR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c5, c0, 0);
}

/* IFSR, Instruction Fault Status Register */
RHINO_INLINE uint32_t os_get_IFSR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c5, c0, 1);
    return reg;
}
RHINO_INLINE void os_set_IFSR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c5, c0, 1);
}

/* DFAR, Data Fault Address Register */
RHINO_INLINE uint32_t os_get_DFAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c6, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_DFAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c6, c0, 0);
}

/* IFAR, Instruction Fault Address Register */
RHINO_INLINE uint32_t os_get_IFAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c6, c0, 2);
    return reg;
}
RHINO_INLINE void os_set_IFAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c6, c0, 2);
}

RHINO_INLINE uint32_t os_get_PAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c7, c4, 0);
    return reg;
}
/********** CP15 c7 register summary, Cache maintenance, address translation, and other functions **********/
/* ICIALLU, Invalidate all instruction caches to PoU */
RHINO_INLINE void os_set_ICIALLU(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c5, 0);
}

/* BPIALL, Invalidate all branch predictors */
RHINO_INLINE void os_set_BPIALL(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c5, 6);
}

/* DCIMVAC, Invalidate data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCIMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c6, 1);
}

/* DCISW, Invalidate data cache line by set/way */
RHINO_INLINE void os_set_DCISW(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c6, 2);
}

/* DCCMVAC, Clean data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCCMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c10, 1);
}

/* DCCSW, Clean data cache line by set/way */
RHINO_INLINE void os_set_DCCSW(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c10, 2);
}

/* ATS1CPR, PL1 read translation */
RHINO_INLINE void os_set_ATS1CPR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c8, 0);
}

/* ATS1CPW, PL1 write translation */
RHINO_INLINE void os_set_ATS1CPW(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c8, 1);
}

/* DCCIMVAC, Clean and invalidate data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCCIMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c14, 1);
}

/* DCCISW, Clean and invalidate data cache line by set/way */
RHINO_INLINE void os_set_DCCISW(uint32_t reg)
{
    OS_SET_CP15(0, reg, c7, c14, 2);
}

/********** CP15 c8 register summary, TLB maintenance operations **********/
/* TLBIALL, invalidate unified TLB */
RHINO_INLINE void os_set_TLBIALL(uint32_t reg)
{
    OS_SET_CP15(0, reg, c8, c7, 0);
}

/********** CP15 c12 register summary, Security Extensions registers **********/
/* VBAR, Vector Base Address Register */
RHINO_INLINE uint32_t os_get_VBAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c12, c0, 0);
    return reg;
}
RHINO_INLINE void os_set_VBAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c12, c0, 0);
}

/********** CP15 c13 register summary, Process, context and thread ID registers **********//* CONTEXTIDR, Context ID Register */
RHINO_INLINE uint32_t os_get_CONTEXTIDR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, c13, c0, 1);
    return reg;
}
RHINO_INLINE void os_set_CONTEXTIDR(uint32_t reg)
{
    OS_SET_CP15(0, reg, c13, c0, 1);
}

typedef struct {
#ifdef FPU_AVL
	long FPEXC;
	/* The Cortex-A5 FPU is a VFPv4-D16 implementation of the ARMv7 floating-point architecture */
    long FPU[32];
    long FPSCR;
#endif
    long CPSR;
    long R0;
    long R1;
    long R2;
    long R3;
    long R4;
    long R5;
    long R6;
    long R7;
    long R8;
    long R9;
    long R10;
    long R11;
    long R12;
    long SP;
    long LR;
    long PC;
}context_t;

typedef struct {
    long exc_type;  /* ARM exception IDs */
    long SP;
    context_t cntx;
}fault_context_t;

#endif /* K_ARCH_H */
