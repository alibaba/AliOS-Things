/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "k_api.h"
#include "debug_api.h"

/* convert int to ascii(HEX)
   while using format % in libc, malloc/free is involved.
   this function avoid using malloc/free. so it works when heap corrupt. */
char *k_int2str(int num, char *str)
{
    char         index[] = "0123456789ABCDEF";
    unsigned int usnum   = (unsigned int)num;

    str[7] = index[usnum % 16];
    usnum /= 16;
    str[6] = index[usnum % 16];
    usnum /= 16;
    str[5] = index[usnum % 16];
    usnum /= 16;
    str[4] = index[usnum % 16];
    usnum /= 16;
    str[3] = index[usnum % 16];
    usnum /= 16;
    str[2] = index[usnum % 16];
    usnum /= 16;
    str[1] = index[usnum % 16];
    usnum /= 16;
    str[0] = index[usnum % 16];
    usnum /= 16;

    return str;
}

#if (K_MM_STATISTIC > 0)
void debug_mm_overview(int (*print_func)(const char *fmt, ...))
{
#if (RHINO_CONFIG_MM_BLK > 0)
    mblk_info_t pool_info;
#endif

    size_t max_free_blk_size = 0;
#if (RHINO_CONFIG_MM_TLF > 0)
    max_free_blk_size = krhino_mm_max_free_size_get();
#endif

    char s_heap_overview[] =
                "      | 0x         | 0x         | 0x         | 0x         | 0x            |\r\n";

    print_func(
                "---------------------------------------------------------------------------\r\n");
    print_func(
                "[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |\r\n");

    k_int2str((int)(g_kmm_head->free_size + g_kmm_head->used_size),
              &s_heap_overview[10]);
    k_int2str((int)g_kmm_head->free_size, &s_heap_overview[23]);
    k_int2str((int)g_kmm_head->used_size, &s_heap_overview[36]);
    k_int2str((int)(g_kmm_head->free_size + g_kmm_head->used_size -
                    g_kmm_head->maxused_size),
              &s_heap_overview[49]);
    k_int2str((int)max_free_blk_size, &s_heap_overview[62]);
    print_func(s_heap_overview);

    print_func(
                "---------------------------------------------------------------------------\r\n");

#if (RHINO_CONFIG_MM_BLK > 0)
    (void)krhino_mblk_info_nolock((mblk_pool_t *)g_kmm_head->fix_pool, &pool_info);
    print_func(
                "[POOL]| PoolSz     | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |\r\n");

    k_int2str((int)pool_info.pool_size, &s_heap_overview[10]);
    k_int2str((int)(pool_info.pool_size - pool_info.used_size), &s_heap_overview[23]);
    k_int2str((int)pool_info.used_size, &s_heap_overview[36]);
    k_int2str((int)(pool_info.pool_size - pool_info.max_used_size), &s_heap_overview[49]);
    k_int2str((int)pool_info.max_blk_size, &s_heap_overview[62]);
    print_func(s_heap_overview);

    print_func(
                "---------------------------------------------------------------------------\r\n");
#endif
}
#else
void debug_mm_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("K_MM_STATISTIC in k_config.h is closed!\r\n");
}
#endif

