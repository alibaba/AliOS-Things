/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_arch.h>

void exceptionHandler(void *context);

void dispatcher_exception(long arg, long exc_type, context_t *contex)
{
    switch (exc_type) {
    case CAUSE_MACHINE_ECALL:
        /* This is a task scheduling request */
        if (arg == 0) {
            return;
        }
        break;
    default:
        exceptionHandler(contex);
        break;
    }
    return;
}

