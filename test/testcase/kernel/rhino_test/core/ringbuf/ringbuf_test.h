/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RINGBUF_TEST_H
#define RINGBUF_TEST_H

#define TASK_RINGBUF_PRI   16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do {if ((value) == 0) { printf("%s:%d\n", __FUNCTION__, __LINE__); }} while (0)

extern ktask_t *task_ringbuf;
extern ktask_t *task_ringbuf_co;

typedef uint8_t (*test_func_t)(void);

void task_ringbuf_entry_register(const char *name, test_func_t *runner,
                                 uint8_t casenum);
void task_ringbuf_entry(void *arg);
void ringbuf_test(void);
void ringbuf_break_test(void);

#endif /* RINGBUF_TEST_H */