#if (RHINO_CONFIG_KOBJ_LIST > 0)
static void debug_task_show(int (*print_func)(const char *fmt, ...), ktask_t *task)
{
    size_t        free_size;
    int           stat_idx;
    int           i;
    int           cpu_idx;
    char         *cpu_stat[] = { "UNK",      "RDY", "PEND",    "SUS",
                                 "PEND_SUS", "SLP", "SLP_SUS", "DEL"
                               };
    const name_t *task_name;

    char s_task_overview[] = "                              0x         0x      "
                             "   0x        (0x        )\r\n";

    if (krhino_task_stack_min_free(task, &free_size) != RHINO_SUCCESS) {
        free_size = 0;
    }
    free_size *= sizeof(cpu_stack_t);

    /* set name */
    task_name = task->task_name == NULL ? "anonym" : task->task_name;
    for (i = 0; i < 20; i++) {
        s_task_overview[i] = ' ';
    }
    for (i = 0; i < strlen(task_name); i++) {
        s_task_overview[i] = task_name[i];
    }

    /* set state */
    stat_idx = task->task_state >= sizeof(cpu_stat) / sizeof(char *)
               ? 0
               : task->task_state;
    for (i = 21; i < 29; i++) {
        s_task_overview[i] = ' ';
    }
    for (i = 21; i < 29; i++) {
        if (cpu_stat[stat_idx][i - 21] == '\0') {
            break;
        }
        s_task_overview[i] = cpu_stat[stat_idx][i - 21];
    }

    /* set stack priority */
    k_int2str(task->prio, &s_task_overview[32]);

    /* set stack info */
    k_int2str((int)task->task_stack_base, &s_task_overview[43]);
    k_int2str((int)task->stack_size * sizeof(cpu_stack_t),
              &s_task_overview[54]);
    k_int2str((int)free_size, &s_task_overview[65]);
    cpu_idx = 65 + 17;

    (void)cpu_idx;
#if (RHINO_CONFIG_CPU_NUM > 1)
    s_task_overview[cpu_idx] = (uint8_t)('0' + task->cpu_binded);
    s_task_overview[cpu_idx + 10] = (uint8_t)('0' + task->cpu_num);
    s_task_overview[cpu_idx + 20] = (uint8_t)('0' + task->cur_exc);
#endif

    /* print */
    if (print_func == NULL) {
        print_func = printf;
    }
    print_func(s_task_overview);

}
void debug_task_overview(int (*print_func)(const char *fmt, ...))
{
    klist_t      *listnode;
    ktask_t      *task;

    print_func("---------------------------------------------------------------"
               "-----------\r\n");
#if (RHINO_CONFIG_CPU_NUM > 1)
    print_func("TaskName             State    Prio       Stack      StackSize "
               "(MinFree)      cpu_binded    cpu_num    cur_exc\r\n");
#else
    print_func("TaskName             State    Prio       Stack      StackSize "
               "(MinFree)\r\n");
#endif
    print_func("---------------------------------------------------------------"
               "-----------\r\n");

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        debug_task_show(print_func, task);
    }
}
#else
void debug_task_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("RHINO_CONFIG_KOBJ_LIST in k_config.h is closed!\r\n");
}
#endif /* #if (RHINO_CONFIG_KOBJ_LIST > 0) */

#if (RHINO_CONFIG_BUF_QUEUE > 0)
#if (RHINO_CONFIG_KOBJ_LIST > 0)
void debug_buf_queue_overview(int (*print_func)(const char *fmt, ...))
{
    int           i;
    klist_t      *listnode;
    klist_t      *blk_list_head;
    ktask_t      *task;
    kbuf_queue_t *buf_queue;
    const name_t *task_name;
    char s_buf_queue_overview[] = "0x         0x         0x         0x         "
                                  "0x                             \r\n";

    print_func(
                "------------------------------------------------------------------\r\n");
    print_func(
                "BufQueAddr TotalSize  PeakNum    CurrNum    MinFreeSz  TaskWaiting\r\n");
    print_func(
                "------------------------------------------------------------------\r\n");
    for (listnode = g_kobj_list.buf_queue_head.next;
         listnode != &g_kobj_list.buf_queue_head; listnode = listnode->next) {

        buf_queue = krhino_list_entry(listnode, kbuf_queue_t, buf_queue_item);
        if (buf_queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
            print_func("BufQueue Type error!\r\n");
            break;
        }

        /* set buf_queue information */
        k_int2str((int)buf_queue, &s_buf_queue_overview[2]);
        k_int2str((int)(buf_queue->ringbuf.end - buf_queue->ringbuf.buf),
                  &s_buf_queue_overview[13]);
        k_int2str((int)buf_queue->peak_num, &s_buf_queue_overview[24]);
        k_int2str((int)buf_queue->cur_num, &s_buf_queue_overview[35]);
        k_int2str((int)buf_queue->min_free_buf_size, &s_buf_queue_overview[46]);

        /* set pending task name */
        blk_list_head = &buf_queue->blk_obj.blk_list;
        if (is_klist_empty(blk_list_head)) {
            task_name = " ";
        } else {
            task = krhino_list_entry(blk_list_head->next, ktask_t, task_list);
            task_name = task->task_name == NULL ? "anonym" : task->task_name;
        }

        for (i = 0; i < 20; i++) {
            s_buf_queue_overview[55 + i] = ' ';
        }
        for (i = 0; i < 20; i++) {
            if (task_name[i] == '\0') {
                break;
            }
            s_buf_queue_overview[55 + i] = task_name[i];
        }

        /* print */
        print_func(s_buf_queue_overview);
    }
}
#else
void debug_buf_queue_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("RHINO_CONFIG_KOBJ_LIST in k_config.h is closed!\r\n");
}
#endif
#endif

