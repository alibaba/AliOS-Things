/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>

#ifdef CONFIG_AOS_CLI
#include "aos/types.h"
#include "aos/cli.h"
extern int csp_printf(const char *fmt, ...);
#define print aos_cli_printf
#else
#define print printf
#endif

void show_mm()
{
#if (K_MM_STATISTIC > 0)
    print("[HEAP]|  TotalSize |   FreeSize |   UsedSize |  MinFreeSz |\r\n");
    print("      | %10d | %10d | %10d | %10d |\r\n",
          g_kmm_head->free_size + g_kmm_head->used_size,
          g_kmm_head->free_size,
          g_kmm_head->used_size,
          g_kmm_head->free_size + g_kmm_head->used_size - g_kmm_head->maxused_size);
#else
    print("K_MM_STATISTIC is cloesd\r\n");
#endif
}

#if (RHINO_CONFIG_MM_DEBUG > 0)


extern klist_t g_mm_region_list_head;

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)


static mm_scan_region_t g_mm_scan_region[AOS_MM_SCAN_REGION_MAX];
static void           **g_leak_match;
static uint32_t         g_recheck_flag = 0;

static uint32_t check_malloc_region(void *adress);
uint32_t if_adress_is_valid(void *adress);
uint32_t dump_mmleak(void);

uint32_t krhino_mm_leak_region_init(void *start, void *end)
{
    static uint32_t i = 0;

    if (i >= AOS_MM_SCAN_REGION_MAX) {
        return i;
    }

    if ((start == NULL) || (end == NULL)) {
        return i;
    }

    g_mm_scan_region[i].start = start;
    g_mm_scan_region[i].end   = end;

    i++;
    return i;
}


static uint32_t check_task_stack(ktask_t *task, void **p)
{
    uint32_t offset = 0;
    kstat_t  rst    = RHINO_SUCCESS;
    void    *start, *cur, *end;

    start = task->task_stack_base;
    end   = task->task_stack_base + task->stack_size;

    rst =  krhino_task_stack_cur_free(task, &offset);
    if (rst == RHINO_SUCCESS) {
        cur = task->task_stack_base + task->stack_size - offset;
    } else {
        k_err_proc(RHINO_SYS_SP_ERR);
        return 0;
    }

    if ((size_t)p >= (size_t)cur &&
        (size_t)p  < (size_t)end) {
        return 1;
    } else if ((size_t)p >= (size_t)start && (size_t)p  < (size_t)cur) {
        return 0;
    }
    /*maybe lost*/
    return 1;
}

static uint32_t check_if_in_stack(void **p)
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;



    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        if (1 == check_task_stack(task, p)) {
            return 1;
        }
    }
    return 0;
}

uint32_t scan_region(void *start, void *end, void *adress)
{
    void **p = (void **)((uint32_t)start & ~(sizeof(size_t) - 1));
    while ((void *)p < end) {
        if (NULL != p && adress  == *p) {
            g_leak_match = p;
            return 1;
        }

        p++;
    }

    return 0;
}
uint32_t check_mm_leak(uint8_t *adress)
{
    uint32_t rst = 0;
    uint32_t i;

    for (i = 0; i < AOS_MM_SCAN_REGION_MAX; i++) {

        if ((NULL == g_mm_scan_region[i].start) || (NULL == g_mm_scan_region[i].end)) {
            continue;
        }

        if (1 == scan_region(g_mm_scan_region[i].start, g_mm_scan_region[i].end,
                             adress)) {
            return 1;
        }
    }

    rst = check_malloc_region(adress);
    if (1 == rst) {
        return 1;
    }

    return 0;
}

