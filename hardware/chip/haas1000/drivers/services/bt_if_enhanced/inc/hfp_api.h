/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HFP_API_H__
#define __HFP_API_H__

#include "bluetooth.h"
#include "conmgr_api.h"
#include "hci_api.h"

#ifdef __BT_ONE_BRING_TWO__
#define HF_CHANNEL_NUM     2
#else
#define HF_CHANNEL_NUM     1
#endif

typedef void *hf_chan_handle_t;

/* start of btif_audio_state_t*/
typedef enum {
    BTIF_HF_AUDIO_DISCON = 0,
    BTIF_HF_AUDIO_CON = 1,
} btif_audio_state_t;

/* End of btif_audio_state_t*/

/*---------------------------------------------------------------------------
 * btif_hf_call_held_state type
 *
 */
typedef uint8_t btif_hf_call_held_state;

#define BTIF_HF_CALL_HELD_NONE      0

#define BTIF_HF_CALL_HELD_ACTIVE    1

#define BTIF_HF_CALL_HELD_NO_ACTIVE 2

/* End of btif_hf_call_held_state */

/* start the definition of btif_hf_call_setup_t*/
typedef uint8_t btif_hf_call_setup_t;

#define BTIF_HF_CALL_SETUP_NONE   0

#define BTIF_HF_CALL_SETUP_IN     1

#define BTIF_HF_CALL_SETUP_OUT    2

#define BTIF_HF_CALL_SETUP_ALERT  3
/* end btif_hf_call_setup_t*/

/* start the definition of btif_hf_call_active_t */
typedef uint8_t btif_hf_call_active_t;

#define BTIF_HF_CALL_NONE           0

#define BTIF_HF_CALL_ACTIVE         1
/* end of btif_hf_call_active_t */

typedef enum {
    BTIF_HF_SCO_CODEC_NONE = 0,
    BTIF_HF_SCO_CODEC_CVSD = 1,
    BTIF_HF_SCO_CODEC_MSBC = 2,
} hfp_sco_codec_t;

typedef enum {
    BTIF_HF_HOLD_RELEASE_HELD_CALLS = 0,
    BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS = 1,
    BTIF_HF_HOLD_HOLD_ACTIVE_CALLS = 2,
    BTIF_HF_HOLD_ADD_HELD_CALL = 3,
    BTIF_HF_HOLD_CALL_TRANSFER = 4,
} btif_hf_hold_call_t;

typedef enum {
    BTIF_HF_STATE_CLOSED = 0,
    BTIF_HF_STATE_CONN_PENDING = 1,
    BTIF_HF_STATE_CONN_INCOMING = 2,
    BTIF_HF_STATE_NEGOTIATE = 3,
    BTIF_HF_STATE_OPEN = 4,
    BTIF_HF_STATE_DISC = 5,
} btif_hf_chan_state_t;

typedef uint16_t hf_gateway_version;

/* Unable to determine the Hands Free Profile version that is supported */
#define BTIF_HF_GW_VERSION_UNKNOWN 0x0000

/* Supports Version 0.96 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_0_96    0x0100

/* Supports Version 1.0 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_0     0x0101

/* Supports Version 1.5 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_5     0x0105

#define BTIF_HF_GW_VERSION_1_6     0x0106

#define BTIF_HF_GW_VERSION_1_7     0x0107

/* End of hf_gateway_version */

