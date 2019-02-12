/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ZEPHYR_H
#define ZEPHYR_H
#include <stdint.h>
#include <stddef.h>

#include <zephyr/types.h>
#include <misc/slist.h>
#include <misc/dlist.h>
#include <port/mesh_hal_os.h>
#include "work.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define _STRINGIFY(x) #x

#ifndef ___in_section
#define ___in_section(a, b, c) \
    __attribute__(             \
      (section("." _STRINGIFY(a) "." _STRINGIFY(b) "." _STRINGIFY(c))))
#endif

#define ARG_UNUSED(x) (void)(x)

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __printf_like
#define __printf_like(f, a) __attribute__((format(printf, f, a)))
#endif
#define STACK_ALIGN 4

#define ASSERT(test, fmt, ...)

#define K_FOREVER -1
#define K_NO_WAIT 0

/* Unaligned access */
#ifndef UNALIGNED_GET
#define UNALIGNED_GET(p)                   \
    __extension__({                        \
        struct __attribute__((__packed__)) \
        {                                  \
            __typeof__(*(p)) __v;          \
        } *__p = (__typeof__(__p))(p);     \
        __p->__v;                          \
    })
#endif

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#define k_oops()

#if defined(__cplusplus)
}
#endif

#endif /* ZEPHYR_H */
