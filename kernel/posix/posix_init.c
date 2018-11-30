/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

extern int pthread_lock_init(void);
extern int timer_lock_init(void);

int posix_init(void)
{
    int ret = -1;

#if (POSIX_PTHREAD_ENABLE > 0)
    ret = pthread_lock_init();
    if (ret != 0) {
        return -1;
    }
#endif

#if (POSIX_TIMER_ENABLE > 0)
    ret = timer_lock_init();
    if (ret != 0) {
        return -1;
    }
#endif

    return ret;
}
