/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef PANIC_H
#define PANIC_H

#if (RHINO_CONFIG_USER_SPACE > 0)

typedef enum {
    PANIC_IN_KERNEL,
    PANIC_IN_USER_KERNEL,
    PANIC_IN_USER,
    PANIC_UNDEFINED
} uspace_panic_type;

void debug_usapce_context_get(void *task, char **pPC, char **pLR, int **pSP);

void debug_uspace_panic_type_set(void *context);

uspace_panic_type debug_uspace_panic_type_get(void);
#endif

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...));

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP);

#endif /* PANIC_H */
