/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_DAC_SYSCALL_ARG_H
#define HAL_DAC_SYSCALL_ARG_H

#include "aos/hal/dac.h"

typedef struct {
    dac_dev_t *dac;
} hal_dac_init_syscall_arg_t;

typedef struct {
    dac_dev_t *dac;
    uint32_t   channel;
} hal_dac_start_syscall_arg_t;

typedef struct {
    dac_dev_t *dac;
    uint32_t   channel;
} hal_dac_stop_syscall_arg_t;

typedef struct {
    dac_dev_t *dac;
    uint32_t   channel;
    uint32_t   data;
} hal_dac_set_value_syscall_arg_t;

typedef struct {
    dac_dev_t *dac;
    uint32_t   channel;
} hal_dac_get_value_syscall_arg_t;

typedef struct {
    dac_dev_t *dac;
} hal_dac_finalize_syscall_arg_t;

#endif /* HAL_DAC_SYSCALL_ARG_H */

