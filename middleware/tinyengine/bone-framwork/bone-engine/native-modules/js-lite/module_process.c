//
//  module_process.c
//
//  Copyright © 2017 alibaba. All rights reserved.
//

#include <be_osal.h>
#include <be_jse_module.h>
#include <hal.h>
#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/yloop.h"

static void nexttick_cb(void *arg)
{
    be_jse_symbol_t* func = (be_jse_symbol_t*)arg;

    be_jse_execute_func(bone_engine_get_executor(), func, 0, NULL);

    DEC_SYMBL_REF(func);
}

static be_jse_symbol_t* module_handle_cb(be_jse_vm_ctx_t* execInfo, be_jse_symbol_t* var, const char* name)
{
    int ret;
    be_jse_symbol_t* arg0;

    if (strcmp(name,"version")==0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.1");
    }

    if (strcmp(name,"platform")==0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        // undefined reference to `aos_get_product_model'
        // $(NAME)_COMPONENTS := log yloop cli netmgr framework.common
        return new_str_symbol(aos_get_product_model());
        //return new_str_symbol("esp32");
    }

    if (strcmp(name,"nextTick")==0) {

        be_jse_handle_function(0, &arg0, 0, 0, 0);
        if( symbol_is_function(arg0) ) {
            ret = be_osal_schedule_call(nexttick_cb, arg0);
            if( ret >= 0 ) {
                // success
                INC_SYMBL_REF(arg0);
            }
        }

        symbol_unlock(arg0);

        return new_symbol(BE_SYM_NULL);
    }

    if(strcmp(name,"uptime")==0) {
        be_jse_handle_function(0, 0, 0, 0, 0);
        return new_int_symbol(aos_now_ms()/1000L);
    }

    if(strcmp(name,"memoryUsage")==0) {
        be_jse_handle_function(0, 0, 0, 0, 0);
        arg0 = new_symbol(BE_SYM_OBJECT);
#ifdef DUMP_SYMBL_USAGE
        be_jse_symbol_t *name;
        be_jse_symbol_t *val;

        name = new_int_symbol(be_jse_get_memory_count());
        val = add_symbol_node_name(arg0, name, "heapTotal");
        symbol_unlock(name);
        symbol_unlock(val);

        name = new_int_symbol(be_jse_get_memory_usage());
        val = add_symbol_node_name(arg0, name, "heapUsed");
        symbol_unlock(name);
        symbol_unlock(val);

#endif
        return arg0;
    }

    return BE_JSE_FUNC_UNHANDLED;
}

void module_process_load(void)
{
    be_jse_module_load("process", module_handle_cb);
}



