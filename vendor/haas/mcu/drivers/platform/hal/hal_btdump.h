/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __HAL_BTDUMP_H__
#define __HAL_BTDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

void hal_btdump_clk_enable(void);
void hal_btdump_clk_disable(void);

void hal_btdump_enable(void);
void hal_btdump_disable(void);

#ifdef __cplusplus
}
#endif

#endif