#if (RHINO_CONFIG_QUEUE > 0)
#if (RHINO_CONFIG_KOBJ_LIST > 0)
void debug_queue_overview(int (*print_func)(const char *fmt, ...))
{
    int           i;
    klist_t      *listnode;
    klist_t      *blk_list_head;
    ktask_t      *task;
    kqueue_t     *queue;
    const name_t *task_name;
    char          s_queue_overview[] =
                "0x         0x         0x         0x                             \r\n";

    print_func("-------------------------------------------------------\r\n");
    print_func("QueAddr    TotalSize  PeakNum    CurrNum    TaskWaiting\r\n");
    print_func("-------------------------------------------------------\r\n");

    for (listnode = g_kobj_list.queue_head.next;
         listnode != &g_kobj_list.queue_head; listnode = listnode->next) {

        queue = krhino_list_entry(listnode, kqueue_t, queue_item);
        if (queue->blk_obj.obj_type != RHINO_QUEUE_OBJ_TYPE) {
            print_func("Queue Type error!\r\n");
            break;
        }

        /* set queue information */
        k_int2str((int)queue->msg_q.queue_start, &s_queue_overview[2]);
        k_int2str((int)queue->msg_q.size, &s_queue_overview[13]);
        k_int2str((int)queue->msg_q.peak_num, &s_queue_overview[24]);
        k_int2str((int)queue->msg_q.cur_num, &s_queue_overview[35]);

        /* set pending task name */
        blk_list_head = &queue->blk_obj.blk_list;
        if (is_klist_empty(blk_list_head)) {
            task_name = " ";
        } else {
            task = krhino_list_entry(blk_list_head->next, ktask_t, task_list);
            task_name = task->task_name == NULL ? "anonym" : task->task_name;
        }

        for (i = 0; i < 20; i++) {
            s_queue_overview[44 + i] = ' ';
        }
        for (i = 0; i < 20; i++) {
            if (task_name[i] == '\0') {
                break;
            }
            s_queue_overview[44 + i] = task_name[i];
        }

        /* print */
        print_func(s_queue_overview);
    }
}
#else
void debug_queue_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("RHINO_CONFIG_KOBJ_LIST in k_config.h is closed!\r\n");
}
#endif
#endif

#if (RHINO_CONFIG_SEM > 0)
#if (RHINO_CONFIG_KOBJ_LIST > 0)
void debug_sem_overview(int (*print_func)(const char *fmt, ...))
{
    int           i, cnt = 0;
    ksem_t       *sem;
    ktask_t      *task;
    klist_t      *listnode;
    klist_t      *blk_list_head;
    const name_t *task_name;
    char          s_sem_overview[] =
                "0x         0x         0x                             \r\n";
    char          s_sem_total[] =
                "Total: 0x         \r\n";

    print_func("--------------------------------------------\r\n");
    print_func("SemAddr    Count      PeakCount  TaskWaiting\r\n");
    print_func("--------------------------------------------\r\n");

    for (listnode = g_kobj_list.sem_head.next;
         listnode != &g_kobj_list.sem_head; listnode = listnode->next) {

        cnt++;

        sem = krhino_list_entry(listnode, ksem_t, sem_item);
        if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
            print_func("Sem Type error sem addr %p!\r\n", sem);
            break;
        }

        /* only show sem waited by task */
        blk_list_head = &sem->blk_obj.blk_list;
        if (is_klist_empty(blk_list_head)) {
            continue;
        }

        /* set sem information */
        k_int2str((int)sem, &s_sem_overview[2]);
        k_int2str((int)sem->count, &s_sem_overview[13]);
        k_int2str((int)sem->peak_count, &s_sem_overview[24]);

        /* set pending task name */
        blk_list_head = &sem->blk_obj.blk_list;
        if (is_klist_empty(blk_list_head)) {
            task_name = " ";
        } else {
            task = krhino_list_entry(blk_list_head->next, ktask_t, task_list);
            task_name = task->task_name == NULL ? "anonym" : task->task_name;
        }

        for (i = 0; i < 20; i++) {
            s_sem_overview[33 + i] = ' ';
        }
        for (i = 0; (i < 20) && i < strlen(task_name); i++) {
            s_sem_overview[33 + i] = task_name[i];
        }

        /* print */
        print_func(s_sem_overview);
    }
    k_int2str(cnt, &s_sem_total[9]);
    print_func(s_sem_total);
}
#else
void debug_sem_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("RHINO_CONFIG_KOBJ_LIST in k_config.h is closed!\r\n");
}
#endif
#endif /* RHINO_CONFIG_SEM */

