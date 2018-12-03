/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>

void idle_task(void *arg)
{
    CPSR_ALLOC();

#if (RHINO_CONFIG_CPU_NUM > 1)
    klist_t *head;
    ktask_t *task_del;
    head =  &task_del_head;
#endif

    /* avoid warning */
    (void)arg;

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_idle_pre_hook();
#endif

    while (RHINO_TRUE) {
#if (RHINO_CONFIG_CPU_NUM > 1)
        RHINO_CPU_INTRPT_DISABLE();
        if (head->next != head) {
            task_del = krhino_list_entry(head->next, ktask_t, task_del_item);
            if (task_del->cur_exc == 0) {
                klist_rm(&task_del->task_del_item);
                krhino_task_dyn_del(task_del);
            }
        }
        RHINO_CPU_INTRPT_ENABLE();
#endif
        RHINO_CPU_INTRPT_DISABLE();
        g_idle_count[cpu_cur_get()]++;
        RHINO_CPU_INTRPT_ENABLE();

#if (RHINO_CONFIG_USER_HOOK > 0)
        krhino_idle_hook();
#endif

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
        cpu_pwr_down();
#endif
    }
}

