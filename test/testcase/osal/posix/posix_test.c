/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

extern void posix_cond_test_case(void);
extern void posix_dirent_test_case(void);
extern void posix_jump_test_case(void);
extern void posix_mutex_test_case(void);
extern void posix_pthread_attr_test_case(void);
extern void posix_sched_test_case(void);
extern void posix_pthread_test_case(void);
extern void posix_sem_test_case(void);
extern void posix_timer_test_case(void);
extern void posix_tsd_test_case(void);
extern void posix_enviro_test_case(void);

void posix_test_entry(void)
{
    posix_cond_test_case();
    posix_dirent_test_case();
    posix_jump_test_case();
    posix_mutex_test_case();
    posix_pthread_attr_test_case();
    posix_sched_test_case();
    posix_pthread_test_case();
    posix_sem_test_case();
    posix_timer_test_case();
    posix_tsd_test_case();
    posix_enviro_test_case();
}
