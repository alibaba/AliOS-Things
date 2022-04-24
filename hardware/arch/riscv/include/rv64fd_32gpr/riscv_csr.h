/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef RISCV_CSR_H
#define RISCV_CSR_H

/* Status register flags */
#define SR_SIE      0x00000002UL        /*  Supervisor Interrupt Enable */
#define SR_MIE      0x00000008UL        /*  Machine Interrupt Enable */
#define SR_SPIE     0x00000020UL        /*  Previous Supervisor IE */
#define SR_MPIE     0x00000080UL        /*  Previous Machine IE */
#define SR_SPP      0x00000100UL        /*  Previously Supervisor mode */
#define SR_MPP_U    0x00000000UL        /*  Previously User mode */
#define SR_MPP_S    0x00000800UL        /*  Previously Supervisor mode */
#define SR_MPP_M    0x00001800UL        /*  Previously Machine mode */
#define SR_SUM      0x00040000UL        /*  Supervisor User Memory Access */

#define SR_FS           0x00006000UL    /*  Floating-point Status */
#define SR_FS_OFF       0x00000000UL
#define SR_FS_INITIAL   0x00002000UL
#define SR_FS_CLEAN     0x00004000UL
#define SR_FS_DIRTY     0x00006000UL

/* Interrupt-enable Registers */
#define IE_MTIE         0x00000080UL
#define IE_MEIE         0x00000800UL

#endif

