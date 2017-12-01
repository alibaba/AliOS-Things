/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BE_JSE_TIMERS_H_
#define BE_JSE_TIMERS_H_

/*
timers全局函数

setTimeout(callback, delay)
clearTimeout(timeoutObject)
setInterval(callback, delay)
clearInterval(intervalObject)



*/

be_jse_symbol_t* timers_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);

#endif
