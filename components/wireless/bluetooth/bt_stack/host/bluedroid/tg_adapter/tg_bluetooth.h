/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bluetooth.h
 * @brief:   vendor bluetooth apis defination
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2019/12/15
 * @version: 1.0
 */


#ifndef _TG_VENDOR_BLUETOOTH_H_
#define _TG_VENDOR_BLUETOOTH_H_

#include <stdint.h>
#include <stdbool.h>
#if __cplusplus
extern "C" {
#endif
#define TG_BT_OK                (0)
#define TG_BT_FAIL              (-1) /* abnormal return must < 0 */
#define TG_BDADDR_MAX_LEN       (18)
#define TG_NAME_MAX_LEN         (96)

#define UINT8       uint8_t
#define UINT16      uint16_t
#define UINT32      uint32_t
#define INT8        int8_t
#define INT16       int16_t
#define INT32       int32_t
#define CHAR        char
#define BOOL        bool

typedef struct _TG_BT_DEV_INFO
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];    /* Bluetooth Address */
    int8_t name[TG_NAME_MAX_LEN];        /* Name of device */
} TG_BT_DEV_INFO;

typedef enum
{
    TG_BT_UNPAIR_FAIL,
    TG_BT_UNPAIR_SUCCESS
} TG_BT_UNPAIR_RESULT;

typedef enum
{
    TG_BT_PAIR_FAIL,
    TG_BT_PAIR_SUCCESS,
    TG_BT_PAIR_TIMEOUT
} TG_BT_PAIR_RESULT;

#define TG_MAX_BT_DEV_NUM 10
typedef enum
{
    TG_BT_A2DP_CONNECTED,
    TG_BT_A2DP_DISCONNECTED,
    TG_BT_A2DP_CONNECTING,
    TG_BT_A2DP_LINK_LOSS,
    TG_BT_A2DP_CONNECT_TIMEOUT,
    TG_BT_A2DP_EVENT_MAX
} TG_BT_A2DP_STATE;

typedef enum
{
    TG_BT_A2DP_ROLE_SINK,
    TG_BT_A2DP_ROLE_SRC,
    TG_BT_A2DP_ROLE_MAX
} TG_BT_A2DP_ROLE;

typedef enum
{
    TG_BT_A2DP_STREAM_STATE_STOP,
    TG_BT_A2DP_STREAM_STATE_PLAYING,
    TG_BT_A2DP_STREAM_STATE_MAX
} TG_BT_A2DP_STREAM_STATE;

typedef struct
{
    uint32_t head_idx;
    uint32_t dev_num;
    TG_BT_DEV_INFO device_list[TG_MAX_BT_DEV_NUM];
} TG_BT_A2DP_DEV_INFO_LIST;

typedef enum
{
    TG_BT_AVRCP_DISCONNECTED,
    TG_BT_AVRCP_CONNECTED
} TG_BT_AVRCP_STATUS;

typedef enum
{
    TG_BT_AVRCP_PLAY_STATUS_STOPPED,
    TG_BT_AVRCP_PLAY_STATUS_PLAYING,
    TG_BT_AVRCP_PLAY_STATUS_PAUSEED,
    TG_BT_AVRCP_PLAY_STATUS_MAX
} TG_BT_AVRCP_PLAY_STATUS;

typedef enum
{
    TG_BT_AVRCP_VOLUME_UP,
    TG_BT_AVRCP_VOLUME_DOWN
} TG_BT_AVRCP_VOLUME_CHANGE;

typedef enum
{
    TG_BT_AVRCP_CMD_TYPE_PLAY = 0,
    TG_BT_AVRCP_CMD_TYPE_PAUSE,
    TG_BT_AVRCP_CMD_TYPE_FWD,
    TG_BT_AVRCP_CMD_TYPE_BWD,
    TG_BT_AVRCP_CMD_TYPE_FFWD,
    TG_BT_AVRCP_CMD_TYPE_RWD,
    TG_BT_AVRCP_CMD_TYPE_STOP,
    TG_BT_AVRCP_CMD_TYPE_VOL_UP,
    TG_BT_AVRCP_CMD_TYPE_VOL_DOWN,
    TG_BT_AVRCP_CMD_TYPE_MAX
} TG_BT_AVRCP_CMD_TYPE;

