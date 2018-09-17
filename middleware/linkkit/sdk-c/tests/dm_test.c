/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifdef SDK_ENHANCE
#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "ut_debug.h"
#include "iotx_utils.h"
#include "lite-cjson.h"
#include "dm_utils.h"
#include "dm_manager.h"
#include "dm_shadow.h"
#include "dm_ipc.h"
#include "dm_dispatch.h"
#include "dm_message.h"
#include "dm_cm_wrapper.h"

#define linkkit_log(...) log_info("linkkit",__VA_ARGS__)

#define LINKKIT_KEY_ID        "id"
#define LINKKIT_KEY_CODE      "code"
#define LINKKIT_KEY_DEVID     "devid"
#define LINKKIT_KEY_SERVICEID "serviceid"
#define LINKKIT_KEY_PAYLOAD   "payload"


const char LINKKIT_TSL_STRING_TEST[] =
            "{\"schema\":\"https://iot-tsl.oss-cn-shanghai.aliyuncs.com/schema.json\",\"profile\":{\"productKey\":\"b1ZFYKi3UG3\"},\"services\":[{\"outputData\":[],\"identifier\":\"set\",\"inputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlan"
            "s\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"method\":\"thing.service.property.set\",\"name\":\"set\",\"required\":true,\"callType\":\"sync\",\"desc\":\"\"},{\"outputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataTyp"
            "e\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"identifier\":\"get\",\"inputData\":[\"GameListString\",\"GameListInt\",\"GameListFloat\",\"GameListDouble\",\"WIFI_Band\",\"EnumTypeItem\",\"PowerSwitch\",\"HeatSwitch\",\"TempPwmDuty\",\"liquidRemain\",\"WaterReplace\",\"NickName\",\"WorkPlans\"],\"method\":\"thing.service.property.get\",\"name\":\"get\",\"required\":true,\"callType\":\"sync\",\"desc\":\"\"}],\"properties\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListInt\",\"da"
            "taType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"HeatSwitchName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\",\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\",\"accessMode\":\"rw\",\"required\":false},"
            "{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\",\"accessMode\":\"rw\",\"required\":false}],\"events\":[{\"outputData\":[{\"identifier\":\"GameListString\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"text\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListStringName\"},{\"identifier\":\"GameListInt\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"int\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListIntName\"},{\"identifier\":\"GameListFloat\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"float\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListFloatName\"},{\"identifier\":\"GameListDouble\",\"dataType\":{\"specs\":{\"item\":{\"type\":\"double\"},\"size\":\"128\"},\"type\":\"array\"},\"name\":\"GameListDoubleName\"},{\"identifier\":\"WIFI_Band\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"WIFI_Band_Name\"},{\"identifier\":\"EnumTypeItem\",\"dataType\":{\"specs\":{\"0\":\"EnumTypeItem1\",\"1\":\"EnumTypeItem2\",\"2\":\"EnumTypeItem3\",\"3\":\"EnumTypeItem4\"},\"type\":\"enum\"},\"name\":\"EnumTypeItemName\"},{\"identifier\":\"PowerSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PowerSwitchName\"},{\"identifier\":\"HeatSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"}"
            ",\"type\":\"bool\"},\"name\":\"HeatSwitchName\"},{\"identifier\":\"TempPwmDuty\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"TempPwmDutyName\"},{\"identifier\":\"liquidRemain\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"double\"},\"name\":\"liquidRemainName\"},{\"identifier\":\"WaterReplace\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"WaterReplaceName\"},{\"identifier\":\"NickName\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"NickNameName\"},{\"identifier\":\"WorkPlans\",\"dataType\":{\"specs\":[{\"identifier\":\"PlantSwitch\",\"dataType\":{\"specs\":{\"0\":\"off\",\"1\":\"on\"},\"type\":\"bool\"},\"name\":\"PlantSwitchName\"},{\"identifier\":\"SelectIndex\",\"dataType\":{\"specs\":{\"unit\":\"0\",\"min\":\"0\",\"max\":\"3\"},\"type\":\"int\"},\"name\":\"SelectIndexName\"},{\"identifier\":\"PlantInfo1\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo1Name\"},{\"identifier\":\"PlantInfo2\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo2Name\"},{\"identifier\":\"PlantInfo3\",\"dataType\":{\"specs\":{\"length\":\"1024\"},\"type\":\"text\"},\"name\":\"PlantInfo3Name\"}],\"type\":\"struct\"},\"name\":\"WorkPlansName\"}],\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"name\":\"post\",\"type\":\"info\",\"required\":true,\"desc\":\"\"},{\"outputData\":[{\"identifier\":\"ErrorCode\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\"},\"type\":\"int\"},\"name\":\"ErrorCodeName\"}],\"identifier\":\"Error\",\"method\":\"thing.event.Error.post\",\"name\":\"ErrorName\",\"type\":\"error\",\"required\":false}]}";

