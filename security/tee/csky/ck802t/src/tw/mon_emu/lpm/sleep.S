/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "ck_pmu.h"
.import resume_from_ram
.import resume_from_disk
.import lpm_resume_stack
#ifdef CONFIG_FPGA
.import revise_bus_prio_for_fpga
#endif

.global arch_tw_do_cpu_save
.global arch_tw_do_cpu_resume
.global resume_from_ram_wrapper
.global resume_from_disk_wrapper

arch_tw_do_cpu_save:
    subi    r14, 4
    stw r0, (r14, 0x0)
    stw r1, (r0, 0x4)
    stw r2, (r0, 0x8)
    stw r3, (r0, 0xc)
    stw r4, (r0, 0x10)
    stw r5, (r0, 0x14)
    stw r6, (r0, 0x18)
    stw r7, (r0, 0x1c)
    stw r8, (r0, 0x20)
    stw r9, (r0, 0x24)
    stw r10, (r0, 0x28)
    stw r11, (r0, 0x2c)
    stw r12, (r0, 0x30)
    stw r13, (r0, 0x34)
    stw r15, (r0, 0x3c)
    mfcr    r2, psr
    stw r2, (r0, 0x40)
    mfcr    r2, vbr
    stw r2, (r0, 0x44)
    mfcr    r2, epsr
    stw r2, (r0, 0x48)
    mfcr    r2, epc
    stw r2, (r0, 0x4c)
    mfcr    r2, cr<11, 0>
    stw r2, (r0, 0x50)
    mfcr    r2, cr<12, 0>
    stw r2, (r0, 0x54)
    mfcr    r2, cr<13, 0>
    stw r2, (r0, 0x58)
    mfcr    r2, cr<18,0>
    stw r2, (r0, 0x5c)
    mfcr    r2, cr<19,0>
    stw r2, (r0, 0x60)
    mfcr    r2, cr<20,0>
    stw r2, (r0, 0x64)
    mfcr    r2, cr<21,0>
    stw r2, (r0, 0x68)
    ldw r2, (r14, 0x0)
    addi    r14, 4
    stw r14, (r0, 0x38)
    stw r2, (r0, 0x0)
    movi    r0, 0
    jmp r15

arch_tw_do_cpu_resume:
    ldw r1, (r0, 0x4)
    ldw r3, (r0, 0xc)
    ldw r4, (r0, 0x10)
    ldw r5, (r0, 0x14)
    ldw r6, (r0, 0x18)
    ldw r7, (r0, 0x1c)
    ldw r8, (r0, 0x20)
    ldw r9, (r0, 0x24)
    ldw r10, (r0, 0x28)
    ldw r11, (r0, 0x2c)
    ldw r12, (r0, 0x30)
    ldw r13, (r0, 0x34)
    ldw r14, (r0, 0x38)
    ldw r15, (r0, 0x3c)
    ldw r2, (r0, 0x40)
    mtcr    r2, psr
    ldw r2, (r0, 0x44)
    mtcr    r2, vbr
    ldw r2, (r0, 0x48)
    mtcr    r2, epsr
    ldw r2, (r0, 0x4c)
    mtcr    r2, epc
    ldw r2, (r0, 0x50)
    mtcr    r2, cr<11, 0>
    ldw r2, (r0, 0x54)
    mtcr    r2, cr<12, 0>
    ldw r2, (r0, 0x58)
    mtcr    r2, cr<13, 0>
    ldw r2, (r0, 0x5c)
    mtcr    r2, cr<18, 0>
    ldw r2, (r0, 0x60)
    mtcr    r2, cr<19, 0>
    ldw r2, (r0, 0x64)
    mtcr    r2, cr<20, 0>
    ldw r2, (r0, 0x68)
    mtcr    r2, cr<21, 0>
    ldw r2, (r0, 0x8)
    ldw r0, (r0, 0x0)
    movi    r0, 1
    jmp r15

resume_from_ram_wrapper:
#ifdef CONFIG_FPGA
    bsr       revise_bus_prio_for_fpga
#endif

    lrw      r13, LPM_RESERVE_STACK_SIZE
    lrw      r14, lpm_resume_stack
    add      r14, r13, r14
    bsr      resume_from_ram

resume_from_disk_wrapper:
#ifdef CONFIG_FPGA
    jbsr       revise_bus_prio_for_fpga
#endif

    lrw      r13, LPM_RESERVE_STACK_SIZE
    lrw      r14, lpm_resume_stack
    add      r14, r13, r14
    bsr      resume_from_disk
