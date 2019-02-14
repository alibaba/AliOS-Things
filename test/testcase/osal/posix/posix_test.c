/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

extern void posix_pthread_create_case(void);
extern void posix_pthread_join_case(void);
extern void posix_pthread_cleanup_case1(void);
extern void posix_pthread_cleanup_case2(void);
extern void posix_pthread_detach_case1(void);
extern void posix_pthread_detach_case2(void);
extern void posix_pthread_once_case(void);
extern void posix_pthread_setschedparam_case1(void);
extern void posix_pthread_setschedparam_case2(void);
extern void posix_pthread_setschedprio_case(void);

extern void posix_pthread_attr_init_case(void);
extern void posix_pthread_attr_destroy_case(void);
extern void posix_pthread_attr_setdetachstate_case(void);
extern void posix_pthread_attr_getdetachstate_case(void);
extern void posix_pthread_attr_setschedpolicy_case(void);
extern void posix_pthread_attr_getschedpolicy_case(void);
extern void posix_pthread_attr_setschedparam_case(void);
extern void posix_pthread_attr_getschedparam_case(void);
extern void posix_pthread_attr_setstacksize_case(void);
extern void posix_pthread_attr_getstacksize_case(void);
extern void posix_pthread_attr_setstackaddr_case(void);
extern void posix_pthread_attr_getstackaddr_case(void);
extern void posix_pthread_attr_setstack_case(void);
extern void posix_pthread_attr_getstack_case(void);
extern void posix_pthread_attr_setinheritsched_case(void);
extern void posix_pthread_attr_getinheritsched_case(void);
extern void posix_pthread_attr_setscope_case(void);
extern void posix_pthread_attr_getscope_case(void);

extern void posix_sem_case1(void);
extern void posix_sem_case2(void);
extern void posix_mutex_case(void);
extern void posix_cond_case(void);
extern void posix_tsd_case(void);

void posix_test_entry(void)
{
    posix_pthread_create_case();
    posix_pthread_join_case();
    posix_pthread_cleanup_case1();
    posix_pthread_cleanup_case2();
    posix_pthread_detach_case1();
    posix_pthread_detach_case2();
    posix_pthread_once_case();
}
