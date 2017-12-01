/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include "aos/aos.h"
#include "json_parser.h"
#include "msdp.h"
#include "devmgr.h"
#include "devmgr_alink.h"
#include "devmgr_cache.h"
#include "service_manager.h"
#include "connectivity_manager.h"
#include "digest_algorithm.h"
#include "alink_protocol.h"
#include "alink_export_internal.h"

#define MODULE_NAME MODULE_NAME_DEVMGR

#define DEVMGR_SERVICE_AUTHORISE_DEVICE_LIST    "AuthoriseDeviceList"
#define DEVMGR_SERVICE_REMOVE_DEVICE            "RemoveDevice"
#define DEVMGR_ATTRIBUTE_JOINED_DEVICE_LIST     "JoinedDeviceList"
#define DEVMGR_SERVICE_PERMITJOIN_DEVICE        "PermitJoin"

#define DEMGR_JOINED_DEVINFO_STRING_FMT "{\"sn\":\"%s\",\"shortModel\":\"%08x\"}"
#define DEVMGR_JSON_KEY_IEEE_ADDR       "ieeeAddr"
#define DEVMGR_JSON_KEY_MAC             "mac"
#define DEVMGR_JSON_KEY_SN              "sn"
#define DEVMGR_JSON_KEY_SN_SET          "snSet"
#define DEVMGR_JSON_KEY_DURATION        "duration"
#define DEVMGR_JSON_KEY_MODEL_ID        "modelId"
#define DEVMGR_JSON_KEY_SHORT_MODEL     "shortModel"
#define DEVMGR_JSON_KEY_ENABLE          "enable"

/* imported from other files to pass compile */
typedef int32_t (*zigbee_permit_join_cb_t)(uint8_t duration);

typedef struct {
    bool enable;
    uint8_t duration;   //permitjoin持续时间
    uint32_t short_model;  //允许加入的设备shortModel，0xffff表示不指定特定shortModel
    uint32_t timestamp; //permitjoin起始时间戳
} permitjoin_t;

/*defined in devmgr.c*/
extern void *devlist_lock;
extern dlist_t dev_head;
extern dlist_t unknown_dev_head;

static permitjoin_t g_permitjoin_config;

static void *disablejoin_work = NULL;

void __work_func(void *work)
{
    log_trace("disable device join");
    g_permitjoin_config.enable = false;
    devmgr_free_unknown_devlist();
}


const char *devmgr_get_device_signature(uint32_t short_model,
                                        const char rand[SUBDEV_RAND_BYTES], char *sign_buff, uint32_t buff_size)
{
    int i = 0;
    char buff[128] = {0};
    unsigned char md5_ret[16] = {0};
    char secret[41] = {0};
    char rand_hexstr[SUBDEV_RAND_BYTES * 2 + 1] = {0};
    int ret = SERVICE_RESULT_ERR;

    if (buff_size <= STR_SIGN_LEN) {
        log_error("buffer size is too small");
        return NULL;
    }

    memcpy(secret, "ISsi2JtDctJf0qW0HUjF7JtJjiiJLMbcZE8wxu4E", sizeof(secret));

    int len = SUBDEV_RAND_BYTES;
    memcpy(buff, rand, len);
    len += snprintf(buff + len, sizeof(buff) - len, "%s", secret);
    digest_md5(buff, len, md5_ret);
    for (i = 0; i < STR_SIGN_LEN / 2; i++) {
        sprintf(sign_buff + i * 2, "%02x", md5_ret[i]);
    }

    bytes_2_hexstr(rand, SUBDEV_RAND_BYTES, rand_hexstr, sizeof(rand_hexstr));
    log_trace("rand hexstr:%s, secret:%s, sign:%s\n", rand_hexstr, secret,
              sign_buff);

    return sign_buff;
}

