#ifndef __LINKKIT_THREAD_H__
#define __LINKKIT_THREAD_H__

typedef struct lk_thread_s lk_thread_t;

int lk_thread_create_detached(char *name, int stack_size, void *(*thread)(void *arg), void *arg);

lk_thread_t *lk_thread_create(char *name, int stack_size, void *(*thread)(void *arg), void *arg);
int lk_thread_join(lk_thread_t *thr);

#endif /* __LINKKIT_THREAD_H__ */
