/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __BE_IOT_MODULE_H__
#define __BE_IOT_MODULE_H__

#include "be_jse.h"
#include "be_jse_api.h"

be_jse_symbol_t *be_iot_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);

void be_module_init(be_jse_executor_ctx_t *parse);

void be_module_exit();

#endif /* __BE_IOT_MODULE_H__ */

