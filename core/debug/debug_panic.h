/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_PANIC_H
#define DEBUG_PANIC_H

#include <k_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/* fault/exception entry
 * notice: this function maybe reentried by double exception
 */
void panicHandler(void *context);
void debug_fatal_error(kstat_t err, char *file, int line);
void debug_reboot_reason_update(unsigned int reason);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_PANIC_H */