typedef void(* tg_bt_gapPairCB)(int32_t result, int8_t *address);
typedef void(* tg_bt_gapUnpairCB)(int32_t result, int8_t *address);
typedef void(* tg_bt_gapScanCB)(const int8_t *name, const int8_t *address);

typedef void(* tg_bt_a2dpStateCB)(uint8_t state, int8_t *address);
typedef void(* tg_bt_a2dpStreamCB)(uint8_t state);

typedef void(* tg_bt_avrcpStateCB)(uint8_t state);
typedef void(* tg_bt_avrcpPlayStateCB)(uint8_t state);
typedef void(* tg_bt_avrcpVolumeChangeCB)(uint8_t direction);
typedef void(* tg_bt_avrcpAbsoluteVolumeCB)(uint32_t vol);
typedef void(* tg_bt_avrcpCmdSrcCB)(uint8_t avrcp_cmd);

typedef struct
{
    tg_bt_gapPairCB gapPairCB;
    tg_bt_gapUnpairCB gapUnpairCB;
    tg_bt_gapScanCB gapGapScanCB;
} TG_BT_GAP_IMPL_CB_FUNC_T;

typedef struct
{
    tg_bt_a2dpStateCB a2dpStateCB;
    tg_bt_a2dpStreamCB a2dpStreamCB;
} TG_BT_A2DP_IMPL_CB_FUNC_T;

typedef struct
{
    tg_bt_avrcpStateCB avrcpStateCB;
    tg_bt_avrcpPlayStateCB avrcpPlayStateCB;
    tg_bt_avrcpVolumeChangeCB volumeChangeCB;
    tg_bt_avrcpAbsoluteVolumeCB absoluteVolumeCB;
    tg_bt_avrcpCmdSrcCB avrcpCmdSrcCB;
} TG_BT_AVRCP_IMPL_CB_FUNC_T;

/**
 * @brief bluetooth set or get function
 * @param[in] set:  1 means set, 0 means get.
 * @param[in] en:  1 means enable, 0 means disable.
 * @return: None
 */
bool tg_bt_set_get_enable(bool set, bool en);

/**
 * @brief bluetooth enable function
 * @param[in] enable:  1 means enable, 0 means disable.
 * @return: None
 */
void tg_bt_enable(int8_t enable);

/* GAP */
/**
 * @brief gap register callback functions
 * @param[in] func:  callback function set.
 * @return: None
 */
void tg_bt_gapCallbackRegister(TG_BT_GAP_IMPL_CB_FUNC_T *func);

/**
 * @brief gap on off bluetooth
 * @param[in] fg_on:  on/off flag, 1 means on; 0 means off.
 *            len:  random length
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gapSetOnOff(bool fg_on);

/**
 * @brief gap get local device info
 * @param[in] local_dev_info:  local device info.
 * @return: None
 */
void tg_bt_gapGetLocalDevInfo(TG_BT_DEV_INFO *local_dev_info);

/**
 * @brief gap get remote device info
 * @param[in] remote_dev_info:  remote device info.
 * @return: None
 */
void tg_bt_gapGetRemoteDevInfo(TG_BT_DEV_INFO *remote_dev_info);

/**
 * @brief gap set scan mode
 * @param[in] conn_flag:  page scan mode, 1 means on, 0 means off.
 * @param[in] disc_flag:  inquiry scan mode, 1 means on, 0 means off.
 * @return: None
 */
void tg_bt_gapSetScanMode(bool conn_flag, bool disc_flag);

/**
 * @brief gap set bluetooth device name
 * @param[in] set_name:  string of bluetooth device name.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gapSetName(const int8_t *set_name);

/**
 * @brief gap start inquiry
 * @param: None
 * @return: None
 */
void tg_bt_startInquiry(void);

/**
 * @brief gap stop inquiry
 * @param: None
 * @return: None
 */ 
void tg_bt_stopInquiry(void);

/**
 * @brief gap unpair device
 * @param[in] mac:  buffer to store random value.
 * @return: None
 */
void tg_bt_gapUnPair(const int8_t *mac);

/* A2DP */
/**
 * @brief a2dp register callback functions set
 * @param[in] func:  a2dp callback functions set.
 * @return: None
 */
void tg_bt_a2dpCallbackRegister(TG_BT_A2DP_IMPL_CB_FUNC_T *func);

