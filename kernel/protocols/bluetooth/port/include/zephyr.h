/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ZEPHYR_H
#define ZEPHYR_H
#include  <stdint.h>
#include  <stddef.h>

#include "kport.h"
#include "mbox.h"
#include "work.h"
#include "timer.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define _STRINGIFY(x) #x

#define ___in_section(a, b, c) \
    __attribute__((section("." _STRINGIFY(a)            \
                           "." _STRINGIFY(b)           \
                           "." _STRINGIFY(c))))

//#define __in_section(a, b, c) ___in_section(a, b, c)

#define __in_section_unique(seg) ___in_section(seg, _FILE_PATH_HASH, \
        __COUNTER__)


#define ARG_UNUSED(x) (void)(x)

#ifndef __aligned
#define __aligned(x)    __attribute__((__aligned__(x)))
#endif

#ifndef __printf_like
#define __printf_like(f, a)   __attribute__((format (printf, f, a)))
#endif
#define  STACK_ALIGN 4
#define __stack __aligned(STACK_ALIGN)

#define __noinit


#define __ASSERT_NO_MSG(test)
#define ASSERT(test, fmt, ...)

#define K_FOREVER -1
#define K_NO_WAIT 0

/* Unaligned access */
#define UNALIGNED_GET(p)                        \
    __extension__ ({                            \
        struct  __attribute__((__packed__)) {               \
            __typeof__(*(p)) __v;                   \
        } *__p = (__typeof__(__p)) (p);                 \
        __p->__v;                           \
    })

#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

struct k_poll_event {
    uint8_t tag;
    uint8_t state;
    union {
        void *obj;
        struct k_fifo *fifo;
        struct k_poll_signal *signal;
    };
};

struct k_poll_signal {
    struct k_poll_event *poll_event;

    unsigned int signaled;

    int result;
};

#define K_POLL_SIGNAL_INITIALIZER() \
    { \
        .poll_event = NULL, \
                      .signaled = 0, \
                                  .result = 0, \
    }

#define K_POLL_STATIC_INITIALIZER(event_obj, event_tag) \
    {\
        .tag = event_tag, \
               .obj = event_obj, \
                      .state = K_POLL_STATE_NOT_READY,\
    }

#define K_POLL_STATE_NOT_READY      0
#define K_POLL_STATE_EADDRINUSE          1
#define K_POLL_STATE_SIGNALED            2
#define K_POLL_STATE_SEM_AVAILABLE       3
#define K_POLL_STATE_DATA_AVAILABLE      4
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE

#define K_POLL_TYPE_IGNORE              0
#define K_POLL_TYPE_SIGNAL              1
#define K_POLL_TYPE_SEM_AVAILABLE       2
#define K_POLL_TYPE_DATA_AVAILABLE      3
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE

static inline void k_poll_event_init(struct k_poll_event *event, void *obj, uint32_t tag)
{
    event->state = K_POLL_STATE_NOT_READY;
    event->obj = obj;
    event->tag = tag;
}

#define BT_STACK(name, size) \
    uint32_t name[(size)];\

#define BT_STACK_NOINIT(name, size) \
    uint32_t name[(size)];\

#if defined(__cplusplus)
    }
#endif

#endif /* ZEPHYR_H */

