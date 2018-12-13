/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef EVENT_TEST_H
#define EVENT_TEST_H

#define TASK_EVENT_PRI       16
#define TASK_TEST_STACK_SIZE 1024

#define MYASSERT(value) do {if ((int)(value) == 0) { printf("xxxx: %d \n", __LINE__); return 1; }} while (0)

extern ktask_t  *task_event;
extern ktask_t  *task_event_co1;
extern ktask_t  *task_event_co2;
extern kevent_t  test_event;
extern kevent_t *test_event_ext;

typedef uint8_t (*test_func_t)(void);

void task_event_entry_register(const char *name, test_func_t *runner,
                               uint8_t casenum);
void task_event_entry(void *arg);
void event_test(void);
void event_param_test(void);
void event_opr_test(void);
void event_break_test(void);
void event_reinit_test(void);
void event_coopr_test(void);

#endif /* EVENT_TEST_H */