static int devmgr_listener(int type, void *data, int dlen, void *result,
                           int *rlen)
{
    //log_trace("DEVMGR recv %s", sm_code2string(type));
    if (type == SERVICE_EVENT) {
        int st = *((int *)data);
        log_trace("DEVMGR recv %s, %s", sm_code2string(type), sm_code2string(st));
        if (st == SERVICE_STATE_READY) {
            devmgr_network_event_cb(NETWORK_EVENT_UP);
        } else if (st == SERVICE_STATE_STOP || st == SERVICE_STATE_INIT) {
            devmgr_network_event_cb(NETWORK_EVENT_DOWN);
        }
    }

    return EVENT_IGNORE;
}

int stdd_get_device_service(dev_info_t *devinfo, char *service_buff,
                            int buff_size)
{
    return 0;
}
static int devmgr_register_remote_service(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_ERR;
    uint8_t service[256] = {0};

    if (NULL == devinfo) {
        log_info("parameter invalid");
        return SERVICE_RESULT_ERR;
    }
    ret = stdd_get_device_service(devinfo, service, sizeof(service));

    if (SERVICE_RESULT_OK != ret ||
        0 == json_get_array_size(service, strlen(service))) {
        log_info("The device %s no service to register\r\n", devinfo->dev_base.uuid);
        return SERVICE_RESULT_OK;
    } else {
        log_trace("The device %s service is %s\r\n", devinfo->dev_base.uuid, service);
        return msdp_register_remote_service(devinfo->dev_base.uuid, service);
    }

}

int devmgr_login_device(dev_info_t *devinfo)
{
    int len, ret;
    uint8_t tx_buff[128] = {0};
    uint8_t digest[16] = {0};
    uint8_t sign[33] = {0};
    uint8_t rand_hexstr[SUBDEV_RAND_BYTES * 2 + 1] = {0};
    alink_data_t data = {METHOD_DEVICE_LOGIN_SUB, tx_buff};

    if (devinfo->dev_base.uuid[0] == '\0') {
        return SERVICE_RESULT_ERR;
    }

    //"sign" : "hmacmd5(sub device's token, rand + sub device's uuid)"
    memcpy(tx_buff, devinfo->dev_base.rand, sizeof(devinfo->dev_base.rand));
    len = sizeof(devinfo->dev_base.rand);
    len += snprintf(tx_buff + len, sizeof(tx_buff) - len, "%s",
                    devinfo->dev_base.uuid);
    digest_hmac(DIGEST_TYPE_MD5, tx_buff, len, devinfo->dev_base.token,
                strlen(devinfo->dev_base.token), digest);
    bytes_2_hexstr(digest, sizeof(digest), sign, sizeof(sign));

    tx_buff[0] = '\0';
    bytes_2_hexstr(devinfo->dev_base.rand, sizeof(devinfo->dev_base.rand),
                   rand_hexstr, sizeof(rand_hexstr));
    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_LOGIN_SUB_FMT, devinfo->dev_base.uuid, rand_hexstr, sign)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);

    ret = ((service_t *)sm_get_service("accs"))->put((void *)&data,
                                                     sizeof(data)); //accs_put
    //ret = SERVICE_RESULT_OK;

    return ret;
}


int devmgr_logout_device(dev_info_t *devinfo)
{
    int ret;
    char tx_buff[64] = {0};
    alink_data_t data = {METHOD_DEVICE_LOGOUT_SUB, tx_buff};

    if (devinfo->dev_base.uuid[0] == '\0') {
        return SERVICE_RESULT_ERR;
    }

    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_LOGOUT_SUB_FMT, devinfo->dev_base.uuid)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);
    ret = ((service_t *)sm_get_service("accs"))->put((void *)&data,
                                                     sizeof(data)); //accs_put
    // TODO: if need check the return code?
    //ret = SERVICE_RESULT_OK;

    return ret;
}


