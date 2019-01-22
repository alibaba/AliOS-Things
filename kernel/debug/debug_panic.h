/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_PANIC_H
#define DEBUG_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

#if (DEBUG_CONFIG_PANIC > 0)
/* fault/exception entry
 * notice: this function maybe reentried by double exception
 */
void panicHandler(void *context);
#endif

#if (DEBUG_CONFIG_ERRDUMP > 0)
void debug_fatal_error(kstat_t err, char *file, int line);
#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_PANIC_H */
