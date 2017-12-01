/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_jse_uart.h
 * gpio JS扩展对象
 *
 */

/*
    JS uart Class



API:

return  0 : on success

uart.open(config);
example:
    config.port=1;
    config.baudRate=115200;
    config.dataBits=8;
    var serial=uart.open(config);

uart.close(port);

uart.write(port, data);

uart.read(port, timeout);

Event:



*/


#ifndef BE_JSE_UART_H_
#define BE_JSE_UART_H_

#include "be_jse.h"


// 注册uart类Native函数扩展
be_jse_symbol_t* uart_module_handle_cb(be_jse_vm_ctx_t *vm, be_jse_symbol_t *var, const char *name);



#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_UART_H_ */




