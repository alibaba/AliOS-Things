/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MUTEX_TEST_H
#define MUTEX_TEST_H

#define TASK_MUTEX_PRI         16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do {if ((int)(value) == 0) { printf("line:%d\n", __LINE__);return 1; }} while (0)

extern ktask_t  *task_mutex;
extern ktask_t  *task_mutex_co1;
extern ktask_t  *task_mutex_co2;
extern ktask_t  *task_mutex_co3;
extern kmutex_t *test_mutex_dyn;
extern kmutex_t  test_mutex;
extern kmutex_t  test_mutex_co1;
extern kmutex_t  test_mutex_co2;

typedef uint8_t (*test_func_t)(void);

void task_mutex_entry_register(const char *name, test_func_t *runner,
                               uint8_t casnum);
void task_mutex_entry(void *arg);
void mutex_test(void);
void mutex_param_test(void);
void mutex_opr_test(void);
void mutex_reinit_test(void);
void mutex_coopr1_test(void);
void mutex_coopr2_test(void);

#endif /* MUTEX_TEST_H */

