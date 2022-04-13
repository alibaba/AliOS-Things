/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __HCI_API__H__
#define __HCI_API__H__
#include "bluetooth.h"
#include "stdbool.h"
#if defined(IBRT)
#include "me_api.h"
#endif

/*---------------------------------------------------------------------------
 * btif_hci_handle_t type
 *
 */
typedef uint16_t btif_hci_handle_t;

#define BTIF_ACTIVE_BROADCAST       0x4000

#define BTIF_PICONET_BROADCAST      0x8000

#define BTIF_INVALID_HCI_HANDLE      0xFFFF

#define BTIF_ACTIVE_BC_HANDLE       (0x0F01 | ACTIVE_BROADCAST)

#define BTIF_PICONET_BC_HANDLE       (0x0F00 | PICONET_BROADCAST)

#define BTIF_PBF_MASK               0x3000

#define BTIF_FIRST_NON_FLUSH_PKT    0x0000

#define BTIF_CONTINUING_PACKET      0x1000

#define BTIF_FIRST_FLUSHABLE_PKT    0x2000

#define BTIF_FIRST_PACKET           0x2000

#define BTIF_SCO_DATA_CORRECT       0x0000

#define BTIF_SCO_DATA_INVALID       0x1000

#define BTIF_SCO_DATA_NONE          0x2000

#define BTIF_SCO_DATA_PARTIAL       0x3000

/* End of btif_hci_handle_t */

/* Group: Link control commands */
#define  BTIF_HCC_INQUIRY                      0x0401
#define  BTIF_HCC_INQUIRY_CANCEL               0x0402
#define  BTIF_HCC_START_PERIODIC_INQ_MODE      0x0403
#define  BTIF_HCC_EXIT_PERIODIC_INQ_MODE       0x0404
#define  BTIF_HCC_CREATE_CONNECTION            0x0405
#define  BTIF_HCC_DISCONNECT                   0x0406
#define  BTIF_HCC_ADD_SCO_CONNECTION           0x0407   /* Not in 1.2 */
#define  BTIF_HCC_CREATE_CONNECTION_CANCEL     0x0408   /* 1.2 */
#define  BTIF_HCC_ACCEPT_CON_REQ               0x0409
#define  BTIF_HCC_REJECT_CON_REQ               0x040A
#define  BTIF_HCC_LINK_KEY_REQ_REPL            0x040B
#define  BTIF_HCC_LINK_KEY_REQ_NEG_REPL        0x040C
#define  BTIF_HCC_PIN_CODE_REQ_REPL            0x040D
#define  BTIF_HCC_PIN_CODE_REQ_NEG_REPL        0x040E
#define  BTIF_HCC_CHNG_CONN_PACKET_TYPE        0x040F
#define  BTIF_HCC_AUTH_REQ                     0x0411
#define  BTIF_HCC_SET_CONN_ENCRYPT             0x0413
#define  BTIF_HCC_CHNG_CONN_LINK_KEY           0x0415
#define  BTIF_HCC_MASTER_LINK_KEY              0x0417
#define  BTIF_HCC_REM_NAME_REQ                 0x0419
#define  BTIF_HCC_REM_NAME_REQ_CANCEL          0x041A   /* 1.2 */
#define  BTIF_HCC_READ_REMOTE_FEATURES         0x041B
#define  BTIF_HCC_READ_REMOTE_EXT_FEATURES     0x041C   /* 1.2 */
#define  BTIF_HCC_READ_REMOTE_VERSION          0x041D
#define  BTIF_HCC_READ_CLOCK_OFFSET            0x041F
#define  BTIF_HCC_READ_LMP_HANDLE              0x0420   /* 1.2 */
#define  BTIF_HCC_EXCHANGE_FIXED_INFO          0x0421   /* 1.2, not in 2.1 */
#define  BTIF_HCC_EXCHANGE_ALIAS_INFO          0x0422   /* 1.2, not in 2.1 */
#define  BTIF_HCC_PRIVATE_PAIRING_REQ_REPL     0x0423   /* 1.2, not in 2.1 */
#define  BTIF_HCC_PRIVATE_PAIRING_REQ_NEG_REPL 0x0424   /* 1.2, not in 2.1 */
#define  BTIF_HCC_GENERATED_ALIAS              0x0425   /* 1.2, not in 2.1 */
#define  BTIF_HCC_ALIAS_ADDRESS_REQ_REPL       0x0426   /* 1.2, not in 2.1 */
#define  BTIF_HCC_ALIAS_ADDRESS_REQ_NEG_REPL   0x0427   /* 1.2, not in 2.1 */
#define  BTIF_HCC_SETUP_SYNC_CONNECTION        0x0428   /* 1.2 */
#define  BTIF_HCC_ACCEPT_SYNC_CON_REQ          0x0429   /* 1.2 */
#define  BTIF_HCC_REJECT_SYNC_CON_REQ          0x042A   /* 1.2 */
#define  BTIF_HCC_IO_CAPABILITY_RESPONSE       0x042B   /* 2.1 */
#define  BTIF_HCC_USER_CONFIRM_REQ_REPL        0x042C   /* 2.1 */
#define  BTIF_HCC_USER_CONFIRM_REQ_NEG_REPL    0x042D   /* 2.1 */
#define  BTIF_HCC_USER_PASSKEY_REQ_REPL        0x042E   /* 2.1 */
#define  BTIF_HCC_USER_PASSKEY_REQ_NEG_REPL    0x042F   /* 2.1 */
#define  BTIF_HCC_REM_OOB_DATA_REQ_REPL        0x0430   /* 2.1 */
#define  BTIF_HCC_REM_OOB_DATA_REQ_NEG_REPL    0x0433   /* 2.1 */
#define  BTIF_HCC_IO_CAPABILITY_NEG_REPL       0x0434   /* 2.1 */

