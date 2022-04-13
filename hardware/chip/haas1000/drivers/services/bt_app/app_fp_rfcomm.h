#ifndef __APP_FP_RFCOMM_H__
#define __APP_FP_RFCOMM_H__

#define FP_MESSAGE_RESERVED_LEN     4   // at least 4 bytes
// TODO: increase this value if needed
#define FP_MESSAGE_STREAM_MAX_ADDITIONAL_DATA_LEN   16
typedef struct
{
    uint8_t messageGroup;
    uint8_t messageCode;
    uint8_t dataLenHighByte;
    uint8_t dataLenLowByte;
    uint8_t data[FP_MESSAGE_STREAM_MAX_ADDITIONAL_DATA_LEN];
}  __attribute__((packed)) FP_MESSAGE_STREAM_T;

// The values is posted at FP spec 8/27/19 revision
#define FP_MSG_GROUP_BLUETOOTH_EVENT                0x01
#define FP_MSG_BT_EVENT_ENABLE_SILENCE_MODE         0x01
#define FP_MSG_BT_EVENT_DISABLE_SILENCE_MODE        0x02

#define FP_MSG_GROUP_COMPANION_APP_EVENT            0x02
#define FP_MSG_COMPANION_APP_LOG_BUF_FULL           0x01

#define FP_MSG_GROUP_DEVICE_INFO                    0x03
#define FP_MSG_DEVICE_INFO_MODEL_ID                 0x01
#define FP_MSG_DEVICE_INFO_BLE_ADD_UPDATED          0x02
#define FP_MSG_DEVICE_INFO_BATTERY_UPDATED          0x03
#define FP_MSG_DEVICE_INFO_REMAINING_BATTERY_TIME   0x04
#define FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_REQ    0x05
#define FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_RSP    0x06
#define FP_MSG_DEVICE_INFO_TELL_CAPABILITIES        0x07
#define FP_MSG_DEVICE_INFO_PLATFORM_TYPE            0x08

#define FP_MSG_GROUP_DEVICE_ACTION                  0x04
#define FP_MSG_DEVICE_ACTION_RING                   0x01

#define FP_MSG_NEITHER_BUD_ACTIVE                   0x00
#define FP_MSG_RIGHT_BUD_ACTIVE                     0x01
#define FP_MSG_LEFT_BUD_ACTIVE                      0x02
#define FP_MSG_BOTH_BUDS_ACTIVE                     0x03

#define FP_MSG_GROUP_ACKNOWLEDGEMENT                0xFF
#define FP_MSG_ACK                                  0x01
#define FP_MSG_NAK                                  0x02

#define FP_MSG_NAK_REASON_NOT_SUPPORTED             0x00
#define FP_MSG_NAK_REASON_DEVICE_BUSY               0x01
#define FP_MSG_NAK_REASON_NOT_ALLOWED               0x02

#ifdef __cplusplus
extern "C" {
#endif

void app_fp_rfcomm_send(uint8_t *ptrData, uint32_t length);
bt_status_t app_fp_rfcomm_init(void);
void app_fp_disconnect_rfcomm(void);
bool app_is_fp_rfcomm_connected(void);

void app_fp_msg_enable_bt_silence_mode(bool isEnable);
void app_fp_msg_send_model_id(void);
void app_fp_msg_send_updated_ble_addr(void);
void app_fp_msg_send_battery_levels(void);

#ifdef __cplusplus
}
#endif

#endif  // __APP_FP_RFCOMM_H__
