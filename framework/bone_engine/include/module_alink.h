/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * module_alink.h
 * Alink协议JS扩展对象
 *
 */

/*
    JS alink Class

API:

alink.getloglevel()
alink.setloglevel(level)
alink.getuuid()
alink.getstatus()  0: stop 1: start 2: connected
alink.isrunning()
alink.start()
alink.postdata(method, jsonCmd)
alink.stop()


Event:
    onStatusChange
JS example:
alink.on("onStatusChange", function(status, uuid){
});

    onSetAttrs
alink.on("onSetAttrs", function(jsonBuf){
});

    onGetAttrs
alink.on("onGetAttrs", function(jsonBuf){
});


*/


#ifndef MODULE_ALINK_H_
#define MODULE_ALINK_H_

#include "be_jse.h"

typedef enum {
    ALINK_GETLOGLEVEL = 0,
    ALINK_SETLOGLEVEL,
    ALINK_GETUUID,
    ALINK_GETSTATUS,
    ALINK_ISRUNNING,
    ALINK_START,
    ALINK_STOP,
    ALINK_POSTDATA,
} ALINK_CMD_e;

typedef int (*ALINK_CMD_CB)(ALINK_CMD_e cmd, int arg0, void* arg1, void* arg2);

// JavaScript调用C层接口
void alink_module_register_cmd_callback(ALINK_CMD_CB cb);


// event
// 通知JavaScript层
void alink_module_onStatusChange(int status, const char* uuid);
void alink_module_onSetAttrs(const char* jsonBuf);
void alink_module_onGetAttrs(const char* jsonBuf);


// 注册Alink类Native函数扩展
be_jse_symbol_t *alink_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);


// only debug, 释放异步函数句柄
void alink_module_release();


#ifdef __cplusplus
}
#endif

#endif /* MODULE_ALINK_H_ */

