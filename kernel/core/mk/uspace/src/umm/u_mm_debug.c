/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "k_api.h"
#include "aos/cli.h"
#include "aos/hal/hal_watchdog.h"
#include "umm.h"
#include "u_default_config.h"

#if (RHINO_CONFIG_UCLI > 0)
#include "cli_task.h"
#endif

typedef int (*UMM_PRINT)(const char *fmt, ...);

UMM_PRINT print = printk;

void debug_mm_overview(int (*print_func)(const char *fmt, ...));

uint32_t debug_task_id_now()
{
    return 0xa5a5;
}
ktask_t *debug_task_find_by_id(uint32_t task_id)
{
    return NULL;
}

#if (RHINO_CONFIG_UMM_DEBUG > 0)
uint8_t g_mmlk_cnt;

#define MM_CHK 0xff

#if (RHINO_CONFIG_MM_TLF > 0)

void print_block(k_mm_list_t *b)
{
    if (!b) {
        return;
    }

    print("0x%08x ", (uintptr_t)b);

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

#if (RHINO_CONFIG_UMM_TRACE_LVL > 0)
    /* If double free, print last alloc trace maybe useful.
    This info is not useful if this mem alloc-and-freed by another module between.
    */
    //if ((b->buf_size & MM_BUFF_FREE) == 0)
    {
        int idx;
        print(" (%p", b->trace[0]);
        for (idx = 1 ; idx < RHINO_CONFIG_UMM_TRACE_LVL ; idx++) {
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
            print("0x%08x ", (uintptr_t)b);
        }
    } else {
        if (b->dye != MM_DYE_USED) {
            print("0x%08x ", (uintptr_t)b);
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

        print("Pool Size %d, Free Slice size %d.\r\n",
              mm_pool->pool_end - mm_pool->pool_start,
              (int)(mm_pool->pool_end - mm_pool->pool_start - mm_pool->slice_cnt * MM_BLK_SLICE_SIZE));
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
}
#else
void dump_kmm_task_info(k_mm_head *mmhead)
{
    print("need config RHINO_CONFIG_KOBJ_LIST > 0\r\n");
}
#endif

uint32_t dumpsys_mm_info_func(uint32_t mm_status)
{
    if (mm_status != UMM_ERROR_LOCKED) {
        printf("%s start\r\n", __func__);
        u_mm_critical_enter();
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
    print("------------------------------- [%s] heap size overview -------------------------------- \r\n",
          RHINO_CONFIG_APP_NAME);
    debug_mm_overview(print);

#if (RHINO_CONFIG_MM_BLK > 0)
    print("\r\n");
    print("--------------------------- mmblk allocation statistic ---------------------------- \r\n");
    dump_kmm_mblk_info(g_kmm_head);
#endif

    if (mm_status != UMM_ERROR_LOCKED) {
        u_mm_critical_exit();
    }

    return RHINO_SUCCESS;
}

uint32_t dumpsys_mm_overview_func(uint32_t len)
{
    print("%s start\r\n", __func__);

    u_mm_critical_enter();

    print("\r\n");
    print("--------------------------- [%s] heap size overview -------------------------------- \r\n",
          RHINO_CONFIG_APP_NAME);
    debug_mm_overview(print);

#if (RHINO_CONFIG_MM_BLK > 0)
    print("\r\n");
    print("--------------------------- mmblk allocation statistic ---------------------------- \r\n");
    dump_kmm_mblk_info(g_kmm_head);
#endif

    u_mm_critical_exit();

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
    printf("%s start\r\n", __func__);

    u_mm_critical_enter();

    if (query_index < 0) {
        query_index = g_mmlk_cnt;
        g_mmlk_cnt = (uint8_t)call_cnt;
    } else if (query_index > g_mmlk_cnt) {
        printf("query_index should be less than %d\r\n", g_mmlk_cnt);
        u_mm_critical_exit();
        return -1;
    }
    print("-----------------All new alloced blocks:----------------\r\n");
    print("Blk_Addr    Stat     Len  Chk      Caller    Point\r\n");

    dump_kmm_chk(g_kmm_head, query_index);

    print("--------------New alloced blocks info ends.-------------\r\n");
    print("\r\n");

    u_mm_critical_exit();
    return RHINO_SUCCESS;
}

void dumpsys_mm_header_check(void)
{
    printf("%s start\r\n", __func__);

    u_mm_critical_enter();

    print("-----------------All error blocks:----------------\r\n");

    dump_kmm_chk(g_kmm_head, MM_CHK);
    print("\r\n");

    u_mm_critical_exit();
}

void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        aos_cli_printf("dumpsys help:\r\n");
        aos_cli_printf("dumpsys mm      : show uspace memory status.\r\n");
        aos_cli_printf("dumpsys mm_info : show the memory has alloced.\r\n");
        return;
    }

    if (0 == strcmp(argv[1], "mm_info")) {
        dumpsys_mm_info_func(0);
    } else if (0 == strcmp(argv[1], "mm")) {
        dumpsys_mm_overview_func(0);
    }
}

#if (K_MM_STATISTIC > 0)
#define UMM_TYPE "      | 0x%08X | 0x%08X | 0x%08X | 0x%08X | 0x%08X    |\r\n"

void debug_mm_overview(int (*print_func)(const char *fmt, ...))
{
#if (RHINO_CONFIG_MM_BLK > 0)
    mblk_info_t pool_info;
#endif

    size_t max_free_blk_size = 0;
#if (RHINO_CONFIG_MM_TLF > 0)
    max_free_blk_size = krhino_mm_max_free_size_get();
#endif

    if (print_func == NULL) {
        print_func = printf;
    }

    print_func(
                "---------------------------------------------------------------------------\r\n");
    print_func(
                "[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |\r\n");

    print_func(UMM_TYPE, (int)(g_kmm_head->free_size + g_kmm_head->used_size),
               (int)g_kmm_head->free_size,
               (int)g_kmm_head->used_size,
               (int)(g_kmm_head->free_size + g_kmm_head->used_size - g_kmm_head->maxused_size),
               (int)max_free_blk_size);
    print_func(
                "---------------------------------------------------------------------------\r\n");

#if (RHINO_CONFIG_MM_BLK > 0)
    (void)krhino_mblk_info_nolock((mblk_pool_t *)g_kmm_head->fix_pool, &pool_info);
    print_func(
                "[POOL]| PoolSz     | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |\r\n");

    print_func(UMM_TYPE, (int)pool_info.pool_size,
               (int)(pool_info.pool_size - pool_info.used_size),
               (int)pool_info.used_size,
               (int)(pool_info.pool_size - pool_info.max_used_size),
               (int)pool_info.max_blk_size);
    print_func(
                "---------------------------------------------------------------------------\r\n");
#endif
}
#else
void debug_mm_overview(int (*print_func)(const char *fmt, ...))
{
    if (print_func == NULL) {
        print_func = printf;
    }
    print_func("K_MM_STATISTIC in k_config.h is closed!\r\n");
}
#endif

static void mem_leak(char *buf, int32_t len, int32_t argc, char **argv)
{
    static int call_cnt = 0;
    int query_index = -1;

    if (call_cnt == 0) {
        printf("memory leak check start.\r\n");
    }
    if (argc == 2) {
        query_index = strtoul(argv[1], NULL, 0);
        if (query_index > call_cnt) {
            query_index = -1;
        }
    } else {
        call_cnt++;
        printf("\r\nAdd tag %d when malloc\r\n", call_cnt);
    }

    dumpsys_mm_leakcheck(call_cnt, query_index);
}

static void mem_check(char *buf, int32_t len, int32_t argc, char **argv)
{
    dumpsys_mm_header_check();
}

#if (RHINO_CONFIG_UCLI_TASK > 0)
static const struct cli_command umm_builtin_cmds[] = {
    {"dumpsys", "dump mm statics", dumpsys_cmd},
    {"mmlk", "memory leak info", mem_leak},
    {"mmc", "memory info check", mem_check},
};

void umm_cli_cmds_register(void)
{
    aos_cli_register_commands(umm_builtin_cmds,
                          sizeof(umm_builtin_cmds) / sizeof(umm_builtin_cmds[0]));
}
#endif

#endif /* #if (RHINO_CONFIG_MM_TLF > 0) */

#endif /* #if (RHINO_CONFIG_UMM_DEBUG > 0) */