/**
 * @brief a2dp connect 
 * @param[in] mac:  bluetooth address need connect.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: None
 */
void tg_bt_a2dpConnect(const int8_t *mac, TG_BT_A2DP_ROLE role);

/**
 * @brief a2dp disconnect 
 * @param[in] mac:  bluetooth address need disconnect.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: None
 */
void tg_bt_a2dpDisconnect(const int8_t *mac, TG_BT_A2DP_ROLE role);

/**
 * @brief local a2dp role set 
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_a2dpSetRole(TG_BT_A2DP_ROLE role);

/**
 * @brief local a2dp role get 
 * @param[in]: None
 * @return: 0 means sink; 1 means source.
 */
TG_BT_A2DP_ROLE tg_bt_a2dpGetRole(void);

/**
 * @brief a2dp paired device list get
 * @param[out] info: paired device info list.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: Zero on success or error code otherwise
 */
void tg_bt_getA2dpPairedList(TG_BT_A2DP_DEV_INFO_LIST *info, TG_BT_A2DP_ROLE role);

/**
 * @brief send a2dp command dependently for BQB Test
 * @param[in] mac: bluetooth address.
 * @param[in] signal_id: avdtp command id which want send out.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_a2dpSendCmd(const int8_t *mac, uint8_t signal_id);

/**
 * @brief start a2dp player
 * @param: None
 * @return: None
 */
void tg_bt_a2dpStartPlayer(void);

/**
 * @brief stop a2dp player
 * @param: None
 * @return: None
 */
void tg_bt_a2dpStopPlayer(void);

/* AVRCP */
/**
 * @brief avrcp register callback functions set
 * @param[in] func:  avrcp callback functions set.
 * @return: None
 */
void tg_bt_avrcpCallbackRegister(TG_BT_AVRCP_IMPL_CB_FUNC_T *func);

/**
 * @brief avrcp connect 
 * @param[in] mac:  bluetooth address need connect.
 * @return: None
 */
void tg_bt_avrcpConnect(const int8_t *mac);

/**
 * @brief avrcp disconnect 
 * @param[in]:  None
 * @return: None
 */
void tg_bt_avrcpDisconnect(void);

/**
 * @brief avrcp send passthrough command 
 * @param[in] cmd_type:  passthrough command type.
 * @return: None
 */
void tg_bt_avrcpSendPassthroughCmd(TG_BT_AVRCP_CMD_TYPE cmd_type);

/**
 * @brief avrcp volume change notification 
 * @param[in] vol:  current volume, range 0-0x7f
 * @param[in] role:  a2dp role, 0 means sink; 1 means source
 * @return: None
 */
int32_t tg_bt_avrcpChangeVolume(uint8_t vol, TG_BT_A2DP_ROLE role);

/**
 * @brief avrcp get current play status 
 * @param[in] mac:  bluetooth address.
 * @return: None
 */
int32_t tg_bt_avrcpGetPlayStatus(const int8_t *mac);

/**
 * @brief avrcp get element attribute tile 
 * @param[in] cmd_type:  bluetooth address.
 * @return: None
 */
int32_t tg_bt_avrcpGetElementAttrTitle(const int8_t *mac);


/* GATT Service */
#define TG_BT_GATT_MAX_ATTR_LEN     600
#define TG_BT_GATT_MAX_UUID_LEN     37
#define TG_BT_MAX_BDADDR_LEN        18
typedef enum
{
    TG_BT_GATTS_REGISTER_SERVER = 0,
    TG_BT_GATTS_CONNECT,
    TG_BT_GATTS_DISCONNECT,
    TG_BT_GATTS_GET_RSSI_DONE,

    TG_BT_GATTS_EVENT_MAX
} TG_BT_GATTS_EVENT_T;

/** GATT ID adding instance id tracking to the UUID */
typedef struct
{
    int8_t  uuid[TG_BT_GATT_MAX_UUID_LEN];
    uint8_t inst_id;
} TG_BT_GATT_ID_T;

/** GATT Service ID also identifies the service type (primary/secondary) */
typedef struct
{
    TG_BT_GATT_ID_T    id;
    uint8_t is_primary;
} TG_BT_GATTS_SRVC_ID_T;

