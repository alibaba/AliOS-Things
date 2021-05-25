/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     trap_c.c
 * @brief    source file for the trap process
 * @version  V1.0
 * @date     12. December 2017
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csi_config.h>
#include <csi_core.h>

void (*trap_c_callback)(void);

void trap_c(uintptr_t *regs)
{
    int i;
    uint64_t vec = 0;

    vec = __get_MCAUSE() & 0x3FF;

    printf("CPU Exception: NO.0x%llx", vec);
    printf("\n");

    for (i = 0; i < 31; i++) {
        printf("x%d: %p\t", i + 1, (void *)regs[i]);

        if ((i % 4) == 3) {
            printf("\n");
        }
    }

    printf("\n");
    printf("mepc   : %p\n", (void *)regs[31]);
    printf("mstatus: %p\n", (void *)regs[32]);

    if (trap_c_callback) {
        trap_c_callback();
    }

    while (1);
}

