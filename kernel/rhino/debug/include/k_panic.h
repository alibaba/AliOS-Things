/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_PANIC_H
#define K_PANIC_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (RHINO_CONFIG_PANIC > 0)
/* fault/exception entry
    notice: this function maybe reentried by double exception */
void panicHandler(void *context);
void debug_fatal_error(kstat_t err, char *file, int line);
#endif

#ifdef __cplusplus
}
#endif

#endif /* K_PANIC_H */
