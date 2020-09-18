/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BT_UTIL_DEBUG_H_
#define _BT_UTIL_DEBUG_H_

#include <ulog/ulog.h>
#include <aos/errno.h>

#if (!defined(unlikely))
#define unlikely(EXPRESSSION) !!(EXPRESSSION)
#endif

#define bt_assert(X)                                                                              \
        do {                                                                                           \
            if (unlikely(!(X))) {                                                                      \
                LOGE("BT_ASSERT","!!!assert: %s: %d, %s\r\n", __LINE__, __PRETTY_FUNCTION__);            \
            }                                                                                          \
        } while (0)
    
#define bt_except_process(err)  bt_assert(0)


#if (!defined(bt_value_check))
#define bt_value_check(X, errno)                                                                        \
    do {                                                                                           \
        if (unlikely(!(X)))                                                                        \
            bt_except_process(errno);                                                                 \
    } while (0)
#endif

#if (!defined(bt_value_check_param))
#define bt_value_check_param(X) bt_value_check(X, EINVAL)
#endif

#if (!defined(bt_value_check_mem))
#define bt_value_check_mem(X) bt_value_check(X, ENOMEM)
#endif

#if (!defined(bt_value_check_return_val))
#define bt_value_check_return_val(X, ret)                                                               \
    do {                                                                                           \
        if (unlikely(!(X))) {                                                                      \
            bt_except_process(EINVAL);                                                                 \
            return ret;                                                                            \
        }                                                                                          \
    } while (0)
#endif

#if (!defined(bt_value_check_return_einval))
#define bt_value_check_return_einval(X) bt_value_check_return_val(X, -EINVAL)
#endif

#if (!defined(bt_value_check_return_null))
#define bt_value_check_return_null(X) bt_value_check_return_val(X, NULL)
#endif

#if (!defined(bt_value_check_return_enomem))
#define bt_value_check_return_enomem(X) bt_value_check_return_val(X, -ENOMEM)
#endif

#if (!defined(bt_value_check_return))
#define bt_value_check_return(X)                                                                        \
    do {                                                                                           \
        if (unlikely(!(X))) {                                                                      \
            bt_except_process(errno);                                                                 \
            return;                                                                                \
        }                                                                                          \
    } while (0)
#endif

#endif
