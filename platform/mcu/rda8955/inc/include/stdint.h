#ifndef _STDINT_H
#define _STDINT_H
#include "cs_types.h"
// #include "unistd.h"
/*
typedef unsigned char uint8_t ;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;
*/

#ifndef INT8_MIN
#define INT8_MIN       (-0x7f-1)
#endif

#ifndef INT8_MAX
#define INT8_MAX       0x7f
#endif

#ifndef INT16_MIN
#define INT16_MIN       (-0x7fff-1)
#endif

#ifndef INT16_MAX
#define INT16_MAX       0x7fff
#endif

#ifndef INT32_MIN
#define INT32_MIN       (-0x7fffffff-1)
#endif

#ifndef INT32_MAX
#define INT32_MAX       0x7fffffff
#endif

#ifndef UINT32_MAX
#define UINT32_MAX      0xffffffff
#endif

#ifndef INT64_MIN
#define INT64_MIN       (-0x7fffffffffffffffLL-1)
#endif

#ifndef INT64_MAX
#define INT64_MAX	0x7fffffffffffffffL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 	0xffffffffffffffffUL
#endif

typedef unsigned int   intptr_t;


#endif
