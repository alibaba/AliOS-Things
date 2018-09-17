/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/errno.h>

#include "msdp_ut.h"
#include "msdp.h"

#define GW_UUID      "123412412312312312312sdfsdgdgdew"
#define ZBDEV_UUID       "5465465465465464865465asdfa12312"
#define ATTR1_NAME   "attr1"
#define ATTR2_NAME   "attr2"

//static char attr1_value[32] = "attrvalue1";
//static char attr2_value[32] = "attrvalue2";
static char attr1_value[128] = "{\"element\":\"value1\"}";
static char attr2_value[128] = "[\"element1\",\"element2\"]";

void dump_all_type_handler();

int stdd_zbnet_supervision_interval_cb(uint8_t ieee_addr[8], uint8_t interval)
{
    return 0;
}

int devmgr_read_attr_cache(const char *devid_or_uuid, const char *attr_name,
                           char **attr_value)
{
    log_trace("read attr cache, devid:%s, attr_name:%s", devid_or_uuid, attr_name);
    char *value_str = NULL;
    if (strcmp(attr_name, ATTR1_NAME) == 0) {
        value_str = (char *)attr1_value;
    } else if (strcmp(attr_name, ATTR2_NAME) == 0) {
        value_str = (char *)attr2_value;
    } else {
        return -1;
    }

    *attr_value = malloc(strlen(value_str) + 1);
    memset(*attr_value, 0, strlen(value_str) + 1);
    strcpy(*attr_value, value_str);

    return 0;
}

int devmgr_get_dev_type(const char *devid_or_uuid, uint8_t *dev_type)
{
    *dev_type = DEV_TYPE_ZIGBEE;
    return 0;
}

int32_t alink_post(const char *method, char *buff)
{
    log_trace("alink post, mehtod:%s, buff:%s", method, buff);
    return 0;
}

int stdd_zbnet_get_attr(const char *devid_or_uuid, const char *attr_name)
{
    log_trace("get attibute, uuid:%s, attrname:%s", devid_or_uuid, attr_name);
    return 0;
}

int stdd_zbnet_set_attr(const char *devid_or_uuid, const char *attr_name,
                        const char *attr_value)
{
    log_trace("set attribute, uuid:%s, attrname:%s, attrvalue:%s", devid_or_uuid,
              attr_name, attr_value);
    if (strcmp(attr_name, ATTR1_NAME) == 0) {
        strcpy(attr1_value, attr_value);
    } else if (strcmp(attr_name, ATTR2_NAME) == 0) {
        strcpy(attr2_value, attr_value);
    } else {
        return -1;
    }

    return 0;
}

int stdd_zbnet_exec_rpc(const char *devid_or_uuid, const char *rpc_name,
                        const char *rpc_args)
{
    log_trace("exec_rpc success, uuid:%s", devid_or_uuid);
    return 0;
}

int stdd_get_device_attrset(const char *devid_or_uuid, char *attrset_buff,
                            int buff_size)
{
    log_trace("get device attrset, uuid:%s", devid_or_uuid);
    snprintf(attrset_buff, buff_size, "[\"attr1\",\"attr2\"]");
    return 0;
}

const char *config_get_main_uuid(void)
{
    return GW_UUID;
}



int gw_get_attr1_cb(char *attr_value_buff, int buff_size)
{
    log_trace("get attr1, value:%s", attr1_value);
    strcpy(attr_value_buff, attr1_value);
    return 0;
}

int gw_set_attr1_cb(char *attr_value)
{
    log_trace("set attr1, value:%s", attr_value);
    strcpy(attr1_value, attr_value);
    return 0;
}

int gw_get_attr2_cb(char *attr_value_buff, int buff_size)
{
    log_trace("get attr2, value:%s", attr2_value);
    strcpy(attr_value_buff, attr2_value);
    return 0;
}

int gw_set_attr2_cb(char *attr_value)
{
    log_trace("set attr2, value:%s", attr_value);
    strcpy(attr2_value, attr_value);

}

int gw_rpc_reboot_cb(char *args, char *rpc_result, int buff_size)
{
    log_trace("rpc reboot, args:%s", args);
    return 0;
}


void dev_attr_init()
{
    msdp_register_attr_cb(DEV_TYPE_GATEWAY, ATTR1_NAME, gw_get_attr1_cb,
                          gw_set_attr1_cb);
    msdp_register_attr_cb(DEV_TYPE_GATEWAY, ATTR2_NAME, gw_get_attr2_cb,
                          gw_set_attr2_cb);
    msdp_register_service_cb(DEV_TYPE_GATEWAY, "reboot", gw_rpc_reboot_cb);
}

int get_attr_ut()
{
    int ret = SERVICE_RESULT_ERR;
    char params[1024] = {0};

    sprintf(params, GET_DEVICE_ATTR_STRING_FMT, GW_UUID, "[]");

    log_trace("params:%s", params);

    ret = msdp_dispatch_event(GW_UUID, METHOD_GET_DEVICE_STATUS, params);
    RET_RETURN(ret, "get device status fail, params:%s", params);

    log_trace("get device status success\n");
    return ret;
}


int set_attr_ut()
{
    int ret = SERVICE_RESULT_ERR;
    char params[1024] = {0};

    sprintf(params, SET_DEVICE_ATTR_STRING_FMT, GW_UUID, "[\"attr1\",\"attr2\"]",
            "\"attr1\":{\"value\":[\"element1\",\"element2\"]},\"attr2\":{\"value\":{\"element\":\"elementvalue\"}}");

    log_trace("params:%s", params);

    ret = msdp_dispatch_event(GW_UUID, METHOD_SET_DEVICE_STATUS, params);
    RET_RETURN(ret, "set device status fail, params:%s", params);

    ret = msdp_set_device_status(ZBDEV_UUID, ATTR1_NAME, "attr_value");
    RET_RETURN(ret, "set device status fail");

    ret = msdp_request_remote_service(ZBDEV_UUID, "reboot", "{\"args\":\"test\"");
    RET_RETURN(ret, "request remote service fail");

    log_trace("set device status success, params:%s\n", params);
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = msdp_init();
    RET_RETURN(ret, "msdp_init fail");

    dev_attr_init();
    dump_all_type_handler();

    //get_attr_ut();
    set_attr_ut();
    //get_attr_ut();

    return 0;
}

