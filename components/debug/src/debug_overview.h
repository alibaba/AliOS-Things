/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_OVERVIEW_H
#define DEBUG_OVERVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_PANIC_IN_USER         1
#define DEBUG_PANIC_IN_KERNEL       0

typedef struct {
    int32_t total_size;
    int32_t free_size;
    int32_t used_size;
    int32_t maxused_size;
    int32_t maxblk_size;
} debug_mm_info_t;

/**
 * convert int to ascii(HEX)
 * while using format % in libc, malloc/free is involved.
 * this function avoid using malloc/free. so it works when heap corrupt.
 * @param[in]   num      number
 * @param[in]   str      fix 8 character str
 * @return  str
 */
char *k_int2str(int num, char *str);

/**
 * This function print the overview of heap
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_mm_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of tasks
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_task_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of buf_queues
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_buf_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of queues
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of sems
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_sem_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of mutex
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_mutex_overview(int (*print_func)(const char *fmt, ...));

/**
 * This function print the overview of all
 * @param[in]   print_func    function to output information, NULL for
 * "printf"
 */
void debug_overview(int (*print_func)(const char *fmt, ...));

void debug_heap_info_get(debug_mm_info_t *mm_info);
void debug_pool_info_get(debug_mm_info_t *mm_info);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_OVERVIEW_H */
