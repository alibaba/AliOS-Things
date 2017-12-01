/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_bi_module.h
 *
 * builtin对象方法实现
 * native扩展方法实现
 *
 *
 */

#ifndef _BE_BI_MODULE_H_
#define _BE_BI_MODULE_H_

#include "be_jse.h"
#include "be_jse_executor.h"

typedef be_jse_symbol_t *(*BE_JSE_FUNCTION_EXECUTE_CB)(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name);

/*
  JS native 函数扩展
*/
void be_jse_register_function_cb(BE_JSE_FUNCTION_EXECUTE_CB func_cb);
be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name);



#endif /* _BE_BI_MODULE_H_ */