int devmgr_register_device(dev_info_t *devinfo)
{
    int ret;
    char tx_buff[512] = {0};
    char features[128] = {0};
    char result[128] = {0};
    char sign_buff[STR_SIGN_LEN + 1] = {0};
    char rand_hexstr[SUBDEV_RAND_BYTES * 2 + 1] = {0};

    alink_data_t data = {METHOD_DEVICE_REGISTER_SUB, tx_buff};

    if (sizeof(features) == snprintf(features, sizeof(features),
                                     FEATURE_DEVICE_LOGIN_SUB_FMT, PROTOCL_ZIGBEE, "1")) {
        return SERVICE_RESULT_ERR;
    }

    char *sign = devinfo->dev_base.sign;
    if (strcmp(get_main_dev()->config->alinkserver,
               default_online_server_with_port) != 0 &&
        NULL != devmgr_get_device_signature(devinfo->dev_base.model_id,
                                            devinfo->dev_base.rand, sign_buff, sizeof(sign_buff))) {
        sign = sign_buff;
    }

    bytes_2_hexstr(devinfo->dev_base.rand, sizeof(devinfo->dev_base.rand),
                   rand_hexstr, sizeof(rand_hexstr));
    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_REGISTER_SUB_FMT,
                                    devinfo->dev_base.model_id, rand_hexstr, sign,
                                    devinfo->dev_base.dev_id, "", "", "",
                                    "", "", "", "", features)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);
    ret = ((service_t *)sm_get_service("accs"))->get((void *)&data, sizeof(data),
                                                     result, sizeof(result));//accs_put
    if (SERVICE_RESULT_OK == ret) {
        int len = 0;
        char *json_str = json_get_value_by_name(result, strlen(result), "token", &len,
                                                NULL);
        if (NULL == json_str) {
            log_error("get token fail, result data = %s", result);
            return SERVICE_RESULT_ERR;
        }
        strncpy(devinfo->dev_base.token, json_str,
                sizeof(devinfo->dev_base.token) > len ? len : sizeof(devinfo->dev_base.token));

        json_str = json_get_value_by_name(result, strlen(result), JSON_KEY_UUID, &len,
                                          NULL);
        if (NULL == json_str) {
            log_error("get uuid fail, result data = %s", result);
            return SERVICE_RESULT_ERR;
        }
        strncpy(devinfo->dev_base.uuid, json_str,
                sizeof(devinfo->dev_base.uuid) > len ? len : sizeof(devinfo->dev_base.uuid));
        log_info("register device success, uuid:%s, token:%s", devinfo->dev_base.uuid,
                 devinfo->dev_base.token);
    }

    return ret;
}


int devmgr_unregister_device(dev_info_t *devinfo)
{
    int len, ret;
    uint8_t digest[16] = {0};
    uint8_t sign[33] = {0};
    uint8_t tx_buff[128] = {0};
    uint8_t rand_hexstr[SUBDEV_RAND_BYTES * 2 + 1] = {0};
    alink_data_t data = {METHOD_DEVICE_UNREGISTER_SUB, tx_buff};

    log_info("unregister device, devid:%s", devinfo->dev_base.dev_id);
    if (devinfo->dev_base.uuid[0] == '\0') {
        return SERVICE_RESULT_ERR;
    }

    bytes_2_hexstr(devinfo->dev_base.rand, sizeof(devinfo->dev_base.rand),
                   rand_hexstr, sizeof(rand_hexstr));

    //"sign" : "hmacmd5(sub device's token, rand + sub device's uuid)"
    memcpy(tx_buff, devinfo->dev_base.rand, sizeof(devinfo->dev_base.rand));
    len = sizeof(devinfo->dev_base.rand);
    len += snprintf(tx_buff + len, sizeof(tx_buff) - len, "%s",
                    devinfo->dev_base.uuid);
    digest_hmac(DIGEST_TYPE_MD5, tx_buff, len, devinfo->dev_base.token,
                strlen(devinfo->dev_base.token), digest);
    bytes_2_hexstr(digest, sizeof(digest), sign, sizeof(sign));

    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_UNREGISTER_SUB_FMT, devinfo->dev_base.uuid, rand_hexstr, sign)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);
    ret = ((service_t *)sm_get_service("accs"))->put((void *)&data,
                                                     sizeof(data)); //accs_put
    ret = SERVICE_RESULT_OK;

    return ret;
}

