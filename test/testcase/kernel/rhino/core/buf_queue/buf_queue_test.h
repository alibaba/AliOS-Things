/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BUF_QUEUE_TEST_H
#define BUF_QUEUE_TEST_H

#define TASK_TEST_STACK_SIZE 256

#define BUFQUEUE_VAL_CHK(value) do {if ((int)(value) == 0) \
        { \
            test_case_critical_enter(); \
            test_case_check_err++;  \
            test_case_critical_exit(); \
            printf("buf queue test is [FAIL %d],file %s, func %s, line %d\n", \
                   (int)++test_case_check_err, __FILE__, __FUNCTION__, __LINE__); \
        }  \
    } while (0)

kstat_t task_buf_queue_send_test(void);
kstat_t task_buf_queue_recv_test(void);
kstat_t task_buf_queue_del_test(void);
kstat_t task_buf_queue_flush_test(void);
kstat_t task_buf_queue_info_get_test(void);
kstat_t task_buf_queue_dyn_create_test(void);

#endif /* BUF_QUEUE_TEST_H */
