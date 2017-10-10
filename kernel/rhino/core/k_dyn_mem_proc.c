/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
void dyn_mem_proc_task(void *arg)
{
    void   *recv_msg;
    kstat_t ret;

    (void)arg;

    while (1) {
        ret = krhino_queue_recv(&g_dyn_queue, RHINO_WAIT_FOREVER, &recv_msg);
        if (ret != RHINO_SUCCESS) {
            k_err_proc(RHINO_DYN_MEM_PROC_ERR);
        }

        krhino_mm_free(recv_msg);
    }
}

void dyn_mem_proc_task_start(void)
{
    ktask_t *dyn_mem_task;

    krhino_task_dyn_create(&dyn_mem_task, "dyn_mem_proc_task", 0,
                           RHINO_CONFIG_K_DYN_MEM_TASK_PRI,
                           0, RHINO_CONFIG_K_DYN_TASK_STACK, dyn_mem_proc_task, 1);

}
#endif

