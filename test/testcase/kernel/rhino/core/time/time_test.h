/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TIME_TEST_H
#define TIME_TEST_H

#define TASK_TIME_PRI          16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do {if ((int)(value) == 0) { return 1; }} while (0)

extern ktask_t *task_time;

typedef uint8_t (*test_func_t)(void);

void task_time_entry_register(const char *name, test_func_t *runner,
                              uint8_t casenum);
void task_time_entry(void *arg);
void time_test(void);
void time_opr_test(void);

#endif /* TIME_TEST_H */