typedef struct
{
    int32_t server_if;
    TG_BT_GATTS_SRVC_ID_T srvc_id;
    int32_t srvc_handle;
} TG_BT_GATTS_ADD_SRVC_RST_T;

typedef struct
{
    int32_t server_if;
    int8_t uuid[TG_BT_GATT_MAX_UUID_LEN];
    int32_t srvc_handle;
    int32_t char_handle;
} TG_BT_GATTS_ADD_CHAR_RST_T ;

typedef struct
{
    int32_t server_if;
    int8_t uuid[TG_BT_GATT_MAX_UUID_LEN];
    int32_t srvc_handle;
    int32_t descr_handle;
} TG_BT_GATTS_ADD_DESCR_RST_T;

typedef struct
{
    int32_t conn_id;
    int32_t trans_id;
    int8_t btaddr[TG_BT_MAX_BDADDR_LEN];
    int32_t attr_handle;
    int32_t offset;
    int32_t length;
    uint8_t need_rsp;
    uint8_t is_prep;
    uint8_t value[TG_BT_GATT_MAX_ATTR_LEN];
} TG_BT_GATTS_REQ_WRITE_RST_T;

typedef struct
{
    int32_t conn_id;
    int32_t trans_id;
    int8_t btaddr[TG_BT_MAX_BDADDR_LEN];
    int32_t attr_handle;
    int32_t offset;
    uint8_t is_long;
} TG_BT_GATTS_REQ_READ_RST_T;

typedef void (*tg_bt_gattsInitCB)(int32_t serverIf);
typedef void (*tg_bt_gattsAddServiceCB)(TG_BT_GATTS_ADD_SRVC_RST_T *bt_gatts_add_srvc);
typedef void (*tg_bt_gattsAddCharCB)(TG_BT_GATTS_ADD_CHAR_RST_T *bt_gatts_add_char);
typedef void (*tg_bt_gattsAddDescCB)(TG_BT_GATTS_ADD_DESCR_RST_T *bt_gatts_add_desc);
typedef void (*tg_bt_gattsReqWriteCB)(TG_BT_GATTS_REQ_WRITE_RST_T *bt_gatts_req_write);
typedef void (*tg_bt_gattsReqReadCB)(TG_BT_GATTS_REQ_READ_RST_T *bt_gatts_req_read);
typedef void (*tg_bt_gattsConnectionEventCB)(TG_BT_GATTS_EVENT_T bt_gatts_connection_evt, uint16_t conn_id);
typedef void (*tg_bt_gattsStartServerCB)(void);
typedef void (*tg_bt_gattsStopServerCB)(void);
typedef void (*tg_bt_gattsDeleteServerCB)(void);

typedef struct
{
    tg_bt_gattsInitCB gattsInitCB;
    tg_bt_gattsAddServiceCB gattsAddServiceCB;
    tg_bt_gattsAddCharCB gattsAddCharCB;
    tg_bt_gattsAddDescCB gattsAddDescCB;
    tg_bt_gattsReqWriteCB gattsReqWriteCB;
    tg_bt_gattsReqReadCB gattsReqReadCB;
    tg_bt_gattsConnectionEventCB gattsConnectionEventCB;
    tg_bt_gattsStartServerCB gattsStartServerCB;
    tg_bt_gattsStopServerCB gattsStopServerCB;
    tg_bt_gattsDeleteServerCB gattsDeleteServerCB;
} TG_BT_GATTS_CB_FUNC_T;

/**
 * @brief gatt service register callback set
 * @param[in] callback:  gatt service callback set.
 * @return: None
 */
void tg_bt_gattsSetCallback(TG_BT_GATTS_CB_FUNC_T *callback);

/**
 * @brief gatt service initial
 *        should callback gattsInitCB when finished
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_initGatts(void);

/**
 * @brief gatt service deinitial
 * @param[in]: None.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_deinitGatts(void);

/**
 * @brief gatt service get registed callback
 * @param[in]: None.
 * @return: callback set
 */
TG_BT_GATTS_CB_FUNC_T *tg_bt_gattsGetCallback(void);

