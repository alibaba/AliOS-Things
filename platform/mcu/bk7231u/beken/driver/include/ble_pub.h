#ifndef _BLE_PUB_H_
#define _BLE_PUB_H_

#include "rtos_pub.h"
#if defined(SUPPORT_MIDEA_BLE)
#include "msm_ble_adapter.h"
#endif

#define BLE_SUCCESS			(0)
#define BLE_FAILURE			(1)
#define BLE_DEV_NAME		"ble"

#define BLE_CMD_MAGIC              (0xe2a0000)
#define MAX_ADV_DATA_LEN           (0x1F)

enum
{
    CMD_BLE_REG_INIT = BLE_CMD_MAGIC + 1,
	CMD_BLE_REG_DEINIT,
	CMD_BLE_SET_CHANNEL,
	CMD_BLE_AUTO_CHANNEL_ENABLE,
	CMD_BLE_AUTO_CHANNEL_DISABLE,
	CMD_BLE_AUTO_SYNCWD_ENABLE,
	CMD_BLE_AUTO_SYNCWD_DISABLE,
	CMD_BLE_SET_PN9_TRX,
	CMD_BLE_SET_GFSK_SYNCWD,
	CMD_BLE_HOLD_PN9_ESTIMATE,
	CMD_BLE_STOP_COUNTING,
	CMD_BLE_START_COUNTING
};

enum
{
    PN9_RX = 0,
    PN9_TX
};

enum
{
	BLE_MSG_POLL          = 0,
    BLE_MSG_SLEEP,    
    BLE_MSG_EXIT,
    BLE_MSG_NULL,
};

#define ABIT(n) (1 << n)

typedef enum
{
    ERR_SUCCESS = 0,
    ERR_STACK_FAIL,
    ERR_MEM_FAIL,
    ERR_INVALID_ADV_DATA,
    ERR_ADV_FAIL,
    ERR_STOP_ADV_FAIL,
    ERR_GATT_INDICATE_FAIL,
    ERR_GATT_NOTIFY_FAIL,
    ERR_SCAN_FAIL,
    ERR_STOP_SCAN_FAIL,
    ERR_CONN_FAIL,
    ERR_STOP_CONN_FAIL,
    ERR_DISCONN_FAIL,
    ERR_READ_FAIL,
    ERR_WRITE_FAIL,
    ERR_REQ_RF,
    /* Add more BLE error code hereafter */
} ble_err_t;

typedef enum
{
    ADV_NAME_SHORT = 0x8,
    ADV_NAME_FULL
} adv_name_type_t;

typedef enum
{
    AD_LIMITED  = ABIT(0), /* Limited Discoverable */
    AD_GENERAL  = ABIT(1), /* General Discoverable */
    AD_NO_BREDR = ABIT(2)  /* BR/EDR not supported */
} adv_flag_t;
typedef enum
{
    BLE_ROLE_NONE = 0,
    BLE_ROLE_MASTER,
    BLE_ROLE_SLAVE,
    BLE_ROLE_HYBRID, /* reserved */
} ble_role_t;

typedef struct
{
    uint8  advData[MAX_ADV_DATA_LEN];
    uint8  advDataLen;
    uint8  respData[MAX_ADV_DATA_LEN];
    uint8  respDataLen;
    uint8  channel_map;
    uint16 interval_min;
    uint16 interval_max;
    /* Subject to add more hereafter in the future */
} adv_info_t;

typedef struct
{
    uint8  filter_en;
    uint8  channel_map;
    uint16 interval;
    uint16 window;
} scan_info_t;

typedef struct rf_time_s
{
    UINT32 ble_rf_time;
    UINT32 wifi_rf_time;
} rf_time_t;

#if defined(SUPPORT_MIDEA_BLE)
extern msm_ble_adv_content_t ms_adv_init_info;
#endif

#define BLE_PS_DEBUG
#ifdef BLE_PS_DEBUG
#define BLE_PS_PRT                 os_printf
#else
#define BLE_PS_PRT                 os_null_printf
#endif

