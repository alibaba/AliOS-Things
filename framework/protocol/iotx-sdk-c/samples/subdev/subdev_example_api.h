
#ifndef SRC_SUBDEVICE_Thing_UTIL_H_
#define SRC_SUBDEVICE_Thing_UTIL_H_

#include "exports/iot_export_subdev.h"

/* The fomat of thing topic */
#define TOPIC_Thing_COMMON_FMT           "/sys/%s/%s/thing/%s/%s"

/* The fomat of thing topic */
#define TOPIC_Thing_COMMON_EXT_FMT       "/sys/%s/%s/thing/%s/%s/%s"

/* The fomat of thing topic */
#define TOPIC_Thing_COMMON_EXT_EXT_FMT   "/sys/%s/%s/thing/%s"


/* Register type */
typedef enum IOTX_Thing_REGISTER_TYPES {
    /* static, via web get device_secert */
    IOTX_Thing_REGISTER_TYPE_STATIC,
    /* dynamic, via register API get device_secert */
    IOTX_Thing_REGISTER_TYPE_DYNAMIC,

    IOTX_Thing_REGISTER_TYPE_MAX
}iotx_thing_register_types_t;


/* Sign method */
typedef enum IOTX_Thing_SIGN_METHOD_TYPES {
    /* HmacSha1 */
    IOTX_Thing_SIGN_METHOD_TYPE_SHA,

    /* HmacMd5 */
    IOTX_Thing_SIGN_METHOD_TYPE_MD5,

    /* Undefined */
    IOTX_Thing_SIGN_METHOD_TYPE_UNDEFINED,

    /* Maximum number of sign method */
    IOTX_Thing_SIGN_METHOD_TYPE_MAX
}iotx_thing_sign_method_types_t;


/* Login clean seesion type */
typedef enum IOTX_Thing_CLEAN_SESSION_TYPES {
    /* True */
    IOTX_Thing_CLEAN_SESSION_TYPE_TRUE,

    /* False */
    IOTX_Thing_CLEAN_SESSION_TYPE_FALSE,

    /* Undefined */
    IOTX_Thing_CLEAN_SESSION_TYPE_UNDEFINED,

    /* Maximum number of login clean seesion type */
    IOTX_Thing_CLEAN_SESSION_TYPE_MAX
}iotx_thing_clean_session_types_t;


/* reply type  */
typedef enum IOTX_Thing_REPLY_ENUM {
    /* thing/sub/register   */
    IOTX_Thing_REPLY_REGISTER,

    /* thing/sub/unregister   */
    IOTX_Thing_REPLY_UNREGISTER,

    /* thing/topo/add   */
    IOTX_Thing_REPLY_TOPO_ADD,

    /* thing/topo/delete   */
    IOTX_Thing_REPLY_TOPO_DELETE,

    /* thing/event/property/post   */
    IOTX_Thing_REPLY_PROPERTY_POST,

    /* thing/dsltemplate/get   */
    IOTX_Thing_REPLY_DSL_GET,

    /* thing/deviceinfo/update*/
    IOTX_Thing_REPLY_DEVICEINFO_UPDATE,

    /* thing/deviceinfo/delete*/
    IOTX_Thing_REPLY_DEVICEINFO_DELETE,

    /* thing/keyelement/post   */
    IOTX_Thing_REPLY_KEYELEMENT_POST,

    /* thing/up_raw   */
    IOTX_Thing_REPLY_UP_RAW,

    /* thing/event/+/post   */
    IOTX_Thing_REPLY_EVENT_POST,

    /* Maximum number of reply type */
    IOTX_Thing_REPLY_MAX
}iotx_thing_reply_type_t;


/* service type  */
typedef enum IOTX_Thing_SERVICE_TYPES {
    /* property/set*/
    IOTX_Thing_SERVICE_TYPE_PROPERTY_SET,

    /* property/get*/
    IOTX_Thing_SERVICE_TYPE_PROPERTY_GET,

    /* Undefined service type   */
    IOTX_Thing_SERVICE_TYPE_UNDEFINED,

    IOTX_Thing_SERVICE_TYPE_TOPO_UPDATE,

    /* Maximum number of service type */
    IOTX_Thing_SERVICE_TYPE_MAX
}iotx_thing_service_type_t;


