
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "subdev_example_api.h"


extern void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);
extern void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);


#define PRODUCT_KEY                    "a1BGmFnszAj"
#define DEVICE_NAME                    "gateway"
#define DEVICE_SECRET                  "Z9lPWO48TyOg90ykmxfkYK4TdJTiZrDF"

#define SUB_1_PRODUCT_KEY              "a1BGmFnszAj"
#define SUB_1_DEVICE_NAME              "subdev_5"
#define SUB_1_DEVICE_SECRET            "iJJcce70yMvXaeJzdRGwBzvNXBpI6cxR"

#define SUB_2_PRODUCT_KEY              "a1BGmFnszAj"
#define SUB_2_DEVICE_NAME              "subdev_6"
#define SUB_2_DEVICE_SECRET            "N7OyAHhVFHBp4NFfLHLz2nEBXyzrjDEC"

#define MSG_LEN_MAX                    (1024 * 4)

char g_product_key[PRODUCT_KEY_LEN + 1];
char g_product_secret[PRODUCT_SECRET_LEN + 1];
char g_device_name[DEVICE_NAME_LEN + 1];
char g_device_secret[DEVICE_SECRET_LEN + 1];

#define TOPIC_GATEWAY_UPDATE           "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_SUBDEVICE1_UPDATE        "/"SUB_1_PRODUCT_KEY"/"SUB_1_DEVICE_NAME"/update"
#define TOPIC_SUBDEVICE2_UPDATE        "/"SUB_2_PRODUCT_KEY"/"SUB_2_DEVICE_NAME"/updae"

static void* g_thing_t = NULL;
static void* g_gateway_t = NULL;

static int _calc_sign(const char* product_key, 
        const char* device_name,
        const char* device_secret,
        char* hmac_sigbuf,
        const int hmac_buflen,
        const char* sign_method,
        const char *client_id, 
        const char *timestamp_str)
{
    char signature[64];                    
    char hmac_source[256];    

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                      client_id,
                      device_name,
                      product_key,
                      timestamp_str);

    if (0 == strncmp(sign_method, "hmacsha1", strlen("hmacsha1"))) {
        utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    device_secret,
                    strlen(device_secret));
    } else if (0 == strncmp(sign_method, "hmacmd5", strlen("hmacmd5"))){
        utils_hmac_md5(hmac_source, strlen(hmac_source),
                   signature,
                   device_secret,
                   strlen(device_secret));
    }

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return 0;
}

       
void rrpc_request_handler(void* gateway,
        const char* product_key,
        const char* device_name,
        const char* message_id,  
        const char* payload)
{
    printf("rrpc_request_callback, product_key [%s], device_name [%s], message_id [%s], payload:%s \n", 
                product_key,
                device_name,
                message_id, 
                payload);

    if (IOT_Gateway_RRPC_Response(gateway, 
            product_key, 
            device_name, 
            message_id, 
            "aa00090000ff00003039001b") >= 0) {
        printf("gateway rrpc repose ok success\n");
    } else {
        printf("gateway rrpc repose ok fail\n");
    }
}

void service_request_handler(void* thing_t, 
        const char* product_key, 
        const char* device_name,
        iotx_thing_service_type_t service_type,
        uint32_t message_id,        
        char* params,
        uint32_t params_length,
        const char* service_id)   // preperty's serviceid is NULL
{
    printf("service request callback product_key[%s]\n device_name[%s]\n message_id[%d]\n"
        "service_type[%d]\n service_id[%s]\n params[%s]\n\n",
        product_key, device_name, message_id, service_type, service_id, params);

    switch(service_type) {
        case IOTX_Thing_SERVICE_TYPE_PROPERTY_SET:
            if (FAIL_RETURN == IOT_Thing_Set_Property_Response(thing_t, 
                                    product_key, 
                                    device_name, 
                                    message_id, 
                                    200)) {
                printf("service reponse fail\n");
            } else {
                printf("service reponse success\n");
            }
            break;
        case IOTX_Thing_SERVICE_TYPE_PROPERTY_GET:
            if (FAIL_RETURN == IOT_Thing_Get_Property_Response(thing_t, 
                                    product_key, 
                                    device_name, 
                                    message_id, 
                                    "Property_get",
                                    200)) {
                printf("service reponse fail\n");
            } else {
                printf("service reponse success\n");
            }
            break;
        case IOTX_Thing_SERVICE_TYPE_UNDEFINED:
            if (FAIL_RETURN == IOT_Thing_Service_Response(thing_t, 
                                    product_key, 
                                    device_name, 
                                    service_id,
                                    message_id, 
                                    200)) {
                printf("service reponse fail\n");
            } else {
                printf("service reponse success\n");
            }
            break;

        default:
            printf("service_type error\n");
            break;
    }    
}

