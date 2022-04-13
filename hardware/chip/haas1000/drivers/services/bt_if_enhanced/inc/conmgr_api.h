/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __CONMGR_API_H_
#define __CONMGR_API_H_

#include "me_api.h"

typedef void btif_cmgr_handler_t;

#ifndef BTIF_CMGR_DEFAULT_SNIFF_EXIT_POLICY
#define BTIF_CMGR_DEFAULT_SNIFF_EXIT_POLICY CMGR_SNIFF_EXIT_ON_AUDIO
#endif /*  */

#ifndef BTIF_CMGR_DEFAULT_SNIFF_TIMER
#define BTIF_CMGR_DEFAULT_SNIFF_TIMER CMGR_SNIFF_DONT_CARE
#endif /*  */

#ifndef BTIF_CMGR_AUDIO_DEFAULT_PARMS
//#define CMGR_AUDIO_DEFAULT_PARMS CMGR_AUDIO_PARMS_S3
#define BTIF_CMGR_AUDIO_DEFAULT_PARMS CMGR_AUDIO_PARMS_S4
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_MIN_INTERVAL
#ifdef BISTO_ENABLED
#define BTIF_CMGR_SNIFF_MIN_INTERVAL 160
#else
#define BTIF_CMGR_SNIFF_MIN_INTERVAL 800
#endif
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_MAX_INTERVAL
#ifdef BISTO_ENABLED
#define BTIF_CMGR_SNIFF_MAX_INTERVAL 160
#else
#define BTIF_CMGR_SNIFF_MAX_INTERVAL 800
#endif
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_ATTEMPT
#define BTIF_CMGR_SNIFF_ATTEMPT 3
#endif /*  */

#ifndef BTIF_CMGR_SNIFF_TIMEOUT
#define BTIF_CMGR_SNIFF_TIMEOUT 1
#endif /*  */

#define BTIF_CMGR_SNIFF_DISABLED    (0xFF)
#define BTIF_CMGR_SNIFF_DONT_CARE   (0)
#define BTIF_CMGR_SNIFF_TIMER       (10000)
#define BTIF_CMGR_TWS_SNIFF_TIMER       (10000)
#define BTIF_CMGR_MOBILE_SNIFF_TIMER     (20000)

typedef U8 btif_cmgr_sniff_exit_policy_t;

#define BTIF_CMGR_SNIFF_EXIT_ON_SEND     0x01

#define BTIF_CMGR_SNIFF_EXIT_ON_AUDIO    0x02

/* End of CmgrSniffExitPolicy */

/*---------------------------------------------------------------------------
 * cmgr_event_t type
 *
 *     All indications and confirmations are sent through a callback function.
 *     The profile registers with the Connection Manager at initialization.  Calls
 *     are made to request ACL and SCO resources.  The Connection Manager calls
 *     the profile's callback function to indicate the results of those requests.
 */
typedef uint8_t cmgr_event_t;

#define BTIF_CMEVENT_DATA_LINK_CON_CNF     1

#define BTIF_CMEVENT_DATA_LINK_CON_IND     2

#define BTIF_CMEVENT_DATA_LINK_DIS         3

#define BTIF_CMEVENT_AUDIO_LINK_REQ        4

#define BTIF_CMEVENT_AUDIO_LINK_CON        5

#define BTIF_CMEVENT_AUDIO_LINK_DIS        6

#define BTIF_CMEVENT_AUDIO_DATA            7

#define BTIF_CMEVENT_AUDIO_DATA_SENT       8

#define BTIF_CMEVENT_ENTER_SNIFF_MODE      9

#define BTIF_CMEVENT_EXIT_SNIFF_PARK_MODE  10

/* End of cmgr_event_t */

