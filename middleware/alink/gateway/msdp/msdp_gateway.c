/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "aos/aos.h"
#include "msdp_common.h"
#include "json_parser.h"
#include "service.h"
#include "devmgr.h"
#include "msdp.h"

#define MODULE_NAME MODULE_NAME_MSDP

static int __get_attribute(const char *uuid, const char *attr_name,
                           char **ppbuf)
{
    int ret = SERVICE_RESULT_ERR;
    attr_hanler_ptr attr_handler = NULL;
    int buff_size, count = 0;

    log_trace("uuid = %s, attr_name = %s", uuid, attr_name);

    *ppbuf = NULL;
    buff_size = MAX_PARAMS_LEN / (1 << MAX_VENDOR_CB_REMALLOC_COUNT);

    device_helper_ptr helper = NULL;
    helper = msdp_get_helper(uuid);
    PTR_RETURN(helper, ret, "no exist model helper, uuid = %s", uuid)

    os_mutex_lock((helper->mutex_lock));
    attr_handler = msdp_get_attr_handler(helper, attr_name);
    if (attr_handler) {
        log_debug("exist attribute(%s) handler", attr_name);

        void *get_cb = NULL;
        get_cb = attr_handler->get_cb;

        PTR_GOTO(get_cb, unlock_out, "attribute %s's get_cb is NULL", attr_name);

        while (count < MAX_VENDOR_CB_REMALLOC_COUNT) {
            buff_size = MAX_PARAMS_LEN / (1 << (MAX_VENDOR_CB_REMALLOC_COUNT - count));
            *ppbuf = msdp_new_buff( buff_size);
            PTR_GOTO(*ppbuf, unlock_out, "malloc failed");

            memset(*ppbuf, 0, buff_size);
            if (helper->dev_type == DEV_TYPE_GATEWAY) {
                ret = ((get_attr_cb)attr_handler->get_cb)(*ppbuf, buff_size);
            } else {
                // modify by wukong 2017-4-17
                char mac[ETHER_ADDR_BYTES] = {0};
                get_mac_by_uuid(uuid, mac);
                //modify by wukong end 2017-4-17

                ret = ((get_subdev_attr_cb)attr_handler->get_cb)(mac, *ppbuf, buff_size);
            }
            if (ret != SERVICE_BUFFER_INSUFFICENT) {
                break;
            }

            log_error("buff too short,buff_size = %d, remalloc", buff_size);
            msdp_free_buff(*ppbuf);
            *ppbuf = NULL;
            count++;
        }

        //检查attribute调用返回值
        RET_GOTO(ret, unlock_out, "get attibute \"%s\" handler faild", attr_name);
        if (NULL == *ppbuf) { // || strlen(*ppbuf) == 0)
            log_error("get attibute \"%s\" handler error, output_len == 0", attr_name);
            ret = SERVICE_RESULT_ERR;
            goto unlock_out;
        }

        log_debug("get attibute \"%s\" handler output = %s", attr_name, *ppbuf);
    } else {
        log_error("unsupport attribute \"%s\"", attr_name);
    }

    os_mutex_unlock((helper->mutex_lock));
    return ret;

unlock_out:
    os_mutex_unlock((helper->mutex_lock));

    if (*ppbuf) {
        msdp_free_buff(*ppbuf);
        *ppbuf = NULL;
    }

    return ret;
}


static int __set_attribute(const char *uuid, const char *attr_name,
                           char *attr_value)
{
    int ret = SERVICE_RESULT_ERR;
    const char *mac = NULL;
    attr_hanler_ptr attr_handler = NULL;

    log_trace("uuid = %s, attr_name = %s, attr_value = %s", uuid, attr_name,
              attr_value);

    device_helper_ptr helper = NULL;
    helper = msdp_get_helper(uuid);
    PTR_RETURN(helper, ret, "no exist model helper, uuid = %s", uuid)

    os_mutex_lock(helper->mutex_lock);

    attr_handler = msdp_get_attr_handler(helper, attr_name);
    if (attr_handler) {
        log_trace("Exist attribute(%s) handler on devtype:%02x", attr_name,
                  helper->dev_type);
        if (helper->dev_type == DEV_TYPE_GATEWAY) {
            PTR_GOTO((attr_handler)->set_cb,
                     unlock_out, "attribute %s's get_cb is NULL", attr_name);
            ret = ((set_attr_cb)attr_handler->set_cb)(attr_value);
            log_trace("set attibute \"%s\" handler ret = %d", attr_name, ret);
        } else {
            // modify by wukong 2017-4-17
            char mac[ETHER_ADDR_BYTES] = {0};
            get_mac_by_uuid(uuid, mac);
            //modify by wukong end 2017-4-17

            PTR_GOTO((attr_handler->set_cb),
                     unlock_out, "subdevice attribute %s's set_cb is NULL", attr_name);
            ret = ((set_subdev_attr_cb)attr_handler->set_cb)(mac, attr_value);
            log_trace("set attibute \"%s\" handler ret = %d\n", attr_name, ret);
        }
    } else {
        log_error("unsupport attribute \"%s\"", attr_name);
    }

unlock_out:
    os_mutex_unlock((helper->mutex_lock));
    return ret;
}