void down_raw_handler(void* thing_t, 
        const char* product_key, 
        const char* device_name,
        const char* raw_data,
        uint32_t raw_data_length)
{
    printf("service request callback product_key[%s], device_name[%s], raw_data[%d][%s]\n", 
                    product_key, device_name, raw_data_length, raw_data);

    if (FAIL_RETURN == IOT_Tmp_Down_Raw_Response(thing_t, 
                                product_key, 
                                device_name, 
                                "down raw reply")){
        printf("down raw reply fail");
        return;
    }

    if (SUCCESS_RETURN == IOT_Thing_Publish_Rawdata(thing_t, 
                                product_key, 
                                device_name, 
                                "up raw data test", 
                                strlen("up raw data test"))) {
        printf("publish raw data success");
    } else {
        printf("publish raw data fail");
    }
}

void thing_control_handler(void* thing_t, 
        const char* product_key, 
        const char* device_name,
        iotx_thing_control_type_t thing_control_type,
        uint32_t message_id)
{
    printf("thing control callback product_key[%s], device_name[%s], thing_control_type[%d] message_id[%d]\n", 
                    product_key, device_name, thing_control_type, message_id);

    if (SUCCESS_RETURN == IOT_Thing_Control_Response(thing_t, 
                                product_key, 
                                device_name, 
                                message_id,
                                200,
                                thing_control_type)) {
        printf("thing control success");
    } else {
        printf("thing control fail");
    }
}


