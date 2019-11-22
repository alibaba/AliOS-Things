#ifndef _OS_TEST_H
#define _OS_TEST_H

//task test api
void os_test_task(void *param);
void task_critical_test(void *param);
void os_task_yield_test(void *param);
//sem test api
void sem_binary_test(void*param);
//timer test api
void timer_test_task(void *param);
//mutex test api
void os_mutex_test(void *param);
//queue test api
void os_queue_test(void *param);
void os_event_test(void *param);


void os_api_test(void);


#endif