/* thing control type  */
typedef enum IOTX_Thing_CONTROL_TYPES {
    /* thing/enable*/
    IOTX_Thing_CONTROL_TYPE_ENABLE,
    /* thing/disable*/
    IOTX_Thing_CONTROL_TYPE_DISABLE,
    /* thing/delete*/
    IOTX_Thing_CONTROL_TYPE_DELETE,

    /* Maximum number of thind control type */
    IOTX_Thing_CONTROL_TYPE_MAX
}iotx_thing_control_type_t;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a service occur.
 *
 * @param thing, the thing context
 * @param product_key, the product key
 * @param deveice_name, the deveice name
 * @param service_type, the service type
 * @param message_id, the service's message id
 * @param params, the service's params
 * @param params_length, the length of service's params
 * @param service_id, the service's id, defined by dsl template *
 *
 * @return none
 */
typedef void (*service_request_callback)(void* thing_t,
        const char* product_key,
        const char* device_name,
        iotx_thing_service_type_t service_type,
        uint32_t message_id,
        char* params,
        uint32_t params_length,
        const char* service_id);


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a raw data down occur.
 *
 * @param thing, the thing context
 * @param product_key, the product key
 * @param deveice_name, the deveice name
 * @param raw_data, the raw data
 * @param raw_data_length, the length of raw data
 *
 * @return none
 */
typedef void (*down_raw_callback)(void* thing_t,
        const char* product_key,
        const char* device_name,
        const char* raw_data,
        uint32_t raw_data_length);


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a thing control occur.
 *
 * @param thing, the thing context
 * @param product_key, the product key
 * @param deveice_name, the deveice name
 * @param raw_data, the raw data
 * @param raw_data_length, the length of raw data
 *
 * @return none
 */
typedef void (*thing_control_callback)(void* thing_t,
        const char* product_key,
        const char* device_name,
        iotx_thing_control_type_t thing_control_type,
        uint32_t message_id);



/* The structure of thing parameter */
typedef struct {
    iotx_mqtt_param_pt              mqtt;
} iotx_thing_param_t, *iotx_thing_param_pt;


/**
 * @brief Create a Thing construction
 *        This function    used to create a Thing construction.
 *
 * @param parameter.
 *
 * @return 0, Create success; -1, Create fail.
 */
void* IOT_Thing_Construct(iotx_thing_param_pt param);


/**
 * @brief Destory Thing construction
 *
 * @param pointer of handle, specify the Thing construction.
 *
 * @return 0, Destory success; -1, Destory fail.
 */
int IOT_Thing_Destroy(void** handle);


/**
 * @brief Device register
 *        This function    used to register device.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param register type.
 *               IOTX_Thing_REGISTER_TYPE_STATIC
 *               IOTX_Thing_REGISTER_TYPE_DYNAMIC
 * @param product key.
 * @param device name.
 * @param timestamp.           [if type = dynamic, must be NULL ]
 * @param client_id.           [if type = dynamic, must be NULL ]
 * @param sign.                [if type = dynamic, must be NULL ]
 * @param sign_method.
 *               IOTX_Thing_SIGN_METHOD_TYPE_SHA
 *               IOTX_Thing_SIGN_METHOD_TYPE_MD5
 *
 * @return 0, Logout success; -1, Logout fail.
 */
int IOT_Thing_Register(void* handle,
        iotx_thing_register_types_t type,
        const char* product_key,
        const char* device_name,
        char* timestamp,
        char* client_id,
        char* sign,
        iotx_thing_sign_method_types_t sign_type);


/**
 * @brief Device unregister
 *        This function    used to unregister device.
 *        The device must dynamic register again if it want to use after unregister.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 *
 * @return 0, Unregister success; -1, Unregister fail.
 */
int IOT_Thing_Unregister(void* handle,
        const char* product_key,
        const char* device_name);


/**
 * @brief Device login
 *        This function    used to login device.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param timestamp.           [if register type = dynamic, NULL ]
 * @param client_id.           [if register type = dynamic, NULL ]
 * @param sign.                [if register type = dynamic, NULL ]
 * @param sign method.         [if register type = dynamic, NO mean ]
 *     IOTX_Thing_SIGN_METHOD_TYPE_SHA
 *     IOTX_Thing_SIGN_METHOD_TYPE_MD5
 * @param clean session set.
 *     IOTX_Thing_CLEAN_SESSION_TYPE_TRUE
 *     IOTX_Thing_CLEAN_SESSION_TYPE_FALSE
 *
 * @return 0, Login success; -1, Login fail.
 */
int IOT_Thing_Login(void* handle,
        const char* product_key,
        const char* device_name,
        char* timestamp,
        char* client_id,
        char* sign,
        iotx_thing_sign_method_types_t sign_method,
        iotx_thing_clean_session_types_t clean_session);


/**
 * @brief Device logout
 *        This function    used to logout device.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 *
 * @return 0, Logout success; -1, Logout fail.
 */
