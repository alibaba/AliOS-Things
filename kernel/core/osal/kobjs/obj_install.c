/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "object_api.h"

extern int aos_obj_sem_lib_init();
extern int aos_obj_event_lib_init();
extern int aos_obj_mutex_lib_init();
extern int aos_obj_timer_lib_init();
extern int aos_obj_queue_lib_init();
extern int aos_obj_workqueue_lib_init();
extern int aos_obj_work_lib_init();

void obj_components_install()
{
#if (AOS_KOBJ_SEM > 0)
    aos_obj_sem_lib_init();
#endif
#if (AOS_KOBJ_EVENT > 0)
    aos_obj_event_lib_init();
#endif
#if (AOS_KOBJ_MUTEX > 0)
    aos_obj_mutex_lib_init();
#endif
#if (AOS_KOBJ_TIMER > 0)
    aos_obj_timer_lib_init();
#endif
#if (AOS_KOBJ_QUEUE > 0)
    aos_obj_queue_lib_init();
#endif
#if (AOS_KOBJ_WORKQUEUE > 0)
    aos_obj_workqueue_lib_init();
    aos_obj_work_lib_init();
#endif
}

