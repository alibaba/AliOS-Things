#ifndef _APP_TMALL_GATT_H_
#define _APP_TMALL_GATT_H_

#include "rwip_config.h"     // SW configuration
#if (BLE_APP_TMALL_GATT)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

#define TMALL_GATT_IDX_MAX        (BLE_CONNECTION_MAX)

#define TMALL_GATT_ADV_LENGTH       (16)
#define TMALL_GATT_ADV_DATA_LENGTH  (0x0f)
#define TMALL_GATT_ADV_DATA_TYPE    (0xff)
#define TMALL_GATT_ADV_DATA_CID     (0x01a8)
#define TMALL_GATT_ADV_DATA_VID     (0x0a)
#define TMALL_GATT_ADV_DATA_FMSK    (0x03)
#define TMALL_GATT_ADV_DATA_PID     (0x00ef1000)
#define TMALL_GATT_ADV_DATA_MAC     (0xb0b448d07882)

typedef struct
{
    uint8_t     tmall_gatt_adv_lenth;
    uint8_t     tmall_gatt_adv_type;
    uint16_t    tmall_gatt_adv_cid;
    uint8_t     tmall_gatt_adv_vid;
    uint8_t     tmall_gatt_adv_fmsk;
    uint32_t    tmall_gatt_adv_pid;
    uint64_t    tmall_gatt_adv_mac;
}__attribute__ ((__packed__)) TMALL_GATT_ADV_DATA_PAC;


extern const TMALL_GATT_ADV_DATA_PAC tmall_gatt_adv_data;

struct app_tmall_gatt_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isNotificationEnabled;
	uint16_t mtu;
};

struct ble_tmall_gatt_tx_notif_config_t
{
	bool    isNotificationEnabled;
};

struct ble_tmall_gatt_rx_data_ind_t
{
	uint16_t    length;
	uint8_t     data[0];
};

struct ble_tmall_gatt_tx_sent_ind_t
{
	uint8_t    status;
};


struct ble_tmall_gatt_send_data_req_t
{
	uint8_t     connecionIndex;
	uint32_t    length;
	uint8_t     value[__ARRAY_EMPTY];
};


typedef void(*app_tmall_gatt_tx_done_t)(void);

void app_tmall_gatt_add(void);
extern const struct ke_state_handler app_tmall_gatt_table_handler;
void app_tmall_gatt_init(void);
void app_tmall_gatt_send_notification(uint8_t* ptrData, uint32_t length);
void app_tmall_gatt_send_indication(uint8_t* ptrData, uint32_t length);
void app_tmall_gatt_mtu_exchanged_handler(uint8_t conidx, uint16_t mtu);

#endif
#endif