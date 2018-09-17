/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 ******************************
 *          HEADERS
 ******************************
 */
#include "tee_dbg.h"

/*
 ******************************
 *          MACROS
 ******************************
 */
#define BACKTRACE_NUM 10

/*
 ******************************
 *          TYPES
 ******************************
 */
typedef struct arch_fplr_in_frame
{
    int pre_fp;
    int lr;
} arch_fplr_in_frame;

/*
 ******************************
 *          VARIABLES
 ******************************
 */

/*
 ******************************
 *          FUNCTIONS
 ******************************
 */
static inline int _backtrace(void **array, int size)
{
    if (size <= 0)
        return 0;

    arch_fplr_in_frame *cur_frame_p;
    int                 cur_fp;
    int                 cur_lr;
    int                 cnt = 0;
    int                 ret = 0;

    __asm__ __volatile("mov %0, r8\n" : "=r"(cur_fp));
    __asm__ __volatile("mov %0, lr\n" : "=r"(cur_lr));

    if (cur_fp != 0) {
        array[cnt++] = (void *)cur_lr;
        cur_frame_p  = (arch_fplr_in_frame *)(cur_fp);
    } else
        return -1;

    if (cur_frame_p->pre_fp != 0)
        cur_frame_p = (arch_fplr_in_frame *)(cur_frame_p->pre_fp);
    else
        return -1;

    while ((cnt <= size) && (&cur_frame_p->pre_fp > 0)) {
        array[cnt++] = (void *)cur_frame_p->lr;
        if (cur_frame_p->pre_fp > 0)
            cur_frame_p = (arch_fplr_in_frame *)(cur_frame_p->pre_fp);
        else
            break;
    }

    ret = ((cnt <= size) ? cnt : size);

    return ret;
}

void arch_backtrace(void)
{
    int   size = 0, i = 0;
    void *a[BACKTRACE_NUM];

    size = _backtrace(a, BACKTRACE_NUM);
#if 0
    int i = 0;
    printf("BACKTRACE:\n");
    for(i = 0; i < size; i ++) {
        printf("0x%x\n", ((uint32_t *)a[i]));
    }
#endif
    uart_print_string("BACKTRACE:\n");
    for (i = 0; i < size; i++) {
        uart_print_uint32(((uint32_t *)a)[i]);
        uart_print_string("\n");
    }
}