static void _linkkit_event_thing_service_request(char *payload)
{
    int res = 0;
    lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_payload;

    /* Parse JSON */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(payload, strlen(payload), &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
        return;
    }

    /* Parse Message ID */
    memset(&lite_item_id, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_ID, strlen(LINKKIT_KEY_ID), &lite_item_id);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {
        return;
    }
    linkkit_log("Current Msg ID: %d", lite_item_id.value_int);

    /* Parse Devid */
    memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_DEVID, strlen(LINKKIT_KEY_DEVID), &lite_item_devid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {
        return;
    }
    linkkit_log("Current Devid: %d", lite_item_devid.value_int);

    /* Parse Serviceid */
    memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_SERVICEID, strlen(LINKKIT_KEY_SERVICEID), &lite_item_serviceid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_serviceid)) {
        return;
    }
    linkkit_log("Current Serviceid: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

    /* Parse Serviceid */
    memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_PAYLOAD, strlen(LINKKIT_KEY_PAYLOAD), &lite_item_payload);
    if (res != SUCCESS_RETURN) {
        return;
    }
    linkkit_log("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);
#ifdef DEPRECATED_LINKKIT
    iotx_dm_deprecated_send_service_response(lite_item_devid.value_int, lite_item_id.value_int, IOTX_DM_ERR_CODE_SUCCESS,
            lite_item_serviceid.value, lite_item_serviceid.value_length);
#endif
}

static void _linkkit_event_subdev_register_reply(char *payload)
{
    int res = 0;
    lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

    if (payload == NULL) {
        return;
    }

    /* Parse JSON */
    res = lite_cjson_parse(payload, strlen(payload), &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
        return;
    }

    /* Parse Message ID */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_ID, strlen(LINKKIT_KEY_ID), &lite_item_id);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {
        return;
    }
    linkkit_log("Current Msg ID: %d", lite_item_id.value_int);

    /* Parse Message Code */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_CODE, strlen(LINKKIT_KEY_CODE), &lite_item_code);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {
        return;
    }
    linkkit_log("Current Code: %d", lite_item_code.value_int);

    /* Parse Devid */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_DEVID, strlen(LINKKIT_KEY_DEVID), &lite_item_devid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {
        return;
    }
    linkkit_log("Current devid: %d", lite_item_devid.value_int);

    #ifdef CONFIG_DM_DEVTYPE_GATEWAY
    iotx_dm_subdev_topo_add(lite_item_devid.value_int);
    #endif
}

static void _linkkit_event_topo_add_reply(char *payload)
{
    int res = 0;
    lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

    if (payload == NULL) {
        return;
    }

    /* Parse JSON */
    res = lite_cjson_parse(payload, strlen(payload), &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
        return;
    }

    /* Parse Message ID */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_ID, strlen(LINKKIT_KEY_ID), &lite_item_id);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {
        return;
    }
    linkkit_log("Current Msg ID: %d", lite_item_id.value_int);

    /* Parse Message Code */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_CODE, strlen(LINKKIT_KEY_CODE), &lite_item_code);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {
        return;
    }
    linkkit_log("Current Code: %d", lite_item_code.value_int);

    /* Parse Devid */
    res = lite_cjson_object_item(&lite, LINKKIT_KEY_DEVID, strlen(LINKKIT_KEY_DEVID), &lite_item_devid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {
        return;
    }
    linkkit_log("Current devid: %d", lite_item_devid.value_int);

    #ifdef CONFIG_DM_DEVTYPE_GATEWAY
    iotx_dm_subdev_login(lite_item_devid.value_int);
    #endif
}

void linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    linkkit_log("Receive Message Type: %d", type);
    if (payload) {
        linkkit_log("Receive Message: %s", payload);
    }

    switch (type) {
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            if (payload == NULL) {
                return;
            }
            _linkkit_event_thing_service_request(payload);
        }
        break;
        case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY: {
            if (payload == NULL) {
                return;
            }
            _linkkit_event_subdev_register_reply(payload);
        }
        break;
        case IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY: {

        }
        break;
        case IOTX_DM_EVENT_TOPO_ADD_REPLY: {
            if (payload == NULL) {
                return;
            }
            _linkkit_event_topo_add_reply(payload);
        }
        break;
        case IOTX_DM_EVENT_COMBINE_LOGIN_REPLY: {

        }
        break;
        default:
            linkkit_log("Not Found Type For Now, Smile");
            break;
    }
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut001)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SHANGHAI;
    dm_init_params.event_callback = linkkit_event_callback;
#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut002)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_PRODUCT;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SHANGHAI;
    dm_init_params.event_callback = linkkit_event_callback;

#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut003)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SINGAPORE;
    dm_init_params.event_callback = linkkit_event_callback;

#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut004)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_PRODUCT;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SINGAPORE;
    dm_init_params.event_callback = linkkit_event_callback;

#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut005)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_TYPES_MAX;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SINGAPORE;
    dm_init_params.event_callback = linkkit_event_callback;
#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, FAIL_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut006)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_PRODUCT;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_MAX;
    dm_init_params.event_callback = linkkit_event_callback;
#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, FAIL_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut007)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
    iotx_dm_init_params_t dm_init_params;
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_PRODUCT;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SINGAPORE;
    dm_init_params.event_callback = NULL;

#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_construct(&dm_init_params);
#endif
    ASSERT_EQ(res, FAIL_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_construct_ut008)
{
    int res = 0;
    LITE_openlog("linkkit");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    HAL_SetProductKey("a1m9COc0YRs");
    HAL_SetDeviceName("gw_test001");
    HAL_SetDeviceSecret("36oIO0wOoXOcspodNz4OYJtXghZ6nX92");
    /* DM Construct */
#ifdef DEPRECATED_LINKKIT
    iotx_dm_init_params_t *dm_init_params = NULL;
    res = iotx_dm_deprecated_construct(dm_init_params);
#endif
    ASSERT_EQ(res, FAIL_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_set_tsl_ut001)
{
    int res = 0;
#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, LINKKIT_TSL_STRING_TEST,
                                     strlen(LINKKIT_TSL_STRING_TEST));
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, IOT_DM_Yield_ut001)
{
    int res = 0;
    int i;
    for (i = 0; i < 5 * 10; i++) {
        res = iotx_dm_yield(100);
        iotx_dm_dispatch();
        HAL_SleepMs(100);
    }
    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(DM_TEST, iotx_dm_deprecated_destroy_ut001)
{
    int res = 0;
#ifdef DEPRECATED_LINKKIT
    res = iotx_dm_deprecated_destroy();
    LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
    LITE_closelog();
#endif
    ASSERT_EQ(res, SUCCESS_RETURN);
}

SUITE(DM_TEST) = {
    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut001),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_set_tsl_ut001),
    ADD_CASE(DM_TEST, IOT_DM_Yield_ut001),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_destroy_ut001),

    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut002),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_set_tsl_ut001),
    ADD_CASE(DM_TEST, IOT_DM_Yield_ut001),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_destroy_ut001),

    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut003),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_set_tsl_ut001),
    ADD_CASE(DM_TEST, IOT_DM_Yield_ut001),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_destroy_ut001),

    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut004),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_set_tsl_ut001),
    ADD_CASE(DM_TEST, IOT_DM_Yield_ut001),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_destroy_ut001),

    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut005),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut006),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut007),
    ADD_CASE(DM_TEST, iotx_dm_deprecated_construct_ut008),
    ADD_CASE_NULL
};
#endif