typedef enum {
    BTIF_HF_EVENT_SERVICE_CONNECT_REQ = 0,
    BTIF_HF_EVENT_SERVICE_CONNECTED = 1,
    BTIF_HF_EVENT_SERVICE_DISCONNECTED = 2,
    BTIF_HF_EVENT_AUDIO_CONNECTED = 3,
    BTIF_HF_EVENT_AUDIO_DISCONNECTED = 4,
    BTIF_HF_EVENT_AUDIO_DATA = 5,
    BTIF_HF_EVENT_AUDIO_DATA_SENT = 6,
    BTIF_HF_EVENT_GATEWAY_FEATURES = 7,
    BTIF_HF_EVENT_GW_HOLD_FEATURES = 8,
    BTIF_HF_EVENT_CALL_STATE = 9,
    BTIF_HF_EVENT_CALLER_ID = 10,
    BTIF_HF_EVENT_CALL_LISTING_ENABLED = 11,
    BTIF_HF_EVENT_RESPONSE_HOLD_APPL = 12,
    BTIF_HF_EVENT_CALL_IND = 13,
    BTIF_HF_EVENT_CALLSETUP_IND = 14,
    BTIF_HF_EVENT_CALLHELD_IND = 15,
    BTIF_HF_EVENT_RING_IND = 16,
    BTIF_HF_EVENT_WAIT_NOTIFY = 17,
    BTIF_HF_EVENT_CALLER_ID_NOTIFY = 18,
    BTIF_HF_EVENT_CURRENT_CALL_STATE = 19,
    BTIF_HF_EVENT_RESPONSE_HOLD = 20,
    BTIF_HF_EVENT_SERVICE_IND = 21,
    BTIF_HF_EVENT_BATTERY_IND = 22,
    BTIF_HF_EVENT_SIGNAL_IND = 23,
    BTIF_HF_EVENT_ROAM_IND = 24,
    BTIF_HF_EVENT_SMS_IND = 25,
    BTIF_HF_EVENT_VOICE_REC_STATE = 26,
    BTIF_HF_EVENT_VOICE_TAG_NUMBER = 27,
    BTIF_HF_EVENT_SPEAKER_VOLUME = 28,
    BTIF_HF_EVENT_MIC_VOLUME = 29,
    BTIF_HF_EVENT_IN_BAND_RING = 30,
    BTIF_HF_EVENT_NETWORK_OPERATOR = 31,
    BTIF_HF_EVENT_SUBSCRIBER_NUMBER = 32,
    BTIF_HF_EVENT_NO_CARRIER = 33,
    BTIF_HF_EVENT_BUSY = 34,
    BTIF_HF_EVENT_NO_ANSWER = 35,
    BTIF_HF_EVENT_DELAYED = 36,
    BTIF_HF_EVENT_BLACKLISTED = 37,
    BTIF_HF_EVENT_PHONEBOOK_STORAGE = 38,
    BTIF_HF_EVENT_PHONEBOOK_INFO = 39,
    BTIF_HF_EVENT_PHONEBOOK_SIZE = 40,
    BTIF_HF_EVENT_PHONEBOOK_ENTRY = 41,
    BTIF_HF_EVENT_AT_RESULT_DATA = 42,
    BTIF_HF_EVENT_COMMAND_COMPLETE = 43,
    BTIF_HF_EVENT_SIRI_STATUS = 44,
    BTIF_HF_EVENT_READ_AG_INDICATORS_STATUS = 45,
    BTIF_HF_EVENT_BES_TEST = 46,
    BTIF_HF_EVENT_SELECT_CHANNEL = 47,
    BTIF_HF_EVENT_RFCOMM_CONNECTED = 48,
} hf_event_t;

/* End of hf_event_t*/

struct hfp_context {
    uint16_t remote_dev_hcihandle;
    /*TODO:should remove the status */
    bt_status_t status;         /* Status of the callback event             */
    btif_hf_chan_state_t state;
    bt_bdaddr_t remote_dev_bdaddr;
    const char *call_number;
    btif_hf_call_active_t call;
    btif_hf_call_setup_t call_setup;
    btif_hf_call_held_state  call_held;
    hf_event_t event;
    uint8_t *audio_data;
    uint16_t audio_data_len;
    uint8_t error_code;
    uint8_t disc_reason;
    uint8_t disc_reason_saved;
    uint32_t speaker_volume;
    uint32_t voice_rec_state;
    uint32_t **chan_sel_channel;
    btif_remote_device_t *chan_sel_remDev;
    char *ptr;
};

struct hfp_vendor_info {
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t version_id;
    uint16_t feature_id;  //default is 3, if you do not known, set it 0
};

typedef void (*hf_event_cb_t) (hf_chan_handle_t chan, struct hfp_context * ctx);