static int devmgr_authorise_device_cb(char *devid, int str_len, int type)
{
    int ret = SERVICE_RESULT_OK;
    char devid_str[32] = {0};

    strncpy(devid_str, devid, str_len);
    ret = devmgr_authorise_device(devid_str);
    RET_RETURN(ret, "authorise device fail, devid:%s", devid_str);

    return ret;
}


//获取临时入网设备列表
static int devmgr_get_joined_devlist_attribute_cb(char *buf, unsigned int buf_len)
{
    int ret = SERVICE_RESULT_OK;
    dev_info_t *pos = NULL;
    char ieeeAddr[STR_DEVID_LEN + 1] = {0};
    int len = 0;

    len += snprintf(buf, buf_len - len - 1, "[");
    os_mutex_lock(devlist_lock);
    dlist_for_each_entry(&unknown_dev_head, pos, dev_info_t, list_node) {
        get_ieeeaddr_string_by_extaddr(pos->dev_base.u.ieee_addr, ieeeAddr,
                                       sizeof(ieeeAddr));

        len += snprintf(buf + len, buf_len - len - 1, DEMGR_JOINED_DEVINFO_STRING_FMT,
                        ieeeAddr, (unsigned int)pos->dev_base.model_id);

        //buff太小
        if (len == buf_len - 1) {
            os_mutex_unlock(devlist_lock);
            return SERVICE_BUFFER_INSUFFICENT;
        }

    }
    os_mutex_unlock(devlist_lock);

    buf[len] = ']';
    buf[len + 1] = '\0';

    return SERVICE_RESULT_OK;
}


static int devmgr_authorise_devlist_service_cb(char *args, char *buf,
                                               unsigned int buf_len)
{
    //args:{"ieeeAddrs":["", ...]}
    int ret = SERVICE_RESULT_OK;
    char *ieee_addrs = NULL;
    int str_len = 0;

    ieee_addrs = json_get_value_by_name(args, strlen(args), DEVMGR_JSON_KEY_SN_SET,
                                        &str_len, NULL);
    PTR_RETURN(ieee_addrs, ret, "get snSet fail, rpc args:%s", args);

    //遍历json数组，并执行处理函数
    char back_chr = 0;
    backup_json_str_last_char(ieee_addrs, str_len, back_chr);

    char *str_pos, *entry;
    int entry_len, type;
    json_array_for_each_entry(ieee_addrs, str_pos, entry, entry_len, type) {
        ret = devmgr_authorise_device_cb(entry, entry_len, type);
        RET_LOG(ret, "authorise device fail, ieee_addrs:%s", ieee_addrs);
    }
    restore_json_str_last_char(ieee_addrs, str_len, back_chr);
    devmgr_free_unknown_devlist();

    return SERVICE_RESULT_OK;
}


