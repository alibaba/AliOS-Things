/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR_UT__H__
#define __DEVMGR_UT__H__
#include <stdint.h>
#include <unistd.h>
#include "alink_export_zigbee.h"

#ifdef __cplusplus
extern "C" {
#endif

#define os_mutex_init()  \
    ((void *)0x010101010)

#define os_mutex_lock(mutex)
#define os_mutex_unlock(mutex)
#define os_mutex_destroy(mutex) (mutex = NULL)

#define CALL_FUCTION_FAILED         "Call function \"%s\" failed\n"

#define MAX_UUID_LEN    33
#define MAX_ATTR_NAME_LEN                  80
#define MAX_SERVICE_NAME_LEN                  80

#define MAX_ALINKGW_ATTR_HANDLER      64
#define MAX_ALINKGW_SERVICE_HANDLER   32
#define ALINKGW_MAX_VENDOR_CALLBACK_REMALLOC_COUNT  4   //回调函数返回buff不够的情况下，最多尝试4次分配内存

#define JSON_KEY_UUID       "uuid"
#define JSON_KEY_ATTRSET    "attrSet"
#define JSON_KEY_SERVICE    "service"
#define JSON_KEY_VALUE      "value"
#define JSON_KEY_WHEN       "when"
#define JSON_KEY_RESULT     "result"
#define JSON_KEY_CODE       "code"
#define JSON_KEY_ARGS       "args"
#define JSON_KEY_SET        "set"
#define JSON_KEY_EXTRA      "extra"



#define log_print(fmt, args...) \
    do {  \
        printf("[DEVMGR] %s:%4d %20s: " fmt "\n", __FILE__, __LINE__, __func__, ##args); \
    } while(0)

#define log_fatal(FMT, args...)     log_print(FMT, ##args)
#define log_error(FMT, args...)     log_print(FMT, ##args)
#define log_info(FMT, args...)      log_print(FMT, ##args)
#define log_dump(FMT, args...)      log_print(FMT, ##args)
#define log_debug(FMT, args...)     log_print(FMT, ##args)
#define log_trace(FMT, args...)     log_print(FMT, ##args)


#define RET_FAILED(ret)  (ret != SERVICE_RESULT_OK)

#define RET_GOTO(Ret,gotoTag,strError, args...)         \
      {\
        if ( RET_FAILED(Ret) )    \
        {   \
            log_trace(strError, ##args); \
            goto gotoTag; \
        }\
      }

#define RET_FALSE(Ret,strError,args...)         \
    {\
        if ( RET_FAILED(Ret) )    \
        {   \
            log_trace(strError, ##args); \
            return false; \
        }\
     }

#define RET_RETURN(Ret,strError,args...)         \
    {\
        if ( RET_FAILED(Ret) )    \
        {   \
            log_trace(strError, ##args); \
            return Ret; \
        }\
    }

#define RET_LOG(Ret,strError,args...)         \
    {\
        if ( RET_FAILED(Ret) )    \
        {   \
            log_error(strError, ##args); \
        }\
    }

#define PTR_RETURN(Pointer,Ret,strError,args...)         \
    {\
        if ( !Pointer)    \
        {   \
            log_trace(strError, ##args); \
            return Ret; \
        }\
     }

#define PTR_FALSE(Pointer,strError,args...)         \
    {\
        if ( !Pointer)    \
        {   \
            log_trace(strError, ##args); \
            return FALSE; \
        }\
    }
#define PTR_LOG(Pointer,strError,args...)         \
    {\
        if ( !Pointer)    \
        {   \
            log_error(strError, ##args); \
        }\
    }


#define PTR_GOTO(Pointer, gotoTag, strError, args...)         \
    {\
        if ( !Pointer)    \
        {   \
            log_trace(strError, ##args); \
            goto gotoTag; \
        }\
     }

#define POINTER_RETURN(Pointer,strError,args...)         \
    {\
        if ( !Pointer)    \
        {   \
            log_trace(strError, ##args); \
            return Pointer; \
        }\
     }



enum SERVICE_CODE {
    SERVICE_BUFFER_INSUFFICENT = -3,
    SERVICE_CODE_UNKNOWN       = -2,
    SERVICE_RESULT_ERR         = -1,
    SERVICE_RESULT_OK          =  0,

    SERVICE_CODE_BEGIN         = 0x0100,
    SERVICE_EVENT,
    SERVICE_DATA,
    SERVICE_CONN_INIT,
    SERVICE_CONN_READY,
    SERVICE_CONN_CLOSE,
    SERVICE_ATTACH,
    SERVICE_DETACH,
    SERVICE_STATE_INIT,
    SERVICE_STATE_PREPARE,
    SERVICE_STATE_READY,
    SERVICE_STATE_BUSY,
    SERVICE_STATE_STOP,
    SERVICE_CODE_END           = 0x01FF,
};

typedef struct alink_data {
    char *method;
    char *data;
} alink_data_t;
#define ALINK_CODE_ERROR_SIGN   (3074)

extern void *alink_cb_func[ALINK_CB_MAX_NUM];

int alink_unregister_service(const char *name);

int get_device_profile_file(uint8_t dev_type, uint32_t model_id,
                            char file_name[], int max_name_length);
int fmgr_get_global_profile(char file_name[], int max_name_length);
int get_kv(const char *key, void *value_buf, *int buf_len);
int set_kv_in_flash(const char *key, const void *value_buf, int buf_len,
                    int flag);
int remove_kv(const char *key);
int alink_register_attribute(const char *name,
                             void *get_cb,
                             void *set_cb);
int alink_register_service(const char *name, void *exec_cb);
#ifdef __cplusplus
}
#endif
#endif

