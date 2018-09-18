/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <k_api.h>

void cmd_free(char *pwbuf, int blen, int argc, char **argv)
{
#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    CPSR_ALLOC();
#endif

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_ENTER();
#else
    krhino_mutex_lock(&g_kmm_head->mm_mutex, RHINO_WAIT_FOREVER);
#endif

    printf("\r\n");
    printf("------------------------- memory allocation statistic ------------------------------ \r\n");
    dump_kmm_statistic_info(g_kmm_head);

#if (RHINO_CONFIG_MM_REGION_MUTEX == 0)
    RHINO_CRITICAL_EXIT();
#else
    krhino_mutex_unlock(&g_kmm_head->mm_mutex);
#endif

}
