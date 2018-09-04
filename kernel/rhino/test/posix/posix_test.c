/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

extern void posix_task_case1(void);
extern void posix_task_case2(void);
extern void posix_task_case3(void);
extern void posix_task_case4(void);
extern void posix_task_case5(void);
extern void posix_task_case6(void);
extern void posix_sem_case1(void);
extern void posix_sem_case2(void);
extern void posix_mutex_case(void);
extern void posix_cond_case(void);

void posix_test_entry(void)
{
    posix_task_case1();
    posix_task_case2();
    posix_task_case3();
    posix_task_case4();
    posix_sem_case1();
    posix_sem_case2();
}
