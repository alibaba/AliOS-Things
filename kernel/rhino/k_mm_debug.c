/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "k_api.h"
#include "debug_api.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#define print aos_cli_printf
#else
#define print printf
#endif

#if (RHINO_CONFIG_MM_DEBUG > 0)

#if (RHINO_CONFIG_MM_TLF > 0)

void print_block(k_mm_list_t *b)
{
    if (!b) {
        return;
    }

    print("%p ", b);

    if (b->buf_size & RHINO_MM_FREE) {
        if (b->dye != RHINO_MM_FREE_DYE) {
            print("!");
        } else {
            print(" ");
        }
        print("free ");
    } else {
        if (b->dye != RHINO_MM_CORRUPT_DYE) {
            print("!");
        } else {
            print(" ");
        }
        print("used ");
    }

    if (MM_GET_BUF_SIZE(b)) {
        print(" %6lu ", (unsigned long)MM_GET_BUF_SIZE(b));
    } else {
        print(" sentinel ");
    }

    print(" %8x ", b->dye);
    print(" 0x%-8x ", b->owner);

    if (b->buf_size & RHINO_MM_PREVFREE) {
        print("pre-free [%8p];", b->prev);
    } else {
        print("pre-used;");
    }

    if (b->buf_size & RHINO_MM_FREE) {
        print(" free[%8p,%8p] ", b->mbinfo.free_ptr.prev, b->mbinfo.free_ptr.next);
    }
    print("\r\n");
}

void dump_kmm_free_map(k_mm_head *mmhead)
{
    int i;

    k_mm_list_t *next;
    k_mm_list_t *tmp;

    if (!mmhead) {
        return;
    }

    print("freelist bitmap: 0x%x\r\n", (unsigned)mmhead->free_bitmap);
    print("address,  stat   size     dye     caller   pre-stat    point\r\n");

    for (i = 0; i < MM_BIT_LEVEL; i++) {
        next = mmhead->freelist[i];
        while (next) {
            print_block(next);
            tmp  = next->mbinfo.free_ptr.next;
            next = tmp;
        }
    }
}

void dump_kmm_map(k_mm_head *mmhead)
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    if (!mmhead) {
        return;
    }

    print("ALL BLOCKS\r\n");
    print("address,  stat   size     dye     caller   pre-stat    point\r\n");
    reginfo = mmhead->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        while (cur) {
            print_block(cur);
            if (MM_GET_BUF_SIZE(cur)) {
                next = MM_GET_NEXT_BLK(cur);
            } else {
                next = NULL;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }
}

void dump_kmm_statistic_info(k_mm_head *mmhead)
{
#if (K_MM_STATISTIC > 0)
    int i;
#endif

    if (!mmhead) {
        return;
    }

    debug_mm_overview(NULL);
    print("\r\n");

#if (K_MM_STATISTIC > 0)
    print("-----------------number of alloc times:-----------------\r\n");
    for (i = 0; i < MM_BIT_LEVEL; i++) {
        if (i % 4 == 0 && i != 0) {
            print("\r\n");
        }
        print("[2^%02d] bytes: %5d   |", (i + MM_MIN_BIT), mmhead->alloc_times[i]);
    }
    print("\r\n");
#endif
}

uint32_t dumpsys_mm_info_func(uint32_t len)
{
    MM_CRITICAL_ENTER(g_kmm_head);

    print("\r\n");
    print("------------------------------- all memory blocks --------------------------------- \r\n");
    print("g_kmm_head = %8x\r\n", (unsigned int)g_kmm_head);

    dump_kmm_map(g_kmm_head);
    print("\r\n");
    print("----------------------------- all free memory blocks ------------------------------- \r\n");
    dump_kmm_free_map(g_kmm_head);
    print("\r\n");
    print("------------------------- memory allocation statistic ------------------------------ \r\n");
    dump_kmm_statistic_info(g_kmm_head);

    MM_CRITICAL_EXIT(g_kmm_head);

    return RHINO_SUCCESS;
}
#endif /* #if (RHINO_CONFIG_MM_TLF > 0) */

#endif /* #if (RHINO_CONFIG_MM_DEBUG > 0) */