#ifdef __cplusplus
extern "C" {
#endif                         

    /*
     *  AT+XAPL=[vendorID]-[productID]-[version],[features]
     *  if you set features as 0,this api will ignore your features setting and use default value
     */
    void btif_hf_init_vendor_info(struct hfp_vendor_info *info_ptr);

    int hfp_hfcommand_mempool_init(void);

    int btif_hfp_initialize(void);

    int btif_hf_register_callback(hf_event_cb_t callback);

    hf_gateway_version btif_hf_get_version(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_update_indicators_batt_level(hf_chan_handle_t chan_h, uint8_t assigned_num, uint8_t level);

    bt_status_t btif_hf_batt_report(hf_chan_handle_t chan_h, uint8_t level);

    bt_status_t btif_hf_siri_report(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_enable_voice_recognition(hf_chan_handle_t chan_h, bool en);

    bt_status_t btif_hf_batt_report(hf_chan_handle_t chan_h, uint8_t level);

    bool btif_hf_is_voice_rec_active(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_disable_nrec(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_report_speaker_volume(hf_chan_handle_t chan_h, uint8_t gain);

    bt_status_t btif_hf_send_at_cmd(hf_chan_handle_t chan_h, const char *at_str);

    bt_status_t btif_hf_list_current_calls(hf_chan_handle_t chan_h);

    bool btif_hf_is_hf_indicators_support(hf_chan_handle_t chan_h);

    bool btif_hf_is_batt_report_support(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_force_disconnect_sco(hf_chan_handle_t chan_h);

    void btif_hf_set_negotiated_codec(hf_chan_handle_t chan_h, hfp_sco_codec_t codec);

    hfp_sco_codec_t btif_hf_get_negotiated_codec(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_answer_call(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_hang_up_call(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_redial_call(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_dial_number(hf_chan_handle_t chan_h, uint8_t *number, uint16_t len);

    bt_status_t btif_hf_disc_audio_link(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_create_audio_link(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_call_hold(hf_chan_handle_t chan_h, btif_hf_hold_call_t action,
                                  uint8_t index);
    bt_status_t btif_hf_switch_calls(hf_chan_handle_t hangup_chan_h,
                                     hf_chan_handle_t answer_chan_h);

    hf_chan_handle_t btif_get_hf_chan_by_address(bt_bdaddr_t *bdaddr);
    
    uint8_t btif_get_hf_chan_audio_up_flag(hf_chan_handle_t chan_h);
    
    btif_hf_chan_state_t btif_get_hf_chan_state(hf_chan_handle_t chan_h);
    
    bool btif_hf_check_AudioConnect_status(hf_chan_handle_t chan_h);

    hf_chan_handle_t btif_hf_create_channel(void);

    int btif_hf_init_channel(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_disconnect_service_link(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_create_service_link(hf_chan_handle_t chan_h, bt_bdaddr_t * bt_addr);

    btif_cmgr_handler_t *btif_hf_get_chan_manager_handler(hf_chan_handle_t chan_h);

    bt_status_t btif_hfp_switch_sco(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_set_master_role(hf_chan_handle_t chan_h, bool role);

    bt_status_t btif_hf_enable_sniff_mode(hf_chan_handle_t chan_h, bool enable);

    bool btif_hf_get_remote_bdaddr(hf_chan_handle_t chan_h, bt_bdaddr_t *bdaddr_p);

    uint16_t btif_hf_get_sco_hcihandle(hf_chan_handle_t chan_h);

    btif_hci_handle_t btif_hf_get_remote_hci_handle(hf_chan_handle_t chan_h);

    bool btif_hf_is_acl_connected(hf_chan_handle_t chan_h);

    btif_remote_device_t *btif_hf_cmgr_get_remote_device(hf_chan_handle_t chan_h);

    bt_status_t btif_hf_send_audio_data(hf_chan_handle_t chan_h, btif_bt_packet_t *packet);

    bt_status_t btif_hf_is_inbandring_enabled(hf_chan_handle_t chan_h);

    uint32_t btif_hfp_profile_save_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);

    uint32_t btif_hfp_profile_restore_ctx(uint8_t *buf, uint32_t buf_len);

#if defined(IBRT)
    bt_status_t btif_hf_sync_conn_audio_connected(hfp_sco_codec_t codec,uint16_t conhdl);
    bt_status_t btif_hf_sync_conn_audio_disconnected(uint16_t conhdl);
    btif_hf_chan_state_t btif_hfp_profile_channel_state(uint8_t bt_device_id);
#endif    

    bt_status_t btif_hf_indicators_1(hf_chan_handle_t chan_h);
    bt_status_t btif_hf_indicators_2(hf_chan_handle_t chan_h);
    bt_status_t btif_hf_indicators_3(hf_chan_handle_t chan_h);

#if defined(HFP_MOBILE_AG_ROLE)
    bt_status_t btif_ag_create_service_link(hf_chan_handle_t chan_h, bt_bdaddr_t * bt_addr);
    bt_status_t btif_ag_disconnect_service_link(hf_chan_handle_t chan_h);
    bt_status_t btif_ag_create_audio_link(hf_chan_handle_t chan_h);
    bt_status_t btif_ag_disc_audio_link(hf_chan_handle_t chan_h);
    bt_status_t btif_ag_send_call_active_status(hf_chan_handle_t chan_h, bool active);
    bt_status_t btif_ag_send_callsetup_status(hf_chan_handle_t chan_h, uint8_t status);
    bt_status_t btif_ag_send_callheld_status(hf_chan_handle_t chan_h, uint8_t status);
    bt_status_t btif_ag_send_calling_ring(hf_chan_handle_t chan_h, const char* number);
    bt_status_t btif_ag_set_speaker_gain(hf_chan_handle_t chan_h, uint8_t volume);
    bt_status_t btif_ag_set_microphone_gain(hf_chan_handle_t chan_h, uint8_t volume);
    bt_status_t btif_ag_send_result_code(hf_chan_handle_t chan_h, const char *data, int len);
    bt_status_t btif_ag_register_mobile_module(hf_chan_handle_t chan_h, struct hfp_mobile_module_handler* handler);
#endif

#ifdef __cplusplus
}
#endif                          /*  */
#endif /*__HFP_API_H__*/
