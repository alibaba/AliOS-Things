#ifndef _SSV_LIB_H_
#define _SSV_LIB_H_

#include <string.h>

typedef char                        s8;
typedef unsigned char               u8;
typedef short                       s16;
typedef unsigned short              u16;
typedef int                         s32;
typedef unsigned int                u32;
typedef long long                   u64;
//typedef u8                          bool;
typedef unsigned long size_t;
#ifndef bool
#define bool                        u8
#endif

#ifndef ARRAY_ELEM_SIZE
#define ARRAY_ELEM_SIZE(TYPE)   ((size_t)(&((TYPE *)100)[1]) - 100U)
#endif

#if 0
size_t strlen(const char *s);
void *memset(void *s, s32 c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
#endif

#define os_memcpy memcpy
#define os_memset memset
#define os_strlen strlen

#define ASSERT(x) \
{ \
    if (!(x)) \
    { \
        printf("Assert!! file: %s, function: %s, line: %d\n\t" #x, __FILE__, \
        	__FUNCTION__, __LINE__); \
        while(1){;} \
    } \
}
#endif /* _SSV_LIB_H_ */