#ifdef __cplusplus
extern "C" {
#endif                          /*  */

    btif_cmgr_handler_t *btif_cmgr_get_first_handler(btif_remote_device_t * remDev);

    bt_status_t btif_cmgr_set_sniff_exit_policy(btif_cmgr_handler_t * cmgr_handler,
                                                btif_cmgr_sniff_exit_policy_t Policy);

    bt_status_t btif_cmgr_set_sniff_timer(btif_cmgr_handler_t * cmgr_handler,
                                          btif_sniff_info_t * SniffInfo, TimeT Time);

    uint32_t btif_cmgr_get_cmgrhandler_sniff_timeout(btif_cmgr_handler_t * cmgr_handler);

    uint16_t  btif_cmgr_get_cmgrhandler_sniff_Interval(btif_cmgr_handler_t * cmgr_handler);

    btif_sniff_info_t*btif_cmgr_get_cmgrhandler_sniff_info(btif_cmgr_handler_t *cmgr_handler);

    btif_remote_device_t *btif_cmgr_get_cmgrhandler_remdev(btif_cmgr_handler_t *cmgr_handler);

    uint16_t btif_cmgr_get_cmgrhandler_remdev_hci_handle(btif_cmgr_handler_t * cmgr_handler);

    btif_handler *btif_cmgr_get_cmgrhandler_remdev_bthandle(btif_cmgr_handler_t *cmgr_handler);

    bt_status_t btif_cmgr_set_sniff_info_to_all_handler_by_remdev(btif_sniff_info_t * SniffInfo,
                                                                  btif_remote_device_t * remDev);

    bt_status_t btif_cmgr_set_sniff_timeout_handler_ext(void (*ext_fn)(evm_timer_t * timer,
                                                        BOOL * skipInternalHandler));

    bt_status_t btif_cmgr_init_sniff_timer(btif_remote_device_t *remDev);

    bt_status_t btif_cmgr_deinit_sniff_timer(btif_remote_device_t *remDev);

    bt_status_t btif_cmgr_clear_sniff_timer(btif_cmgr_handler_t * Handler);

    bt_status_t btif_cmgr_disable_sniff_timer(btif_cmgr_handler_t * Handler);

    btif_link_type_t btif_cmgr_get_sco_connect_sco_link_type(btif_cmgr_handler_t * cmgr_handler);

    uint16_t btif_cmgr_get_sco_connect_sco_Hcihandler(btif_cmgr_handler_t * cmgr_handler);

    uint8_t btif_cmgr_get_sco_connect_sco_rx_parms_sco_transmission_interval(btif_cmgr_handler_t *
                                                                             cmgr_handler);

    uint8_t btif_cmgr_get_sco_connect_sco_rx_parms_sco_retransmission_window(btif_cmgr_handler_t *
                                                                             cmgr_handler);

    uint8_t btif_cmgr_set_sco_connect_sco_rx_parms_sco_transmission_interval(
                                                            btif_cmgr_handler_t * cmgr_handler,
                                                            uint8_t interval);

    uint8_t btif_cmgr_set_sco_connect_sco_rx_parms_sco_retransmission_window(
                                                            btif_cmgr_handler_t *cmgr_handler,
                                                            uint8_t window);

    bt_status_t btif_cmgr_remove_audio_link(btif_cmgr_handler_t * cmgr_handler);

    bool btif_cmgr_is_audio_up(btif_cmgr_handler_t *Handler);

    bt_status_t btif_cmgr_set_master_role(btif_cmgr_handler_t *cmgr_handler, BOOL flag);

    void btif_evm_start_timer(evm_timer_t *timer, TimeT ms);

    void *btif_cmgr_get_default_conn_handler(void);

    void *btif_cmgr_handler_create(void);

    void btif_cmgr_handler_init(void);

    bt_status_t btif_cmgr_register_handler(btif_cmgr_handler_t *cmgr_handler,
                                           btif_cmgr_callback callback);

    bt_status_t btif_cmgr_create_data_link(btif_cmgr_handler_t *cmgr_handler,
                                           bt_bdaddr_t *bd_addr);

    bt_status_t btif_cmgr_remove_data_link(btif_cmgr_handler_t *cmgr_handler);

    btif_cmgr_handler_t *btif_cmgr_get_acl_handler(btif_remote_device_t *remDev);

    btif_cmgr_handler_t * btif_cmgr_get_conn_ind_handler(btif_remote_device_t *remDev);

    bool btif_cmgr_is_link_up(btif_cmgr_handler_t *cmgr_handler);
    btif_remote_device_t *btif_cmgr_pts_get_remDev(btif_cmgr_handler_t *cmgr_handler);

#if defined(IBRT)
    uint32_t btif_save_cmgr_cxt(uint8_t * ctx_buffer);
    uint32_t btif_set_cmgr_cxt(uint8_t * ctx_buffer, uint8_t bt_devices_idx,uint8_t rm_devtbl_idx);
    uint32_t btif_save_avrcp_ctx(uint8_t * ctx_buffer);
    uint32_t btif_set_avrcp_ctx(uint8_t * ctx_buffer, uint8_t bt_devices_idx,uint8_t rm_devtbl_idx, uint8_t avd_ctx_device_idx);
#endif

#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /*  */