/* Group: Link policy commands */
#define  BTIF_HCC_HOLD_MODE                    0x0801
#define  BTIF_HCC_SNIFF_MODE                   0x0803
#define  BTIF_HCC_EXIT_SNIFF_MODE              0x0804
#define  BTIF_HCC_PARK_MODE                    0x0805
#define  BTIF_HCC_EXIT_PARK_MODE               0x0806
#define  BTIF_HCC_QOS_SETUP                    0x0807
#define  BTIF_HCC_ROLE_DISCOVERY               0x0809
#define  BTIF_HCC_SWITCH_ROLE                  0x080B
#define  BTIF_HCC_READ_LINK_POLICY             0x080C
#define  BTIF_HCC_WRITE_LINK_POLICY            0x080D
#define  BTIF_HCC_READ_DEFAULT_LINK_POLICY     0x080E   /* 1.2 */
#define  BTIF_HCC_WRITE_DEFAULT_LINK_POLICY    0x080F   /* 1.2 */
#define  BTIF_HCC_FLOW_SPECIFICATION           0x0810   /* 1.2 */
#define  BTIF_HCC_SNIFF_SUBRATING              0x0811   /* 2.1 */

/* Group: Host controller and baseband commands */
#define BTIF_HCC_SET_EVENT_MASK               0x0C01
#define BTIF_HCC_RESET                        0x0C03
#define BTIF_HCC_EVENT_FILTER                 0x0C05
#define BTIF_HCC_FLUSH                        0x0C08
#define BTIF_HCC_READ_PIN_TYPE                0x0C09
#define BTIF_HCC_WRITE_PIN_TYPE               0x0C0A
#define BTIF_HCC_CREATE_NEW_UNIT_KEY          0x0C0B
#define BTIF_HCC_READ_STORED_LINK_KEY         0x0C0D
#define BTIF_HCC_WRITE_STORED_LINK_KEY        0x0C11
#define BTIF_HCC_DEL_STORED_LINK_KEY          0x0C12
#if BTIF_BT_BEST_SYNC_CONFIG == BTIF_ENABLED
#define BTIF_HCC_WRITE_SYNC_CONFIG            0xFC51
#endif /*  */
#define BTIF_HCC_CHNG_LOCAL_NAME              0x0C13
#define BTIF_HCC_READ_LOCAL_NAME              0x0C14
#define BTIF_HCC_READ_CONN_ACCEPT_TIMEOUT     0x0C15
#define BTIF_HCC_WRITE_CONN_ACCEPT_TIMEOUT    0x0C16
#define BTIF_HCC_READ_PAGE_TIMEOUT            0x0C17
#define BTIF_HCC_WRITE_PAGE_TIMEOUT           0x0C18
#define BTIF_HCC_READ_SCAN_ENABLE             0x0C19
#define BTIF_HCC_WRITE_SCAN_ENABLE            0x0C1A
#define BTIF_HCC_READ_PAGE_SCAN_ACTIVITY      0x0C1B
#define BTIF_HCC_WRITE_PAGE_SCAN_ACTIVITY     0x0C1C
#define BTIF_HCC_READ_INQ_SCAN_ACTIVITY       0x0C1D
#define BTIF_HCC_WRITE_INQ_SCAN_ACTIVITY      0x0C1E
#define BTIF_HCC_READ_AUTH_ENABLE             0x0C1F
#define BTIF_HCC_WRITE_AUTH_ENABLE            0x0C20
#define BTIF_HCC_READ_ENCRYPT_MODE            0x0C21    /* Not in 2.1 */
#define BTIF_HCC_WRITE_ENCRYPT_MODE           0x0C22    /* Not in 2.1 */
#define BTIF_HCC_READ_CLASS_OF_DEVICE         0x0C23
#define BTIF_HCC_WRITE_CLASS_OF_DEVICE        0x0C24
#define BTIF_HCC_READ_VOICE_SETTING           0x0C25
#define BTIF_HCC_WRITE_VOICE_SETTING          0x0C26
#define BTIF_HCC_READ_AUTO_FLUSH_TIMEOUT      0x0C27
#define BTIF_HCC_WRITE_AUTO_FLUSH_TIMEOUT     0x0C28
#define BTIF_HCC_READ_NUM_BC_RETRANSMIT       0x0C29
#define BTIF_HCC_WRITE_NUM_BC_RETRANSMIT      0x0C2A
#define BTIF_HCC_READ_HOLD_MODE_ACTIVITY      0x0C2B
#define BTIF_HCC_WRITE_HOLD_MODE_ACTIVITY     0x0C2C
#define BTIF_HCC_READ_XMIT_POWER_LEVEL        0x0C2D
#define BTIF_HCC_READ_SCO_FC_ENABLE           0x0C2E
#define BTIF_HCC_WRITE_SCO_FC_ENABLE          0x0C2F
#define BTIF_HCC_SET_CTRLR_TO_HOST_FLOW_CTRL  0x0C31
#define BTIF_HCC_HOST_BUFFER_SIZE             0x0C33
#define BTIF_HCC_HOST_NUM_COMPLETED_PACKETS   0x0C35
#define BTIF_HCC_READ_LINK_SUPERV_TIMEOUT     0x0C36
#define BTIF_HCC_WRITE_LINK_SUPERV_TIMEOUT    0x0C37
#define BTIF_HCC_READ_NUM_IAC                 0x0C38
#define BTIF_HCC_READ_CURRENT_IAC_LAP         0x0C39
#define BTIF_HCC_WRITE_CURRENT_IAC_LAP        0x0C3A
#define BTIF_HCC_READ_PAGE_SCAN_PERIOD_MODE   0x0C3B    /* Not in 2.1 */
#define BTIF_HCC_WRITE_PAGE_SCAN_PERIOD_MODE  0x0C3C    /* Not in 2.1 */
#define BTIF_HCC_READ_PAGE_SCAN_MODE          0x0C3D    /* Not in 1.2 */
#define BTIF_HCC_WRITE_PAGE_SCAN_MODE         0x0C3E    /* Not in 1.2 */
#define BTIF_HCC_SET_AFH_HOST_CHAN_CLASS      0x0C3F    /* 1.2 */
#define BTIF_HCC_READ_INQ_SCAN_TYPE           0x0C42    /* 1.2 */
#define BTIF_HCC_WRITE_INQ_SCAN_TYPE          0x0C43    /* 1.2 */
#define BTIF_HCC_READ_INQ_MODE                0x0C44    /* 1.2 */
#define BTIF_HCC_WRITE_INQ_MODE               0x0C45    /* 1.2 */
#define BTIF_HCC_READ_PAGE_SCAN_TYPE          0x0C46    /* 1.2 */
#define BTIF_HCC_WRITE_PAGE_SCAN_TYPE         0x0C47    /* 1.2 */
#define BTIF_HCC_READ_AFH_CHAN_ASSESS_MODE    0x0C48    /* 1.2 */
#define BTIF_HCC_WRITE_AFH_CHAN_ASSESS_MODE   0x0C49    /* 1.2 */
#define BTIF_HCC_READ_ANONYMITY_MODE          0x0C4A    /* 1.2, not in 2.1 */
#define BTIF_HCC_WRITE_ANONYMITY_MODE         0x0C4B    /* 1.2, not in 2.1 */
#define BTIF_HCC_READ_ALIAS_AUTH_ENABLE       0x0C4C    /* 1.2, not in 2.1 */
#define BTIF_HCC_WRITE_ALIAS_AUTH_ENABLE      0x0C4D    /* 1.2, not in 2.1 */
#define BTIF_HCC_READ_ANON_ADDR_CHNG_PARM     0x0C4E    /* 1.2, not in 2.1 */
#define BTIF_HCC_WRITE_ANON_ADDR_CHNG_PARM    0x0C4F    /* 1.2, not in 2.1 */
#define BTIF_HCC_RESET_FIXED_ADDR_COUNTER     0x0C50    /* 1.2, not in 2.1 */
#define BTIF_HCC_READ_EXTENDED_INQ_RESP       0x0C51    /* 2.1 */
#define BTIF_HCC_WRITE_EXTENDED_INQ_RESP      0x0C52    /* 2.1 */
#define BTIF_HCC_REFRESH_ENCRYPT_KEY          0x0C53    /* 2.1 */
#define BTIF_HCC_READ_SIMPLE_PAIRING_MODE     0x0C55    /* 2.1 */
#define BTIF_HCC_WRITE_SIMPLE_PAIRING_MODE    0x0C56    /* 2.1 */
#define BTIF_HCC_READ_LOCAL_OOB_DATA          0x0C57    /* 2.1 */
#define BTIF_HCC_READ_INQ_RESP_TRANS_PWR_LVL  0x0C58    /* 2.1 */
#define BTIF_HCC_WRITE_INQ_TRANS_PWR_LVL      0x0C59    /* 2.1 */
#define BTIF_HCC_READ_DEF_ERR_DATA_REPORTING  0x0C5A    /* 2.1 */
#define BTIF_HCC_WRITE_DEF_ERR_DATA_REPORTING 0x0C5B    /* 2.1 */
#define BTIF_HCC_ENHANCED_FLUSH               0x0C5F    /* 2.1 */
#define BTIF_HCC_SEND_KEYPRESS_NOTIFY         0x0C60    /* 2.1 */

