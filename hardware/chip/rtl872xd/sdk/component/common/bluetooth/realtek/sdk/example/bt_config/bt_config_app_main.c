/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

// #include <os_sched.h>
#include <platform/platform_stdlib.h>
#include <string.h>
#include <trace_app.h>
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include <gap_conn_le.h>
#include <profile_server.h>
#include <gap_msg.h>
#include <gap_config.h>
#include <bt_flags.h>
#include "bt_config_wifi.h"
#include "bt_config_service.h"
#include "bt_config_app_flags.h"
#include "bt_config_app_task.h"
#include "bt_config_peripheral_app.h"
#include <osdep_service.h>
#include "trace_uart.h"
#include "bte.h"
#include "wifi_constants.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"

extern bool bt_trace_uninit(void);
extern void wifi_btcoex_set_bt_on(void);

/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            320
/** @brief  Default Maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            400


/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief  GAP - scan response data (max size = 31 bytes) */
static const uint8_t scan_rsp_data[] =
{
    0x03,                             /* length */
    GAP_ADTYPE_APPEARANCE,            /* type="Appearance" */
    LO_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
    HI_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static const uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length */
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_BT_CONFIG_PROFILE),
    HI_WORD(GATT_UUID_BT_CONFIG_PROFILE),
    /* Local name */
    0x0D,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'A', 'm', 'e', 'b', 'a', '_', 'x', 'x', 'y', 'y', 'z', 'z',
};

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
 * @brief  Config bt stack related feature
 *
 * NOTE: This function shall be called before @ref bte_init is invoked.
 * @return void
 */
void bt_config_stack_config_init(void)
{
    gap_config_max_le_link_num(APP_MAX_LINKS);
}
/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
static void bt_config_app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "Ameba_xxyyzz";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
    uint8_t  slave_init_mtu_req = false;

    /* Advertising parameters */
    uint8_t  adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t  adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  adv_direct_addr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t  adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MIN;

    /* GAP Bond Manager parameters */
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
#if F_BT_LE_SMP_OOB_SUPPORT
    uint8_t  auth_oob = false;
#endif
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
	uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    /* Set device name and device appearance */
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);
    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),
                     &slave_init_mtu_req);

    /* Set advertising parameters */
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);
    le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *)adv_data);
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_rsp_data), (void *)scan_rsp_data);

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
#if F_BT_LE_SMP_OOB_SUPPORT
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
#endif
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);
    /* register gap message callback */
    le_register_app_cb(bt_config_app_gap_callback);
}

/**
 * @brief  Add GATT services and register callbacks
 * @return void
 */
void bt_config_app_le_profile_init(void)
{
    server_init(1);
    bt_config_srv_id = bt_config_service_add_service((void *)bt_config_app_profile_callback);
    server_register_app_cb(bt_config_app_profile_callback);
}

/**
 * @brief    Contains the initialization of all tasks
 * @note     There is only one task in BLE Peripheral APP, thus only one APP task is init here
 * @return   void
 */
void bt_config_task_init(void)
{
	bt_config_app_task_init();
	bt_config_wifi_init();
}

void bt_config_task_deinit(void)
{
	bt_config_wifi_deinit();
	bt_config_app_task_deinit();
}

extern T_GAP_DEV_STATE bt_config_gap_dev_state;
extern T_GAP_CONN_STATE bt_config_gap_conn_state;
extern uint8_t bt_config_conn_id;
extern void bt_coex_init(void);
extern void bt_config_app_set_adv_data(void);
static uint8_t bt_config_state = BC_DEV_DISABLED;

uint8_t get_bt_config_state(void)
{
	return bt_config_state;
}

void set_bt_config_state(uint8_t state)
{
	bt_config_state = state;
}

int bt_config_app_init(void)
{	
	int bt_stack_already_on = 0;
	T_GAP_CONN_INFO conn_info;
	
	/*Check WIFI init complete*/
	if( !wifi_is_up(RTW_STA_INTERFACE) ) {
		BC_printf("WIFI is disabled\n\r");
		return -1;
	}
	
	set_bt_config_state(BC_DEV_INIT); // BT Config on
	
#if CONFIG_AUTO_RECONNECT
	/* disable auto reconnect */
	wifi_set_autoreconnect(0);
#endif
	
	wifi_disable_powersave();
	
	wifi_disconnect();
#if CONFIG_LWIP_LAYER
	LwIP_ReleaseIP(WLAN0_IDX);
#endif
	
	le_get_gap_param(GAP_PARAM_DEV_STATE , &bt_config_gap_dev_state);
	le_get_conn_info(bt_config_conn_id, &conn_info);
	bt_config_gap_conn_state = conn_info.conn_state;
	
	if (bt_config_gap_dev_state.gap_init_state == GAP_INIT_STATE_STACK_READY) {
		bt_stack_already_on = 1;
		BC_printf("BT Stack already on\n\r");
	}
	else{
		bt_trace_init();
		bt_config_stack_config_init();
		bte_init();
		le_gap_init(APP_MAX_LINKS);
		bt_config_app_le_profile_init();
	}
	
	bt_config_app_le_gap_init();
	bt_config_task_init();
	
	bt_coex_init();
	
	/*Wait BT init complete*/
	do {
		vTaskDelay(100 / portTICK_RATE_MS);
		le_get_gap_param(GAP_PARAM_DEV_STATE , &bt_config_gap_dev_state);
	}while(bt_config_gap_dev_state.gap_init_state != GAP_INIT_STATE_STACK_READY);
	
	/*Start BT WIFI coexistence*/
	wifi_btcoex_set_bt_on();
	
	if (bt_stack_already_on) {
		bt_config_app_set_adv_data();
		le_adv_start();
		set_bt_config_state(BC_DEV_IDLE); // BT Config Ready
	}
	
	return 0;
}

void bt_config_app_deinit(void)
{
	set_bt_config_state(BC_DEV_DEINIT);
	bt_config_task_deinit();
	
	le_get_gap_param(GAP_PARAM_DEV_STATE , &bt_config_gap_dev_state);
	if (bt_config_gap_dev_state.gap_init_state != GAP_INIT_STATE_STACK_READY) {
		BC_printf("BT Stack is not running\n\r");
	}
#if F_BT_DEINIT
	else {
		bte_deinit();
		bt_trace_uninit();
		BC_printf("BT Stack deinitalized\n\r");
	}
#endif
	set_bt_config_state(BC_DEV_DISABLED); // BT Config off
}
