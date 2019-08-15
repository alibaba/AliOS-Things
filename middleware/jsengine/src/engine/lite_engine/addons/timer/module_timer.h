/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef MODULE_TIMER_H
#define MODULE_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * register the add-on of RTC module
 *
 * @return  void
 */
be_jse_symbol_t *timers_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var,
                                         const char *name);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_TIMER_H */