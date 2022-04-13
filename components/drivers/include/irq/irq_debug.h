/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_IRQ_DEBUG_H
#define AOS_IRQ_DEBUG_H

#include <aos/kernel.h>
#include <k_api.h>

/* DEBUG LEVEL definition*/
#define LOG_DEBUG  3
#define LOG_INFO   2
#define LOG_WARN   1
#define LOG_ERROR  0
#define PRINTF(x...) PRINTF_D(x)

#define DEBUGLEVEL 2

#define PRINTF_D(x...)            \
({                                \
    if (LOG_DEBUG <= DEBUGLEVEL)  \
        printf(x);                \
})

#define PRINTF_I(x...)            \
({                                \
    if (LOG_INFO <= DEBUGLEVEL)   \
        printf(x);                \
})

#define PRINTF_W(x...)            \
({                                \
    if (LOG_WARN <= DEBUGLEVEL)   \
        printf(x);                \
})


#define PRINTF_E(x...)            \
({                                \
    if (LOG_ERROR <= DEBUGLEVEL)  \
        printf(x);                \
})

#define PRINTF_ONCE(x...)                                   \
({                                                              \
        static char __print_once;                 \
                                                                \
        if (!__print_once) {                                    \
                __print_once = 1;                            \
                printf(x);                     \
        }                                                       \
})

#endif /* AOS_IRQ_DEBUG_H */
