
#ifndef CDX_MALLOC_DBG_H
#define CDX_MALLOC_DBG_H

#define DBG_MALLOC 0
#define DBG_MALLOC_TOTAL 1
#define DBG_MALLOC_LIST 0
#define DBG_MALLOC_OVERFLOW 1
#define DBG_MALLOC_LOG_ALL 0
#define FREE_NULL_WARNING 0

#if DBG_MALLOC_LOG_ALL
extern int wrap_printf(const char *fmt, ...);
#define DBG_MALLOC_PRINTF(fmt, arg...) wrap_printf(fmt, ##arg)
#else
#define DBG_MALLOC_PRINTF(fmt, arg...)
#endif

#include <stdlib.h>
#include <string.h>
void *dbg_malloc(unsigned int size, const char *func, int line);
void dbg_free(void *p, const char *func, int line);
void *dbg_calloc(unsigned int n, unsigned int size, const char *func, int line);
void *dbg_strdup(const char *s, const char *func, int line);
#if DBG_MALLOC_TOTAL
void *dbg_realloc(void *ptr, unsigned int newsize, const char *func, int line);
#endif

#if DBG_MALLOC
#define malloc(s) dbg_malloc(s, __func__, __LINE__)
#define calloc(n,s) dbg_calloc(n, s, __func__, __LINE__)
#define free(p) dbg_free(p, __func__, __LINE__)
#define strdup(s) dbg_strdup(s, __func__, __LINE__)
#if DBG_MALLOC_TOTAL
#define realloc(p,s) dbg_realloc(p, s, __func__, __LINE__)
#endif
#endif



#endif

