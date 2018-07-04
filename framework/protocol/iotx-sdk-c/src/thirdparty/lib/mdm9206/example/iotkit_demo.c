#include <string.h>

#include "qcli_api.h"
#include "iot_import.h"
#include "iot_export.h"

QCLI_Group_Handle_t qcli_iotkit_handle;     /* Handle for IOT Command Group. */

/******************************************************/
/**********************HTTP****************************/
#define HTTP_PRODUCT_KEY            "******************"
#define HTTP_DEVICE_NAME            "******************"
#define HTTP_DEVICE_SECRET          "******************"
#define HTTP_DEVICE_ID              "******************"
/**********************END HTTP************************/

/******************************************************/
/**********************MQTT****************************/

#define MQTT_PRODUCT_KEY            "******************"
#define MQTT_DEVICE_NAME            "******************"
#define MQTT_DEVICE_SECRET          "******************"

static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    QCLI_Printf(qcli_iotkit_handle, "Topic:%s,Payload:%s\n", ptopic_info->ptopic, ptopic_info->payload);
}

static void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            QCLI_Printf(qcli_iotkit_handle, "undefined event occur.\n");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            QCLI_Printf(qcli_iotkit_handle, "MQTT disconnect.\n");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            QCLI_Printf(qcli_iotkit_handle, "MQTT reconnect.\n");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            QCLI_Printf(qcli_iotkit_handle, "subscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            QCLI_Printf(qcli_iotkit_handle, "subscribe wait ack timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            QCLI_Printf(qcli_iotkit_handle, "subscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            QCLI_Printf(qcli_iotkit_handle, "unsubscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            QCLI_Printf(qcli_iotkit_handle, "unsubscribe timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            QCLI_Printf(qcli_iotkit_handle, "unsubscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            QCLI_Printf(qcli_iotkit_handle, "publish success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            QCLI_Printf(qcli_iotkit_handle, "publish timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            QCLI_Printf(qcli_iotkit_handle, "publish nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            QCLI_Printf(qcli_iotkit_handle, "topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s\n",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            QCLI_Printf(qcli_iotkit_handle, "Should NOT arrive here.\n");
            break;
    }
}


/*****************END MQTT**********************************/

static QCLI_Command_Status_t mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t http(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static const QCLI_Command_t iot_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description */
    {mqtt, false, "mqtt", "a <key> <name> <secret> or a #auth\n"
        "       c <keepalive> <clean> <timeout> or c #config\n"
            "       p <topic> <message>  #publish\n"
            "       s <topic> #subscribe\n"
            "       u <topic> #unsubscribe\n"
            "       y <timeout> or y#yield\n"
            "       d #destroy", "test mqtt function",NULL},

    {http, false, "http", "#i <key><secret><name><id> or i#init\n"
        "       a #auth\n"
            "       p <message> #post\n"
            "       d #deinit","test http function",NULL},
};

static const QCLI_Command_Group_t iot_cmd_group =
{
    "IoTKit",              /* Group_String: will display cmd prompt as "IOT> " */
    sizeof(iot_cmd_list)/sizeof(iot_cmd_list[0]),   /* Command_Count */
    iot_cmd_list        /* Command_List */
};

/*****************************************************************************
 * This function is used to register the IOT Command Group with QCLI.
 *****************************************************************************/
void Initialize_IoTKit_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_iotkit_handle = QCLI_Register_Command_Group(NULL, &iot_cmd_group);
    if (qcli_iotkit_handle)
    {
      QCLI_Printf(qcli_iotkit_handle, "IOT Registered\n");
    }
    return;
}

static QCLI_Command_Status_t mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

#define MSG_LEN_MAX                 (1024)
#define MQTT_PRODUCT_KEY_LEN        (32)
#define MQTT_DEVICE_NAME_LEN        (32)
#define MQTT_DEVICE_SECRET_LEN      (64)

    static char isInit = 0;
    static char msg_buf[MSG_LEN_MAX];
    static char msg_readbuf[MSG_LEN_MAX] ;

    static iotx_conn_info_pt pconn_info = NULL;
    static iotx_mqtt_param_t mqtt_params;
    static void *pclient=NULL;
    char command = '0';

    static char product_key[MQTT_PRODUCT_KEY_LEN];
    static char device_name[MQTT_DEVICE_NAME_LEN];
    static char device_secret[MQTT_DEVICE_SECRET_LEN];

    if(Parameter_Count < 1){
        return QCLI_STATUS_USAGE_E;
    }

    if(isInit == 0){
        memset(msg_buf, 0, sizeof(msg_buf));
        memset(msg_readbuf, 0, sizeof(msg_readbuf));
        memset(&mqtt_params, 0, sizeof(mqtt_params));
        memset(product_key, 0, sizeof(product_key));
        memset(device_name, 0, sizeof(device_name));
        memset(device_secret, 0, sizeof(device_secret));
        isInit = 1;
    }

    command = (char)Parameter_List[0].String_Value[0];

    switch ( command ) {
        case 's':
        case 'S':
            {
                int rc = 0;
                char topic_data[100]={0};
                if ( NULL == pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "need constructed first\n");
                    break;
                }
                if(Parameter_Count != 2){
                    return QCLI_STATUS_USAGE_E;
                }
                sprintf(topic_data, "/%s/%s/%s", product_key, device_name, Parameter_List[1].String_Value);
                rc = IOT_MQTT_Subscribe(pclient, topic_data, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
                if (rc < 0) {
                    IOT_MQTT_Destroy(&pclient);
                    pclient = NULL;
                    QCLI_Printf(qcli_iotkit_handle, "subscribe command error\n");
                }
                else{
                    QCLI_Printf(qcli_iotkit_handle, "subscribe %s \n", topic_data);
                }
            }
            break;

        case 'a':
        case 'A':
            {
                if (pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "mqtt initialized\n");
                    break;
                }

                IOT_OpenLog("mqtt");
                IOT_SetLogLevel(IOT_LOG_DEBUG);

                if(Parameter_Count == 1){
                    strncpy(product_key, MQTT_PRODUCT_KEY, sizeof(product_key) - 1);
                    strncpy(device_name, MQTT_DEVICE_NAME, sizeof(device_name) - 1);
                    strncpy(device_secret, MQTT_DEVICE_SECRET, sizeof(device_secret) - 1);
                }
                else if(Parameter_Count == 4){
                    strncpy(product_key, (char *)Parameter_List[1].String_Value, sizeof(product_key) - 1);
                    strncpy(device_name, (char *)Parameter_List[2].String_Value, sizeof(device_name) - 1);
                    strncpy(device_secret, (char *)Parameter_List[3].String_Value, sizeof(device_secret) - 1);
                }
                else{
                    return QCLI_STATUS_USAGE_E;
                }

                if (0 != IOT_SetupConnInfo(product_key, device_name, device_secret, (void **)&pconn_info)) {
                    QCLI_Printf(qcli_iotkit_handle, "auth request failed\n");
                }

                QCLI_Printf(qcli_iotkit_handle, "auth command ok\n");
            }
            break;

        case 'c':
        case 'C':
            {
                int  keepalive,clean,timeout;
                if ( pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "had constructed\n");
                    break;
                }
                if ((NULL == pconn_info) || (0 == pconn_info->client_id)){
                    QCLI_Printf(qcli_iotkit_handle, "please auth first\n");
                    break;
                }
                if(Parameter_Count == 1){
                    keepalive=60000;
                    clean=0;
                    timeout=2000;
                }
                else if(Parameter_Count == 4){
                    keepalive=Parameter_List[1].Integer_Value;;
                    clean=Parameter_List[2].Integer_Value;
                    timeout=Parameter_List[3].Integer_Value;
                }
                else{
                    return QCLI_STATUS_USAGE_E;
                }
                memset(&mqtt_params, 0x0, sizeof(mqtt_params));
                mqtt_params.port = pconn_info->port;
                mqtt_params.host = pconn_info->host_name;
                mqtt_params.client_id = pconn_info->client_id;
                mqtt_params.username = pconn_info->username;
                mqtt_params.password = pconn_info->password;
                mqtt_params.pub_key = pconn_info->pub_key;

                mqtt_params.request_timeout_ms = timeout;
                mqtt_params.clean_session = clean;
                mqtt_params.keepalive_interval_ms = keepalive;
                mqtt_params.pread_buf = msg_readbuf;
                mqtt_params.read_buf_size = MSG_LEN_MAX;
                mqtt_params.pwrite_buf = msg_buf;
                mqtt_params.write_buf_size = MSG_LEN_MAX;

                mqtt_params.handle_event.h_fp = event_handle;
                mqtt_params.handle_event.pcontext = NULL;

                pclient = IOT_MQTT_Construct(&mqtt_params);
                if (NULL == pclient)
                    QCLI_Printf(qcli_iotkit_handle, "MQTT construct failed\n");
                else
                    QCLI_Printf(qcli_iotkit_handle, "MQTT construct success\n");
            }
            break;

        case 'p':
        case 'P':
            {
                int rc=0;
                iotx_mqtt_topic_info_t topic_msg;
                char topic_data[100]={0};
                if ( NULL == pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "need constructed first\n");
                    break;
                }
                if(Parameter_Count != 3 ){
                    return QCLI_STATUS_USAGE_E;
                }
                memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
                topic_msg.qos = IOTX_MQTT_QOS1;
                topic_msg.retain = 0;
                topic_msg.dup = 0;
                topic_msg.payload = (void *)Parameter_List[2].String_Value;
                topic_msg.payload_len = strlen(topic_msg.payload);
                sprintf(topic_data, "/%s/%s/%s", product_key, device_name, Parameter_List[1].String_Value);

                rc = IOT_MQTT_Publish(pclient, topic_data, &topic_msg);
                QCLI_Printf(qcli_iotkit_handle, "topic_data:%s rc = IOT_MQTT_Publish() = %d", topic_data, rc);
            }
            break;

        case 'u':
        case 'U':
            {
                char topic_data[100]={0};
                if ( NULL == pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "need constructed first\n");
                    break;
                }
                if(Parameter_Count != 2){
                    return QCLI_STATUS_USAGE_E;
                }
                sprintf(topic_data, "/%s/%s/%s", product_key, device_name, Parameter_List[1].String_Value);
                IOT_MQTT_Unsubscribe(pclient, topic_data);
                QCLI_Printf(qcli_iotkit_handle, "unsubscribe command\n");
            }
            break;

        case 'y':
        case 'Y':
            {
                int yield = 0;
                if ( NULL == pclient) {
                    QCLI_Printf(qcli_iotkit_handle, "need constructed first\n");
                    break;
                }
                if(Parameter_Count == 1){
                    yield = 2000;
                }
                else if(Parameter_Count == 2){
                    yield = Parameter_List[1].Integer_Value;
                }
                else {
                    return QCLI_STATUS_USAGE_E;
                }
                IOT_MQTT_Yield(pclient,yield);
                QCLI_Printf(qcli_iotkit_handle, "yield command\n");
            }
            break;

        case 'd':
        case 'D':
            if ( NULL == pclient) {
                QCLI_Printf(qcli_iotkit_handle, "need constructed first\n");
                break;
            }
            IOT_MQTT_Destroy(&pclient);
            IOT_CloseLog();
            pclient = NULL;
            QCLI_Printf(qcli_iotkit_handle, "destroy command\n");
            break;

        default:
            break;
    }				/* -----  end switch  ----- */

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t http(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    static void *http_handle = NULL;

    static char isInit = 0;
    static iotx_device_info_t device_info;
    static iotx_http_param_t http_param;

    char command = '0';

    if(Parameter_Count < 1){
        return QCLI_STATUS_USAGE_E;
    }

    if(0 == isInit){
        memset(&device_info, 0, sizeof(device_info));
        memset(&http_param, 0 ,sizeof(http_param));
        isInit = 1;
    }

    command = (char)Parameter_List[0].String_Value[0];

    switch ( command ) {
       case 'i':
       case 'I':
           {
                if(http_handle){
                    QCLI_Printf(qcli_iotkit_handle, "HTTP had initiled\n");
                    break;
                }
                memset(&device_info , 0x00, sizeof(iotx_device_info_t));
                memset(&http_param , 0x00, sizeof(iotx_http_param_t));
                if(Parameter_Count == 1){
                    strncpy(device_info.product_key, HTTP_PRODUCT_KEY, sizeof(device_info.product_key) - 1);
                    strncpy(device_info.device_secret, HTTP_DEVICE_SECRET, sizeof(device_info.device_secret) - 1);
                    strncpy(device_info.device_name, HTTP_DEVICE_NAME, sizeof(device_info.device_name) - 1);
                    strncpy(device_info.device_id, HTTP_DEVICE_ID, sizeof(device_info.device_id) - 1);
                }
                else if(Parameter_Count == 5){
                    strncpy(device_info.product_key, (char*)Parameter_List[1].String_Value, sizeof(device_info.product_key) - 1);
                    strncpy(device_info.device_secret, (char*)Parameter_List[2].String_Value, sizeof(device_info.device_secret) - 1);
                    strncpy(device_info.device_name, (char*)Parameter_List[3].String_Value, sizeof(device_info.device_name) - 1);
                    strncpy(device_info.device_id, (char*)Parameter_List[4].String_Value, sizeof(device_info.device_id) - 1);
                }
                else{
                    return QCLI_STATUS_USAGE_E;
                }

                IOT_OpenLog("http");
                IOT_SetLogLevel(IOT_LOG_DEBUG);

                http_param.device_info = &device_info;

                http_handle = IOT_HTTP_Init(&http_param);
                if(NULL == http_handle)
                     QCLI_Printf(qcli_iotkit_handle,"http construct failed\n");
                else
                    QCLI_Printf(qcli_iotkit_handle, "construct command ok\n");

            }
           break;
        case 'a':
        case 'A':
            {
                if(NULL == http_handle){
                    QCLI_Printf(qcli_iotkit_handle, "http uninitialized\n");
                    break;
                }
                if(0 != IOT_HTTP_DeviceNameAuth(http_handle)){
                    QCLI_Printf(qcli_iotkit_handle, "http auth failed\n");
                }
                else{
                    QCLI_Printf(qcli_iotkit_handle, "http auth success\n");
                }
            }
            break;
        case 'p':
        case 'P':
            {
                char path[IOTX_URI_MAX_LEN + 1] = {0};
                char request_buf[1024];
                iotx_http_message_param_t msg_param;

                if(NULL == http_handle){
                    QCLI_Printf(qcli_iotkit_handle, "http uninitialized\n");
                    break;
                }

                if(Parameter_Count !=2 ){
                    return QCLI_STATUS_USAGE_E;
                }

                msg_param.request_payload = (char*)Parameter_List[1].String_Value;
                msg_param.response_payload = request_buf;
                msg_param.timeout_ms = 5000;
                msg_param.request_payload_len = strlen(msg_param.request_payload) + 1;
                msg_param.response_payload_len = 1024;
                msg_param.topic_path = path;

                snprintf(msg_param.topic_path, IOTX_URI_MAX_LEN, "/topic/%s/%s/update", (char *)device_info.product_key,
                 (char *)device_info.device_name);
                if (0 == IOT_HTTP_SendMessage(http_handle, &msg_param))
                    QCLI_Printf(qcli_iotkit_handle, "message response is %s\r\n", msg_param.response_payload);
                else
                    QCLI_Printf(qcli_iotkit_handle, "IOT_HTTP_SendMessage error\r\n");

            }
            break;
        case 'd':
        case 'D':
            {
                IOT_HTTP_Disconnect(http_handle);
                IOT_HTTP_DeInit(&http_handle);
                http_handle = NULL;
                IOT_DumpMemoryStats(IOT_LOG_DEBUG);
                IOT_CloseLog();
                QCLI_Printf(qcli_iotkit_handle, "destroy command\n");
            }
            break;
        default:
            break;
    }
    return QCLI_STATUS_SUCCESS_E;
}