static int devmgr_permitjoin_service_cb(char *args, char *buf,
                                        unsigned int buf_len)
{
    int str_len, ret = SERVICE_RESULT_ERR;
    char buff[32] = {0};
    char *str_pos = NULL;
    //args:{"enable", "duration":"","shortModel":""}

    //enable,获取使能状态
    str_pos = json_get_value_by_name(args, strlen(args), DEVMGR_JSON_KEY_ENABLE,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, ret, "get enable fail, args:%s", args);
    g_permitjoin_config.enable = (*str_pos != '0') ? true : false;

    //duration
    str_pos = json_get_value_by_name(args, strlen(args), DEVMGR_JSON_KEY_DURATION,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, ret, "get duration fail, args:%s", args);
    strncpy(buff, str_pos, str_len);
    g_permitjoin_config.duration = (uint8_t)atoi(buff);

    if (g_permitjoin_config.duration < 1) {
        g_permitjoin_config.enable = false;
    }

    memset(buff, 0, sizeof(buff));
#if 0/*2016.11.15开始modelId参数变更为shortModel*/
    str_pos = json_get_value_by_name(args, strlen(args),
                                     DEVMGR_JSON_KEY_SHORT_MODEL, &str_len, NULL);
    PTR_RETURN(str_pos, ret, "get short model fail, args:%s", args);
    strncpy(buff, str_pos, str_len);
#else/*获取shortModel失败再获取modelId过渡使用*/
    str_pos = json_get_value_by_name(args, strlen(args), DEVMGR_JSON_KEY_MODEL_ID,
                                     &str_len, NULL);
    if (NULL == str_pos) {
        str_pos = json_get_value_by_name(args, strlen(args),
                                         DEVMGR_JSON_KEY_SHORT_MODEL, &str_len, NULL);
        PTR_RETURN(str_pos, ret, "get short model fail, args:%s", args);
    }
    strncpy(buff, str_pos, str_len);
#endif

    sscanf(buff, "%08x", (unsigned int *)&g_permitjoin_config.short_model);
    //设置时间戳
    g_permitjoin_config.timestamp = time(NULL);

    if (alink_cb_func[_ALINK_ZIGBEE_PERMIT_JOIN]) {
        ret = ((zigbee_permit_join_cb_t)alink_cb_func[_ALINK_ZIGBEE_PERMIT_JOIN])(
                  g_permitjoin_config.duration);
        RET_RETURN(ret, "permit join fail");
    } else {
        log_error("permit_device callback is NULL");
    }

    //设置定时器
    aos_cancel_work(disablejoin_work, __work_func, NULL);
    if (g_permitjoin_config.enable == true &&
        g_permitjoin_config.duration > 0) {
        log_trace("delay disable device join, duration:%d",
                  g_permitjoin_config.duration);
        disablejoin_work = aos_loop_schedule_work(g_permitjoin_config.duration * 1000,
                                                  __work_func, NULL, NULL, NULL);
    }

    return ret;
}



static int devmgr_remove_device_service_cb(char *args, char *buf,
                                           unsigned int buf_len)
{
    int str_len, ret = SERVICE_RESULT_ERR;
    char ieee_addr[STR_DEVID_LEN + 1] = {0};
    char *str_pos = NULL;
    //args:{"ieeeAddr":""}

    str_pos = json_get_value_by_name(args, strlen(args), DEVMGR_JSON_KEY_SN,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, ret, "get sn fail, args:%s", args);

    strncpy(ieee_addr, str_pos,
            sizeof(ieee_addr) > str_len ? str_len : sizeof(ieee_addr) - 1);
    ret = devmgr_delete_device(ieee_addr);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "devmgr_delete_device");

    return ret;
}



static int devmgr_network_down_event_handler(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_OK;

    switch (devinfo->cloud_state) {
        case DEVICE_STATE_AUTHORISED:
        case DEVICE_STATE_LOGOUTED:
            //nothing todo
            break;
        case DEVICE_STATE_REGISTERED:
        case DEVICE_STATE_LOGINED:
            ret = devmgr_logout_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                break;
            }
            devinfo->cloud_state = DEVICE_STATE_LOGOUTED;

            break;
        default:
            break;
    }

    return ret;
}


void devmgr_delay_disable_join(int duration)
{
    //permit join nothing todi
    if (g_permitjoin_config.enable == true) {
        return;
    }

    log_trace("permit device join, duration:%d", duration);
    g_permitjoin_config.short_model = DEVMGR_PERMITJOIN_ANY_MODEL;
    g_permitjoin_config.enable = true;
    aos_cancel_work(disablejoin_work, __work_func, NULL);
    disablejoin_work = aos_loop_schedule_work(duration * 1000, __work_func, NULL, NULL,
                                              NULL);

    return;
}

