/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_TYPES_H_
#define _TEE_TYPES_H_

#ifndef __STDINT__
#define __STDINT__

typedef char                int8_t;
typedef unsigned char       uint8_t;

typedef short               int16_t;
typedef unsigned short      uint16_t;

typedef int                 int32_t;
typedef unsigned int        uint32_t;

typedef long long           int64_t;
typedef unsigned long long  uint64_t;

typedef signed long int     ssize_t;
typedef unsigned long       size_t;

typedef long                long_t;
typedef unsigned long       ulong_t;

#endif /* __STDINT__ */

#ifndef __STDBOOL__
#define __STDBOOL__

typedef unsigned char       bool;

#endif /* __STDBOOL__ */

typedef uint32_t            tee_stat_t;

#define INLINE

#ifndef false
#define false               (0)
#endif

#ifndef true
#define true                (1)
#endif

#ifndef NULL
#define NULL                ((void *)0)
#endif

#endif /* _TEE_TYPES_H_ */
