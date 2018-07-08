#ifndef _MEM_PUB_H_
#define _MEM_PUB_H_

#include <stdarg.h>
#include <aos/kernel.h>

INT32 os_memcmp(const void *s1, const void *s2, UINT32 n);
void *os_memmove(void *out, const void *in, UINT32 n);
void *os_memcpy(void *out, const void *in, UINT32 n);
void *os_memset(void *b, int c, UINT32 len);
void os_mem_init(void);
#if 0
void *os_malloc(size_t size);
void *os_zalloc(size_t size);
void *os_realloc(void *ptr, size_t size);
void *os_free(void *ptr);
#else
#define os_malloc  aos_malloc
#define os_zalloc  aos_zalloc
#define os_realloc aos_realloc
#define os_free    aos_free
#endif

int os_memcmp_const(const void *a, const void *b, size_t len);

#endif // _MEM_PUB_H_

// EOF
