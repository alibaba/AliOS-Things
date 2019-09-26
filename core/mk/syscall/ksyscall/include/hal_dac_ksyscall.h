/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_DAC_KSYSCALL_H
#define HAL_DAC_KSYSCALL_H

int32_t sys_hal_dac_init_stub(void *arg);

int32_t sys_hal_dac_start_stub(void *arg);

int32_t sys_hal_dac_stop_stub(void *arg);

int32_t sys_hal_dac_set_value_stub(void *arg);

int32_t sys_hal_dac_get_value_stub(void *arg);

int32_t sys_hal_dac_finalize_stub(void *arg);

#endif /* HAL_DAC_KSYSCALL_H */

