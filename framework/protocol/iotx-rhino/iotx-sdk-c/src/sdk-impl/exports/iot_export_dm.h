#ifndef _IOT_EXPORT_DM_H_
#define _IOT_EXPORT_DM_H_

#include "iot_import_product.h"
#ifndef _IN_
#define _IN_
#endif

#ifndef _OU_
#define _OU_
#endif

#if defined (CONFIG_DM_DEVTYPE_SINGLE)
#define IOTX_DM_DEVICE_TYPE IOTX_DM_DEVICE_SINGLE
#elif defined (CONFIG_DM_DEVTYPE_GATEWAY)
#define	IOTX_DM_DEVICE_TYPE IOTX_DM_DEVICE_GATEWAY
#else
#error "You Must Select Device Type!"
#endif

#define IOTX_DMGR_LOCAL_NODE_DEVID (0)

#define IOTX_DM_DEVICE_SINGLE  (0x01)
#define IOTX_DM_DEVICE_SUBDEV  (0x02)
#define IOTX_DM_DEVICE_GATEWAY (0x04)
#define IOTX_DM_DEVICE_ALL     (IOTX_DM_DEVICE_SINGLE|IOTX_DM_DEVICE_SUBDEV|IOTX_DM_DEVICE_GATEWAY)

/* Service Type 0~7bit: type, 8~15bit: extended*/
#define IOTX_DM_SERVICE_CLOUD         (0x0001)
#define IOTX_DM_SERVICE_LOCAL         (0x0002)
#define IOTX_DM_SERVICE_LOCAL_NO_AUTH (0x0000)
#define IOTX_DM_SERVICE_LOCAL_AUTH    (0x0100)

#define IOTX_DM_LOCAL_AUTH            (IOTX_DM_SERVICE_LOCAL|IOTX_DM_SERVICE_LOCAL_AUTH)
#define IOTX_DM_LOCAL_NO_AUTH         (IOTX_DM_SERVICE_LOCAL|IOTX_DM_SERVICE_LOCAL_NO_AUTH)

#define IOTX_DM_SERVICE_ALL           (IOTX_DM_SERVICE_CLOUD|IOTX_DM_LOCAL_AUTH)

#define IOTX_DM_DIPC_MSGLIST_MAXLEN   (100)

typedef enum {
	IOTX_DM_ERR_CODE_SUCCESS              = 200,
	IOTX_DM_ERR_CODE_REQUEST_ERROR        = 400,
	IOTX_DM_ERR_CODE_REQUEST_PARAMS_ERROR = 460,
	IOTX_DM_ERR_CODE_REQUEST_TOO_MANY     = 429,
	IOTX_DM_ERR_CODE_TIMEOUT              = 100000
}iotx_dm_error_code_t;

typedef enum {
	IOTX_DM_EVENT_CLOUD_CONNECTED  = 0,
	IOTX_DM_EVENT_CLOUD_DISCONNECT,
	IOTX_DM_EVENT_CLOUD_RECONNECT,
	IOTX_DM_EVENT_LOCAL_CONNECTED,
	IOTX_DM_EVENT_LOCAL_DISCONNECT,
	IOTX_DM_EVENT_LOCAL_RECONNECT,
	IOTX_DM_EVENT_FOUND_DEVICE,
	IOTX_DM_EVENT_REMOVE_DEVICE,
	IOTX_DM_EVENT_REGISTER_RESULT,
	IOTX_DM_EVENT_UNREGISTER_RESULT,
	IOTX_DM_EVENT_REGISTER_COMPLETED,
	IOTX_DM_EVENT_SEND_RESULT,
	IOTX_DM_EVENT_ADD_SERVICE_RESULT,
	IOTX_DM_EVENT_REMOVE_SERVICE_RESULT,
	IOTX_DM_EVENT_NEW_DATA_RECEIVED,
	IOTX_DM_EVENT_PROPERTY_SET,
	IOTX_DM_EVENT_TOPO_ADD_NOTIFY,
	IOTX_DM_EVENT_THING_SERVICE_REQUEST,
	IOTX_DM_EVENT_THING_DISABLE,
	IOTX_DM_EVENT_THING_ENABLE,
	IOTX_DM_EVENT_THING_DELETE,
	IOTX_DM_EVENT_MODEL_DOWN_RAW,
	IOTX_DM_EVENT_GATEWAY_PERMIT,
	IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY,
	IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY,
	IOTX_DM_EVENT_TOPO_ADD_REPLY,
	IOTX_DM_EVENT_TOPO_DELETE_REPLY,
	IOTX_DM_EVENT_TOPO_GET_REPLY,
	IOTX_DM_EVENT_TOPO_ADD_NOTIFY_REPLY,
	IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY,
	IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY,
	IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY,
	IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY,
	IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY,
	IOTX_DM_EVENT_COMBINE_LOGIN_REPLY,
	IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY,
	IOTX_DM_EVENT_MODEL_UP_RAW_REPLY,
	IOTX_DM_EVENT_LEGACY_THING_CREATED,
	IOTX_DM_EVENT_MAX
}iotx_dm_event_types_t;

typedef void (*iotx_dm_event_callback)(iotx_dm_event_types_t type, char *payload);