static uint32_t recheck(void *start, void *end)
{
    void **p    = (void **)((uint32_t)start & ~(sizeof(size_t) - 1));

    g_recheck_flag = 1;

    while ((void *)p <= end) {
        if (NULL != p && 1 == if_adress_is_valid(*p)) {
            if ( 1 == check_mm_leak(*p)) {
                g_recheck_flag = 0;
                return 1;
            }
        }
        p++;
    }

    g_recheck_flag = 0;

    return 0;
}
#if (RHINO_CONFIG_MM_TLF > 0)
uint32_t check_malloc_region(void *adress)
{
    uint32_t            rst = 0;
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    NULL_PARA_CHK(g_kmm_head);

    reginfo = g_kmm_head->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        /*jump first blk*/
        cur = MM_GET_NEXT_BLK(cur);
        while (cur) {
            if (MM_GET_BUF_SIZE(cur)) {
                next = MM_GET_NEXT_BLK(cur);
                if (0 == g_recheck_flag && !(cur->buf_size & RHINO_MM_FREE)) {
                    if ((uint8_t *)krhino_cur_task_get()->task_stack_base >= cur->mbinfo.buffer
                        && (uint8_t *)krhino_cur_task_get()->task_stack_base < (uint8_t *)next) {
                        cur = next;
                        continue;
                    }
                    rst = scan_region(cur->mbinfo.buffer, (void *) next, adress);
                    if (1 == rst) {
                        return check_if_in_stack(g_leak_match);
                    }
                }
            } else {
                next = NULL;
            }
            if (1 == g_recheck_flag &&
                (uint32_t)adress >= (uint32_t)cur->mbinfo.buffer &&
                (uint32_t)adress < (uint32_t)next) {
                return 1;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }

    return 0;
}



uint32_t if_adress_is_valid(void *adress)
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    reginfo = g_kmm_head->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        /*jump first blk*/
        cur = MM_GET_NEXT_BLK(cur);
        while (cur) {
            if (MM_GET_BUF_SIZE(cur)) {
                next = MM_GET_NEXT_BLK(cur);
                if (!(cur->buf_size & RHINO_MM_FREE) &&
                    (size_t)adress >= (size_t)cur->mbinfo.buffer && (size_t)adress < (size_t)next ) {
                    return 1;
                }

            } else {
                next = NULL;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }

    return 0;
}


uint32_t dump_mmleak()
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    MM_CRITICAL_ENTER(g_kmm_head);

    reginfo = g_kmm_head->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        /*jump first blk*/
        cur = MM_GET_NEXT_BLK(cur);
        while (cur) {
            if (MM_GET_BUF_SIZE(cur)) {
                next = MM_GET_NEXT_BLK(cur);
                if (!(cur->buf_size & RHINO_MM_FREE) &&
                    0 == check_mm_leak(cur->mbinfo.buffer)
                    && 0 == recheck((void *)cur->mbinfo.buffer , (void *)next)) {
                    print("adress:0x%0x owner:0x%0x len:%-5d type:%s\r\n",
                          (void *)cur->mbinfo.buffer, cur->owner,
                          MM_GET_BUF_SIZE(cur), "leak");
                }

            } else {
                next = NULL;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }

    MM_CRITICAL_EXIT(g_kmm_head);

    return 0;
}
#endif
#endif

#if (RHINO_CONFIG_MM_TLF > 0)

void print_block(k_mm_list_t *b)
{
    if (!b) {
        return;
    }
    print("%p ", b);
    if (b->buf_size & RHINO_MM_FREE) {

#if (RHINO_CONFIG_MM_DEBUG > 0u)
        if (b->dye != RHINO_MM_FREE_DYE) {
            print("!");
        } else {
            print(" ");
        }
#endif
        print("free ");
    } else {
#if (RHINO_CONFIG_MM_DEBUG > 0u)
        if (b->dye != RHINO_MM_CORRUPT_DYE) {
            print("!");
        } else {
            print(" ");
        }
#endif
        print("used ");
    }
    if (MM_GET_BUF_SIZE(b)) {
        print(" %6lu ", (unsigned long)MM_GET_BUF_SIZE(b));
    } else {
        print(" sentinel ");
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    print(" %8x ", b->dye);
    print(" 0x%-8x ", b->owner);
#endif

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
    k_mm_list_t *next, *tmp;
    int         i;

    if (!mmhead) {
        return;
    }

    print("freelist bitmap: 0x%x\r\n", (unsigned)mmhead->free_bitmap);
    print("address,  stat   size     dye     caller   pre-stat    point\r\n");

    for (i = 0; i < MM_BIT_LEVEL; i++) {
        next = mmhead->freelist[i];
        while (next) {
            print_block(next);
            tmp = next->mbinfo.free_ptr.next;
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
#if (RHINO_CONFIG_MM_BLK > 0)
    mblk_pool_t *pool;
#endif

    if (!mmhead) {
        return;
    }
#if (K_MM_STATISTIC > 0)
    print("     free     |     used     |     maxused\r\n");
    print("  %10d  |  %10d  |  %10d\r\n",
          mmhead->free_size, mmhead->used_size, mmhead->maxused_size);
    print("\r\n");
    print("-----------------number of alloc times:-----------------\r\n");
    for (i = 0; i < MM_BIT_LEVEL; i++) {
        if (i % 4 == 0 && i != 0) {
            print("\r\n");
        }
        print("[2^%02d] bytes: %5d   |", (i + MM_MIN_BIT), mmhead->alloc_times[i]);
    }
    print("\r\n");
#endif
#if (RHINO_CONFIG_MM_BLK > 0)
    pool = mmhead->fix_pool;
    if ( pool != NULL ) {
        print("-----------------fix pool information:-----------------\r\n");
        print("     free     |     used     |     total\r\n");
        print("  %10d  |  %10d  |  %10d\r\n",
              pool->blk_avail * RHINO_CONFIG_MM_BLK_SIZE,
              (pool->blk_whole - pool->blk_avail)*RHINO_CONFIG_MM_BLK_SIZE,
              pool->blk_whole * RHINO_CONFIG_MM_BLK_SIZE);
    }
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

#endif

#endif