int devmgr_network_leave_event_handler(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_OK;

    switch (devinfo->cloud_state) {
        case DEVICE_STATE_AUTHORISED:
            //nothing todo
            break;
        case DEVICE_STATE_REGISTERED:
        case DEVICE_STATE_LOGINED:
        case DEVICE_STATE_LOGOUTED:
            ret = devmgr_unregister_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                break;
            }
            devinfo->cloud_state = DEVICE_STATE_INITIAL;

            break;
        default:
            break;
    }

    return ret;
}


int devmgr_network_up_event_handler(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_OK;

    switch (devinfo->cloud_state) {
        case DEVICE_STATE_AUTHORISED:
            //发起设备注册，注册成功则更新设备信息和状态
            ret = devmgr_register_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                break;
            }
            ret = devmgr_login_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                break;
            }
            devmgr_register_remote_service(devinfo);
            devinfo->cloud_state = DEVICE_STATE_LOGINED;

            break;
        case DEVICE_STATE_REGISTERED:
        case DEVICE_STATE_LOGOUTED:
            if (LINK_STATE_ONLINE == devinfo->link_state) {
                ret = devmgr_login_device(devinfo);
                if (ret != SERVICE_RESULT_OK) {
                    log_warn("login sub device fail, devid:%s,uuid:%s, ret: %d",
                             devinfo->dev_base.dev_id, devinfo->dev_base.uuid, ret);
                    /* ALINK_CODE_ERROR_SUBDEV_NO_RELATIONSHIP:
                     *      sub device relation not exist, delete the device from the gateway
                     * ALINK_CODE_ERROR_SUBDEV_INVALID_UUID:
                     *      gw or sub device's uuid is invalid, delete the device from the gateway
                     */
                    if (ALINK_CODE_ERROR_SUBDEV_NO_RELATIONSHIP == ret
                        || ALINK_CODE_ERROR_SUBDEV_INVALID_UUID == ret) {
                        devinfo->cloud_state = DEVICE_STATE_AUTHORISED; /*needn't do unregister*/
                        devmgr_delete_device_by_devinfo(devinfo);
                        ret = SERVICE_RESULT_OK;
                    }
                    break;
                }
                devmgr_register_remote_service(devinfo);
                devinfo->cloud_state = DEVICE_STATE_LOGINED;
            }
            break;
        case DEVICE_STATE_LOGINED:
        default:
            break;
    }

    return ret;
}



uint32_t devmgr_get_permitjoin_short_model()
{
    //app端触发入网/按键触发入网，再超时时间内置直接返回指定short_model
    if (g_permitjoin_config.timestamp + g_permitjoin_config.duration > time(NULL)
        || g_permitjoin_config.duration == 0xff) {
        return g_permitjoin_config.short_model;
    }

    //网关按键触发入网
    return DEVMGR_PERMITJOIN_ANY_MODEL;
}


static int devmgr_update_status_cb(char *name, int name_len, char *value,
                                   int value_len, int value_type, const char *devid)
{
    int str_len, ret = SERVICE_RESULT_ERR;
    char attr_name[MAX_ATTR_NAME_LEN] = {0};
    char attr_value[128] = {0};

    //忽略uuid,attrset参数
    if (strcmp(name, JSON_KEY_ATTRSET) == 0 || strcmp(name, JSON_KEY_UUID)) {
        return SERVICE_RESULT_OK;
    }

    strncpy(attr_name, name, sizeof(attr_name) - 1);
    char *str_pos = json_get_value_by_name(value, value_len, JSON_KEY_VALUE,
                                           &str_len, NULL);
    PTR_RETURN(str_pos, ret, "get value fail, attr_value:%s", value);
    strncpy(attr_value, str_pos,
            sizeof(attr_value) > str_len ? str_len : sizeof(attr_value) - 1);

    log_trace("attr_name:%s, attr_value:%s", attr_name, attr_value);
    ret = devmgr_update_attr_cache(devid, attr_name, attr_value);

    return ret;
}


