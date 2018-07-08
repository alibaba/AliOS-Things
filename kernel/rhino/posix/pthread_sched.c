
#include <pthread.h>

int sched_yield(void)
{
    krhino_task_yield();
    return 0;
}

int sched_get_priority_min(int policy)
{
    return RHINO_CONFIG_USER_PRI_MAX;
}

int sched_get_priority_max(int policy)
{
    return 10;
}

int sched_setscheduler(pid_t pid, int policy)
{
    return 0;
}

