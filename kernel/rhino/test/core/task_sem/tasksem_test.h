/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TASKSEM_TEST_H
#define TASKSEM_TEST_H

#define TASK_SEM_PRI         16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do {if ((int)(value) == 0) { return 1; }} while (0)
#define MYASSERT_INFO(value) do {if ((int)(value) == 0) { printf("%s:%d\n", __FUNCTION__, __LINE__); }} while (0)

extern ktask_t *task_tasksem;
extern ktask_t *task_tasksem_co1;
extern ktask_t *task_tasksem_co2;
extern ksem_t   test_tasksem;
extern ksem_t   test_tasksem_co1;
extern ksem_t   test_tasksem_co2;

typedef uint8_t (*test_func_t)(void);

void task_tasksem_entry_register(const char *name, test_func_t *runner,
                                 uint8_t casnum);
void task_tasksem_entry(void *arg);
void tasksem_test(void);
void tasksem_param_test(void);
void tasksem_count_test(void);
void tasksem_opr_test(void);
void tasksem_coopr1_test(void);
void tasksem_coopr2_test(void);

#endif /* TASKSEM_TEST_H */