// 示例如何使用subdev的API直接进行开发
int demo_gateway_function(char *msg_buf, char *msg_readbuf)
{
    int rc, msg_len, cnt = 0;;
    iotx_conn_info_pt puser_info;
    iotx_mqtt_param_t mqtt_t;
    iotx_gateway_param_t gateway_param;  
    iotx_mqtt_topic_info_t topic_msg; 
    char msg_pub[128] = {0}; 
    char sign[41] = {0};
    char timestamp[20] = {0};
    char client_id[64/*32*/] = {0};

    /**< get device info*/
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/

    /* Device AUTH */
    rc = IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        printf("rc = IOT_SetupConnInfo() = %d\n", rc);
        return rc;
    }

    /* Construct a master-slave */
    memset(&mqtt_t, 0, sizeof(iotx_mqtt_param_t));
    memset(&gateway_param, 0, sizeof(iotx_gateway_param_t));

    gateway_param.mqtt = &mqtt_t;
    gateway_param.mqtt->port = puser_info->port;
    gateway_param.mqtt->host = puser_info->host_name;
    gateway_param.mqtt->client_id = puser_info->client_id;
    gateway_param.mqtt->username = puser_info->username;
    gateway_param.mqtt->password = puser_info->password;
    gateway_param.mqtt->pub_key = puser_info->pub_key;

    gateway_param.mqtt->request_timeout_ms = 2000;
    gateway_param.mqtt->clean_session = 0;
    gateway_param.mqtt->keepalive_interval_ms = 60000;
    gateway_param.mqtt->pread_buf = msg_readbuf;
    gateway_param.mqtt->read_buf_size = MSG_LEN_MAX;
    gateway_param.mqtt->pwrite_buf = msg_buf;
    gateway_param.mqtt->write_buf_size = MSG_LEN_MAX;

    gateway_param.mqtt->handle_event.h_fp = NULL;
    gateway_param.mqtt->handle_event.pcontext = NULL;
    
    printf(" ~~~~~~~~~~~~~~~~ start test ~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    /* construct */
    printf(" ~~~~~~~~~~~~~~ start construct ~~~~~~~~~~~~~ \n");
    g_gateway_t = IOT_Gateway_Construct(&gateway_param);
    if (NULL == g_gateway_t) {
        printf("construct Gateway failed!\n");
        return rc;
    }
    if (FAIL_RETURN == IOT_Gateway_RRPC_Register(g_gateway_t, 
            PRODUCT_KEY, 
            DEVICE_NAME, 
            rrpc_request_handler)) {
        printf("rrpc register error\n");
        IOT_Gateway_Destroy(&g_gateway_t);
        return FAIL_RETURN;
    }
    printf(" ~~~~~~~~~~~~ construct success ~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    printf(" ~~~~~~~~~~~~~~~ start login ~~~~~~~~~~~~~~~~ \n");    
            
    /* timestamp */
    strncpy(timestamp, "2524608000000", strlen("2524608000000") + 1);
   
    /* client id */
    HAL_Snprintf(client_id, /*32*/64, "%s.%s", SUB_1_PRODUCT_KEY, SUB_1_DEVICE_NAME);
    
    /* sign */    
    if (FAIL_RETURN == _calc_sign(SUB_1_PRODUCT_KEY,
                            SUB_1_DEVICE_NAME,
                            SUB_1_DEVICE_SECRET,
                            sign, 
                            41,
                            "hmacsha1",
                            client_id,
                            timestamp)) {
        printf("sign fail \n");
        return FAIL_RETURN;
    }

    printf(" ~~~~~~~~~~~~~~ start register ~~~~~~~~~~~~~~~ \n");

    if (FAIL_RETURN == (rc = IOT_Subdevice_Register(g_gateway_t, 
                    IOTX_Thing_REGISTER_TYPE_STATIC,
                    SUB_1_PRODUCT_KEY, 
                    SUB_1_DEVICE_NAME, 
                    timestamp,
                    client_id,
                    sign,
                    IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA))) {
        printf("register fail \n");
        IOT_Gateway_Destroy(&g_gateway_t);
        return FAIL_RETURN;
    }
                            
    rc = IOT_Subdevice_Login(g_gateway_t, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                timestamp,
                client_id,
                sign,
                IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA,
                IOTX_SUBDEV_CLEAN_SESSION_TRUE);

    if (SUCCESS_RETURN != rc) {
        printf("1  rc = IOT_Subdevice_Login() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }
    if (FAIL_RETURN == IOT_Gateway_RRPC_Register(g_gateway_t, 
            SUB_1_PRODUCT_KEY, 
            SUB_1_DEVICE_NAME, 
            rrpc_request_handler)) {
        printf("rrpc register error\n");
        return FAIL_RETURN;
    }   
            
    printf(" ~~~~~~~~~~~~~~ start register ~~~~~~~~~~~~~~~ \n");

            
    // 如果想测试动态注册，请将如下代码打开，并且将SUB_2_DEVICE_NAME重新定义一个值，例如“subdev_6”
#if 0
    printf(" ~~~~~~~~~~~~~ dynamic register ~~~~~~~~~~~~~~ \n");

    if (FAIL_RETURN == (rc = IOT_Subdevice_Register(g_gateway_t, 
                    IOTX_Thing_REGISTER_TYPE_DYNAMIC,
                    SUB_2_PRODUCT_KEY, 
                    SUB_2_DEVICE_NAME, 
                    NULL,
                    NULL,
                    NULL,
                    IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA))) {
        printf("2 register fail \n");
        IOT_Gateway_Destroy(&g_gateway_t);
        return FAIL_RETURN;
    }
    rc = IOT_Subdevice_Login(g_gateway_t, 
                SUB_2_PRODUCT_KEY, 
                SUB_2_DEVICE_NAME, 
                NULL,
                NULL,
                NULL,
                IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA,
                IOTX_SUBDEV_CLEAN_SESSION_FALSE);

    if (SUCCESS_RETURN != rc) {
        printf("2  rc = IOT_Subdevice_Login() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }    
    
    if (FAIL_RETURN == IOT_Gateway_RRPC_Register(g_gateway_t, 
            SUB_2_PRODUCT_KEY, 
            SUB_2_DEVICE_NAME, 
            rrpc_request_handler)) {
        printf("rrpc register error\n");
        return FAIL_RETURN;
    }
#else
    memset(client_id, 0x0, 64);
    /* client id */
    HAL_Snprintf(client_id, /*32*/64, "%s.%s", SUB_2_PRODUCT_KEY, SUB_2_DEVICE_NAME);

    memset(sign, 0x0, 41);
    /* sign */    
    if (FAIL_RETURN == _calc_sign(SUB_2_PRODUCT_KEY,
                            SUB_2_DEVICE_NAME,
                            SUB_2_DEVICE_SECRET,
                            sign, 
                            41,
                            "hmacsha1",
                            client_id,
                            timestamp)) {
        printf("sign fail \n");
        return FAIL_RETURN;
    }
                            
        if (FAIL_RETURN == (rc = IOT_Subdevice_Register(g_gateway_t, 
                        IOTX_Thing_REGISTER_TYPE_STATIC,
                        SUB_2_PRODUCT_KEY, 
                        SUB_2_DEVICE_NAME,
                        timestamp,
                        client_id,
                        sign,
                        IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA))) {
            printf("2 register fail \n");
            IOT_Gateway_Destroy(&g_gateway_t);
            return FAIL_RETURN;
        }
        rc = IOT_Subdevice_Login(g_gateway_t, 
                    SUB_2_PRODUCT_KEY, 
                    SUB_2_DEVICE_NAME, 
                    timestamp,
                    client_id,
                    sign,
                    IOTX_SUBDEV_SIGN_METHOD_TYPE_SHA,
                    IOTX_SUBDEV_CLEAN_SESSION_FALSE);
        
        if (SUCCESS_RETURN != rc) {
            printf("2  rc = IOT_Subdevice_Login() = %d\n", rc);
            IOT_Gateway_Destroy((void**)&g_gateway_t);
            return rc;
        }    
        
        if (FAIL_RETURN == IOT_Gateway_RRPC_Register(g_gateway_t, 
                SUB_2_PRODUCT_KEY, 
                SUB_2_DEVICE_NAME, 
                rrpc_request_handler)) {
            printf("rrpc register error\n");
            return FAIL_RETURN;
        }

#endif
    printf(" ~~~~~~~~~~~~~~ login success ~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    printf(" ~~~~~~~~~~~~~ start publish ~~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~ gateway publish ~~~~~~~~~~~~~~ \n");  
    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    strcpy(msg_pub, "message: hello! start!");

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    rc = IOT_Gateway_Publish(g_gateway_t, TOPIC_GATEWAY_UPDATE, &topic_msg);
    printf("gateway rc = IOT_Gateway_Publish() = %d\n", rc);

    do {
        /* Generate topic message */
        cnt++;
        printf(" ~~~~~~~~~~~ subdevice1 publish ~~~~~~~~~~~~ \n");   
        msg_len = snprintf(msg_pub, sizeof(msg_pub), "subdevice1, hello");
        if (msg_len < 0) {
            printf("Error occur! Exit program\n");
            IOT_Gateway_Destroy((void**)&g_gateway_t);
            return -1;
        }

        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = msg_len;

        rc = IOT_Gateway_Publish(g_gateway_t, TOPIC_SUBDEVICE1_UPDATE, &topic_msg);
        if (rc < 0) {
            printf("error occur when publish\n");
            IOT_Gateway_Destroy((void**)&g_gateway_t);
            return -1;
        }
        printf("subdevice1 packet_id=%u, publish topic msg=%s\n", (uint32_t)rc, msg_pub);

        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_Gateway_Yield(g_gateway_t, 200); 

        printf(" ~~~~~~~~~~~ subdevice2 publish ~~~~~~~~~~~~ \n");
        memset(msg_pub, 0x0, 128);
        msg_len = snprintf(msg_pub, sizeof(msg_pub), "subdevice2, hello");

        topic_msg.qos = IOTX_MQTT_QOS0;
        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = strlen(msg_pub);
        rc = IOT_Gateway_Publish(g_gateway_t, TOPIC_SUBDEVICE2_UPDATE, &topic_msg);
        if (rc < 0) {
            printf("error occur when publish\n");
            IOT_Gateway_Destroy((void**)&g_gateway_t);
            return -1;
        }
        printf("subdevice2 packet_id=%u, publish topic msg=%s\n", (uint32_t)rc, msg_pub);
    } while (0);
    
    printf(" ~~~~~~~~~~~~ publish success ~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    /* handle the MQTT packet received from TCP or SSL connection */
    while (cnt < 30) {
        IOT_Gateway_Yield(g_gateway_t, 200);
        cnt++;
    }

    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    HAL_SleepMs(200);
    
    printf(" ~~~~~~~~~~~~~ start logout ~~~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    rc = IOT_Subdevice_Logout(g_gateway_t, SUB_1_PRODUCT_KEY, SUB_1_DEVICE_NAME);

    if (SUCCESS_RETURN != rc) {
        printf("1  rc = IOT_Subdevice_Logout() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }
    
    rc = IOT_Subdevice_Logout(g_gateway_t, SUB_2_PRODUCT_KEY, SUB_2_DEVICE_NAME);

    if (SUCCESS_RETURN != rc) {
        printf("2  rc = IOT_Subdevice_Logout() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }
    printf(" ~~~~~~~~~~~~~ logout success ~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    // 请勿随意unregister 一个设备，如果该设备被unregister后，该设备就不能再使用，请注意
#if 0
    rc = IOT_Subdevice_Unregister(g_gateway_t, SUB_1_PRODUCT_KEY, SUB_1_DEVICE_NAME); 
    if (SUCCESS_RETURN != rc) {
        printf("1  rc = IOT_Subdevice_Unregister() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }
    
    rc = IOT_Subdevice_Unregister(g_gateway_t, SUB_2_PRODUCT_KEY, SUB_2_DEVICE_NAME);
    if (SUCCESS_RETURN != rc) {
        printf("2  rc = IOT_Subdevice_Unregister() = %d\n", rc);
        IOT_Gateway_Destroy((void**)&g_gateway_t);
        return rc;
    }
#endif

    IOT_Gateway_Destroy((void**)&g_gateway_t);
    printf(" ~~~~~~~~~~~~ destory success ~~~~~~~~~~~~~~ \n"); 

    return SUCCESS_RETURN;
}


/* test for only one device */
// 示例使用subdev_example_api.h中对topic进行封装的API进行单品设备开发
int demo_only_one_device(char *msg_buf, char *msg_readbuf)
{
    int rc, cnt = 0;
    uint32_t length = 0;
    iotx_conn_info_pt puser_info;
    iotx_mqtt_param_t mqtt_t;
    iotx_thing_param_t thing_param;   
    
    //具体取值请根据自行创建的产品模型进行调整，可以参考运行时日志会提示dsl具体大小，取值需比dsl大小大即可
    char sub_dsltemplate_get[1024 * 4] = {0} ;

    /**< get device info*/
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/

    /* Device AUTH */
    rc = IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        printf("rc = IOT_SetupConnInfo() = %d\n", rc);
        return rc;
    }

    /* Construct a master-slave */
    memset(&mqtt_t, 0, sizeof(iotx_mqtt_param_t));
    memset(&thing_param, 0, sizeof(iotx_thing_param_t));

    thing_param.mqtt = &mqtt_t;
    thing_param.mqtt->port = puser_info->port;
    thing_param.mqtt->host = puser_info->host_name;
    thing_param.mqtt->client_id = puser_info->client_id;
    thing_param.mqtt->username = puser_info->username;
    thing_param.mqtt->password = puser_info->password;
    thing_param.mqtt->pub_key = puser_info->pub_key;

    thing_param.mqtt->request_timeout_ms = 2000;
    thing_param.mqtt->clean_session = 0;
    thing_param.mqtt->keepalive_interval_ms = 60000;
    thing_param.mqtt->pread_buf = msg_readbuf;
    thing_param.mqtt->read_buf_size = MSG_LEN_MAX;
    thing_param.mqtt->pwrite_buf = msg_buf;
    thing_param.mqtt->write_buf_size = MSG_LEN_MAX;

    thing_param.mqtt->handle_event.h_fp = NULL;
    thing_param.mqtt->handle_event.pcontext = NULL;
    
    printf(" ~~~~~~~~~~~~~~~~ start test ~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    /* construct */
    printf(" ~~~~~~~~~~~~~~ start construct ~~~~~~~~~~~~~ \n");
    g_thing_t = IOT_Thing_Construct(&thing_param);
    if (NULL == g_thing_t) {
        printf("construct device Thing failed!\n");
        return rc;
    }
    if (FAIL_RETURN == IOT_Thing_RRPC_Register(g_thing_t, 
            PRODUCT_KEY, 
            DEVICE_NAME, 
            rrpc_request_handler)) {
        printf("rrpc register error\n");
        return FAIL_RETURN;
    }            
    printf(" ~~~~~~~~~~~~ construct success ~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    printf(" ~~~~~~~~ start register callback ~~~~~~~~~~ \n");    
    printf(" ~~~~~~~~~~~~ service register ~~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Service_Register(g_thing_t, service_request_handler);         
    if (SUCCESS_RETURN != rc) {
        printf("service register rc = IOT_Thing_Service_Register() = %d\n", rc);
        goto exit;
    }
    
    printf(" ~~~~~~~~~~~~ down raw register ~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Down_Raw_Register(g_thing_t, down_raw_handler);           
    if (SUCCESS_RETURN != rc) {
        printf("raw register rc = IOT_Thing_Down_Raw_Register() = %d\n", rc);
        goto exit;
    }
    
    printf(" ~~~~~~~~~~ thing control register ~~~~~~~~~~ \n");
    rc = IOT_Thing_Control_Register(g_thing_t, thing_control_handler);           
    if (SUCCESS_RETURN != rc) {
        printf("raw register rc = IOT_Thing_Down_Raw_Register() = %d\n", rc);
        goto exit;
    }
    printf(" ~~~~~~~~ register callback success ~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    length = 1024 * 4; //具体取值请根据自行创建的产品模型进行调整，可以参考运行时日志会提示dsl具体大小，取值需比dsl大小大即可
    printf(" ~~~~~~~~~~~~ start dsl template ~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~ get ~~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Get_Dsl_Template(g_thing_t, 
                PRODUCT_KEY, 
                DEVICE_NAME, 
                sub_dsltemplate_get, 
                &length);     
    if (SUCCESS_RETURN != rc) {
        printf("dsl template get rc = IOT_Thing_Dsl_Template() = %d\n", rc);
        goto exit;
    }

    printf("sub_dsltemplate_get: length[%d]:[%s]\n", length, sub_dsltemplate_get);
    printf(" ~~~~~~~~~~~ dsl template success ~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");    
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~ start thing property ~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~ post ~~~~~~~~~~~~~~~~~~ \n");   
    rc = IOT_Thing_Post_Property(g_thing_t, 
                PRODUCT_KEY, 
                DEVICE_NAME, 
                "{\"afabwa\":\"sfs\"}");
    printf(" ~~~~~~~~~~ thing property success ~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");   
  
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~ start trigger event ~~~~~~~~~~~ \n");  
    rc = IOT_Thing_Trigger_Event(g_thing_t,
                PRODUCT_KEY, 
                DEVICE_NAME, 
                "{\"dafd\":0.90}",
                "propertyIdentifier20");
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~~~ start deviceinfo ~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~ update ~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Update_Deviceinfo(g_thing_t, 
                PRODUCT_KEY, 
                DEVICE_NAME, 
                "{\"attrKey\":\"Temperature\",\"attrValue\":36.8}");     
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~~~~~~~~ delete ~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Delete_Deviceinfo(g_thing_t, 
                PRODUCT_KEY, 
                DEVICE_NAME, 
                "{\"attrKey\":\"Temperature\",\"attrValue\":\"36.8\"}");     

    printf(" ~~~~~~~~~~~~ deviceinfo success ~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~ start publish rawdata ~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Publish_Rawdata(g_thing_t,
                PRODUCT_KEY, 
                DEVICE_NAME, 
                "aa00090000ff00003039001b",
                strlen("aa00090000ff00003039001b"));
    printf(" ~~~~~~~~~ publish rawdata success ~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    printf(" ~~~~~~~~~~~~~~~ start yield ~~~~~~~~~~~~~~~ \n"); 
    while(cnt < 30) {
        IOT_Thing_Yield(g_thing_t, 200);
        cnt++;
    }    

    exit:

    HAL_SleepMs(200);

    IOT_Thing_Destroy((void**)&g_thing_t);
    printf(" ~~~~~~~~~~~~ destory success ~~~~~~~~~~~~~~ \n"); 
    
    return SUCCESS_RETURN;
}


// 示例使用subdev_example_api.h中对topic进行封装的API进行网关开发
int demo_thing_function(char *msg_buf, char *msg_readbuf)
{
    int rc, cnt = 0;
    uint32_t length = 0;
    uint32_t topo_length = 0;
    iotx_conn_info_pt puser_info;
    iotx_mqtt_param_t mqtt_t;
    iotx_thing_param_t thing_param;    
    
    //具体取值请根据自行创建的产品模型进行调整，可以参考运行时日志会提示dsl具体大小，取值需比dsl大小大即可
    char sub_dsltemplate_get[1024 * 4] = {0} ;
    
    char dsltemplate_printf[512] = {0};
    int printf_num = 0; 
    char sign[41] = {0};
    char timestamp[20] = {0};
    char client_id[32] = {0};  //请根据devicename和productkey调整大小
    
    char get_topo_reply[512] = {0};

    /**< get device info*/
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/

    /* Device AUTH */
    rc = IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        printf("rc = IOT_SetupConnInfo() = %d\n", rc);
        return rc;
    }

    /* Construct a master-slave */
    memset(&mqtt_t, 0, sizeof(iotx_mqtt_param_t));
    memset(&thing_param, 0, sizeof(iotx_thing_param_t));

    thing_param.mqtt = &mqtt_t;
    thing_param.mqtt->port = puser_info->port;
    thing_param.mqtt->host = puser_info->host_name;
    thing_param.mqtt->client_id = puser_info->client_id;
    thing_param.mqtt->username = puser_info->username;
    thing_param.mqtt->password = puser_info->password;
    thing_param.mqtt->pub_key = puser_info->pub_key;

    thing_param.mqtt->request_timeout_ms = 2000;
    thing_param.mqtt->clean_session = 0;
    thing_param.mqtt->keepalive_interval_ms = 60000;
    thing_param.mqtt->pread_buf = msg_readbuf;
    thing_param.mqtt->read_buf_size = MSG_LEN_MAX;
    thing_param.mqtt->pwrite_buf = msg_buf;
    thing_param.mqtt->write_buf_size = MSG_LEN_MAX;

    thing_param.mqtt->handle_event.h_fp = NULL;
    thing_param.mqtt->handle_event.pcontext = NULL;
    
    printf(" ~~~~~~~~~~~~~~~~ start test ~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    /* construct */
    printf(" ~~~~~~~~~~~~~~ start construct ~~~~~~~~~~~~~ \n");
    g_thing_t = IOT_Thing_Construct(&thing_param);
    if (NULL == g_thing_t) {
        printf("construct device Thing failed!\n");
        return rc;
    }

    if (FAIL_RETURN == IOT_Thing_RRPC_Register(g_thing_t, //g_thing_t, 
            PRODUCT_KEY, 
            DEVICE_NAME, 
            rrpc_request_handler)) {
        printf("rrpc register error\n");
        return FAIL_RETURN;
    }
    printf(" ~~~~~~~~~~~~ construct success ~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    // 静态注册子设备
    printf(" ~~~~~~~~~~~~~~ start register ~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~ static ~~~~~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~ sub1 ~~~~~~~~~~~~~~~~~~~ \n");
    
    /* timestamp */
    strncpy(timestamp, "2524608000000", strlen("2524608000000") + 1);   
   
    /* client id */
    HAL_Snprintf(client_id, 32, "%s.%s",SUB_1_PRODUCT_KEY, SUB_1_DEVICE_NAME);
    
    /* sign */    
    if (FAIL_RETURN == _calc_sign(SUB_1_PRODUCT_KEY,
                            SUB_1_DEVICE_NAME,
                            SUB_1_DEVICE_SECRET,
                            sign, 
                            41,
                            "hmacsha1",
                            client_id,
                            timestamp)) {
        printf("sign fail \n");
        return FAIL_RETURN;
    }
    rc = IOT_Thing_Register(g_thing_t, 
                IOTX_Thing_REGISTER_TYPE_STATIC, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                timestamp, 
                client_id,
                sign,
                IOTX_Thing_SIGN_METHOD_TYPE_SHA); 
    if (SUCCESS_RETURN != rc) {
        printf("static rc = IOT_Thing_Register() = %d\n", rc);
        IOT_Thing_Destroy((void**)&g_thing_t);
        return rc;
    }

    // login
    printf(" ~~~~~~~~~~~~~~~ start login ~~~~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Login(g_thing_t, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                timestamp, 
                client_id,
                sign,
                IOTX_Thing_SIGN_METHOD_TYPE_SHA,                
                IOTX_Thing_CLEAN_SESSION_TYPE_TRUE);  
    if (SUCCESS_RETURN != rc) {
        printf("login sub1 rc = IOT_Thing_Login() = %d\n", rc);
        goto exit;
        return rc;
    }
    
    printf(" ~~~~~~~~~~~~~~ login success ~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    if (FAIL_RETURN == (rc = IOT_Thing_RRPC_Register(g_thing_t, 
                            SUB_1_PRODUCT_KEY, 
                            SUB_1_DEVICE_NAME, 
                            rrpc_request_handler))) {
        printf("rrpc register error\n");
        goto exit;
    }
        
    // 动态注册子设备请查看demo_gateway_function中的示例
    printf(" ~~~~~~~~~~~~~~~~~~ sub2 ~~~~~~~~~~~~~~~~~~~ \n");
    /* client id */
    memset(client_id, 0x0, 32);
    HAL_Snprintf(client_id, 32, "%s.%s", SUB_2_PRODUCT_KEY, SUB_2_DEVICE_NAME);
    
    /* sign */    
    memset(sign, 0x0, 41);
    if (FAIL_RETURN == _calc_sign(SUB_2_PRODUCT_KEY,
                            SUB_2_DEVICE_NAME,
                            SUB_2_DEVICE_SECRET,
                            sign, 
                            41,
                            "hmacmd5",
                            client_id,
                            timestamp)) {
        printf("sign fail \n");
        return FAIL_RETURN;
    }
    rc = IOT_Thing_Register(g_thing_t, 
                IOTX_Thing_REGISTER_TYPE_STATIC, 
                SUB_2_PRODUCT_KEY, 
                SUB_2_DEVICE_NAME, 
                timestamp,
                client_id,
                sign,
                IOTX_Thing_SIGN_METHOD_TYPE_MD5); 
    if (SUCCESS_RETURN != rc) {
        printf("dynamic rc = IOT_Thing_Register() = %d\n", rc);
        IOT_Thing_Destroy((void**)&g_thing_t);
        return rc;
    }
    //printf("dynamic register, device_secret = %s\n", sub2_devcie_secret);
    printf(" ~~~~~~~~~~~~ register success ~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~ start login ~~~~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Login(g_thing_t, 
                SUB_2_PRODUCT_KEY, 
                SUB_2_DEVICE_NAME,         
                timestamp,
                client_id,
                sign,
                IOTX_Thing_SIGN_METHOD_TYPE_MD5,
                IOTX_Thing_CLEAN_SESSION_TYPE_FALSE); 
    if (SUCCESS_RETURN != rc) {
        printf("login sub2 rc = IOT_Thing_Login() = %d\n", rc);
        goto exit;
    }
    if (FAIL_RETURN == ( rc = IOT_Thing_RRPC_Register(g_thing_t, 
                            SUB_2_PRODUCT_KEY, 
                            SUB_2_DEVICE_NAME, 
                            rrpc_request_handler))) {
        printf("rrpc register error\n");
        goto exit;
    }
    printf(" ~~~~~~~~~~~~~~ login success ~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    
    printf(" ~~~~~~~~ start register callback ~~~~~~~~~~ \n");    
    printf(" ~~~~~~~~~~~~ service register ~~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Service_Register(g_thing_t, service_request_handler);         
    if (SUCCESS_RETURN != rc) {
        printf("service register rc = IOT_Thing_Service_Register() = %d\n", rc);
        goto exit;
    }
    
    printf(" ~~~~~~~~~~~~ down raw register ~~~~~~~~~~~~ \n");
    rc = IOT_Thing_Down_Raw_Register(g_thing_t, down_raw_handler);           
    if (SUCCESS_RETURN != rc) {
        printf("raw register rc = IOT_Thing_Down_Raw_Register() = %d\n", rc);
        goto exit;
    }
    
    printf(" ~~~~~~~~~~ thing control register ~~~~~~~~~~ \n");
    rc = IOT_Thing_Control_Register(g_thing_t, thing_control_handler);           
    if (SUCCESS_RETURN != rc) {
        printf("raw register rc = IOT_Thing_Down_Raw_Register() = %d\n", rc);
        goto exit;
    }
    printf(" ~~~~~~~~ register callback success ~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    length = 1024 * 4;   //具体取值请根据自行创建的产品模型进行调整，可以参考运行时日志会提示dsl具体大小，取值需比dsl大小大即可
    printf(" ~~~~~~~~~~~~ start dsl template ~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~ get ~~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Get_Dsl_Template(g_thing_t, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                sub_dsltemplate_get, 
                &length);    

    printf("sub_dsltemplate_get: length[%d]", length);
    while(printf_num < length) {
        memset(dsltemplate_printf, 0x0, 512);
        if (length-printf_num > 500) {
            strncpy(dsltemplate_printf, &sub_dsltemplate_get[printf_num], 500);      
            printf_num += 500;            
        } else if (length - printf_num <= 500){
            strncpy(dsltemplate_printf, &sub_dsltemplate_get[printf_num], length-printf_num);  
            printf_num = length;
        }        
        printf("%s", dsltemplate_printf);
    }
    printf("\n");
    printf(" ~~~~~~~~~~~ dsl template success ~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n"); 

    topo_length = 512;
    if (FAIL_RETURN == ( rc = IOT_Thing_Get_TOPO(g_thing_t, get_topo_reply, &topo_length))) {
        printf("IOT_Thing_Get_TOPO error\n");
        goto exit;
    }
    printf(" get_topo_reply length %d \n", topo_length);
    printf(" get_topo_reply %s \n", get_topo_reply);


    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~ start thing property ~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~ post ~~~~~~~~~~~~~~~~~~ \n");   
    rc = IOT_Thing_Post_Property(g_thing_t, 
                SUB_1_PRODUCT_KEY,//SUB_2_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME,//SUB_2_DEVICE_NAME,                 
                "{\"afabwa\":\"sfs\"}");       
    printf(" ~~~~~~~~~~ thing property success ~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");   
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~ start trigger event ~~~~~~~~~~~ \n");  
    rc = IOT_Thing_Trigger_Event(g_thing_t,
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                "{\"propertyIdentifier20\":\"0.90\"}",
                "propertyIdentifier20");        

    printf(" ~~~~~~~~~~~ trigger event success ~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~~~ start deviceinfo ~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~ update ~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Update_Deviceinfo(g_thing_t, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                "[{\"attrKey\":\"Temperature\",\"attrValue\":\"36.8\"}]");     

    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~~~~~~~~ delete ~~~~~~~~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Delete_Deviceinfo(g_thing_t, 
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                "[{\"attrKey\":\"Temperature\",\"attrValue\":\"36.8\"}]");     

    printf(" ~~~~~~~~~~~~ deviceinfo success ~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    //具体参数请自行参考云端控制台提示
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~ start publish rawdata ~~~~~~~~~~ \n"); 
    rc = IOT_Thing_Publish_Rawdata(g_thing_t,
                SUB_1_PRODUCT_KEY, 
                SUB_1_DEVICE_NAME, 
                "aa00090000ff00003039001b",
                strlen("aa00090000ff00003039001b"));

    printf(" ~~~~~~~~~ publish rawdata success ~~~~~~~~~ \n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    
    printf(" ~~~~~~~~~~~~~~~ start yield ~~~~~~~~~~~~~~~ \n"); 
    while(cnt < 30) {
        IOT_Thing_Yield(g_thing_t, 200);
        cnt++;
    }    

    exit:
    
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    IOT_Thing_Logout(g_thing_t, SUB_1_PRODUCT_KEY, SUB_1_DEVICE_NAME);
    IOT_Thing_Logout(g_thing_t, SUB_2_PRODUCT_KEY, SUB_2_DEVICE_NAME);

    printf(" ~~~~~~~~~~~~~ logout success ~~~~~~~~~~~~~~ \n"); 
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

    HAL_SleepMs(200);

    IOT_Thing_Destroy((void**)&g_thing_t);
    printf(" ~~~~~~~~~~~~ destory success ~~~~~~~~~~~~~~ \n"); 
    
    return SUCCESS_RETURN;
}


int main()
{
    char msg_buf[MSG_LEN_MAX] ={0};
    char msg_readbuf[MSG_LEN_MAX] ={0};
    
    IOT_OpenLog("masterslave");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /**< set device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    /**< end*/
    
    // 示例如何使用subdev的API直接进行开发
    //demo_gateway_function(msg_buf, msg_readbuf);

    // 示例使用subdev_example_api.h中对topic进行封装的API进行网关开发
    demo_thing_function(msg_buf, msg_readbuf);

    // 示例使用subdev_example_api.h中对topic进行封装的API进行单品设备开发
    //demo_only_one_device(msg_buf, msg_readbuf);

    printf("out of demo!\n");
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    return 0;
}

