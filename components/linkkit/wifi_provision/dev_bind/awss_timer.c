/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int awss_stop_timer(void *timer)
{
    if (timer == NULL)
        return 0;
    HAL_Timer_Stop(timer);
    HAL_Timer_Delete(timer);
    return 0;
}

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif
