/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_LINKKIT_SYSCALL > 0)

#include "stdio.h"
#include "syscall.h"
#include "dev_model_api.h"
#include "linkkit_syscall_arg.h"
#include "syscall_no.h"


int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info)
{
    IOT_Linkkit_Open_syscall_arg_t arg;

    arg.dev_type  = dev_type;
    arg.meta_info = meta_info;

    return (int)SYSCALL(SYS_IOT_LINKKIT_OPEN, &arg);
}


int IOT_Linkkit_Connect(int devid)
{
    IOT_Linkkit_Connect_syscall_arg_t arg;

    arg.devid = devid;

    return (int)SYSCALL(SYS_IOT_LINKKIT_CONNECT, &arg);
}

void IOT_Linkkit_Yield(int timeout_ms)
{
    IOT_Linkkit_Yield_syscall_arg_t arg;

    arg.timeout_ms = timeout_ms;

    (void)SYSCALL(SYS_IOT_LINKKIT_YIELD, &arg);
}

int IOT_Linkkit_Close(int devid)
{
    IOT_Linkkit_Close_syscall_arg_t arg;

    arg.devid = devid;

    return (int)SYSCALL(SYS_IOT_LINKKIT_CLOSE, &arg);
}

int IOT_Linkkit_Report(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    IOT_Linkkit_Report_syscall_arg_t arg;

    arg.devid       = devid;
    arg.msg_type    = msg_type;
    arg.payload     = payload;
    arg.payload_len = payload_len;

    return (int)SYSCALL(SYS_IOT_LINKKIT_REPORT, &arg);
}

int IOT_Linkkit_Query(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    IOT_Linkkit_Query_syscall_arg_t arg;

    arg.devid       = devid;
    arg.msg_type    = msg_type;
    arg.payload     = payload;
    arg.payload_len = payload_len;

    return (int)SYSCALL(SYS_IOT_LINKKIT_QUERY, &arg);
}

int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len)
{
    IOT_Linkkit_TriggerEvent_syscall_arg_t arg;

    arg.devid       = devid;
    arg.eventid     = eventid;
    arg.eventid_len = eventid_len;
    arg.payload     = payload;
    arg.payload_len = payload_len;

    return (int)SYSCALL(SYS_IOT_lINKKIT_TRIGGEREVENT, &arg);
}

void IOT_SetLogLevel(IOT_LogLevel level)
{
    IOT_SetLogLevel_syscall_arg_t arg;

    arg.level = level;

    (void)SYSCALL(SYS_IOT_SETLOGLEVEL, &arg);
}

void IOT_DumpMemoryStats(IOT_LogLevel level)
{
    IOT_DumpMemoryStats_syscall_arg_t arg;

    arg.level = level;

    (void)SYSCALL(SYS_IOT_DUMPMEMORYSTATS, &arg);
}

int IOT_SetupConnInfo(const char *product_key,
                      const char *device_name,
                      const char *device_secret,
                      void **info_ptr)
{
    IOT_SetupConnInfo_syscall_arg_t arg;

    arg.product_key   = product_key;
    arg.device_name   = device_name;
    arg.device_secret = device_secret;
    arg.info          = info_ptr;

    return (int)SYSCALL(SYS_IOT_SETUPCONNINFO, &arg);
}

int IOT_Ioctl(int option, void *data)
{
    IOT_Ioctl_syscall_arg_t arg;

    arg.option = option;
    arg.data = data;

    return (int)SYSCALL(SYS_IOT_IOCTL, &arg);

}

int IOT_HAL_Ioctl(int cmd, void *arg)
{
    IOT_HAL_Ioctl_syscall_arg_t _arg;

    _arg.cmd = cmd;
    _arg.arg = arg;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &_arg);
}


static int _IOT_RegisterCallback(iotx_ioctl_event_t evt, void *cb)
{
    IOT_RegisterCallback_syscall_arg_t arg;

    arg.event    = evt;
    arg.callback = cb;

    return (int)SYSCALL(SYS_IOT_REGSITERCALLBACK, &arg);
}

int iotx_register_for_ITE_AWSS_STATUS(int (*callback)(int))
{
    return _IOT_RegisterCallback(ITE_AWSS_STATUS, (void*)callback);
}

int iotx_register_for_ITE_CONNECT_SUCC(int (*callback)(void))
{
    return _IOT_RegisterCallback(ITE_CONNECT_SUCC, (void*)callback);
}

int iotx_register_for_ITE_CONNECT_FAIL(int (*callback)(void))
{
    return _IOT_RegisterCallback(ITE_CONNECT_FAIL, (void*)callback);
}

