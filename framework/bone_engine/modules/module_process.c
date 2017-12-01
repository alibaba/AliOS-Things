/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "module_process.h"
#include "be_jse_api.h"

#ifdef USE_PROCESS

#include "aos/kernel.h"
#include "aos/yloop.h"

void nextTick_cb(void *arg)
{
    be_jse_symbol_t* func = (be_jse_symbol_t*)arg;

    be_jse_execute_func(bone_engine_get_executor(), func, 0, NULL);

    DEC_SYMBL_REF(func);
}

be_jse_symbol_t* process_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    int ret;
    be_jse_symbol_t* arg0;
    char buf[128]= {0};
    if (get_symbol_node_id(var) == execInfo->executor->processClass) {

        printf("[%s][%d] process.%s  \n", __FUNCTION__, __LINE__, name);

        if (strcmp(name,"nextTick")==0) {

            be_jse_handle_function(0, &arg0, 0, 0, 0);
            if( symbol_is_function(arg0) ) {
                ret = aos_schedule_call(nextTick_cb, arg0);
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
            arg0 = new_str_symbol("");
#ifdef DUMP_SYMBL_USAGE
            snprintf(buf, 127, "{ heapTotal: %d,\r\n heapUsed: %d }", BE_JSE_SYMBOL_TABLE_SIZE, be_jse_get_memory_usage());
#endif
            symbol_str_append(arg0, buf);
            return arg0;
        }

#ifdef USE_AOS
        if (strcmp(name,"version")==0) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol("0.0.1");
        }

        if (strcmp(name,"platform")==0) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol(aos_get_product_model());
        }
#endif

    }

    return BE_JSE_FUNC_UNHANDLED;
}

#endif

