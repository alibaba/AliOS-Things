/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Common macros.
 */
#ifndef __OMV_COMMON_H__

#define OMV_ATTR_ALIGNED(x, a)   x __attribute__((aligned(a)))
#define OMV_ATTR_SECTION(x, s)   x __attribute__((section(s)))
#define OMV_ATTR_ALWAYS_INLINE  inline __attribute__((always_inline))
#define OMV_ATTR_OPTIMIZE(o)    __attribute__((optimize(o)))

#define OMG_BREAK() __asm__ volatile ("BKPT")

#ifdef OMV_DEBUG_PRINTF
#define debug_printf(fmt, ...) \
            do { printf("%s(): " fmt, __func__, ##__VA_ARGS__);} while (0)
#else
#define debug_printf(...)
#endif

#define OMV_MAX(a,b)                    \
({                                      \
    __typeof__ (a) _a = (a);            \
    __typeof__ (b) _b = (b);            \
    _a > _b ? _a : _b;                  \
})

#define OMV_MIN(a,b)                    \
({                                      \
    __typeof__ (a) _a = (a);            \
    __typeof__ (b) _b = (b);            \
    _a < _b ? _a : _b;                  \
})
#endif //__OMV_COMMON_H__
