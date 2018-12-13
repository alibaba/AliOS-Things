/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYS_TEST_H
#define SYS_TEST_H

#define TASK_SYS_PRI         16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do { printf("ERROR: %s:%d, ERROR-NO: %d\n", __FUNCTION__, __LINE__, value); } while (0)

extern ktask_t *task_sys;

typedef uint8_t (*test_func_t)(void);

void task_sys_entry_register(const char *name, test_func_t *runner,
                             uint8_t casenum);
void task_sys_entry(void *arg);
void sys_test(void);
void sys_opr_test(void);

#endif /* SYS_TEST_H */

