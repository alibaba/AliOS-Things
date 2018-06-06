#ifndef POSIX_SEM_H
#define POSIX_SEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <k_api.h>
#include <time.h>

typedef ksem_t *sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
sem_t *sem_open(const char *name, int oflag, ...);
int sem_post(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_trywait(sem_t *sem);
int sem_unlink(const char *name);
int sem_wait(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);
int sem_close(sem_t *sem);
int sem_destroy(sem_t *sem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*POSIX_SEM_H*/