int iotx_register_for_ITE_DISCONNECTED(int (*callback)(void))
{
    return _IOT_RegisterCallback(ITE_AWSS_STATUS, (void*)callback);
}

int iotx_register_for_ITE_RAWDATA_ARRIVED(int (*callback)(const int, const unsigned char *, const int))
{
    return _IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, callback);
}

int iotx_register_for_ITE_SERVICE_REQUEST(int (*callback)(const int, const char *, const int, const char *,
            const int, char **, int *))
{
    return _IOT_RegisterCallback(ITE_SERVICE_REQUEST, (void*)callback);
}

int iotx_register_for_ITE_PROPERTY_SET(int (*callback)(const int, const char *, const int))
{
    return _IOT_RegisterCallback(ITE_PROPERTY_SET, (void*)callback);
}

#ifdef DEVICE_MODEL_SHADOW
int iotx_register_for_ITE_PROPERTY_DESIRED_GET_REPLY(int (*callback)(const char *, const int))
{
    return _IOT_RegisterCallback(ITE_PROPERTY_DESIRED_GET_REPLY, (void*)callback);
}
#endif

int iotx_register_for_ITE_PROPERTY_GET(int (*callback)(const int, const char *, const int, char **, int *))
{
    return _IOT_RegisterCallback(ITE_PROPERTY_GET, (void*)callback);
}

int iotx_register_for_ITE_REPORT_REPLY(int (*callback)(const int, const int, const int, const char *, const int))
{
    return _IOT_RegisterCallback(ITE_REPORT_REPLY, (void*)callback);
}

int iotx_register_for_ITE_TRIGGER_EVENT_REPLY(int (*callback)(const int, const int, const int, const char *,
            const int, const char *, const int))
{
    return _IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, (void*)callback);
}

int iotx_register_for_ITE_TIMESTAMP_REPLY(int (*callback)(const char *))
{
    return _IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, (void*)callback);
}

int iotx_register_for_ITE_TOPOLIST_REPLY(int (*callback)(const int, const int, const int, const char *, const int))
{
    return _IOT_RegisterCallback(ITE_TOPOLIST_REPLY, (void*)callback);
}

int iotx_register_for_ITE_PERMIT_JOIN(int (*callback)(const char *, int))
{
    return _IOT_RegisterCallback(ITE_PERMIT_JOIN, (void*)callback);
}

int iotx_register_for_ITE_INITIALIZE_COMPLETED(int (*callback)(const int))
{
    return _IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, (void*)callback);
}

int iotx_register_for_ITE_FOTA(int (*callback)(const int, const char *))
{
    return _IOT_RegisterCallback(ITE_FOTA, (void*)callback);
}

int iotx_register_for_ITE_COTA(int (*callback)(const int, const char *, int, const char *,
            const char *, const char *, const char *))
{
    return _IOT_RegisterCallback(ITE_COTA, (void*)callback);
}

int iotx_register_for_ITE_MQTT_CONNECT_SUCC(int (*callback)(void))
{
    return _IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC, (void*)callback);
}

int iotx_register_for_ITE_CLOUD_ERROR(int (*callback)(const int, const char *, const char *))
{
    return _IOT_RegisterCallback(ITE_CLOUD_ERROR, (void*)callback);
}

/* HAL API */
int HAL_SetProductKey(char *product_key)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_SET_PRODUCT_KEY;
    arg.arg = (void*)product_key;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);
}

int HAL_GetProductKey(char *product_key)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_GET_PRODUCT_KEY;
    arg.arg = (void*)product_key;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);

}

int HAL_SetProductSecret(char *product_secret)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_SET_PRODUCT_SECRET;
    arg.arg = (void*)product_secret;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);

}

int HAL_GetProductSecret(char *product_secret)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_GET_PRODUCT_SECRET;
    arg.arg = (void*)product_secret;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);

}

int HAL_SetDeviceName(char *device_name)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_SET_DEVICE_NAME;
    arg.arg = (void*)device_name;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);
}

int HAL_GetDeviceName(char *device_name)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_GET_DEVICE_NAME;
    arg.arg = (void*)device_name;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);

}

int HAL_SetDeviceSecret(char *device_secret)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_SET_DEVICE_SECRET;
    arg.arg = (void*)device_secret;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);
}

int HAL_GetDeviceSecret(char *device_secret)
{
    IOT_HAL_Ioctl_syscall_arg_t arg;

    arg.cmd = (unsigned int)IOT_HAL_GET_DEVICE_SECRET;
    arg.arg = (void*)device_secret;

    return (int)SYSCALL(SYS_IOT_HAL_IOCTL, &arg);
}

#endif /* RHINO_CONFIG_LINKKIT_SYSCALL */

