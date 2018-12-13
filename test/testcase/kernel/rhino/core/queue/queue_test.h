/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef QUEUE_TEST_H
#define QUEUE_TEST_H

#define TASK_TEST_STACK_SIZE 256

#define QUEUE_VAL_CHK(value) do {if ((int)(value) == 0) \
        {printf("queue test is [FAIL %d], func %s, line %d\n", \
                (int)++test_case_fail,__FUNCTION__, __LINE__);}}while(0)

kstat_t task_queue_back_send_test(void);
kstat_t task_queue_nowait_recv_test(void);
kstat_t task_queue_notify_set_test(void);
kstat_t task_queue_is_full_test(void);
kstat_t task_queue_flush_test(void);
kstat_t task_queue_del_test(void);
kstat_t task_queue_info_get_test(void);

#endif /* QUEUE_TEST_H */
