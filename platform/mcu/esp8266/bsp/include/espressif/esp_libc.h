/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_LIBC_H__
#define __ESP_LIBC_H__

#ifdef __cplusplus
extern "C" {
#endif

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s);
char *strstr(const char *s1, const char *s2);
char *strcat(char *dst, const char *src);
char *strncat(char *dst, const char *src, size_t count);
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);
char *strtok_r(char *s, const char *delim, char **ptrptr);
char *strtok(char *s, const char *delim);
char *strrchr(const char *s, int c);
char *strdup(const char *s);
char *strchr(const char *s, int c);
long strtol(const char *str, char **endptr, int base);

void bzero(void *s, size_t n);

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *m1, const void *m2, size_t n);
void *memmove(void *dst, const void *src, size_t n);

int rand(void);

int printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);
int snprintf(char *buf, unsigned int count, const char *format, ...);
int puts(const char *str);

void *malloc(size_t n);
void free(void *p);
void *calloc(size_t c, size_t n);
void *zalloc(size_t n);
void *realloc(void *p, size_t n);

int atoi(const char *s);
long atol(const char *s);

unsigned long os_random(void);
int os_get_random(unsigned char *buf, size_t len);

#ifndef os_printf
/* NOTE: don't use printf_opt in irq handler, for test */
#define os_printf(fmt, ...) do {    \
        static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt;  \
        printf(flash_str, ##__VA_ARGS__);   \
    } while(0)
#endif

/* Note: check_memleak_debug_enable is a weak function inside SDK.
 * please copy following codes to user_main.c.
#include "esp_libc.h"

bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}
*/

#ifndef MEMLEAK_DEBUG
#define MEMLEAK_DEBUG_ENABLE    0
#ifndef os_free
#define os_free(s)        free(s)
#endif

#ifndef os_malloc
#define os_malloc(s)      malloc(s)
#endif

#ifndef os_calloc
#define os_calloc(p, s)   calloc(p, s)
#endif

#ifndef os_realloc
#define os_realloc(p, s)  realloc(p, s)
#endif

#ifndef os_zalloc
#define os_zalloc(s)      zalloc(s)
#endif
#else
#define MEMLEAK_DEBUG_ENABLE    1

#ifndef os_free
#define os_free(s) \
do{\
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
    vPortFree(s, mem_debug_file, __LINE__);\
}while(0)
#endif

#ifndef os_malloc
#define os_malloc(s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortMalloc(s, mem_debug_file, __LINE__, false);  \
    })
#endif

#ifndef os_malloc_iram
#define os_malloc_iram(s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortMalloc(s, mem_debug_file, __LINE__, true);  \
    })
#endif

#ifndef os_calloc
#define os_calloc(p, s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortCalloc(p, s, mem_debug_file, __LINE__);  \
    })
#endif

#ifndef os_realloc
#define os_realloc(p, s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortRealloc(p, s, mem_debug_file, __LINE__);  \
    })
#endif

#ifndef os_zalloc
#define os_zalloc(s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortZalloc(s, mem_debug_file, __LINE__);  \
    })
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* __LIBC_H__ */