/*通告局域网内设备状态变更*/
/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["OnOff"],"OnOff":{"value":"1","when":"1404443369"}}
*/
int devmgr_annunciate_device_status(const char *devid, char *params)
{
    int ret = SERVICE_RESULT_OK;

    /*  unknonw device */
    dev_info_t *devinfo = devmgr_get_devinfo(devid);
    PTR_RETURN(devinfo, ret, "get devinfo fail, devid:%s", devid);
    devmgr_put_devinfo_ref(devinfo);

    //遍历attr
    char *pos, *key, *val;
    int klen, vlen, vtype;
    json_object_for_each_kv(params, pos, key, klen, val, vlen, vtype) {
        ret = devmgr_update_status_cb(key, klen, val, vlen, vtype, devid);
        RET_LOG(ret, "update device status fail, key:%s, value:%s", key, val);
    }

    return ret;
}



int devmgr_link_state_event_handler(dev_info_t *devinfo, link_state_t state)
{
    int ret = SERVICE_RESULT_ERR;

    devinfo->link_state = state;
    if (LINK_STATE_ONLINE == state) {
        ret = devmgr_network_up_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_up_event_handler");
    } else if (LINK_STATE_OFFLINE == state) {
        ret = devmgr_network_down_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_down_event_handler");
    } else {
        log_error("unknown link state, state:%d", state);
    }

    return ret;
}

int devmgr_network_event_cb(network_event_t event)
{
    //遍历设备，逐个注册、attatch
    int ret = SERVICE_RESULT_OK;
    dev_info_t *pos, *next = NULL;

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry(&unknown_dev_head, pos, dev_info_t, list_node) {
        switch (event) {
            case NETWORK_EVENT_DOWN: //network disconnected
                //ret = devmgr_network_down_event_handler(pos);
                pos->cloud_state = DEVICE_STATE_LOGOUTED;
                break;
            case NETWORK_EVENT_UP: //network connected
                ret = devmgr_network_up_event_handler(pos);
                break;
            default:
                break;
        }
    }
    os_mutex_unlock(devlist_lock);

    return ret;
}

int devmgr_alink_init()
{
    int ret = SERVICE_RESULT_OK;

    memset(&g_permitjoin_config, 0, sizeof(g_permitjoin_config));
#if 0
    //属性注册
    ret = alink_register_attribute(DEVMGR_ATTRIBUTE_JOINED_DEVICE_LIST,
                                   devmgr_get_joined_devlist_attribute_cb, NULL);

    //服务注册
    ret = alink_register_service(DEVMGR_SERVICE_AUTHORISE_DEVICE_LIST,
                                 devmgr_authorise_devlist_service_cb);
    ret = alink_register_service(DEVMGR_SERVICE_REMOVE_DEVICE,
                                 devmgr_remove_device_service_cb);
    ret = alink_register_service(DEVMGR_SERVICE_PERMITJOIN_DEVICE,
                                 devmgr_permitjoin_service_cb);
#endif
    sm_attach_service("accs", &devmgr_listener);

    return ret;
}

void devmgr_alink_exit()
{
    sm_detach_service("accs", &devmgr_listener);

    aos_cancel_work(disablejoin_work, __work_func, NULL);
    alink_unregister_service(DEVMGR_SERVICE_AUTHORISE_DEVICE_LIST);
    alink_unregister_service(DEVMGR_SERVICE_REMOVE_DEVICE);
    alink_unregister_service(DEVMGR_SERVICE_PERMITJOIN_DEVICE);
}


