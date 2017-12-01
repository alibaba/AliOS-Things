/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "be_iot_module.h"

#ifdef USE_ALINK
#include "module_alink.h"
#include "aos_alink_impl.h"
#endif

#ifdef USE_PROCESS
#include "module_process.h"
#endif

#ifdef USE_GPIO
#include "module_gpio.h"
#endif

#ifdef USE_TIMERS
#include "module_timers.h"
#endif

#ifdef USE_UART
#include "module_uart.h"
#endif

be_jse_symbol_t *be_iot_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    be_jse_symbol_t* value = BE_JSE_FUNC_UNHANDLED;

#ifdef USE_ALINK
    value = alink_module_handle_cb(execInfo, var, name);
    if( value != BE_JSE_FUNC_UNHANDLED )
        return value;
#endif

#ifdef USE_PROCESS
    value = process_module_handle_cb(execInfo, var, name);
    if( value != BE_JSE_FUNC_UNHANDLED )
        return value;
#endif

#ifdef USE_GPIO
    value = gpio_module_handle_cb(execInfo, var, name);
    if( value != BE_JSE_FUNC_UNHANDLED )
        return value;
#endif

#ifdef USE_TIMERS
    value = timers_module_handle_cb(execInfo, var, name);
    if (value != BE_JSE_FUNC_UNHANDLED)
        return value;
#endif

#ifdef USE_UART
    value = uart_module_handle_cb(execInfo, var, name);
    if( value != BE_JSE_FUNC_UNHANDLED )
        return value;
#endif

    return value;
}

void be_module_init(be_jse_executor_ctx_t *executor)
{
    be_jse_symbol_t *name;

#ifdef USE_ALINK
    // alink 协议扩展
    name = lookup_named_child_symbol(executor->root, "alink", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->alinkClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);


    // only demo
    // 使用 健康-体重秤
    // alink初始化
    aos_alink_init();

#endif

#ifdef USE_PROCESS
    // process 扩展
    name = lookup_named_child_symbol(executor->root, "process", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->processClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

#if 0
    /*
        be_jse_symbol_t* process_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
        JS应用程序只读访问
    */

    // 如下方法是注册到JSE符号表中, JS应用程序可以读写
    be_jse_symbol_t* value = new_str_symbol("0.0.1");
    symbol_unlock(add_symbol_node_name(executor->processClass, get_symbol_node_id(value), "version"));
    symbol_unlock(value);

    value = new_str_symbol(aos_get_product_model());
    symbol_unlock(add_symbol_node_name(executor->processClass, get_symbol_node_id(value), "platform"));
    symbol_unlock(value);

#endif
#endif

#ifdef USE_GPIO
    // gpio 扩展
    name = lookup_named_child_symbol(executor->root, "gpio", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->gpioClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    gpio_module_init();
#endif

#ifdef USE_UART
    name = lookup_named_child_symbol(executor->root, "uart", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->uartClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

#endif

}

void be_module_exit()
{
    // only debug
#ifdef USE_ALINK
    aos_alink_exit();
    alink_module_release();
#endif

}