int IOT_Thing_Logout(void* handle,
        const char* product_key,
        const char* device_name);

/**
 * @brief Get topo
 *        This function publish a packet with topo/get topic and wait for the reply (with TOPO_GET_REPLY topic).
 *
 * @param pointer of handle, specify the Thing construction.
 * @param get_toop_reply.
 * @param length [in/out]. in -- get_topo_reply buffer length, out -- reply length
 *
 * @return 0, logout success; -1, logout failed.
 */
int IOT_Thing_Get_TOPO(void* handle,
        char* get_toop_reply,
        uint32_t* length);

/**
 * @brief Get config
 *        This function publish a packet with config/get topic and wait for the reply (with CONFIG_GET_REPLY topic).
 *
 * @param pointer of handle, specify the Thing construction.
 * @param get_config_reply.
 * @param length [in/out]. in -- get_config_reply buffer length, out -- reply length
 *
 * @return 0, logout success; -1, logout failed.
 */
int IOT_Thing_Get_Config(void* handle,
        char* get_config_reply,
        uint32_t* length);

/**
 * @brief Publish Found List
 *        This function publish a packet with new subdevice found list.
 *
 * @return 0, publish success; -1, publish failed.
 */
int IOT_Thing_Publish_Found_List(void* handle, const char* product_key,
    const char* device_name);


/**
 * @brief Get DSL template
 *        This function    used to get DSL template.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param DSL template buffer [output].
 * @param buffer size [output].
 *
 * @return 0, Get success; -1, Get fail.
 */
int IOT_Thing_Get_Dsl_Template(void* handle,
        const char* product_key,
        const char* device_name,
        char* dsl_template,
        uint32_t* length);


/**
 * @brief Post key element
 *        This function    used to post key element to set specify template.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param event.
 * @param property.
 * @param service.
 *
 * @return 0, Post success; -1, Post fail.
 */
int IOT_Thing_Post_Property(void* handle,
        const char* product_key,
        const char* device_name,
        const char* property);


/**
 * @brief Update device information
 *        This function    used to update device information.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param device information.
 *
 * @return 0, Update success; -1, Update fail.
 */
int IOT_Thing_Update_Deviceinfo(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo);


/**
 * @brief Delete device information
 *        This function    used to delete device information.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param device information.
 *
 * @return 0, Delete success; -1, Delete fail.
 */
int IOT_Thing_Delete_Deviceinfo(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo);


/**
 * @brief Post key element
 *        This function    used to post key element to set specify template.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param event.
 * @param property.
 * @param service.
 *
 * @return 0, Post success; -1, Post fail.
 */
int IOT_Thing_Post_Keyelement(void* handle,
        const char* product_key,
        const char* device_name,
        const char* evnet,
        const char* property,
        const char* service);


/**
 * @brief Response set propety request
 *        This function    used to response set propety request.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param set propety request message id.
 * @param response code.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Thing_Set_Property_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t msg_id,
        uint32_t code);


/**
 * @brief Response get propety request
 *        This function    used to response get propety request.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param get propety request message id.
 * @param property data.
 * @param response code.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Thing_Get_Property_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t msg_id,
        const char* property_data,
        uint32_t code);


/**
 * @brief Register service request callback
 *        This function    used to register service request callback.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param callback.
 *
 * @return 0, Register success; -1, Register fail.
 */
int IOT_Thing_Service_Register(void* handle,
        service_request_callback service_callback);


/**
 * @brief Response service request request
 *        This function    used to response service request request.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param service request service id.
 * @param service request message id.
 * @param response code.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Thing_Service_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* service_id,
        uint32_t message_id,
        uint32_t code);


/**
 * @brief Register thing control callback
 *        This function    used to register thing control callback.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param callback.
 *
 * @return 0, Register success; -1, Register fail.
 */
int IOT_Thing_Control_Register(void* handle,
        thing_control_callback thing_callback);


/**
 * @brief Response thing control request
 *        This function    used to response thing control request.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param thing control request message id.
 * @param response code.
 * @param thing control type.
 *     IOTX_THING_CONTROL_TYPE_ENABLE
 *     IOTX_THING_CONTROL_TYPE_DISABLE
 *     IOTX_THING_CONTROL_TYPE_DELETE
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Thing_Control_Response(void* handle,
        const char* product_key,
        const char* device_name,
        uint32_t message_id,
        uint32_t code,
        iotx_thing_control_type_t control_type);


/**
 * @brief Register down raw callback
 *        This function    used to register down raw callback.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param callback.
 *
 * @return 0, Register success; -1, Register fail.
 */
