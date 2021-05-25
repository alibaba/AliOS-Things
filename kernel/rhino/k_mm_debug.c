/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_api.h"

#if AOS_COMP_DEBUG
#include "aos/debug.h"
#endif

#if 0
#include "cli_console.h"

extern cli_console cli_uart_console;
#define KMM_CRITICAL_ENTER(head, cpsr)                 \
    do {                                               \
        cli_console *console = get_current_console();  \
        if(console && console == &cli_uart_console) {  \
            MM_CRITICAL_ENTER(head, cpsr);             \
        }                                              \
    } while(0);

#define KMM_CRITICAL_EXIT(head, cpsr)                  \
    do {                                               \
        cli_console *console = get_current_console();  \
        if(console && console == &cli_uart_console) {  \
            MM_CRITICAL_EXIT(head, cpsr);              \
        }                                              \
    } while(0);

#endif
#define KMM_CRITICAL_ENTER(head, cpsr) MM_CRITICAL_ENTER(head, cpsr)
#define KMM_CRITICAL_EXIT(head, cpsr)  MM_CRITICAL_EXIT(head, cpsr)

typedef int (*KMM_PRINT)(const char *fmt, ...);

KMM_PRINT print = printf;

#if (RHINO_CONFIG_MM_DEBUG > 0)
uint8_t g_mmlk_cnt;

#define MM_CHK 0xff

#if (RHINO_CONFIG_MM_TLF > 0)