#define RPC_RESULT_FMT  "{\"set\":%s,\"when\":\"%s\",\"extra\":\"\"}"
static int msdp_rpc_handler(const device_helper_ptr helper,
                            const char *rpc_name,
                            const char *rpc_args)
{
    int ret = SERVICE_RESULT_ERR;
    service_handler_ptr phandler = NULL;
    int buff_size, count = 0;
    char *rpc_result = NULL;

    os_mutex_lock((helper->mutex_lock));
    phandler = msdp_get_service_handler(helper, rpc_name);
    if (phandler) {
        log_trace("Exist service(%s) handler", rpc_name);
        ret = phandler->service_cb((char *)rpc_args, NULL, 0);
        RET_LOG(ret, "excute rpc fail, rpcname:%s", rpc_name);
    } else {
        log_error("unsupport service \"%s\"", rpc_name);
    }
    os_mutex_unlock((helper->mutex_lock));

    return ret;
}


/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["wlanSwitchState", "lightSwitchState"]}
<-- {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["wlanSwitchState"],"wlanSwitchState":{"value":"1","when":"1404443369"}}
*/
static int msdp_get_status(char *params)
{
    int ret = SERVICE_RESULT_ERR;
    char *attrset;
    char *json_out = NULL;
    char params_in[512] = {0};
    int attrset_size, attrset_len = 0;

    //char szPostString[1024] = "{\"uuid\":\"2427287C75CD579897D3DB5854E46A2F\",\"wlanSwitchState\":{\"value\":\"1\",\"when\":\"1404443369\"}}";
    attrset = json_get_value_by_name(params, strlen(params), JSON_KEY_ATTRSET,
                                     &attrset_len, NULL);
    if (attrset) {
        attrset_size = json_get_array_size(attrset, attrset_len);
    }

    if (NULL == attrset || attrset_size == 0) {
        char uuid[MAX_UUID_LEN] = {0};
        int uuid_len = 0;
        char *uuid_ptr = json_get_value_by_name(params, strlen(params), JSON_KEY_UUID,
                                                &uuid_len, NULL);
        PTR_RETURN(uuid_ptr, SERVICE_RESULT_ERR, CALL_FUCTION_FAILED,
                   "json_get_value_by_name(uuid)");
        strncpy(uuid, uuid_ptr, sizeof(uuid) > uuid_len ? uuid_len : sizeof(uuid) - 1);

        char attrset[512] = {0};
        ret = msdp_get_all_attrname(uuid, attrset, sizeof(attrset));
        RET_RETURN(ret, "get all attribute name fail, uuid:%s", uuid);
        log_trace("attrset = %s\n", attrset);

        snprintf(params_in, sizeof(params_in) - 1, GET_DEVICE_ATTR_STRING_FMT, uuid,
                 attrset);
        params = params_in;
    }

    //遍历attrset，并调用厂商注册的接口，返回属性json串
    ret = msdp_get_device_status_handler(params, msdp_get_attr_each_cb,
                                         __get_attribute, &json_out);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_status_handler");

    log_trace("json_out = %s", json_out ? (json_out) : "NULL");
    //todo: postDeviceData/deviceDataChange
    if (json_out) {
        int str_len, post_ret = SERVICE_RESULT_ERR;
        char devid_str[18] = {0};
        char *dev_id = json_get_value_by_name(params, strlen(params), JSON_KEY_DEVID,
                                              &str_len, NULL);
        if (dev_id != NULL) {
            post_ret = lmns_post_device_data(devid_str, json_out);
            RET_GOTO(post_ret, end, CALL_FUCTION_FAILED, "lmns_post_device_data");
        } else {
            int post_ret = msdp_add_asyncpost_task(json_out);
            RET_GOTO(post_ret, end, "add async post task fail, params:%s", json_out);
            json_out = NULL;
        }
    }

end:
    if (json_out) {
        msdp_free_buff(json_out);
    }

    return ret;
}