/**
 * @brief gatt service add service
 *        should callback gattsAddServiceCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_uuid:  service UUID.
 * @param[in] is_primary:  1 means primary, 0 means included.
 * @param[in] number:  attribute handle number occupied by this service.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddService(int32_t server_if, int8_t *service_uuid, uint8_t is_primary, int32_t number);

/**
 * @brief gatt service add character
 *        should callback gattsAddCharCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle: service handle, returned in gattsAddServiceCB.
 * @param[in] uuid:  character UUID.
 * @param[in] properties:  gatt properties set.
 * @param[in] permissions:  gatt permission set.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddChar(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t properties,
                  int32_t permissions);

/**
 * @brief gatt service add character descriptor
 *        should callback gattsAddDescCB when finished
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @param[in] uuid:  descriptor UUID.
 * @param[in] permissions:  gatt permission set.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsAddDesc(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t permissions);

/**
 * @brief gatt service start service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @param[in] is_primary:  1 means primary, 0 means included.
 * @param[in] number:  attribute handle number occupied by this service.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsStartService(int32_t server_if, int32_t service_handle, int32_t transport);

/**
 * @brief gatt service stop service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsStopService(int32_t server_if, int32_t service_handle);

/**
 * @brief gatt service delete service
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] service_handle:  service handle, returned in gattsAddServiceCB.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gattsDeleteService(int32_t server_if, int32_t service_handle);

/**
 * @brief gatt unregister all services on this server interface 
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @return: None
 */
int32_t tg_bt_gattsUnregisterService(int32_t server_if);

/**
 * @brief gatt send reponse when received remote read/write att command
 * @param[in] conn_id:  bluetooth connection handle.
 * @param[in] trans_id:  bluetooth transaction id in 
 * @param[in] status:  reserved, should set to 0
 * @param[in] handle:  attribute handle which want response
 * @param[in] p_value:  attribute value which want response
 * @param[in] offset:  attribute value offset which want response
 * @param[in] value_len:  attribute value length which want response
 * @param[in] auth_req: reserved, should set to 0
 * @return: None
 */
int32_t tg_bt_gattsSendResponse(int32_t conn_id, int32_t trans_id, int32_t status, int32_t handle, int8_t *p_value,
                       int32_t offset, int32_t value_len, int32_t auth_req);

/**
 * @brief gatt send indication/notification to remote 
 * @param[in] server_if:  service interface id, returned in gattsInitCB.
 * @param[in] handle:  attribute handle which want indication
 * @param[in] conn_id:  bluetooth connection handle
 * @param[in] fg_confirm:  1 means indication, 0 means notification
 * @param[in] p_value:  attribute value which want indication
 * @param[in] value_len:  attribute value length which want indication
 * @return: None
 */
int32_t tg_bt_gattsSendIndication(int32_t server_if, int32_t handle, int32_t conn_id, int32_t fg_confirm,
                         int8_t *p_value, int32_t value_len);

/**
 * @brief gatt service set server interface
 * @param[in] serverIf: service interface id, returned in gattsInitCB.
 * @return: None
 */
void tg_bt_gattsSetServerIf(int32_t serverIf);

/**
 * @brief gatt service delete server interface 
 * @param[in] serverIf: service interface id, returned in gattsInitCB.
 * @return: None
 */
void tg_bt_gattsDelServerIf(int32_t serverIf);

/**
 * @brief gatt server advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: None
 */
int32_t tg_bt_gattsSetAdvData(uint8_t *data, int len);

/**
 * @brief gatt server enable/disable advertise broadcast 
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: None
 */
int32_t tg_bt_gattsEnableAdv(bool enable);


/* GATT Client */

#define GATT_MAX_UUID_LEN 37
typedef enum {
    TG_BT_GATT_EVENT_CONNECT,
    TG_BT_GATT_EVENT_DISCONNECT,
    TG_BT_GATT_EVENT_OPEN,
    TG_BT_GATT_EVENT_MTU_EXCHGED,
    TG_BT_GATT_EVENT_MAX
} TG_BT_gattcEvent;

typedef enum {
    TG_BT_GATT_DB_PRIMARY_SERVICE,
    TG_BT_GATT_DB_SECONDARY_SERVICE,
    TG_BT_GATT_DB_INCLUDED_SERVICE,
    TG_BT_GATT_DB_CHARACTERISTIC,
    TG_BT_GATT_DB_DESCRIPTOR,
} TG_BT_gattcType;

