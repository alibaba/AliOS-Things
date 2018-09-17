/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include <string.h>
#include "iotx_system.h"
#include "iot_import.h"
#include "iot_export.h"

#include "iotx_utils.h"

int iotx_midreport_reqid(char *requestId, char *product_key, char *device_name)
{
    int ret;
    /* requestId = pk+devicename+mid */
    ret = HAL_Snprintf(requestId,
                       MIDREPORT_REQID_LEN,
                       "%s_%s_mid",
                       product_key,
                       device_name);
    return ret;
}

int iotx_midreport_payload(char *msg, char *requestId, char *mid, char *pid)
{
    int ret;
    /*topic's json data: {"id":"requestId" ,"params":{"_sys_device_mid":mid,"_sys_device_pid":pid }}*/
    ret = HAL_Snprintf(msg,
                       MIDREPORT_PAYLOAD_LEN,
                       "{\"id\":\"%s\",\"params\":{\"_sys_device_mid\":\"%s\",\"_sys_device_pid\":\"%s\"}}",
                       requestId,
                       mid,
                       pid);
    return ret;
}

int iotx_midreport_topic(char *topic_name, char *topic_head, char *product_key, char *device_name)
{
    int ret;
    /* reported topic name: "/sys/${productKey}/${deviceName}/thing/status/update" */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "%s/sys/%s/%s/thing/status/update",
                       topic_head,
                       product_key,
                       device_name);
    return ret;
}

/* generate AOS versoin report topic */
int iotx_gen_aos_report_topic(char *topic_name, char *product_key, char *device_name)
{
    int ret;
    /* linkkit version topic name: "/sys/${productKey}/${deviceName}/thing/deviceinfo/update" */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       product_key,
                       device_name);
    return ret;
}

/* generate AOS version report message json data */
int iotx_gen_aos_report_payload(char *msg, int requestId, char *versionData)
{
    /* Topic's Json data
    {
        "id" : 123,
        "version":"1.0",
        "params" : [{
            "attrKey" : "SYS_ALIOS_ACTIVATION",
            "attrValue" : "some data",
            "domain" : "SYSTEM"
        }],
        "method" : "thing.deviceinfo.update"
    }
    */
    int ret;
    ret = HAL_Snprintf(msg,
                       AOS_VERSON_MSG_LEN,
                       "{\"id\":\"%d\",\"versoin\":\"1.0\",\"params\":[{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"}],\"method\":\"thing.deviceinfo.update\"}",
                       requestId,
                       versionData
                      );
    return ret;
}


