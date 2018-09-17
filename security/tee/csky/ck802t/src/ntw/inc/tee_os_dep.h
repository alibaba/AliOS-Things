/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_OS_DEP_H_
#define _TEE_OS_DEP_H_
#include "tee_types.h"

/* printk */
extern int printk(const char *fmt, ...);
/* malloc/free */
#define GFP_KERNEL 0
extern void *kmalloc(size_t size, int flags);
extern void  kfree(const void *ptr);

#define malloc(sz) kmalloc(sz, GFP_KERNEL)
#define free(ptr) kfree(ptr)

/* string */
extern void *  memcpy(void *dest, const void *src, size_t n);
extern void *  memset(void *s, int32_t c, size_t n);
extern int32_t memcmp(const void *s1, const void *s2, size_t n);
extern size_t  strlen(const int8_t *s);
extern int8_t *strcpy(int8_t *dest, const int8_t *src);
extern int32_t strncmp(const int8_t *s1, const int8_t *s2, size_t n);
extern char *  strncpy(int8_t *dest, const int8_t *src, size_t count);

extern void backtrace(void);
#if 0
/* semaphore */
struct semaphore {
    int32_t rsvd;
};
#define HZ 10000
#define MSEC_TO_JIFFIES(msec) (((msec)*HZ + 500) / 1000)

extern void sema_init(struct semaphore *sem, int val);
extern void down(struct semaphore *sem);
extern int down_trylock(struct semaphore *sem);
extern int down_timeout(struct semaphore *sem, long jiffies);
extern int down_interruptible(struct semaphore *sem);
extern void up(struct semaphore *sem);
#endif

#endif /* _TEE_OS_DEP_H_ */