#if (RHINO_CONFIG_KOBJ_LIST > 0)
void debug_mutex_overview(int (*print_func)(const char *fmt, ...))
{
    int           i, cnt = 0;
    kmutex_t     *mutex;
    ktask_t      *task;
    klist_t      *listnode;
    klist_t      *blk_list_head;
    const name_t *task_name;
    char          s_mutex_overview[] =
                "0x                              0x                              \r\n";
    char          s_mutex_total[] =
                "Total: 0x         \r\n";

    print_func("--------------------------------------------\r\n");
    print_func("MutexAddr  TaskOwner            NestCnt    TaskWaiting\r\n");
    print_func("--------------------------------------------\r\n");

    for (listnode = g_kobj_list.mutex_head.next;
         listnode != &g_kobj_list.mutex_head; listnode = listnode->next) {

        cnt++;

        mutex = krhino_list_entry(listnode, kmutex_t, mutex_item);
        if (mutex->blk_obj.obj_type != RHINO_MUTEX_OBJ_TYPE) {
            print_func("Mutex Type error!\r\n");
            break;
        }

        /* only show mutex waited by task */
        task = mutex->mutex_task;
        if (task == NULL) {
            continue;
        }

        /* set mutex information */
        k_int2str((int)mutex, &s_mutex_overview[2]);
        k_int2str((int)mutex->owner_nested, &s_mutex_overview[34]);

        /* set owner task name */
        task_name = task->task_name == NULL ? "anonym" : task->task_name;
        for (i = 0; i < 20; i++) {
            s_mutex_overview[11 + i] = ' ';
        }
        for (i = 0; i <= strlen(task_name); i++) {
            s_mutex_overview[11 + i] = task_name[i];
        }

        /* set pending task name */
        blk_list_head = &mutex->blk_obj.blk_list;
        if (!is_klist_empty(blk_list_head)) {
            task = krhino_list_entry(blk_list_head->next, ktask_t, task_list);
            task_name = task->task_name == NULL ? "anonym" : task->task_name;
            for (i = 0; i < 20; i++) {
                s_mutex_overview[43 + i] = ' ';
            }
            for (i = 0; (i < 20) && (i < strlen(task_name)); i++) {
                s_mutex_overview[43 + i] = task_name[i];
            }
        }

        /* print */
        print_func(s_mutex_overview);
    }

    k_int2str(cnt, &s_mutex_total[9]);
    print_func(s_mutex_total);
}
#else
void debug_mutex_overview(int (*print_func)(const char *fmt, ...))
{
    print_func("RHINO_CONFIG_KOBJ_LIST in k_config.h is closed!\r\n");
}
#endif

void debug_overview(int (*print_func)(const char *fmt, ...))
{
#if (RHINO_CONFIG_MM_TLF > 0)
    print_func("========== Heap Info  ==========\r\n");
    debug_mm_overview(print_func);
#endif
    print_func("========== Task Info  ==========\r\n");
    debug_task_overview(print_func);
#if (RHINO_CONFIG_QUEUE > 0)
    print_func("========== Queue Info ==========\r\n");
    debug_queue_overview(print_func);
#endif
#if (RHINO_CONFIG_BUF_QUEUE > 0)
    print_func("======== Buf Queue Info ========\r\n");
    debug_buf_queue_overview(print_func);
#endif
#if (RHINO_CONFIG_SEM > 0)
    print_func("========== Sem Waiting ==========\r\n");
    debug_sem_overview(print_func);
#endif
    print_func("========= Mutex Waiting =========\r\n");
    debug_mutex_overview(print_func);
}

#if (K_MM_STATISTIC > 0)
#if (RHINO_CONFIG_MM_TLF > 0)
void debug_heap_info_get(debug_mm_info_t *mm_info)
{
    mm_info->total_size   = g_kmm_head->free_size + g_kmm_head->used_size;
    mm_info->free_size    = g_kmm_head->free_size;
    mm_info->used_size    = g_kmm_head->used_size;
    mm_info->maxused_size = g_kmm_head->maxused_size;
    mm_info->maxblk_size  = -1; /* not support current, will be replaced by krhino_mm_max_free_size_get() */
    return;
}
#endif

#if (RHINO_CONFIG_MM_BLK > 0)
void debug_pool_info_get(debug_mm_info_t *mm_info)
{
    mblk_info_t pool_info;
    (void)krhino_mblk_info_nolock((mblk_pool_t *)g_kmm_head->fix_pool, &pool_info);
    mm_info->total_size   = pool_info.pool_size;
    mm_info->free_size    = pool_info.pool_size - pool_info.used_size;
    mm_info->used_size    = pool_info.used_size;
    mm_info->maxused_size = pool_info.max_used_size;
    mm_info->maxblk_size  = pool_info.max_blk_size;
    return;
}
#endif
#endif