/* Represents an invalid Host Controller handle. */
#define BTIF_HCI_INVALID_HANDLE      0xFFFF

typedef uint8_t btif_hci_buf_flags_t;

#define BTIF_HCI_COMMAND_DATA_PACKET 0x01
#define BTIF_HCI_ACL_DATA_PACKET     0x02
#define BTIF_HCI_SCO_DATA_PACKET     0x03
#define BTIF_HCI_EVENT_PACKET        0x04
#define BTIF_HCI_BUFTYPE_COMMAND      0x01
#define BTIF_HCI_BUFTYPE_EVENT        0x02  /* Never sent as an HciPacket */
#define BTIF_HCI_BUFTYPE_ACL_DATA     0x04
#define BTIF_HCI_BUFTYPE_SCO_DATA     0x08
#define BTIF_HCI_BUFTYPE_INTERMEDIATE 0x10  /* Intermediate part of an L2CAP packet */
#define BTIF_HCI_BUFTYPE_L2CAP        0x20  /* L2CAP owns this buffer */

#ifdef __IAG_BLE_INCLUDE__
#define BTIF_HCI_BUFTYPE_BLE          0x40
#endif /*  */

typedef struct {
    list_entry_t node;          /* Node element for linked list management. */
    uint16_t len;               /* Length of data in "buffer". */
    uint8_t *buffer;            /* Pointer to received data buffer. */
    uint8_t flags;              /* Flags to identify the type of packet.
                                 * Actually uses the HciBufFlags type.
                                 */

#ifdef BTIF_XA_STATISTICS
    uint32_t hci_timer;

#endif                          /* XA_STATISTICS == XA_ENABLED */
} btif_hci_buffer_t;

typedef struct {
    uint8_t event;              /* The type of HCI event received  */
    U8 len;                     /* Length of the event parameters */
    U8 *parms;                  /* Event-specific parameters */
#ifdef __IAG_BLE_INCLUDE__
    btif_hci_buffer_t *rx_buff; /* Received packet  */
#endif
} hci_event_t;



typedef void (*btif_tws_data_received_callback_func)(uint8_t * data, uint16_t dataLen);

typedef void (*btif_bt_hci_dbg_ll_monitor_callback_func)(const unsigned char *buf,
                                                          unsigned int buf_len);

typedef void (*btif_bt_hci_dbg_trace_warning_evt_code_callback_func)(const unsigned char *buf,
                                                                      unsigned int buf_len);

typedef btif_hci_buffer_t *btif_rx_buff_handle_t;

typedef void (*btif_tws_role_switch_cmp_callback_func) (uint16_t conhdl, uint8_t ret);

typedef void (*btif_bt_addr_exchanged_callback_func) (uint8_t * newBtAddr);

typedef void (*btif_hci_vendor_event_callback_func)(uint8_t* pbuf, uint32_t length);

#if defined(IBRT)
typedef void (*btif_ibrt_connect_ind_callback_func)(uint16_t conhdl,uint8_t ret);

typedef void (*btif_ibrt_disconnect_ind_callback_func)(uint16_t conhdl,uint8_t ret);

