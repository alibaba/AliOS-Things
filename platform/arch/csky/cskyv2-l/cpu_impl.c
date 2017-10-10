/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_auto_config.h>
#include <k_api.h>
#include <watchpoint.h>
#include <k_config.h>
#include <stdio.h>

void cpu_lpm_wait(void)
{
    __asm__("wait");
}

void cpu_lpm_doze(void)
{
    __asm__("doze");
}

void cpu_lpm_stop(void)
{
    __asm__("stop");
}

__attribute__((naked)) void cpu_context_save(unsigned int addr)
{
    __asm__ __volatile__(
        "ldw    r0,  (r0) \n"

        "stw    r1,  (r0, 0x04) \n"
        "stw    r2,  (r0, 0x08) \n"
        "stw    r3,  (r0, 0x0c) \n"
        "stw    r4,  (r0, 0x10) \n"
        "stw    r5,  (r0, 0x14) \n"
        "stw    r6,  (r0, 0x18) \n"
        "stw    r7,  (r0, 0x1c) \n"
        "stw    r8,  (r0, 0x20) \n"
        "stw    r9,  (r0, 0x24) \n"
        "stw    r10, (r0, 0x28) \n"
        "stw    r11, (r0, 0x2c) \n"
        "stw    r12, (r0, 0x30) \n"
        "stw    r13, (r0, 0x34) \n"
        "stw    r15, (r0, 0x3c) \n"
        "mfcr   r2,  psr    \n"
        "stw    r2,  (r0, 0x40) \n"
        "mfcr   r2,  vbr    \n"
        "stw    r2,  (r0, 0x44) \n"
        "mfcr   r2,  epsr   \n"
        "stw    r2,  (r0, 0x48) \n"
        "mfcr   r2,  epc        \n"
        "stw    r2,  (r0, 0x4c) \n"
        "mfcr   r2,  cr<11, 0>  \n"
        "stw    r2,  (r0, 0x50) \n"
        "mfcr   r2,  cr<12, 0>  \n"
        "stw    r2,  (r0, 0x54) \n"
        "mfcr   r2,  cr<13, 0>  \n"
        "stw    r2,  (r0, 0x58) \n"
        "mfcr   r2,  cr<18, 0>  \n"
        "stw    r2,  (r0, 0x5c) \n"
        "mfcr   r2,  cr<19, 0>  \n"
        "stw    r2,  (r0, 0x60) \n"
        "mfcr   r2,  cr<20, 0>  \n"
        "stw    r2,  (r0, 0x64) \n"
        "mfcr   r2,  cr<21, 0>  \n"
        "stw    r2,  (r0, 0x68) \n"

        "ldw    r2,  (r14, 0x00)\n"
        "addi   r14, 4  \n"
        "stw    r14, (r0, 0x38) \n"
        "stw    r2,  (r0, 0x00) \n"

        "jmp    r15 \n"
    );
}

__attribute__((naked)) void cpu_context_restore(unsigned int addr)
{
    __asm__ __volatile__(
        "ldw    r0,  (r0)   \n"

        "stw    r1,  (r0, 0x04) \n"
        "stw    r3,  (r0, 0x0c) \n"
        "stw    r4,  (r0, 0x10) \n"
        "stw    r5,  (r0, 0x14) \n"
        "stw    r6,  (r0, 0x18) \n"
        "stw    r7,  (r0, 0x1c) \n"
        "stw    r8,  (r0, 0x20) \n"
        "stw    r9,  (r0, 0x24) \n"
        "stw    r10, (r0, 0x28) \n"
        "stw    r11, (r0, 0x2c) \n"
        "stw    r12, (r0, 0x30) \n"
        "stw    r13, (r0, 0x34) \n"
        "stw    r14, (r0, 0x38) \n"
        "stw    r15, (r0, 0x3c) \n"

        "ldw    r2,  (r0, 0x40) \n"
        "mtcr   r2,  psr    \n"
        "ldw    r2,  (r0, 0x44) \n"
        "mtcr   r2,  vbr    \n"
        "ldw    r2,  (r0, 0x48) \n"
        "mtcr   r2,  epsr   \n"
        "ldw    r2,  (r0, 0x4c) \n"
        "mtcr   r2,  epc        \n"
        "ldw    r2,  (r0, 0x50) \n"
        "mtcr   r2,  cr<11, 0>  \n"
        "ldw    r2,  (r0, 0x54) \n"
        "mtcr   r2,  cr<12, 0>  \n"
        "ldw    r2,  (r0, 0x58) \n"
        "mtcr   r2,  cr<13, 0>  \n"
        "ldw    r2,  (r0, 0x5c) \n"
        "mtcr   r2,  cr<18, 0>  \n"
        "ldw    r2,  (r0, 0x60) \n"
        "mtcr   r2,  cr<19, 0>  \n"
        "ldw    r2,  (r0, 0x64) \n"
        "mtcr   r2,  cr<20, 0>  \n"
        "ldw    r2,  (r0, 0x68) \n"
        "mtcr   r2,  cr<21, 0>  \n"

        "ldw    r2,  (r0, 0x8)\n"
        "ldw    r2,  (r0, 0x00) \n"

        "jmp    r15 \n"
    );
}

#if (RHINO_CONFIG_STACK_OVF_CHECK_HW != 0)
void cpu_intrpt_stack_protect(void)
{
}

void task_stack_crash_warning(void)
{
    printf("****The task stack base has been broken !!!****\n");
}

void cpu_task_stack_protect(cpu_stack_t *base, size_t size)
{
    uint32_t base_addr = (uint32_t)base;

    int num_return = wp_register(base_addr, AWATCH, task_stack_crash_warning);
    if (num_return == 1) {
        wp_enable(1);
    } else if (num_return == 2 || num_return == -1) {
          wp_unregister(1);
          int number_tmp = wp_register(base_addr, AWATCH, task_stack_crash_warning);
          if (number_tmp == 1){
              wp_enable(1);
          }
      }
}
#endif
