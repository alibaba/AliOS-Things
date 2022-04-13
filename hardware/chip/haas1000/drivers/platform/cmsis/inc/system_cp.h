/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SYSTEM_CP_H__
#define __SYSTEM_CP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void system_cp_reset_handler(void);

void system_cp_init(int load_code);

void system_cp_term(void);

#ifdef __cplusplus
}
#endif

#endif