void print_block(k_mm_list_t *b)
{
    if (!b) {
        return;
    }

    print("0x%p ", (void *)b);

    if (b->buf_size & MM_BUFF_FREE) {
        if (b->dye != MM_DYE_FREE) {
            print("!");
        } else {
            print(" ");
        }
        print("free ");
    } else {
        if (b->dye != MM_DYE_USED) {
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

    if (b->buf_size & MM_BUFF_FREE) {
        if (b->dye != MM_DYE_FREE) {
            print(" %8x ", b->dye);
        } else {
            print("  OK ");
        }
    } else {
        if (b->dye != MM_DYE_USED) {
            print(" %8x ", b->dye);
        } else {
            print("  OK ");
        }
    }

    print(" 0x%-8x ", b->owner);

#if (RHINO_CONFIG_MM_TRACE_LVL > 0)
    /* If double free, print last alloc trace maybe useful.
    This info is not useful if this mem alloc-and-freed by another module between.
    */
    //if ((b->buf_size & MM_BUFF_FREE) == 0)
    {
        int idx;
        print(" (%p", b->trace[0]);
        for (idx = 1 ; idx < RHINO_CONFIG_MM_TRACE_LVL ; idx++) {
            print(" <- %p", b->trace[idx]);
        }
        print(")");
    }
#endif

    print("\r\n");
}

void check_block(k_mm_list_t *b)
{
    if (!b) {
        return;
    }

    if (b->buf_size & MM_BUFF_FREE) {
        if (b->dye != MM_DYE_FREE) {
            print("0x%p ", (void *)b);
        }
    } else {
        if (b->dye != MM_DYE_USED) {
            print("0x%p ", (void *)b);
        }
    }
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
    print("   Address  Stat     Len  Chk    Caller\r\n");

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
    print("Blk_Addr    Stat     Len  Chk      Caller    Point\r\n");
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

#if (K_MM_STATISTIC > 0)
    for (i = 0; i < MM_BIT_LEVEL; i++) {
        if (i % 4 == 0 && i != 0) {
            print("\r\n");
        }
        print("[2^%02d] bytes: %5d   |", (i + MM_MIN_BIT), mmhead->alloc_times[i]);
    }
    print("\r\n");
#endif
}

#if (RHINO_CONFIG_MM_BLK > 0)
void dump_kmm_mblk_info(k_mm_head *mmhead)
{
    uint32_t idx;
    uint32_t slice = 0;
    mblk_list_t *blk_list;
    mblk_pool_t *mm_pool;

    size_t UsedSize = 0;
    size_t FreeSize = 0;
    size_t MaxUsedSz = 0;
    size_t AllocFail = 0;

    print(
                "---------------------------------------------------------------------------\r\n");
    print(
                "[POOL]| BlkSize    | UsedSize   | FreeSize   | MaxUsedSz  | AllocFail  |\r\n");

    mm_pool = mmhead->fix_pool;
    if (mm_pool != NULL) {
        for (idx = 0 ; idx < MM_BLK_SLICE_BIT ; idx++) {
            blk_list = &(mm_pool->blk_list[idx]);
            if (blk_list->slice_cnt > 0) {
                print("      | %-10d | %-10d | %-10d | %-10d | %-10d |\r\n",
                      blk_list->blk_size, \
                      (int)blk_list->nofree_cnt * blk_list->blk_size,
                      (int)blk_list->freelist_cnt * blk_list->blk_size,
                      (blk_list->slice_cnt - 1)*MM_BLK_SLICE_SIZE + blk_list->slice_offset,
                      (int)blk_list->fail_cnt);

                UsedSize += blk_list->nofree_cnt * blk_list->blk_size;
                FreeSize += blk_list->freelist_cnt * blk_list->blk_size;
                MaxUsedSz += (blk_list->slice_cnt - 1) * MM_BLK_SLICE_SIZE + blk_list->slice_offset;
                AllocFail += blk_list->fail_cnt;

                slice += blk_list->slice_cnt;
            }
        }
        print("Total |            | %-10d | %-10d | %-10d | %-10d |\r\n",
              UsedSize, FreeSize, MaxUsedSz, AllocFail);

        print("Pool Size %d, Free Slice size %p.\r\n",
              mm_pool->pool_end - mm_pool->pool_start,
              (intptr_t)(mm_pool->pool_end - mm_pool->pool_start - mm_pool->slice_cnt * MM_BLK_SLICE_SIZE));
    }
    print(
                "---------------------------------------------------------------------------\r\n");
}
#endif

#if (RHINO_CONFIG_KOBJ_LIST > 0)
void dump_kmm_task_info(k_mm_head *mmhead)
{
    k_mm_region_info_t *reginfo;
    k_mm_list_t *next, *cur;
    ktask_t     *task_owner;
    uint32_t     isr_alloc_size = 0;
    uint32_t     unk_alloc_size = 0;
    uint32_t     buff_size;

    klist_t     *listnode;
    ktask_t     *task;

    if (!mmhead) {
        return;
    }

    reginfo = mmhead->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        while (cur) {
            buff_size = MM_GET_BUF_SIZE(cur);
            if (cur->dye == MM_DYE_USED) {
                if (cur->owner_id == 0) {
                    isr_alloc_size += buff_size;
                } else if (cur->owner_id != MM_OWNER_ID_SELF) {
                    task_owner = debug_task_find_by_id(cur->owner_id);
                    if (task_owner != NULL) {
                        task_owner->task_alloc_size += buff_size;
                    } else {
                        unk_alloc_size += buff_size;
                    }
                }
            }

            if (buff_size) {
                next = MM_GET_NEXT_BLK(cur);
            } else {
                next = NULL;
            }
            cur = next;
        }
        reginfo = reginfo->next;
    }

    print("TaskName                 Prio  StackSize  HeapAllocSize\r\n");

    /* Traversing all task */
    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {

        task = krhino_list_entry(listnode, ktask_t, task_stats_item);

        if (task->task_alloc_size != 0) {
            print("%-24s %-5d %-10d %-10d\r\n",
                  task->task_name, task->prio, (int)task->stack_size, (int)task->task_alloc_size);
            task->task_alloc_size = 0;
        }
    }
#if (RHINO_CONFIG_MM_BLK > 0)
    isr_alloc_size -= (RHINO_CONFIG_MM_TLF_BLK_SIZE + MM_ALIGN_UP(sizeof(mblk_pool_t)));
#endif
    print("Allocated in ISR %d, Allocated by deleted task %d\r\n", (int)isr_alloc_size, (int)unk_alloc_size);
}
#else
void dump_kmm_task_info(k_mm_head *mmhead)
{
    print("need config RHINO_CONFIG_KOBJ_LIST > 0\r\n");
}
#endif

uint32_t dumpsys_mm_info_func(uint32_t mm_status)
{
    cpu_cpsr_t flags_cpsr = 0;

    if (mm_status != KMM_ERROR_LOCKED) {
        KMM_CRITICAL_ENTER(g_kmm_head, flags_cpsr);
    } else {
        print = printk;
    }

    print("\r\n");
    print("------------------------------- all memory blocks --------------------------------- \r\n");
    print("g_kmm_head = %8x\r\n", (unsigned int)g_kmm_head);
    dump_kmm_map(g_kmm_head);

    print("\r\n");
    print("----------------------------- all free memory blocks ------------------------------ \r\n");
    dump_kmm_free_map(g_kmm_head);

    print("\r\n");
    print("--------------------------- memory allocation statistic --------------------------- \r\n");
    dump_kmm_statistic_info(g_kmm_head);

    print("\r\n");
    print("-------------------------------[kernel] heap size overview -------------------------------- \r\n");
    debug_mm_overview(print);

#if (RHINO_CONFIG_MM_BLK > 0)
    print("\r\n");
    print("--------------------------- mmblk allocation statistic ---------------------------- \r\n");
    dump_kmm_mblk_info(g_kmm_head);
#endif

    print("\r\n");
    print("--------------------------- task allocation statistic ----------------------------- \r\n");
    dump_kmm_task_info(g_kmm_head);

    print("\r\n");
    print("----------------------------------------------------------------------------------- \r\n");

    if (mm_status != KMM_ERROR_LOCKED) {
        KMM_CRITICAL_EXIT(g_kmm_head, flags_cpsr);
    }

    return RHINO_SUCCESS;
}

uint32_t dumpsys_mm_overview_func(uint32_t len)
{
    cpu_cpsr_t flags_cpsr = 0;

    KMM_CRITICAL_ENTER(g_kmm_head, flags_cpsr);

    print("\r\n");
    print("-------------------------------[kernel] heap size overview -------------------------------- \r\n");
    debug_mm_overview(print);

#if (RHINO_CONFIG_MM_BLK > 0)
    print("\r\n");
    print("--------------------------- mmblk allocation statistic ---------------------------- \r\n");
    dump_kmm_mblk_info(g_kmm_head);
#endif

    print("\r\n");
    print("--------------------------- task allocation statistic ----------------------------- \r\n");
    dump_kmm_task_info(g_kmm_head);

    KMM_CRITICAL_EXIT(g_kmm_head, flags_cpsr);

    print("\r\n");
    print("----------------------------------------------------------------------------------- \r\n");

    return RHINO_SUCCESS;
}



void dump_kmm_chk(k_mm_head *mmhead, uint8_t care_cnt)
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    if (!mmhead || care_cnt == 0) {
        return;
    }

    reginfo = mmhead->regioninfo;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        while (cur) {
            if (cur->trace_id == care_cnt) {
                print_block(cur);
            } else if (care_cnt == MM_CHK) {
                check_block(cur);
            }
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

uint32_t dumpsys_mm_leakcheck(uint32_t call_cnt, int32_t query_index)
{
    cpu_cpsr_t flags_cpsr = 0;
    KMM_CRITICAL_ENTER(g_kmm_head, flags_cpsr);

    if (query_index < 0) {
        query_index = g_mmlk_cnt;
        g_mmlk_cnt = (uint8_t)call_cnt;
    } else if (query_index > g_mmlk_cnt) {
        printf("query_index should be less than %d\r\n", g_mmlk_cnt);
        return -1;
    }
    print("-----------------All new alloced blocks:----------------\r\n");
    print("Blk_Addr    Stat     Len  Chk      Caller    Point\r\n");

    dump_kmm_chk(g_kmm_head, query_index);

    print("--------------New alloced blocks info ends.-------------\r\n");
    print("\r\n");

    KMM_CRITICAL_EXIT(g_kmm_head, flags_cpsr);
    return RHINO_SUCCESS;
}

void dumpsys_mm_header_check(void)
{
    cpu_cpsr_t flags_cpsr = 0;
    KMM_CRITICAL_ENTER(g_kmm_head, flags_cpsr);

    print("-----------------All error blocks:----------------\r\n");

    dump_kmm_chk(g_kmm_head, MM_CHK);
    print("\r\n");

    KMM_CRITICAL_EXIT(g_kmm_head, flags_cpsr);
}

#endif /* #if (RHINO_CONFIG_MM_TLF > 0) */

#endif /* #if (RHINO_CONFIG_MM_DEBUG > 0) */

