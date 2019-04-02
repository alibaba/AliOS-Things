/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_OVERVIEW_H
#define K_OVERVIEW_H

#ifdef __cplusplus
extern "C"
{
#endif

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
    void krhino_mm_overview(int (*print_func)(const char *fmt, ...));

    /**
     * This function print the overview of tasks
     * @param[in]   print_func    function to output information, NULL for
     * "printf"
     */
    void krhino_task_overview(int (*print_func)(const char *fmt, ...));

    /**
     * This function print the overview of buf_queues
     * @param[in]   print_func    function to output information, NULL for
     * "printf"
     */
    void krhino_buf_queue_overview(int (*print_func)(const char *fmt, ...));

    /**
     * This function print the overview of queues
     * @param[in]   print_func    function to output information, NULL for
     * "printf"
     */
    void krhino_queue_overview(int (*print_func)(const char *fmt, ...));

    /**
     * This function print the overview of sems
     * @param[in]   print_func    function to output information, NULL for
     * "printf"
     */
    void krhino_sem_overview(int (*print_func)(const char *fmt, ...));

    /**
     * This function print the overview of all
     * @param[in]   print_func    function to output information, NULL for
     * "printf"
     */
    void krhino_overview();

#ifdef __cplusplus
}
#endif

#endif /* K_OVERVIEW_H */