typedef enum {
    TG_BT_GATT_WRITE_NO_RESPONSE = 1,
    TG_BT_GATT_WRITE_WITH_RESPONSE = 2,
    TG_BT_GATT_WRITE_PREPARE = 3,
} TG_BT_gattcWriteType;

typedef struct {
    uint32_t id;
    int8_t uuid[GATT_MAX_UUID_LEN];
    TG_BT_gattcType type;
    uint32_t handle;
    /*
     * If |type| is |CSM_GATT_DB_PRIMARY_SERVICE|, or
     * |CSM_GATT_DB_SECONDARY_SERVICE|, this contains the start and end attribute
     * handles.
     */
    uint32_t start_handle;
    uint32_t end_handle;
    /*
     * If |type| is |CSM_GATT_DB_CHARACTERISTIC|, this contains the properties of
     * the characteristic.
     */
    uint8_t properties;
} TG_BT_gattcServiceElem;

typedef struct {
    int32_t connId;
    int32_t count;
    TG_BT_gattcServiceElem *pElem;
} TG_BT_gattcServices;

typedef void(*tg_bt_gattcInitedCB)(int32_t client_if);
typedef void(*tg_bt_gattcScanResultCB)(const int8_t *bdAddr, int32_t rssi, uint8_t *pData, int32_t size);
typedef void(*tg_bt_gattcGattEventCB)(int32_t connId, const int8_t *bdAddr, TG_BT_gattcEvent event, void* data);
typedef void(*tg_bt_gattcServiceDiscoveredCB)(uint16_t conn_id, uint32_t st);
typedef void(*tg_bt_gattcCharacteristicReadCB)(int32_t connId, uint32_t handle, int32_t status,
                                           const uint8_t *data, int32_t size);
typedef void(*tg_bt_gattcCharacteristicWriteCB)(int32_t connId, uint32_t handle, int32_t status);
typedef void(*tg_bt_gattcDescriptorReadCB)(int32_t connId, uint32_t handle, int32_t status,
                                       const uint8_t *data, int32_t size);
typedef void(*tg_bt_gattcCnDescriptorWriteCB)(int32_t connId, uint32_t handle, int32_t status);
typedef void(*tg_bt_gattcCCharacteristicChangedCB)(int32_t connId, uint32_t handle, const int8_t *bdaddr,
                                              const uint8_t *data, int32_t size, uint8_t isNotify);
typedef void(*tg_bt_gattcSetCharacteristicNotiCB)(int32_t connId, uint32_t handle, int32_t registered);
typedef void(*tg_bt_gattcAdvEnabledCB)(int32_t status);

typedef struct {
    tg_bt_gattcInitedCB gattcInitedCB;
    tg_bt_gattcScanResultCB gattcScanResultCB;
    tg_bt_gattcGattEventCB gattcGattEventCB;
    tg_bt_gattcServiceDiscoveredCB gattcServiceDiscoveredCB;
    tg_bt_gattcCharacteristicReadCB gattcCharacteristicReadCB;
    tg_bt_gattcCharacteristicWriteCB gattcCharacteristicWriteCB;
    tg_bt_gattcDescriptorReadCB gattcDescriptorReadCB;
    tg_bt_gattcCnDescriptorWriteCB gattcCnDescriptorWriteCB;
    tg_bt_gattcCCharacteristicChangedCB gattcCCharacteristicChangedCB;
    tg_bt_gattcSetCharacteristicNotiCB gattcSetCharacteristicNotiCB;
    tg_bt_gattcAdvEnabledCB gattcAdvEnabledCB;
} TG_BT_GATTC_CB_FUNC_T;

/**
 * @brief gatt client register callback set
 * @param[in] callback:  gatt client callback set.
 * @return: None
 */
void tg_bt_gattcSetCallback(TG_BT_GATTC_CB_FUNC_T *callback);

/**
 * @brief gatt client advertise broadcast packet data set
 * @param[in] data:  advertise data.
 * @param[in] len:  advertise data length.
 * @return: None
 */
int32_t tg_bt_gattcSetAdvData(uint8_t *data, int len);

/**
 * @brief gatt client enable/disable advertise broadcast 
 * @param[in] enable: 1 means enable; 0 means disable.
 * @return: None
 */
int32_t tg_bt_gattcEnableAdv(bool enable);

void yoc_ble_gap_open_gattc(uint8_t* remote_bda, int type);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_BLUETOOTH_H_ */