/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["wlanSwitchState"],"wlanSwitchState":{"value":"1","when":"1404443369"}}
*/
static int msdp_set_status(char *params)
{
    int ret = SERVICE_RESULT_OK;
    char *json_out = NULL;
    //log_trace("json_in = %s\n", json_in);

    //遍历attrset，设置属性值
    ret = msdp_set_device_status_handler(params, msdp_set_attr_each_cb,
                                         __set_attribute);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_set_attr_each_cb");

    //遍历attrset，获取属性值
    ret = msdp_get_device_status_handler(params, msdp_get_attr_each_cb,
                                         __get_attribute, &json_out);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_get_attr_each_cb");

    log_trace("json_out = %s", json_out ? (json_out) : "NULL");
    //todo: postDeviceData/deviceDataChange
    if (json_out) {
        int str_len, post_ret = SERVICE_RESULT_ERR;
        char *dev_id = json_get_value_by_name(params, strlen(params), JSON_KEY_DEVID,
                                              &str_len, NULL);
        if (dev_id != NULL) {
            char devid_str[18] = {0};
            strncpy(devid_str, dev_id, str_len);

            post_ret = lmns_post_device_data(devid_str, json_out);
            RET_GOTO(post_ret, end, CALL_FUCTION_FAILED, "lmns_post_device_data");
        } else {
            int post_ret = msdp_add_asyncpost_task(json_out);
            RET_GOTO(post_ret, end, "add async post task fail, params:%s", json_out);
            json_out = NULL;
        }
    }
end:
    if (json_out) {
        msdp_free_buff(json_out);
    }

    return ret;
}


/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","service\":"startBwCheck","args":{}}
<-- {"uuid": "42E6E69DAEF6483FBBA412A28AF7CD76", "service": "stopBwCheck", "code": "1", "result": [ ] }
*/
static int msdp_rpc(char *params)
{
    int ret = SERVICE_RESULT_ERR;
    char *json_ptr, *rpc_name, *rpc_args;
    json_ptr = rpc_name = rpc_args = NULL;
    int len = 0;

    log_trace("params = %s", params);

    json_ptr = json_get_value_by_name(params, strlen(params), JSON_KEY_SERVICE,
                                      &len, NULL);
    PTR_GOTO(json_ptr, end, CALL_FUCTION_FAILED, "json_get_value_by_name");
    rpc_name = msdp_dup_buff(json_ptr, len + 1);
    PTR_GOTO(rpc_name, end, "pmalloc fail");
    rpc_name[len] = '\0';

    json_ptr = json_get_value_by_name(params, strlen(params), JSON_KEY_ARGS, &len,
                                      NULL);
    PTR_GOTO(json_ptr, end, CALL_FUCTION_FAILED, "json_get_value_by_name");
    rpc_args = msdp_dup_buff(json_ptr, len + 1);
    PTR_GOTO(rpc_args, end, "pmalloc fail");
    rpc_args[len] = '\0';

    char uuid[MAX_UUID_LEN] = {0};
    json_ptr = json_get_value_by_name(params, strlen(params), JSON_KEY_UUID, &len,
                                      NULL);
    PTR_GOTO(json_ptr, end, CALL_FUCTION_FAILED, "json_get_value_by_name");
    strncpy(uuid, json_ptr, len);

    device_helper_ptr helper = msdp_get_helper(uuid);
    PTR_GOTO(helper, end, "no exist model helper, uuid = %s", uuid);

    ret = msdp_rpc_handler(helper, rpc_name, rpc_args);
    RET_GOTO(ret, end, CALL_FUCTION_FAILED, "msdp_service_handler");

    ret = SERVICE_RESULT_OK;
end:
    if (rpc_name) {
        msdp_free_buff(rpc_name);
    }
    if (rpc_args) {
        msdp_free_buff(rpc_args);
    }

    return ret;
}


int msdp_get_gw_attr(const char *gw_uuid, const char *attr_name,
                     char *attr_value_buff, int buff_size)
{
    int ret = SERVICE_RESULT_ERR;
    char *output = NULL;

    ret = __get_attribute(gw_uuid, attr_name, &output);
    RET_RETURN(ret, "get attr fail, attr_name:%s", attr_name);

    if (output != NULL) {
        if (strlen(output) >= buff_size) {
            ret = SERVICE_RESULT_ERR;
            log_error("the buffer size is too small");
            goto out;
        }

        strncpy(attr_value_buff, output, buff_size - 1);
    } else {
        ret = SERVICE_RESULT_ERR;
    }

out:
    if (output) {
        msdp_free_buff(output);
    }
    return ret;
}

int msdp_gw_init()
{
    dev_option_cb callback[MAX_CALLBACK_NUM] = {NULL};

    /*默认cmd callback*/
    callback[ACB_GET_DEVICE_STATUS] = msdp_get_status;
    callback[ACB_SET_DEVICE_STATUS] = msdp_set_status;
    callback[ACB_SET_DEVICE_STATUS_ARRAY] = msdp_set_status_array;
    callback[ACB_REQUEST_REMOTE_SERVICE] = msdp_rpc;

    /*注册网关设备事件处理接口*/
    return msdp_register_device_helper(DEV_TYPE_GATEWAY, callback);

#ifdef ROUTER_SDK
    /*注册WIFI下联子设备model*/
    return msdp_register_device_helper(DEV_TYPE_WIFI_SUB, callback);
#endif

    return 0;
}


