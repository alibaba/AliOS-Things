/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MM_TEST_H
#define MM_TEST_H

#define TASK_MM_PRI          16
#define TASK_TEST_STACK_SIZE 1024
#define MM_POOL_SIZE         (1024 * 11)

#define MYASSERT(value) do {if ((int)(value) == 0) { printf("%s:%d\n", __FUNCTION__, __LINE__);return 1; }} while (0)

#if (RHINO_CONFIG_MM_TLF > 0)
extern ktask_t *task_mm;
extern ktask_t *task_mm_co;
extern k_mm_head *pmmhead;
extern char mm_pool[MM_POOL_SIZE];

typedef uint8_t (*test_func_t)(void);

void task_mm_entry_register(const char *name, test_func_t *runner,
                            uint8_t casenum);
void task_mm_entry(void *arg);
void mm_test(void);
void mm_param_test(void);
//void mm_reinit_test(void);
void mm_break_test(void);
void mm_opr_test(void);
void mm_coopr_test(void);
#endif
#endif /* MM_TEST_H */

