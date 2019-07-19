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

#ifdef AOS_UND
/* reboot reason*/
#define SYS_REBOOT_REASON "reboot reason"

typedef struct {
    unsigned int return_reason;
    unsigned int update_reason;
} debug_reason_t;

void         debug_reboot_reason_update(unsigned int reason);
unsigned int debug_reboot_reason_get(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_PANIC_H */
