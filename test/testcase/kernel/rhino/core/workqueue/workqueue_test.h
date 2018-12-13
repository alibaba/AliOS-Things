/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WORKQUEUE_TEST_H
#define WORKQUEUE_TEST_H

#define TASK_WORKQUEUE_PRI   16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do { printf("ERROR: %s:%d, ERROR-NO: %d\n", __FUNCTION__, __LINE__, value); } while (0)

extern ktask_t *task_workqueue;

typedef uint8_t (*test_func_t)(void);

void task_workqueue_entry_register(const char *name, test_func_t *runner,
                                   uint8_t casenum);
void task_workqueue_entry(void *arg);
void workqueue_test(void);
void workqueue_interface_test(void);

#endif /* WORKQUEUE_TEST_H */

