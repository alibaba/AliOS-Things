/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "be_jse_api.h"
#include "module_alink.h"


#ifdef USE_ALINK

static ALINK_CMD_CB      cmdFunc = NULL;


// 事件
typedef enum {
    onStatusChange = 0,
    onSetAttrs,
    onGetAttrs
} ALINK_EVENT_e;
static be_jse_symbol_t      *onStatusChangeFunc = NULL;
static be_jse_symbol_t      *onSetAttrsFunc = NULL;
static be_jse_symbol_t      *onGetAttrsFunc = NULL;

// c call javascript
void alink_jse_save_event_func(ALINK_EVENT_e event, be_jse_symbol_t *listener)
{
    switch( event ) {
        case onStatusChange:
            if( onStatusChangeFunc ) {
                DEC_SYMBL_REF(onStatusChangeFunc);
                onStatusChangeFunc = NULL;
            }
            onStatusChangeFunc = listener;
            if( onStatusChangeFunc )
                INC_SYMBL_REF(onStatusChangeFunc);
            break;
        case onSetAttrs:
            if( onSetAttrsFunc ) {
                DEC_SYMBL_REF(onSetAttrsFunc);
                onSetAttrsFunc = NULL;
            }
            onSetAttrsFunc = listener;
            if( onSetAttrsFunc )
                INC_SYMBL_REF(onSetAttrsFunc);
            break;
        case onGetAttrs:
            if( onGetAttrsFunc ) {
                DEC_SYMBL_REF(onGetAttrsFunc);
                onGetAttrsFunc = NULL;
            }
            onGetAttrsFunc = listener;
            if( onGetAttrsFunc )
                INC_SYMBL_REF(onGetAttrsFunc);
            break;
        default:
            return;
    }

}


void alink_module_register_cmd_callback(ALINK_CMD_CB cb)
{
    cmdFunc = cb;
}

void alink_module_onStatusChange(int status, const char* uuid)
{

    if (onStatusChangeFunc) {

        be_jse_symbol_t* argv[2];

        argv[0] = symbol_lock(get_symbol_node_id(new_int_symbol(status))); // status
        if( uuid )
            argv[1] = symbol_lock(get_symbol_node_id(new_str_symbol(uuid))); // uuid
        else
            argv[1] = symbol_lock(get_symbol_node_id(new_symbol(BE_SYM_NULL)));

        symbol_relock(onStatusChangeFunc);

        be_jse_execute_func(bone_engine_get_executor(), onStatusChangeFunc, 2, argv);

        symbol_unlock(argv[0]);
        symbol_unlock(argv[1]);

        symbol_unlock(onStatusChangeFunc);

    }
}


static void alink_jse_onAttrs(be_jse_symbol_t* func, const char* jsonBuf)
{
    if( func) {

        be_jse_symbol_t* argv[2];

        argv[0] = symbol_lock(get_symbol_node_id(new_str_symbol(jsonBuf)));
        symbol_relock(func);

        be_jse_execute_func(bone_engine_get_executor(), func, 1, argv);

        symbol_unlock(argv[0]);

        symbol_unlock(func);
    }
}

void alink_module_onSetAttrs(const char* jsonBuf)
{
    alink_jse_onAttrs(onSetAttrsFunc, jsonBuf);
}

void alink_module_onGetAttrs(const char* jsonBuf)
{
    alink_jse_onAttrs(onGetAttrsFunc, jsonBuf);
}

void alink_module_release()
{

    if( onStatusChangeFunc ) {
        DEC_SYMBL_REF(onStatusChangeFunc);
        onStatusChangeFunc = NULL;
    }

    if( onSetAttrsFunc ) {
        DEC_SYMBL_REF(onSetAttrsFunc);
        onSetAttrsFunc = NULL;
    }

    if( onGetAttrsFunc ) {
        DEC_SYMBL_REF(onGetAttrsFunc);
        onGetAttrsFunc = NULL;
    }
}

be_jse_symbol_t *alink_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)

{
    be_jse_symbol_t *arg;
    be_jse_symbol_t *eventName;
    be_jse_symbol_t *listener;
    int arg0=0;

    char method[32];
    char buf[256];          // 限定JSON字符串的最大长度, 根据实际情况修改或使用aos_malloc

    if (get_symbol_node_id(var) == execInfo->executor->alinkClass) {

        if (strcmp(name,"on")==0) {

            be_jse_handle_function(0, &eventName, &listener, 0, 0);

            symbol_to_str(eventName, buf, 256);

            printf("[%s][%d] Alink.on(%s, listener = %p) \n", __FUNCTION__, __LINE__, buf, listener);
            if(strcmp(buf, "onStatusChange")==0) {
                alink_jse_save_event_func(onStatusChange,listener);
            } else if(strcmp(buf, "onSetAttrs")==0) {
                alink_jse_save_event_func(onSetAttrs,listener);
            } else if(strcmp(buf, "onGetAttrs")==0) {
                alink_jse_save_event_func(onGetAttrs,listener);
            }

            symbol_unlock(eventName);
            symbol_unlock(listener);

            return new_symbol(BE_SYM_NULL);
        }

        if(strcmp(name, "getloglevel")==0) {

            be_jse_handle_function(0, 0, 0, 0, 0);
            arg0 = 0;
            if(cmdFunc)
                arg0 = cmdFunc(ALINK_GETLOGLEVEL, 0, NULL, NULL);

            return new_int_symbol(arg0);

        }

        if( strcmp(name, "setloglevel")==0) {

            be_jse_handle_function(0, &arg, 0, 0, 0);

            arg0 = (int)get_symbol_int(arg);

            symbol_unlock(arg);

            if(cmdFunc)
                cmdFunc(ALINK_SETLOGLEVEL, arg0, NULL, NULL);

            return new_symbol(BE_SYM_NULL);

        }

        if( strcmp(name, "getuuid")==0) {
            buf[0] = 0;
            be_jse_handle_function(0, 0, 0, 0, 0);

            if(cmdFunc)
                arg0 = cmdFunc(ALINK_GETUUID, 0, buf, NULL);

            return new_str_symbol(buf);

        }

        if( strcmp(name, "getstatus")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            arg0 = 0;
            if(cmdFunc)
                arg0 = cmdFunc(ALINK_GETSTATUS, 0, NULL, NULL);

            return new_int_symbol(arg0);

        }

        if( strcmp(name, "isrunning")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            arg0 = 0;
            if(cmdFunc)
                arg0 = cmdFunc(ALINK_ISRUNNING, 0, NULL, NULL);

            return new_int_symbol(arg0);

        }

        if( strcmp(name, "start")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            if(cmdFunc)
                arg0 = cmdFunc(ALINK_START, 0, NULL, NULL);
            return new_int_symbol(arg0);
        }

        if( strcmp(name, "stop")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            if(cmdFunc)
                arg0 = cmdFunc(ALINK_STOP, 0, NULL, NULL);
            return new_int_symbol(arg0);

        }

        if( strcmp(name, "postdata")==0) {
            be_jse_handle_function(0, &arg, &eventName, 0, 0);

            symbol_to_str(arg, method, 32);
            symbol_to_str(eventName, buf, 256);

            printf("[%s][%d] Alink.postdata(jsonStr = %s)  cmdFunc = %p \n", __FUNCTION__, __LINE__, buf, cmdFunc);

            if(cmdFunc)
                arg0 = cmdFunc(ALINK_POSTDATA, 0, method, buf);

            symbol_unlock(arg);
            symbol_unlock(eventName);

            return new_symbol(BE_SYM_NULL);
        }
    }


    return BE_JSE_FUNC_UNHANDLED;

}

#endif

