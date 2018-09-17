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

/* Instruction Synchronization Barrier */
#define OS_ISB() \
    __asm volatile ("isb sy":::"memory")

/* Data Memory Barrier */
#define OS_DMB() \
    __asm volatile ("dmb sy":::"memory")

/* Data Synchronization Barrier */
#define OS_DSB() \
    __asm volatile ("dsb sy":::"memory")

/* Get coprocessor 15 ( System control functionality ) */
#define OS_GET_CP15(op1, Rt, CRn, CRm, op2) \
    __asm volatile("mrc p15, " # op1 ", %0, c" # CRn ", c" # CRm ", " # op2 : "=r" (Rt) : : "memory" )

/* Set coprocessor 15 ( System control functionality ) */
#define OS_SET_CP15(op1, Rt, CRn, CRm, op2) \
    __asm volatile("mcr p15, " # op1 ", %0, c" # CRn ", c" # CRm ", " # op2 : : "r" (Rt) : "memory" )


/********** CP15 c0 register summary, identification registers **********/
/* CSSELR, Cache Size Selection Register */
RHINO_INLINE uint32_t os_get_CSSELR(void)
{
    uint32_t reg;
    OS_GET_CP15(2, reg, 0, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_CSSELR(uint32_t reg)
{
    OS_SET_CP15(2, reg, 0, 0, 0);
}

/********** CP15 c1 register summary, system control registers **********/
/* SCTLR, System Control Register */
RHINO_INLINE uint32_t os_get_SCTLR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 1, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_SCTLR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 1, 0, 0);
}

/* ACTLR, Auxiliary Control Register, IMPLEMENTATION DEFINED */
RHINO_INLINE uint32_t os_get_ACTLR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 1, 0, 1);
    return reg;
}
RHINO_INLINE void os_set_ACTLR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 1, 0, 1);
}

/* CPACR, Coprocessor Access Control Register */
RHINO_INLINE uint32_t os_get_CPACR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 1, 0, 2);
    return reg;
}
RHINO_INLINE void os_set_CPACR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 1, 0, 2);
}

/********** CP15 c2 and c3 register summary, Memory protection and control registers **********/
/* TTBR0, Translation Table Base Register 0 */
RHINO_INLINE uint32_t os_get_TTBR0(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 2, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_TTBR0(void *reg)
{
    OS_SET_CP15(0, reg, 2, 0, 0);
}

/* TTBCR, Translation Table Base Control Register */
RHINO_INLINE void *os_get_TTBCR(void)
{
    void *reg;
    OS_GET_CP15(0, reg, 2, 0, 2);
    return reg;
}
RHINO_INLINE void os_set_TTBCR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 2, 0, 2);
}

/* DACR, Domain Access Control Register */
RHINO_INLINE uint32_t os_get_DACR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 3, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_DACR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 3, 0, 0);
}

/********** CP15 c5 and c6 register summary, Memory system fault registers **********/
/* DFSR, Data Fault Status Register */
RHINO_INLINE uint32_t os_get_DFSR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 5, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_DFSR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 5, 0, 0);
}

/* IFSR, Instruction Fault Status Register */
RHINO_INLINE uint32_t os_get_IFSR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 5, 0, 1);
    return reg;
}
RHINO_INLINE void os_set_IFSR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 5, 0, 1);
}

/* DFAR, Data Fault Address Register */
RHINO_INLINE uint32_t os_get_DFAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 6, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_DFAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 6, 0, 0);
}

/* IFAR, Instruction Fault Address Register */
RHINO_INLINE uint32_t os_get_IFAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 6, 0, 2);
    return reg;
}
RHINO_INLINE void os_set_IFAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 6, 0, 2);
}

/********** CP15 c7 register summary, Cache maintenance, address translation, and other functions **********/
/* ICIALLU, Invalidate all instruction caches to PoU */
RHINO_INLINE void os_set_ICIALLU(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 5, 0);
}

/* BPIALL, Invalidate all branch predictors */
RHINO_INLINE void os_set_BPIALL(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 5, 6);
}

/* DCIMVAC, Invalidate data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCIMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 6, 1);
}

/* DCISW, Invalidate data cache line by set/way */
RHINO_INLINE void os_set_DCISW(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 6, 2);
}

/* DCCMVAC, Clean data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCCMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 10, 1);
}

/* DCCSW, Clean data cache line by set/way */
RHINO_INLINE void os_set_DCCSW(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 10, 2);
}

/* DCCIMVAC, Clean and invalidate data cache line by MVA to PoC */
RHINO_INLINE void os_set_DCCIMVAC(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 14, 1);
}

/* DCCISW, Clean and invalidate data cache line by set/way */
RHINO_INLINE void os_set_DCCISW(uint32_t reg)
{
    OS_SET_CP15(0, reg, 7, 14, 2);
}

/********** CP15 c8 register summary, TLB maintenance operations **********/
/* TLBIALL, invalidate unified TLB */
RHINO_INLINE void os_set_TLBIALL(uint32_t reg)
{
    OS_SET_CP15(0, reg, 8, 7, 0);
}

/********** CP15 c12 register summary, Security Extensions registers **********/
/* VBAR, Vector Base Address Register */
RHINO_INLINE uint32_t os_get_VBAR(void)
{
    uint32_t reg;
    OS_GET_CP15(0, reg, 12, 0, 0);
    return reg;
}
RHINO_INLINE void os_set_VBAR(uint32_t reg)
{
    OS_SET_CP15(0, reg, 12, 0, 0);
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
    long LR;
    long PC;
}context_t;

typedef struct {
    long exc_type;  /* ARM exception IDs */
    long SP;
    context_t cntx;
}fault_context_t;

#endif /* K_ARCH_H */

