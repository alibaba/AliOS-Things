/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR_ALINK__H__
#define __DEVMGR_ALINK__H__

#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

#define PROTOCL_ZIGBEE                  "zigbee"
#define PROTOCL_RF433                   "rf433"

#define METHOD_DEVICE_REGISTER_SUB      "device.registerSub"
#define METHOD_DEVICE_UNREGISTER_SUB    "device.unregisterSub"
#define METHOD_DEVICE_LOGIN_SUB         "device.loginSub"
#define METHOD_DEVICE_LOGOUT_SUB        "device.logoutSub"

#define DEVMGR_PERMITJOIN_ANY_MODEL     (0xffffffff)
/*按键触发入网开关，默认超时时间*/
#define DEVMGR_DEFAULT_PERMITJOIN_DURATION  240


//"sign":"hmacmd5(secret, rand+sn)"
#define PARAMS_DEVICE_REGISTER_SUB_FMT  ("{\"shortModel\":\"%08x\",\"rand\":\"%s\",\"sign\":\"%s\","\
                    "\"sn\":\"%s\",\"model\":\"%s\",\"mac\":\"%s\",\"version\":\"%s\",\"name\":\"%s\","\
                    "\"barcode\":\"%s\",\"cid\":\"%s\",\"description\":\"%s\",\"features\":%s}")

#define PARAMS_DEVICE_UNREGISTER_SUB_FMT    ("{\"uuid\":\"%s\",\"rand\":\"%s\",\"sign\":\"%s\"}")
//"sign" : "hmacmd5(sub device's token, rand + sub device's uuid)"
#define PARAMS_DEVICE_LOGIN_SUB_FMT     ("{\"uuid\":\"%s\",\"rand\":\"%s\",\"sign\":\"%s\"}")
#define PARAMS_DEVICE_LOGOUT_SUB_FMT    ("{\"uuid\":\"%s\"}")
#define FEATURE_DEVICE_LOGIN_SUB_FMT    ("{\"linkCapability\":[{\"protocol\":\"%s\",\"profile\":\"%s\"}]}")
#define RESULT_DEVICE_REGISTER_SUB_FMT  ("{\"uuid\":\"%s\",\"token\":\"%s\"}")


int devmgr_network_up_event_handler(dev_info_t *devinfo);
uint32_t devmgr_get_permitjoin_short_model();

/*通告局域网内设备状态变更*/
/*
--> {"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["OnOff"],"OnOff":{"value":"1","when":"1404443369"}}
*/
const char *devmgr_get_device_signature(uint32_t short_model,
                                        const char rand[SUBDEV_RAND_BYTES], char *sign_buff, uint32_t buff_size);
int devmgr_login_device(dev_info_t *devinfo);
int devmgr_logout_device(dev_info_t *devinfo);
int devmgr_register_device(dev_info_t *devinfo);
int devmgr_unregister_device(dev_info_t *devinfo);
int devmgr_annunciate_device_status(const char *devid, char *params);
int devmgr_link_state_event_handler(dev_info_t *devinfo, link_state_t state);
int devmgr_network_leave_event_handler(dev_info_t *devinfo);
int devmgr_network_event_cb(network_event_t event);
void devmgr_delay_disable_join(int duration);
int devmgr_alink_init();
void devmgr_alink_exit();

#ifdef __cplusplus
}
#endif
#endif

