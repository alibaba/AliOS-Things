

#ifndef __HAL_OSAL_H__
#define __HAL_OSAL_H__

#include <stdint.h>

enum hal_task_prio {
    HAL_TASK_HIGH_PRIO = 31,
    HAL_TASK_NORMAL_PRIO = 32,
    HAL_TASK_LOW_PRIO = 33,
};
typedef void (*task_func)(void *);

int hal_task_start(task_func cb, void *arg, int stack_size, enum hal_task_prio  prio);
void *hal_mutex_new(void);
void hal_mutex_free(void *mutex);
int hal_mutex_lock(void *mutex);
int hal_mutex_unlock(void *mutex);
void *hal_semaphore_new(void);
void hal_semaphore_free(void *sem);
void hal_semaphore_post(void *sem);
int hal_semaphore_wait(void *sem, uint32_t timeout_ms);

void *hal_malloc(uint32_t size);
void hal_free(void *ptr);

void hal_srand(uint32_t seed);
uint32_t hal_rand();
uint64_t hal_now_ms();

void *hal_queue_new(void *buf, uint32_t size, int max_msg);
void hal_queue_free(void *queue);
int hal_queue_send(void *queue, void *msg, uint32_t size);
int hal_queue_recv(void *queue, uint32_t ms, void *msg, unsigned int *size);
#endif /* __HAL_OSAL_H__ */