int IOT_Thing_Down_Raw_Register(void* handle,
        down_raw_callback raw_callback);


/**
 * @brief Response Down Raw
 *        This function    used to response the raw data down.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param Response data.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Tmp_Down_Raw_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* response);


/**
 * @brief Register RRPC callback
 *        This function    used to register one RRPC callback.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param rrpc callback function.
 *          every device has RRPC callback by itself.
 *           RRPC's  payload is consists by message id, method and params
 *                  for example:
 *                      "method"："thing.service.property.set";
 *                      "method"："thing.service.property.get";
 *                      "method"："thing.service.{dsl.service.identifer}";
 *                      "method"："thing.service.disable";
 *                      "method"："thing.service.delete";
 *                      "method"："thing.service.enable";
 *
 *
 * @return 0, Register success; -1, Register fail.
 */
int IOT_Thing_RRPC_Register(        void* handle,
        const char* product_key,
        const char* device_name,
        rrpc_request_callback rrpc_callback);


/**
 * @brief Response RRPC request
 *        This function    used to response the RRPC request.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param The message id of RRPC request.
 * @param Response data.
 *
 * @return 0, Response success; -1, Response fail.
 */
int IOT_Thing_RRPC_Response(void* handle,
        const char* product_key,
        const char* device_name,
        const char* message_id,
        const char* response);


/**
 * @brief trigger event
 *        This function    used to trigger an event.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param event parameter.
 * @param event id.
 *
 * @return 0, trigger success; -1, trigger fail.
 */
int IOT_Thing_Trigger_Event(void* handle,
        const char* product_key,
        const char* device_name,
        const char* params,
        const char* event_id);


/**
 * @brief publish a raw data
 *        This function    used to publish a raw data.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param product key.
 * @param device name.
 * @param raw data.
 * @param raw data length.
 *
 * @return 0, publish success; -1, publish fail.
 */
int IOT_Thing_Publish_Rawdata(void* handle,
        const char* product_key,
        const char* device_name,
        const char* raw_data,
        uint32_t raw_data_length);


/**
 * @brief yield
 *        This function    used to receive data.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param timeout.
 *
 * @return 0, recieve success; -1, recieve fail. */
int IOT_Thing_Yield(void* handle, uint32_t timeout);


/**
 * @brief subscribe
 *        This function    used to subscribe a topic.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param topic.
 * @param qos.
 * @param received data callback.
 * @param callback user data.
 *
 * @return 0, subscribe success; -1, subscribe fail.
 */
int IOT_Thing_Subscribe(void* handle,
        const char *topic_filter,
        int qos,
        iotx_subdev_event_handle_func_fpt topic_handle_func,
        void *pcontext);


/**
 * @brief unsubscribe
 *        This function    used to unsubscribe a topic.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param topic.
 *
 * @return 0, unsubscribe success; -1, unsubscribe fail.
 */
int IOT_Thing_Unsubscribe(void* handle,
        const char *topic_filter);


/**
 * @brief unsubscribe
 *        This function    used to publish a mqtt packet.
 *
 * @param pointer of handle, specify the Thing construction.
 * @param topic.
 * @param mqtt message.
 *
 * @return 0, publish success; -1, publish fail.
 */
int IOT_Thing_Publish(void* handle,
        const char *topic_name,
        iotx_mqtt_topic_info_pt topic_msg);
/* The structure of common reply data */
typedef struct iotx_thing_common_reply_data_st{
    int32_t                         id;
    uint32_t                        code;
    char                            topic[GATEWAY_TOPIC_LEN_MAX];
    char*                           data;                   /* the data size is dynamic, so data is a ptr, dynamic malloc buffer */
}iotx_thing_common_reply_data_t,*iotx_thing_common_reply_data_pt;


/* The structure of thing data */
typedef struct iotx_thing_masterlave_data_st {
    uint32_t                        sync_status;
    iotx_thing_common_reply_data_t    replys[IOTX_Thing_REPLY_MAX];
} iotx_thing_masterlave_data_t, *iotx_thing_masterlave_data_pt;


/* The structure of thing context */
typedef struct iotx_thing_masterlave_st {
    void*                           gateway;
    iotx_thing_masterlave_data_pt     thing_data_t;
    service_request_callback        service_callback;
    down_raw_callback               raw_callback;
    thing_control_callback          thing_callback;
} iotx_thing_masterlave_t, *iotx_thing_masterlave_pt;

extern iotx_thing_masterlave_pt g_thing_masterlave_t;


