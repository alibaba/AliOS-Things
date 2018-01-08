/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#if defined (__CC_ARM)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}
extern long long aos_now_ms(void);
int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t = aos_now_ms();
    tv->tv_sec = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;
    return 0;
}

extern void *aos_malloc(unsigned int size);
extern void aos_alloc_trace(void *addr, size_t allocator);
extern void aos_free(void *mem);
extern void *aos_realloc(void *mem, unsigned int size);

void *malloc(size_t size)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    mem = aos_malloc(size | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_malloc(size);
#endif

    return mem;
}

void free(void *mem)
{
    aos_free(mem);
}

void *realloc(void *old, size_t newlen)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    mem = aos_realloc(old, newlen | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_realloc(old, newlen);
#endif

    return mem;
}

void *calloc(size_t len, size_t elsize)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    mem = aos_malloc((elsize * len) | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_malloc(elsize * len);
#endif

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}

char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = aos_malloc(len);
    if (dup_str == NULL)
        return NULL;
    return (char *)memcpy(dup_str, s, len);
}
#endif
