/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef K_ARCH_H
#define K_ARCH_H

#include "riscv_csr.h"

#if __riscv_xlen == 64
#define REGISTER_LEN   8
#define OS_UDF()                                   \
    do {                                           \
        *(long *)(0xffffffffffffffff) = 1; \
    } while (0)
#elif __riscv_xlen == 32
#define REGISTER_LEN   4
#define OS_UDF()                                   \
    do {                                           \
        *(long *)(0xffffffff) = 1;         \
    } while (0)
#else
#error Assembler did not define __riscv_xlen
#endif

/* Exception Code */
#define CAUSE_MISALIGNED_FETCH       0x0
#define CAUSE_FETCH_ACCESS           0x1
#define CAUSE_ILLEGAL_INSTRUCTION    0x2
#define CAUSE_BREAKPOINT             0x3
#define CAUSE_MISALIGNED_LOAD        0x4
#define CAUSE_LOAD_ACCESS            0x5
#define CAUSE_MISALIGNED_STORE       0x6
#define CAUSE_STORE_ACCESS           0x7
#define CAUSE_USER_ECALL             0x8
#define CAUSE_HYPERVISOR_ECALL       0x9
#define CAUSE_SUPERVISOR_ECALL       0xa
#define CAUSE_MACHINE_ECALL          0xb
#define CAUSE_FETCH_PAGE_FAULT       0xc
#define CAUSE_LOAD_PAGE_FAULT        0xd
#define CAUSE_STORE_PAGE_FAULT       0xf
#define CAUSE_FETCH_GUEST_PAGE_FAULT 0x14
#define CAUSE_LOAD_GUEST_PAGE_FAULT  0x15
#define CAUSE_STORE_GUEST_PAGE_FAULT 0x17

#ifndef __ASSEMBLY__
typedef struct {
    long X1;
    long X4;
    long X5;
    long X6;
    long X7;
    long X8;
    long X9;
    long X10;
    long X11;
    long X12;
    long X13;
    long X14;
    long X15;
    long X16;
    long X17;
    long X18;
    long X19;
    long X20;
    long X21;
    long X22;
    long X23;
    long X24;
    long X25;
    long X26;
    long X27;
    long X28;
    long X29;
    long X30;
    long X31;
    long PC;
    long MSTATUS;
    long FPU[32];
} context_t;

#else
/* Define the offset of the register in the task context */
#define BASE_CONTEXT_LEN   (31 * REGISTER_LEN)
#define FLOAT_CONTEXT_LEN  (32 * REGISTER_LEN)

#define X1_OFFSET  0
#define X4_OFFSET  (X1_OFFSET + REGISTER_LEN)
#define X5_OFFSET  (X4_OFFSET + REGISTER_LEN)
#define X6_OFFSET  (X5_OFFSET + REGISTER_LEN)
#define X7_OFFSET  (X6_OFFSET + REGISTER_LEN)
#define X8_OFFSET  (X7_OFFSET + REGISTER_LEN)
#define X9_OFFSET  (X8_OFFSET + REGISTER_LEN)
#define X10_OFFSET  (X9_OFFSET + REGISTER_LEN)
#define X11_OFFSET  (X10_OFFSET + REGISTER_LEN)
#define X12_OFFSET  (X11_OFFSET + REGISTER_LEN)
#define X13_OFFSET  (X12_OFFSET + REGISTER_LEN)
#define X14_OFFSET  (X13_OFFSET + REGISTER_LEN)
#define X15_OFFSET  (X14_OFFSET + REGISTER_LEN)
#define X16_OFFSET  (X15_OFFSET + REGISTER_LEN)
#define X17_OFFSET  (X16_OFFSET + REGISTER_LEN)
#define X18_OFFSET  (X17_OFFSET + REGISTER_LEN)
#define X19_OFFSET  (X18_OFFSET + REGISTER_LEN)
#define X20_OFFSET  (X19_OFFSET + REGISTER_LEN)
#define X21_OFFSET  (X20_OFFSET + REGISTER_LEN)
#define X22_OFFSET  (X21_OFFSET + REGISTER_LEN)
#define X23_OFFSET  (X22_OFFSET + REGISTER_LEN)
#define X24_OFFSET  (X23_OFFSET + REGISTER_LEN)
#define X25_OFFSET  (X24_OFFSET + REGISTER_LEN)
#define X26_OFFSET  (X25_OFFSET + REGISTER_LEN)
#define X27_OFFSET  (X26_OFFSET + REGISTER_LEN)
#define X28_OFFSET  (X27_OFFSET + REGISTER_LEN)
#define X29_OFFSET  (X28_OFFSET + REGISTER_LEN)
#define X30_OFFSET  (X29_OFFSET + REGISTER_LEN)
#define X31_OFFSET  (X30_OFFSET + REGISTER_LEN)
#define PC_OFFSET   (X31_OFFSET + REGISTER_LEN)
#define MSTATUS_OFFSET   (PC_OFFSET + REGISTER_LEN)

#define F0_OFFSET  (0)
#define F1_OFFSET  (F0_OFFSET + REGISTER_LEN)
#define F2_OFFSET  (F1_OFFSET + REGISTER_LEN)
#define F3_OFFSET  (F2_OFFSET + REGISTER_LEN)
#define F4_OFFSET  (F3_OFFSET + REGISTER_LEN)
#define F5_OFFSET  (F4_OFFSET + REGISTER_LEN)
#define F6_OFFSET  (F5_OFFSET + REGISTER_LEN)
#define F7_OFFSET  (F6_OFFSET + REGISTER_LEN)
#define F8_OFFSET  (F7_OFFSET + REGISTER_LEN)
#define F9_OFFSET  (F8_OFFSET + REGISTER_LEN)
#define F10_OFFSET  (F9_OFFSET + REGISTER_LEN)
#define F11_OFFSET  (F10_OFFSET + REGISTER_LEN)
#define F12_OFFSET  (F11_OFFSET + REGISTER_LEN)
#define F13_OFFSET  (F12_OFFSET + REGISTER_LEN)
#define F14_OFFSET  (F13_OFFSET + REGISTER_LEN)
#define F15_OFFSET  (F14_OFFSET + REGISTER_LEN)
#define F16_OFFSET  (F15_OFFSET + REGISTER_LEN)
#define F17_OFFSET  (F16_OFFSET + REGISTER_LEN)
#define F18_OFFSET  (F17_OFFSET + REGISTER_LEN)
#define F19_OFFSET  (F18_OFFSET + REGISTER_LEN)
#define F20_OFFSET  (F19_OFFSET + REGISTER_LEN)
#define F21_OFFSET  (F20_OFFSET + REGISTER_LEN)
#define F22_OFFSET  (F21_OFFSET + REGISTER_LEN)
#define F23_OFFSET  (F22_OFFSET + REGISTER_LEN)
#define F24_OFFSET  (F23_OFFSET + REGISTER_LEN)
#define F25_OFFSET  (F24_OFFSET + REGISTER_LEN)
#define F26_OFFSET  (F25_OFFSET + REGISTER_LEN)
#define F27_OFFSET  (F26_OFFSET + REGISTER_LEN)
#define F28_OFFSET  (F27_OFFSET + REGISTER_LEN)
#define F29_OFFSET  (F28_OFFSET + REGISTER_LEN)
#define F30_OFFSET  (F29_OFFSET + REGISTER_LEN)
#define F31_OFFSET  (F30_OFFSET + REGISTER_LEN)

#endif

#endif /*  K_ARCH_H */

