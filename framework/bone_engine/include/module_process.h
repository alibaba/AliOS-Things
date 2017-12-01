/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * module_process.h
 * process JS扩展对象
 *
 */

/*
    JS process Class




API:

process.uptime()
process.memoryUsage()
process.nextTick(callback)

成员变量
process.version
process.platform

Event:



*/


#ifndef MODULE_PROCESS_H_
#define MODULE_PROCESS_H_

#include "be_jse.h"



// 注册process类Native函数扩展
be_jse_symbol_t* process_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);


#ifdef __cplusplus
}
#endif

#endif /* MODULE_PROCESS_H_ */