typedef enum {
	BLE_PS_FORBID_KRL = 1,
	BLE_PS_FORBID_ENABLED = 2,
	BLE_PS_FORBID_PREVENT = 3,
	BLE_PS_FORBID_TM = 4,
	BLE_PS_FORBID_RWBT = 5,
	BLE_PS_FORBID_RWBLE = 6,
	BLE_PS_FORBID_EA = 7,
	BLE_PS_FORBID_H4TL = 8,
} BLE_PS_FORBID_STATUS;

extern uint8_t ble_flag;
extern beken_queue_t ble_msg_que;
extern adv_info_t adv_info;
extern scan_info_t scan_info;
extern uint8_t ble_init_over;
extern uint8_t ble_deep_sleep;
extern ble_role_t ble_role_mode;

typedef enum
{
    BLE_STACK_OK,
    BLE_STACK_FAIL,
    BLE_CONNECT,
    BLE_DISCONNECT,
    BLE_MTU_CHANGE,
    BLE_CFG_NOTIFY,
    BLE_CFG_INDICATE,
    BLE_TX_DONE
} ble_event_t;

typedef void (*ble_write_cb_t)(uint16_t char_id, uint8_t *buf, uint8_t len);
typedef void (*ble_event_cb_t)(ble_event_t event, void *param);
typedef void (*ble_recv_adv_cb_t)(uint8_t *buf, uint8_t len);
typedef void (*ble_read_cb_t)(uint16_t char_id, uint8_t *buf, uint8_t len);

extern ble_write_cb_t ble_write_cb;
extern ble_event_cb_t ble_event_cb;
extern ble_recv_adv_cb_t ble_recv_adv_cb;
extern ble_read_cb_t ble_read_cb;
extern uint32_t  ble_dut_flag;

typedef struct
{
    uint8_t    channel_map;
    uint16_t  interval;
    uint16_t  window;
} ble_scan_param_t;

typedef struct
{
    uint8_t    channel_map;
    uint16_t  interval_min;
    uint16_t  interval_max;
} ble_adv_param_t;

void ble_init(void);
void ble_exit(void);
void ble_activate(char *ble_name);

void ble_intc_set(uint32_t enable);
void ble_clk_power_up(void);
void ble_clk_power_down(void);
void ble_switch_rf_to_wifi(void);
void ble_switch_rf_to_ble(void);
void ble_set_power_up(uint32 up);
void ble_dut_start(void);

UINT8 ble_is_start(void);
UINT8* ble_get_mac_addr(void);
UINT8* ble_get_name(void);

void ble_send_msg(UINT32 msg);
uint8_t if_ble_sleep(void);

void rf_wifi_used_clr(void);
void rf_wifi_used_set(void);
UINT32 if_rf_wifi_used(void );
void rf_not_share_for_ble(void);
void rf_can_share_for_ble(void);
void ble_set_write_cb(ble_write_cb_t func);
void ble_set_event_cb(ble_event_cb_t func);
void ble_set_read_cb(ble_read_cb_t func);
void ble_set_recv_adv_cb(ble_recv_adv_cb_t func);
void ble_set_role_mode(ble_role_t role);
ble_role_t ble_get_role_mode();


typedef struct temp_message 
{
	uint32_t data;
}BLE_MSG_T;

#define APP_DEVICE_NAME_LENGTH_MAX      (18)
/// Default Device Name if no value can be found in NVDS
#define APP_DFLT_DEVICE_NAME            ("BK7231BT-01")
#define APP_DFLT_DEVICE_NAME_LEN        (sizeof(APP_DFLT_DEVICE_NAME))

extern char app_dflt_dev_name[APP_DEVICE_NAME_LENGTH_MAX];

#define BLE_USER_CMD_OPCODE             (0xFCE0)
#define BLE_USER_CMD_EXIT_DUT_LEN       (2)
#define BLE_USER_CMD_EXIT_DUT_CMD       (0x0E)
#define BLE_USER_CMD_EXIT_DUT_ACT       (0xA0)
#define BLE_USER_CMD_TX_PWR_SET_CMD     (0x10)
#define BLE_USER_CMD_TX_PWR_SAVE_CMD    (0x11)
#define BLE_USER_CMD_XTAL_SET_CMD       (0x12)

#endif /* _BLE_PUB_H_ */
