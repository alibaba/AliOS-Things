/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TIMER_TEST_H
#define TIMER_TEST_H

#define TASK_TEST_STACK_SIZE 256

#define TIMER_VAL_CHK(value) do {if ((int)(value) == 0) \
        {printf("timer test is [FAIL %d], func %s, line %d\n", \
                (int)++test_case_fail,__FUNCTION__, __LINE__);}}while(0)

kstat_t task_timer_create_del_test(void);
kstat_t task_timer_dyn_create_del_test(void);
kstat_t task_timer_start_stop_test(void);
kstat_t task_timer_change_test(void);

#endif /* TIMER_TEST_H */
