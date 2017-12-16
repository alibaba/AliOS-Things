/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_DBG_H_
#define _TEE_DBG_H_

#include "tee_types.h"
#include "tee_tos.h"

#ifdef CONFIG_DBG
#define tee_dbg_print(_l, _f, _a ...)       \
    ({                                          \
        int32_t __ret__ = 0;                    \
        if ((_l) <= g_dbg_level) {              \
            __ret__ = printf(_f, ##_a);         \
        }                                       \
        __ret__;                                \
     })

#define tee_err_handle(_err)                \
    do {                                        \
        tee_chk_stk_boundary();             \
        backtrace();                    \
    } while (0)

#define TEE_ASSERT(_x)                                      \
    do {                                                        \
        if (!(_x)) {                                            \
            tee_dbg_print(ERR, "ASSERT FAILURE:\n");        \
            tee_dbg_print(ERR, (int8_t *)"%s (%d): %s\n",   \
                    __FILE__, __LINE__, __FUNCTION__);          \
            tee_err_handle(-1);                             \
            while (1) /* loop */;                               \
        }                                                       \
    } while (0)
#define TEE_ERROR(_x)                                       \
    do {                                                        \
        tee_dbg_print(ERR, "TEE ERROR: 0x%08x:\n", (_x));   \
        tee_err_handle(_x);                                 \
    } while (0)

#else /* !CONFIG_DBG */

#define tee_dbg_print(_l, _f, _a ...)
#define TEE_ASSERT(_x)                                          \
    do {                                                        \
        if (!(_x)) {                                            \
            tee_err_handle(-1);                                 \
            while (1) /* loop */;                               \
        }                                                       \
    } while (0)
#define TEE_ERROR(_x)                                           \
    do {                                                        \
        tee_err_handle(_x);                                     \
    } while (0)
#endif

#ifdef CONFIG_DBG
enum {
    ERR = 0, /* error */
    WRN = 1, /* warning */
    INF = 2, /* information */
};

extern uint32_t g_dbg_level;
#endif

#ifndef CONFIG_DBG
void tee_err_handle(uint32_t err);
#endif

#ifdef CONFIG_DBG
void backtrace(void);
void tee_set_stk_boundary(void *boundary);
void tee_chk_stk_boundary(void);
#endif

#endif /* _TEE_DBG_H_ */
