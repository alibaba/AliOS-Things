
#ifndef OS_H
#define OS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "wm_mem.h"

#ifndef os_strlen
#define os_strlen(s) strlen(s)
#endif

void * os_zalloc(u32 size);
void * os_realloc(void *ptr, size_t size);
char * os_strdup(const char *s);

#define os_time_after(a, b)  ((long)(b) - (long)(a) < 0)

#ifndef os_memcpy
#define os_memcpy(d, s, n) MEMCPY((d), (s), (n))
#endif
#ifndef os_memmove
#define os_memmove(d, s, n) memmove((d), (s), (n))
#endif
#ifndef os_memset
#define os_memset(s, c, n) memset(s, c, n)
#endif
#ifndef os_memcmp
#define os_memcmp(s1, s2, n) memcmp((s1), (s2), (n))
#endif
#ifndef os_strstr
#define os_strstr(h, n) strstr((h), (n))
#endif
#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif
#ifndef os_strncmp
#define os_strncmp(s1, s2, n) strncmp((s1), (s2), (n))
#endif
#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif
#ifndef os_strcpy
#define os_strcpy(s1,s2) strcpy((s1),(s2))
#endif
#endif
