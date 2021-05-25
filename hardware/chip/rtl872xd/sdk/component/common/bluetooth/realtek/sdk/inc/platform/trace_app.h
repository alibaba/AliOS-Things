/**
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _TRACE_APP_H_
#define _TRACE_APP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "trace.h"

#define LOG_LEVEL_ERROR     0
#define LOG_LEVEL_WARN      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_TRACE     3
#define LOG_LEVEL_NUM       4

#define TRACE_APP_DATA __attribute__((section(".BTTRACE"))) __attribute__((aligned(4))) __attribute__((used))

#define COMBINE_TRACE_INFO_APP(type, subtype, module, level)  (uint32_t)(((type)<<24) | ((subtype)<<16) | ((module)<<8) | (level))

/* Internal macro that is wrapped by internal macro DBG_BUFFER. */
#if 0
#define DBG_BUFFER_APP(type, sub_type, module, level, fmt, param_num, ...) do {\
        static const char format[] TRACE_APP_DATA = fmt;\
        trace_log_buffer(COMBINE_TRACE_INFO_APP(type, sub_type, module, level), (uint32_t)format, param_num, ##__VA_ARGS__);\
    } while (0)
#else
#define DBG_BUFFER_APP
#endif

/**
 * trace_app.h
 *
 * \name    APP_PRINT_TRACE
 * \brief   Bluetooth APP Trace Interfaces.
 * \anchor  APP_PRINT_TRACE
 */
/**
 * \ingroup TRACE
 */
/**@{*/
#define APP_PRINT_ERROR0(fmt)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR, "!!!"fmt, 0)
#define APP_PRINT_ERROR1(fmt, arg0)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 1, arg0)
#define APP_PRINT_ERROR2(fmt, arg0, arg1)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 2, arg0, arg1)
#define APP_PRINT_ERROR3(fmt, arg0, arg1, arg2)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 3, arg0, arg1, arg2)
#define APP_PRINT_ERROR4(fmt, arg0, arg1, arg2, arg3)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 4, arg0, arg1, arg2, arg3)
#define APP_PRINT_ERROR5(fmt, arg0, arg1, arg2, arg3, arg4)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 5, arg0, arg1, arg2, arg3, arg4)
#define APP_PRINT_ERROR6(fmt, arg0, arg1, arg2, arg3, arg4, arg5)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 6, arg0, arg1, arg2, arg3, arg4, arg5)
#define APP_PRINT_ERROR7(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 7, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define APP_PRINT_ERROR8(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_ERROR,  "!!!"fmt, 8, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#define APP_PRINT_WARN0(fmt)     \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN,  "!!*"fmt, 0)
#define APP_PRINT_WARN1(fmt, arg0)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 1, arg0)
#define APP_PRINT_WARN2(fmt, arg0, arg1)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 2, arg0, arg1)
#define APP_PRINT_WARN3(fmt, arg0, arg1, arg2)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 3, arg0, arg1, arg2)
#define APP_PRINT_WARN4(fmt, arg0, arg1, arg2, arg3)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 4, arg0, arg1, arg2, arg3)
#define APP_PRINT_WARN5(fmt, arg0, arg1, arg2, arg3, arg4)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 5, arg0, arg1, arg2, arg3, arg4)
#define APP_PRINT_WARN6(fmt, arg0, arg1, arg2, arg3, arg4, arg5)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 6, arg0, arg1, arg2, arg3, arg4, arg5)
#define APP_PRINT_WARN7(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 7, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define APP_PRINT_WARN8(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_WARN, "!!*"fmt, 8, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#define APP_PRINT_INFO0(fmt)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 0)
#define APP_PRINT_INFO1(fmt, arg0)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 1, arg0)
#define APP_PRINT_INFO2(fmt, arg0, arg1)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 2, arg0, arg1)
#define APP_PRINT_INFO3(fmt, arg0, arg1, arg2)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 3, arg0, arg1, arg2)
#define APP_PRINT_INFO4(fmt, arg0, arg1, arg2, arg3)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 4, arg0, arg1, arg2, arg3)
#define APP_PRINT_INFO5(fmt, arg0, arg1, arg2, arg3, arg4)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 5, arg0, arg1, arg2, arg3, arg4)
#define APP_PRINT_INFO6(fmt, arg0, arg1, arg2, arg3, arg4, arg5)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 6, arg0, arg1, arg2, arg3, arg4, arg5)
#define APP_PRINT_INFO7(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 7, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define APP_PRINT_INFO8(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_INFO, "!**"fmt, 8, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#define APP_PRINT_TRACE0(fmt)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 0)
#define APP_PRINT_TRACE1(fmt, arg0)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 1, arg0)
#define APP_PRINT_TRACE2(fmt, arg0, arg1)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 2, arg0, arg1)
#define APP_PRINT_TRACE3(fmt, arg0, arg1, arg2)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 3, arg0, arg1, arg2)
#define APP_PRINT_TRACE4(fmt, arg0, arg1, arg2, arg3)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 4, arg0, arg1, arg2, arg3)
#define APP_PRINT_TRACE5(fmt, arg0, arg1, arg2, arg3, arg4)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 5, arg0, arg1, arg2, arg3, arg4)
#define APP_PRINT_TRACE6(fmt, arg0, arg1, arg2, arg3, arg4, arg5)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 6, arg0, arg1, arg2, arg3, arg4, arg5)
#define APP_PRINT_TRACE7(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 7, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define APP_PRINT_TRACE8(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)   \
    DBG_BUFFER_APP(TYPE_BTLIB, SUBTYPE_FORMAT, TRACE_MODULE_APP, LOG_LEVEL_TRACE, fmt, 8, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
/**@}*/

/**
 * trace_app.h
 *
 * \brief    Initialize module trace task.
 *
 * \param[in]   NULL.
 *
 * \return      True ok, false fail.
 *
 * \ingroup
 */
bool bt_trace_init(void);

/**
 * trace_app.h
 *
 * \brief    DeInitialize module trace task.
 *
 * \param[in]   NULL.
 *
 * \return      True ok, false fail.
 *
 * \ingroup
 */
bool bt_trace_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _TRACE_H_ */
