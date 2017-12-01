/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * module_gpio.h
 * gpio JS扩展对象
 *
 */

/*
    JS gpio Class




API:


gpio.open(id,config)
gpio.write(id,value)
gpio.read(id)
gpio.close(id)


Event:



*/


#ifndef MODULE_GPIO_H_
#define MODULE_GPIO_H_

#include "be_jse.h"


// 注册gpio类Native函数扩展
be_jse_symbol_t* gpio_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);


// 内部函数，内部调用(be_jse_executor_init)
// gpio_dev_t初始化, index id与硬件port映射
void gpio_module_init();


#ifdef __cplusplus
}
#endif

#endif /* MODULE_GPIO_H_ */



