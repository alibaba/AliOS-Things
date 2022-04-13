/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef DEBUG_PANIC_H
#define DEBUG_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_PANIC_STEP_MAX 32
#define OS_PANIC_BY_NMI      0x31415926
#define OS_PANIC_NOT_REBOOT  0x21314916

/* how many steps has finished when crash */
extern volatile uint32_t g_crash_steps;
/* crash status */
extern volatile uint32_t g_crash_by_NMI;
extern volatile uint32_t g_crash_not_reboot;

/* fault/exception entry
 * notice: this function maybe reentried by double exception
 */
void panicHandler(void *context);
void panicNmiFlagSet();
int  panicNmiFlagCheck();
void debug_init(void);
void debug_cpu_stop(void);
uint32_t debug_cpu_in_crash(void);
void fiqafterpanicHandler(void *context);
void debug_fatal_error(kstat_t err, char *file, int line);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_PANIC_H */
