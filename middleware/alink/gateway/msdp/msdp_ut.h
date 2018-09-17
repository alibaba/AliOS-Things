/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __MSDP_UT__H__
#define __MSDP_UT__H__
#include <stdint.h>
#include <unistd.h>

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


typedef int (*ALINK_SERVICE_EXECUTE_CB)(const char *args,
                                        char *json_out_buf, unsigned int buf_sz);

#define log_print(fmt, args...) \
    do {  \
        printf("[MSDP] %s:%4d %20s: " fmt "\n", __FILE__, __LINE__, __func__, ##args); \
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
/**
 * command argument
 */
typedef struct cmd_argument {
    char arg_name[32];
    unsigned char data_type;
    struct cmd_argument *next_arg;
} cmd_argument_t;

/**
 * attr element
 */
typedef struct attr_element {
    struct attr_element *next_element;
    unsigned char data_type;/**< defined by zigbee protocol */
    char element_name[32];
    struct attr_element *child_element;
} attr_element_t;

/**
 * alink attribute profile description
 */
typedef struct attr_profile {
    char attr_name[32];
    unsigned char data_type;
    unsigned short cluster_id;
    unsigned short attribute_id;
    attr_element_t *attr_element;
    unsigned char reserved[4];
} attr_profile_t;

/**
 * alink command profile description
 */
typedef struct cmd_profile {
    char cmd_name[32];
    unsigned short cluster_id;
    unsigned char cmd_id;
    unsigned char direction; /**< 0:c to s; 1:s to c */
    cmd_argument_t *args;
    unsigned char reserved[4];
} cmd_profile_t;


int devmgr_get_dev_type(const char *devid_or_uuid, uint8_t *dev_type);
int32_t alink_post(const char *method, char *buff);
int devmgr_read_attr_cache(const char *devid_or_uuid, const char *attr_name,
                           char **attr_value);

const char *config_get_main_uuid(void);

int stdd_zbnet_update_attr_profile_cb(attr_profile_t *profile[]);

int stdd_zbnet_update_cmd_profile_cb(cmd_profile_t *profile[]);

int stdd_zbnet_get_attr_cb(uint8_t ieee_addr[8], \
                           uint8_t endpoint_id, const char *attr_set[]);

int stdd_zbnet_set_attr_cb(uint8_t ieee_addr[8], uint8_t endpoint_id, \
                           const char *attr_name, const char *attr_value);

int stdd_zbnet_exec_cmd_cb(uint8_t ieee_addr[8], uint8_t endpoint_id, \
                           const char *cmd_name, const char *cmd_args);


int stdd_zbnet_get_attr(const char *devid_or_uuid, const char *attr_name);

int stdd_zbnet_set_attr(const char *devid_or_uuid, const char *attr_name,
                        const char *attr_value);

int stdd_zbnet_exec_rpc(const char *devid_or_uuid, const char *rpc_name,
                        const char *rpc_args);

int stdd_get_device_attrset(const char *devid_or_uuid, char *attrset_buff,
                            int buff_size);

#ifdef __cplusplus
}
#endif
#endif

