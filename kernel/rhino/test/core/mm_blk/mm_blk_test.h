/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MM_BLK_TEST_H
#define MM_BLK_TEST_H

#define TASK_MM_BLK_PRI      16
#define TASK_TEST_STACK_SIZE 1024

#define MBLK_POOL_SIZE       1024

#define MYASSERT(value) do {if ((int)(value) == 0) { printf("%s:%d\n", __FUNCTION__, __LINE__);return 1; }} while (0)

extern ktask_t *task_mm_blk;
extern ktask_t *task_mm_blk_co;
extern mblk_pool_t mblk_pool_test;
extern char mblk_pool[MBLK_POOL_SIZE];

typedef uint8_t (*test_func_t)(void);

void task_mm_blk_entry_register(const char *name, test_func_t *runner,
                                uint8_t casenum);
void task_mm_blk_entry(void *arg);
void mm_blk_test(void);
void mm_blk_param_test(void);
void mm_blk_opr_test(void);
void mm_blk_break_test(void);
void mm_blk_reinit_test(void);
void mm_blk_fragment_test(void);
void mm_blk_coopr_test(void);

#endif /* MM_BLK_TEST_H */

