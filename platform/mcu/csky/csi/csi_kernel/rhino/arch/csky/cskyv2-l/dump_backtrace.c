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

#include <csi_config.h>

#ifdef CONFIG_BACKTRACE

#include <stdio.h>
#include <stdlib.h>
#include <backtrace.h>

#define BT_SIZE 40
void *g_bt_buffer[BT_SIZE];
#define BT_STACK_SIZE 600
int g_bt_stack[BT_STACK_SIZE / 4];
char *g_dumpbuf = NULL;
int g_dump_len = 0;
int g_dump_max = 0;

void dump_backtrace(void)
{
    int    i;
    int    nptr;

    g_dump_len = 0;

    nptr = backtrace(g_bt_buffer, BT_SIZE);

    if (nptr <= 0) {
        printf("error: backtrace_symbols\n");
        return;
    }

    for (i = 0; i < nptr; i++) {
        if (g_dumpbuf == NULL) {
            printf("[<%p>]\n", g_bt_buffer[i]);
        } else {
            g_dump_len += snprintf(&g_dumpbuf[g_dump_len], g_dump_max - g_dump_len, "[<%p>]\n", g_bt_buffer[i]);

            if (g_dump_len >= g_dump_max) {
                printf("error, user buffer no space \n");
                return;
            }
        }
    }
}


void __attribute__((naked))csky_show_backtrace(void *stack)
{
    asm volatile(
        "subi    sp, 68\n\t"
        "stm     r0-r13, (sp)\n\t"
        "mtcr    sp, cr<14, 1>\n\t"
        "stw     r15, (sp, 56)\n\t"
        "mov     sp, %0\n\t"

        "lrw     r0, g_bt_stack\n\t"
        "mov     r1, sp\n\t"
        "sub     r1, %1\n\t"
        "mov     r2, %1\n\t"
        "mov     r4, %1\n\t"
        "jbsr    memcpy\n\t"

        "ldw     r8, (sp, 32)\n\t"
        "ldw     r15, (sp, 64)\n\t"
        "subi    sp, 16\n\t"
        "stw     r8, (sp, 0)\n\t"
        "stw     r15, (sp, 4)\n\t"
        "mov     r8, sp\n\t"
        "jbsr    dump_backtrace\n\t"

        "addi    sp, 16\n\t"
        "lrw     r1, g_bt_stack\n\t"
        "mov     r0, sp\n\t"
        "sub     r0, r4\n\t"
        "mov     r2, r4\n\t"
        "jbsr    memcpy\n\t"

        "mfcr    sp, cr<14, 1>\n\t"
        "ldw     r15, (sp, 56)\n\t"
        "ldm     r0-r13, (sp)\n\t"
        "addi    sp, 68\n\t"
        "jmp     r15\n\t"
        ::"r"(stack), "r"(BT_STACK_SIZE):);

}

int csky_task_backtrace(void *stack, void *buf, int len)
{
    g_dumpbuf = buf;
    g_dump_max = len;

    csky_show_backtrace(stack);

    return g_dump_len;
}
#endif