typedef enum {
    IOTX_DM_DEVICE_SECRET_PRODUCT,
    IOTX_DM_DEVICE_SECRET_DEVICE,
    IOTX_DM_DEVICE_SECRET_TYPES_MAX
}iotx_dm_device_secret_types_t;

typedef enum {
	IOTX_DM_CLOUD_DOMAIN_SHANGHAI,
    IOTX_DM_CLOUD_DOMAIN_SINGAPORE,
    IOTX_DM_CLOUD_DOMAIN_MAX
}iotx_dm_cloud_domain_types_t;

typedef enum {
    IOTX_DM_MESSAGE_NO_AUTH,
    IOTX_DM_MESSAGE_AUTH,
    IOTX_DM_MESSAGE_AUTH_MAX
}iotx_dm_message_auth_types_t;

typedef enum {
	IOTX_DM_TSL_SOURCE_LOCAL,
	IOTX_DM_TSL_SOURCE_CLOUD
}iotx_dm_tsl_source_t;

typedef enum {
	IOTX_DM_TSL_TYPE_ALINK,
	IOTX_DM_TSL_TYPE_TLV
}iotx_dm_tsl_type_t;

typedef struct {
	iotx_dm_device_secret_types_t secret_type;
	iotx_dm_cloud_domain_types_t domain_type;
	iotx_dm_event_callback event_callback;
}iotx_dm_init_params_t;

typedef enum {
	IOTX_DMGR_DEV_AVAIL_ENABLE,
	IOTX_DMGR_DEV_AVAIL_DISABLE
}iotx_dm_dev_avail_t;

typedef enum {
	IOTX_DMGR_DEV_STATUS_UNAUTHORIZED,   /* Subdev Created */
	IOTX_DMGR_DEV_STATUS_AUTHORIZED,     /* Receive Topo Add Notify */
	IOTX_DMGR_DEV_STATUS_REGISTERED,     /* Receive Subdev Registered */
	IOTX_DMGR_DEV_STATUS_ATTACHED,       /* Receive Subdev Topo Add Reply */
	IOTX_DMGR_DEV_STATUS_LOGINED,        /* Receive Subdev Login Reply */
	IOTX_DMGR_DEV_STATUS_ONLINE          /* After All Topic Subscribed */
}iotx_dm_dev_status_t;


#define IOTX_DM_POST_PROPERTY_ALL (NULL)

int IOT_DM_Construct(_IN_ iotx_dm_init_params_t *init_params);
int IOT_DM_Destroy(void);
int IOT_DM_Set_TSL(_IN_ int devid, _IN_ iotx_dm_tsl_source_t source, _IN_ const char *tsl, _IN_ int tsl_len);
int IOT_DM_Set_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);
int IOT_DM_Get_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int IOT_DM_Set_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);
int IOT_DM_Get_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int IOT_DM_Get_Service_Input_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int IOT_DM_Set_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value, _IN_ int value_len);
int IOT_DM_Get_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

int IOT_DM_Post_Property_Start(_IN_ int devid, _OU_ void **handle);
int IOT_DM_Post_Property_Add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len);
int IOT_DM_Post_Property_End(_IN_ void **handle);
int IOT_DM_Post_Event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len);

int IOT_DM_Post_Property_Direct(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int IOT_DM_Post_Event_Direct(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);
int IOT_DM_Send_Service_Response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len);
int IOT_DM_Post_Rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);

int IOT_DM_DeviceInfo_Update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int IOT_DM_DeviceInfo_Delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int IOT_DM_Yield(int timeout_ms);
void IOT_DM_Dispatch(void);
int IOT_DM_Subdev_Create(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid);
int IOT_DM_Subdev_Destroy(_IN_ int devid);
int IOT_DM_Subdev_Number(void);
int IOT_DM_Subdev_Register(_IN_ int devid);
int IOT_DM_Subdev_Unregister(_IN_ int devid);
int IOT_DM_Subdev_Topo_Add(_IN_ int devid);
int IOT_DM_Subdev_Topo_Del(_IN_ int devid);
int IOT_DM_Subdev_Login(_IN_ int devid);
int IOT_DM_Subdev_Logout(_IN_ int devid);

int IOT_DM_Get_Device_Type(_IN_ int devid, _OU_ int *type);
int IOT_DM_Get_Device_Avail_Status(_IN_ int devid, _OU_ iotx_dm_dev_avail_t *status);
int IOT_DM_Get_Device_Status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status);

int IOT_DM_Legacy_Set_Property_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int IOT_DM_Legacy_Set_Event_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int IOT_DM_Legacy_Set_Service_Output_Value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

int IOT_DM_Legacy_Get_Pkdn_By_Devid(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN], _OU_ char device_name[DEVICE_NAME_MAXLEN]);
int IOT_DM_Legacy_Get_Devid_By_Pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid);
int IOT_DM_Legacy_Get_ThingId_By_Devid(_IN_ int devid, _OU_ void **thing_id);
int IOT_DM_Legacy_Get_Devid_By_ThingId(_IN_ void *thing_id, _OU_ int *devid);
int IOT_DM_Legacy_Get_Pkdn_Ptr_By_Devid(_IN_ int devid, _OU_ char **product_key, _OU_ char **device_name);
int IOT_DM_Legacy_Send_Service_Response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);
int IOT_DM_Legacy_Send_Rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);

#endif