#define PARAMETER_Thing_CHECK(thing_t) \
    do { \
        if ((thing_t) == NULL) { \
            printf("param error"); \
            return FAIL_RETURN; \
        } \
        if ((thing_t) != g_thing_masterlave_t) { \
            printf("param error"); \
            return FAIL_RETURN; \
        } \
        if ((thing_t)->gateway == NULL) { \
            printf("param error"); \
            return FAIL_RETURN; \
        } \
        if ((thing_t)->thing_data_t == NULL) { \
            printf("param error"); \
            return FAIL_RETURN; \
        } \
    } while(0)


#define PARAMETER_NULL_CHECK_WITH_RESULT(param, result) \
    do { \
        if ((param) == NULL) { \
            printf("param error"); \
            return (result); \
        } \
    } while(0)

#define PARAMETER_STRING_NULL_CHECK_WITH_RESULT(ptr, result) \
    do { \
        if (NULL == (ptr)) { \
            printf("Invalid argument, %s = %p", #ptr, (ptr)); \
            return (result); \
        } \
        if (0 == strlen((ptr))) { \
            printf("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return (result); \
        } \
    } while(0)


#define MALLOC_MEMORY_WITH_FREE_AND_RESULT(buffer, length, free_buffer, result) \
    do { \
        if (buffer) \
            LITE_free(buffer); \
        (buffer) = (void*)LITE_malloc(length); \
        if (NULL == (buffer)) { \
            printf("Not enough memory"); \
            LITE_free(free_buffer); \
            return (result); \
        } \
        memset((buffer), 0x0, (length)); \
    } while(0)

#define PARAMETER_NULL_CHECK(param) \
    do { \
        if ((param) == NULL) { \
            printf("param error"); \
            return; \
        } \
    } while(0)

typedef enum IOTX_Thing_SUBSCRIBE_UNSUBSCRIBE_TYPES {
    IOTX_Thing_SUBSCRIBE_TYPE,
    IOTX_Thing_UNSUBSCRIBE_TYPE
}iotx_thing_subcribe_unsubscribe_types_t;

char* iotx_thing_splice_common_event_packet(
        const char* params,
        const char* event_id,
        int32_t* msg_id);

char* iotx_thing_splice_common_deviceinfo_packet(const char* deviceinfo,
        const char* method,
        int32_t* msg_id);

char *iotx_thing_splice_default_reply_get_packet(int32_t msg_id,
        uint32_t code,
        const char* data);

char *iotx_thing_splice_default_reply_packet(int32_t msg_id, uint32_t code);

char *iotx_thing_splice_topo_add_packet(const char* product_key,
        const char* device_name,
        const char* sign,
        const char* sign_method,
        const char* timestamp,
        const char* client_id,
        int32_t* msg_id);


char* iotx_thing_splice_common_packet(const char* product_key,
        const char* device_name,
        int32_t* msg_id,
        const char* param1,
        const char* param2,
        int flag);

int iotx_thing_publish_common_packet(void* handle,
        const char* topic_product_key,
        const char* topic_device_name,
        const char* packet_product_key,
        const char* packet_device_name,
        const char* param1,
        const char* param2,
        const char* param3,
        iotx_thing_reply_type_t reply_type);

int iotx_thing_publish_common_event_packet(void* handle,
        const char* product_key,
        const char* device_name,
        const char* topic_params,
        const char* packet_params,
        iotx_thing_reply_type_t reply_type);

int iotx_thing_publish_common_deviceinfo_packet(void* handle,
        const char* product_key,
        const char* device_name,
        const char* deviceinfo,
        const char* param1,
        const char* param2,
        iotx_thing_reply_type_t reply_type);

int iotx_thing_publish_topic_sync(iotx_thing_masterlave_pt thing_t,
        int32_t msg_id,
        const char* topic,
        iotx_mqtt_topic_info_pt topic_msg,
        iotx_thing_reply_type_t reply);

int iotx_thing_subscribe_unsubscribe_topic(iotx_thing_masterlave_pt thing_t,
        const char* topic,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe);

int iotx_thing_subscribe_unsubscribe_basic(iotx_thing_masterlave_pt thing_t,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe);

int iotx_thing_subscribe_unsubscribe_enhance(iotx_thing_masterlave_pt thing_t,
        const char* product_key,
        const char* device_name,
        iotx_thing_subcribe_unsubscribe_types_t is_subscribe);

void iotx_thing_splice_device_cloud_id(char* device_cloud_id,
        const char* product_key,
        const char* device_name);

#endif /* SRC_SUBDEVICE_Thing_UTIL_H_ */