typedef void (*btif_ibrt_switch_cmp_callback_func)(uint16_t conhdl,uint8_t role);

typedef void (*btif_ibrt_env_init_cmp_callback_func)(uint8_t sniffer_role);

typedef void (*btif_ibrt_prase_data_callback_func)(btif_hci_buffer_t* hcibuffer,uint16_t conhdl);
#endif

#ifdef __cplusplus
extern "C" {
#endif

    bool btif_hci_Is_any_pending_hci_xfer(void);

    void btif_HciBypassProcessReceivedDataExt(uint32_t(*ext_fn) (void));

    void btif_hci_send_complete_packet_command_right_now(uint16_t handle, uint8_t num);

    list_entry_t *btif_hci_get_rx_buffer_list(void);

    void btif_hci_rxbuffer_free(btif_rx_buff_handle_t rxBuffHandle);

    void btif_register_hci_ll_monitor_evt_code_callback(
                                    btif_bt_hci_dbg_ll_monitor_callback_func func);

    void btif_register_hci_dbg_trace_warning_evt_code_callback
                        (btif_bt_hci_dbg_trace_warning_evt_code_callback_func func);

    unsigned short btif_hci_h4_receive_msg_generic(bool isSimulated, const uint8_t * buf,
                                                   uint32_t size);

    void btif_hci_clean_host_num_of_completed_packet(uint16_t connHanlde);

    bool btif_hci_restore_new_master_num_of_completed_packet(uint16_t connHanlde);

    void btif_register_tws_role_switch_cmp_callback(btif_tws_role_switch_cmp_callback_func func);

    void btif_register_bt_addr_exchanged_callback(btif_bt_addr_exchanged_callback_func func);

    uint32_t btif_hci_current_rx_packet_complete(void);

    uint32_t btif_hci_current_rx_aclfreelist_cnt(void);

    uint32_t btif_hci_get_acl_buffers_num(void);

    void *btif_hci_get_PktsCompleteTimerHandler(void);

    int btif_hci_init(void);

    void btif_register_vendor_event_callback(btif_hci_vendor_event_callback_func func);
    uint8_t* btif_hci_peek_hci_tx_buf(uint8_t tx_type,uint16_t handle);
    void btif_hci_ahead_hci_tx_buf(uint8_t* hci_buf);

#if defined(IBRT)
    uint8_t btif_hci_disconnect_connection_direct(uint16_t conn_handle, uint8_t reason);
	uint8_t btif_hci_tws_switch_direct(uint8_t op);
    uint32_t btif_save_hci_ctx(uint8_t * ctx_buffer, uint16_t dev_id);
    uint32_t btif_set_hci_ctx(uint8_t* ctx_buffer, uint16_t dev_id);
    uint16_t btif_hci_get_con_hci_handle(uint8_t con_idx);
    uint8_t btif_hci_get_con_conflags(uint8_t con_idx);
    void btif_hci_register_tx_buf_tss_process_callback(btif_callback_ext3 cb);

#endif

#ifdef __cplusplus
}
#endif
#endif /*__HCI_API__H__*/

