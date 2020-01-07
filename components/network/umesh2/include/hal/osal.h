

#ifndef UMESH_OSAL_H_
#define UMESH_OSAL_H_

enum umesh_task_prio{
    UMESH_TASK_HIGH_PRIO = 31,
    UMESH_TASK_NORMAL_PRIO = 32,
    UMESH_TASK_LOW_PRIO = 33,
};
typedef void (*task_func)(void *);

int umesh_task_start(task_func cb,void *arg, int stack_size,enum umesh_task_prio  prio);
void *umesh_mutex_new(void);
void umesh_mutex_free(void *mutex);
int umesh_mutex_lock(void *mutex);
int umesh_mutex_unlock( void *mutex);
void *umesh_semaphore_new(void);
void umesh_semaphore_free(void *sem);
void umesh_semaphore_post( void *sem);
int umesh_semaphore_wait( void *sem, uint32_t timeout_ms);

void *umesh_malloc(uint32_t size);
void umesh_free(void *ptr);

void umesh_srand(uint32_t seed);
uint32_t umesh_rand();
uint64_t umesh_now_ms();
#endif /* UMESH_OSAL_H_ */
