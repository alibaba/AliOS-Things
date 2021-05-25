/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef K_ARCH_H
#define K_ARCH_H

#include <stdint.h>
#include "k_types.h"

typedef struct {
    long X1;
    long X3;
    long X4;
    long X5;
    long X6;
    long X7;
    long X8;
    long X9;
    long X10;
    long X11;
    long X12;
    long X13;
    long X14;
    long X15;
    long X16;
    long X17;
    long X18;
    long X19;
    long X20;
    long X21;
    long X22;
    long X23;
    long X24;
    long X25;
    long X26;
    long X27;
    long X28;
    long X29;
    long X30;
    long X31;
    long PC;
    long FPU[32];
} context_t;

typedef struct {
    long exc_type;  /* ARM exception IDs */
    long SP;
    context_t cntx;
} fault_context_t;

#endif /* K_ARCH_H */
