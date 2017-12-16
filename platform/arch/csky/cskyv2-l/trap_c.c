/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>
#include <csi_kernel.h>

void trap_c(uint32_t *regs)
{
    int i;
    uint32_t vec = 0;

    asm volatile(
        "mfcr    %0, psr \n"
        "lsri    %0, 16 \n"
        "sextb   %0 \n"
        :"=r"(vec):);

    printf("CPU Exception : %d", vec);

    if (vec == 2) {
        printf(",maybe Stack-Guard stopped the invalid behaviors!");
    }

    printf("\n");

    for (i = 0; i < 16; i++) {
        printf("r%d: %08x\t", i, regs[i]);

        if ((i % 5) == 4) {
            printf("\n");
        }
    }

    printf("\n");
    printf("epsr: %8x\n", regs[16]);
    printf("epc : %8x\n", regs[17]);

//    csi_kernel_task_list(NULL, 0);

    while (1);
}

