/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

typedef char *sds;

struct sdshdr {
    unsigned int len;
    unsigned int free;
    char buf[];
};

size_t sdslen(const sds s);

size_t sdsavail(const sds s);

sds sdsnewlen(const void *init, size_t initlen);

sds sdsnewEmpty(size_t preAlloclen);

sds sdsnew(const char *init);

sds sdsempty(void);

size_t sdslen(const sds s);

sds sdsdup(const sds s);

void sdsfree(sds s);

size_t sdsavail(const sds s);

sds sdsgrowzero(sds s, size_t len);

sds sdscatlen(sds s, const void *t, size_t len);

sds sdscat(sds s, const char *t);

sds sdscatchar(sds s, char c);

sds sdscatsds(sds s, const sds t);

sds sdscpylen(sds s, const char *t, size_t len);

sds sdscpy(sds s, const char *t);

sds sdscatvprintf(sds s, const char *fmt, va_list ap);

#ifdef __GNUC__

sds sdscatprintf(sds s, const char *fmt, ...)
__attribute__((format(printf, 2, 3)));

#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif


#endif