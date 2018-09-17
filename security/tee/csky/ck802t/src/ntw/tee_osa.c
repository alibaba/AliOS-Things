/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include "tee_types.h"

extern int   printf(char *fmt, ...);
extern int   vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern void *malloc(unsigned int size);
extern void  free(void *mem);

#define PRINTK_BUF_SZ (128)
int32_t printk(const int8_t *fmt, ...)
{
    va_list args;
    int8_t  buf[PRINTK_BUF_SZ];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf) - 1, fmt, args);
    va_end(args);
    buf[PRINTK_BUF_SZ - 1] = '\0';
    printf("%s", buf);
    return 0;
}

void *kmalloc(size_t size, int flags)
{
    return malloc(size);
}

void kfree(const void *ptr)
{
    return free((void *)ptr);
}