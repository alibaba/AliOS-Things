/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BTAPP_H__
#define __BTAPP_H__

#include "app_key.h"
#include "hfp_api.h"
#include "a2dp_api.h"
#include "app_a2dp_codecs.h"
#if defined(__BTMAP_ENABLE__)
#include "map_api.h"
#endif

//#define __EARPHONE_STAY_BOTH_SCAN__

/* bt config */
#ifdef _SCO_BTPCM_CHANNEL_
#define SYNC_CONFIG_PATH (0<<8|1<<4|1<<0) /* all links use hci */
#else
#define SYNC_CONFIG_PATH (0<<8|0<<4|0<<0) /* all links use hci */
#endif
#define SYNC_CONFIG_MAX_BUFFER (0) /* (e)sco use Packet size */
#ifdef CVSD_BYPASS
#define SYNC_CONFIG_CVSD_BYPASS (1) /* use pcm over hci */
#else
#define SYNC_CONFIG_CVSD_BYPASS (0) /* use pcm over hci */
#endif
#define INQ_EXT_RSP_LEN 240
///a2dp app include
/* a2dp */
/* Default SBC codec configuration */
/* data type for the SBC Codec Information Element*/
/*****************************************************************************
**  Constants
*****************************************************************************/
/* the length of the SBC Media Payload header. */
#define A2D_SBC_MPL_HDR_LEN         1

/* the LOSC of SBC media codec capabilitiy */
#define A2D_SBC_INFO_LEN            6

/* for Codec Specific Information Element */
#if defined(A2DP_SCALABLE_ON)
#define A2D_SCALABLE_IE_SAMP_FREQ_MSK    0xFF    /* b7-b0 sampling frequency */
#endif

#if defined(A2DP_LHDC_ON)
#define A2D_LHDC__IE_SAMP_FREQ_MSK    0xFF    /* b7-b0 sampling frequency */
#endif


#define A2D_STREAM_SAMP_FREQ_MSK    0xFF    /* b7-b4 sampling frequency */

#define A2D_SBC_IE_SAMP_FREQ_MSK    0xF0    /* b7-b4 sampling frequency */

#define A2D_SBC_IE_SAMP_FREQ_16     0x80    /* b7:16  kHz */
#define A2D_SBC_IE_SAMP_FREQ_32     0x40    /* b6:32  kHz */
#define A2D_SBC_IE_SAMP_FREQ_44     0x20    /* b5:44.1kHz */
#define A2D_SBC_IE_SAMP_FREQ_48     0x10    /* b4:48  kHz */

#ifdef A2DP_SCALABLE_ON
#define A2D_SBC_IE_SAMP_FREQ_96     0x08    /* b4:48  kHz */
#endif

#ifdef A2DP_LHDC_ON
#ifndef A2D_SBC_IE_SAMP_FREQ_96
#define A2D_SBC_IE_SAMP_FREQ_96     0x08    /* b4:48  kHz */
#endif
#endif


#ifdef A2DP_LDAC_ON
#ifndef A2DP_LDAC_OCTET_NUMBER
#define A2DP_LDAC_OCTET_NUMBER                     (8)
#endif

#ifndef A2D_SBC_IE_SAMP_FREQ_96
#define A2D_SBC_IE_SAMP_FREQ_96     0x08    /* b4:96  kHz */
#endif
#ifndef A2D_SBC_IE_SAMP_FREQ_88
#define A2D_SBC_IE_SAMP_FREQ_88     0x04    /* b4:88.2  kHz */
#endif
#endif

#define A2D_SBC_IE_BIT_NUM_16       0x16
#define A2D_SBC_IE_BIT_NUM_24       0x24

#define A2D_SBC_IE_CH_MD_MSK        0x0F    /* b3-b0 channel mode */
#define A2D_SBC_IE_CH_MD_MONO       0x08    /* b3: mono */
#define A2D_SBC_IE_CH_MD_DUAL       0x04    /* b2: dual */
#define A2D_SBC_IE_CH_MD_STEREO     0x02    /* b1: stereo */
#define A2D_SBC_IE_CH_MD_JOINT      0x01    /* b0: joint stereo */

#define A2D_SBC_IE_BLOCKS_MSK       0xF0    /* b7-b4 number of blocks */
#define A2D_SBC_IE_BLOCKS_4         0x80    /* 4 blocks */
#define A2D_SBC_IE_BLOCKS_8         0x40    /* 8 blocks */
#define A2D_SBC_IE_BLOCKS_12        0x20    /* 12blocks */
#define A2D_SBC_IE_BLOCKS_16        0x10    /* 16blocks */

#define A2D_SBC_IE_SUBBAND_MSK      0x0C    /* b3-b2 number of subbands */
#define A2D_SBC_IE_SUBBAND_4        0x08    /* b3: 4 */
#define A2D_SBC_IE_SUBBAND_8        0x04    /* b2: 8 */

#define A2D_SBC_IE_ALLOC_MD_MSK     0x03    /* b1-b0 allocation mode */
#define A2D_SBC_IE_ALLOC_MD_S       0x02    /* b1: SNR */
#define A2D_SBC_IE_ALLOC_MD_L       0x01    /* b0: loundess */

#define A2D_SBC_IE_MIN_BITPOOL      2
#define A2D_SBC_IE_MAX_BITPOOL      250

#ifdef __cplusplus
extern "C" {
#endif

//extern A2dpStream a2dp_stream;
//extern btif_avdtp_codec_t a2dp_avdtpcodec;
extern const unsigned char a2dp_codec_elements[];
//extern enum AUD_SAMPRATE_T a2dp_sample_rate;

//#define AVRCP_TRACK_CHANGED

#define AVRCP_KEY_NULL                  0
#define AVRCP_KEY_STOP                  1
#define AVRCP_KEY_PLAY                  2
#define AVRCP_KEY_PAUSE                 3
#define AVRCP_KEY_FORWARD               4
#define AVRCP_KEY_BACKWARD              5
#define AVRCP_KEY_VOLUME_UP             6
#define AVRCP_KEY_VOLUME_DOWN           7

#define HFP_KEY_ANSWER_CALL             8
#define HFP_KEY_HANGUP_CALL             9
#define HFP_KEY_REDIAL_LAST_CALL        10
#define HFP_KEY_CHANGE_TO_PHONE         11
#define HFP_KEY_ADD_TO_EARPHONE         12
#define HFP_KEY_MUTE                    13
#define HFP_KEY_CLEAR_MUTE              14
//3way calls oper
#define HFP_KEY_THREEWAY_HOLD_AND_ANSWER              15
#define HFP_KEY_THREEWAY_HANGUP_AND_ANSWER             16
#define HFP_KEY_THREEWAY_HOLD_REL_INCOMING            17
#ifdef __BT_ONE_BRING_TWO__
#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER      18
#define HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER  19
#define HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER    20
#endif
#define HFP_KEY_THREEWAY_HOLD_ADD_HELD_CALL           21

//hsp
#define HSP_KEY_CKPD_CONTROL     	21
#define HSP_KEY_ADD_TO_EARPHONE 	22
#define HSP_KEY_CHANGE_TO_PHONE	23

#if defined (__HSP_ENABLE__)
void hsp_callback(HsChannel *Chan, HsCallbackParms *Info);
#endif

#define APP_REPORT_SPEAKER_VOL_CMD 	0x01
#define APP_CPKD_CMD				0x02
#define SPP_MAX_TX_PACKET_NUM	5
#define AVRCP_PDU_PARAM_MAX_LEN	40


enum BT_DEVICE_ID_T{
    BT_DEVICE_ID_1 = 0,
#ifdef __BT_ONE_BRING_TWO__
    BT_DEVICE_ID_2,
#endif
    BT_DEVICE_NUM
};

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
enum BT_DEVICE_SRC_SNK_T{
	BT_DEVICE_SNK=0,
	BT_DEVICE_SRC
};
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
#define APP_BD_ADDR_SIZE    6
typedef struct APP_BT_BD_ADDR {
    U8    addr[APP_BD_ADDR_SIZE];
} APP_BT_BD_ADDR;
#endif
#endif

struct BT_DEVICE_T{
    btif_a2dp_stream_t*  a2dp_stream[BT_DEVICE_NUM];
#ifdef __A2DP_AVDTP_CP__
        uint8_t avdtp_cp[BT_DEVICE_NUM];
#endif
#if defined(A2DP_LHDC_ON)
    btif_a2dp_stream_t* a2dp_lhdc_stream[BT_DEVICE_NUM];
    uint8_t a2dp_lhdc_llc[BT_DEVICE_NUM];
#endif

#if defined(A2DP_LDAC_ON)
    btif_a2dp_stream_t* a2dp_ldac_stream[BT_DEVICE_NUM];
    int channel_mode;
#endif

#if defined(A2DP_AAC_ON)
     btif_a2dp_stream_t* a2dp_aac_stream[BT_DEVICE_NUM];
#endif
#if defined(A2DP_SCALABLE_ON)
     btif_a2dp_stream_t* a2dp_scalable_stream[BT_DEVICE_NUM];
#endif
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	uint8_t src_or_snk;//src or snk fkag
	uint8_t input_onoff;
	APP_BT_BD_ADDR         inquried_snk_bdAddr;           /* Device Address */
	//BT_BD_ADDR         rmt_bdAddr;           /* Device Address */
#endif

    a2dp_stream_t * a2dp_connected_stream[BT_DEVICE_NUM];
    a2dp_stream_t * a2dp_outconfiged_stream[BT_DEVICE_NUM];
    btif_remote_device_t * a2dp_outconfiged_rem[BT_DEVICE_NUM];
    enum BT_DEVICE_ID_T curr_a2dp_stream_id;
    uint16_t current_a2dp_conhdl;

    
    uint8_t a2dp_state[BT_DEVICE_NUM];
    uint8_t a2dp_streamming[BT_DEVICE_NUM];
    uint8_t a2dp_play_pause_flag;
    uint8_t	avrcpPendingKey;
	uint8_t latestPausedDevId;
    uint8_t	avrcpPressedKey;
    btif_avdtp_codec_type_t codec_type[BT_DEVICE_NUM];

    uint8_t sample_rate[BT_DEVICE_NUM];
    uint8_t sample_bit[BT_DEVICE_NUM];
    uint8_t avrcp_state[BT_DEVICE_NUM];
    btif_avrcp_channel_t*  avrcp_channel[BT_DEVICE_NUM];

    hf_chan_handle_t hf_channel[BT_DEVICE_NUM];
#if defined (__HSP_ENABLE__)
    HsChannel hs_channel[BT_DEVICE_NUM];
#endif
#if defined (__BTMAP_ENABLE__)
    btif_map_session_handle_t map_session_handle[BT_DEVICE_NUM];
#endif
    enum BT_DEVICE_ID_T curr_hf_channel_id;
    btif_hf_call_setup_t hfchan_callSetup[BT_DEVICE_NUM];
    btif_hf_call_active_t hfchan_call[BT_DEVICE_NUM];
    btif_audio_state_t hf_audio_state[BT_DEVICE_NUM];
    btif_hf_call_held_state hf_callheld[BT_DEVICE_NUM];
    uint32_t hf_callsetup_time[BT_DEVICE_NUM];
#if defined (__HSP_ENABLE__)
    enum BT_DEVICE_ID_T curr_hs_channel_id;
    HsCallActiveState hschan_call[BT_DEVICE_NUM];
    HsAudioConnectState hs_audio_state[BT_DEVICE_NUM];
#endif
#ifdef BTIF_AVRCP_ADVANCED_CONTROLLER
    uint32_t avrcp_advancedPdu_size;
    void *avrcp_cmd1[BT_DEVICE_NUM];
    void *avrcp_cmd2[BT_DEVICE_NUM];
    void *avrcp_get_capabilities_rsp[BT_DEVICE_NUM];
    void *avrcp_control_rsp[BT_DEVICE_NUM];
    void *avrcp_notify_rsp[BT_DEVICE_NUM];
    uint8_t volume_report[BT_DEVICE_NUM];
    void *avrcp_volume_cmd[BT_DEVICE_NUM];

    void *avrcp_custom_cmd[BT_DEVICE_NUM];
#ifdef AVRCP_TRACK_CHANGED
    uint8_t track_changed[BT_DEVICE_NUM];
#endif
#endif
#ifdef __TWS__
    void *tws_notify_rsp;
#endif

    uint8_t hf_conn_flag[BT_DEVICE_NUM];
    uint8_t hf_voice_en[BT_DEVICE_NUM];
    uint8_t hf_endcall_dis[BT_DEVICE_NUM];
    uint8_t hf_mute_flag;
    uint8_t phone_earphone_mark;
#if defined (__HSP_ENABLE__)
    uint8_t hs_conn_flag[BT_DEVICE_NUM];
    uint8_t hs_voice_en[BT_DEVICE_NUM];
    uint8_t hs_mute_flag;
#endif
#ifdef BTIF_HID_DEVICE
    HidChannel  hid_channel[BT_DEVICE_NUM];
#endif

    uint8_t callSetupBitRec;
};



struct BT_DEVICE_ID_DIFF{
    enum BT_DEVICE_ID_T id;
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T id_other;
#endif
};

/////app key handle include
void a2dp_handleKey(uint8_t a2dp_key);
void hfp_handle_key(uint8_t hfp_key);
void hsp_handle_key(uint8_t hsp_key);
void btapp_a2dp_report_speak_gain(void);

#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
#define   BTAPP_FUNC_KEY			APP_KEY_CODE_FN1
#define   BTAPP_VOLUME_UP_KEY		APP_KEY_CODE_FN2
#define   BTAPP_VOLUME_DOWN_KEY		APP_KEY_CODE_FN3
#ifdef SUPPORT_SIRI
#define   BTAPP_RELEASE_KEY			APP_KEY_CODE_NONE
#endif
#else
#define   BTAPP_FUNC_KEY			APP_KEY_CODE_PWR
#define   BTAPP_VOLUME_UP_KEY		APP_KEY_CODE_FN1
#define   BTAPP_VOLUME_DOWN_KEY		APP_KEY_CODE_FN2
#ifdef SUPPORT_SIRI
#define   BTAPP_RELEASE_KEY			APP_KEY_CODE_NONE
#endif
#endif
void bt_key_init(void);
void bt_key_send(APP_KEY_STATUS *status);
void bt_key_handle(void);

void a2dp_callback(a2dp_stream_t *Stream, const a2dp_callback_parms_t *Info);
void avrcp_init(void);
//void avrcp_callback(AvrcpChannel *chnl, const AvrcpCallbackParms *Parms);

void avrcp_callback_CT(btif_avrcp_chnl_handle_t  chnl, const avrcp_callback_parms_t *Parms);
void avrcp_callback_TG(btif_avrcp_chnl_handle_t chnl, const avrcp_callback_parms_t *Parms);
int a2dp_volume_get(enum BT_DEVICE_ID_T id);
int a2dp_volume_get_tws(void);
bool avrcp_get_tg_play_status_play_pause(void);
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE) || defined(__APP_A2DP_SOURCE__)
#ifdef __TWS__
//void avrcp_set_slave_volume(uint8_t transid,int8_t volume);
#endif


//#define AVRCP_TRACK_CHANGED
void a2dp_set_config_codec(btif_avdtp_codec_t *config_codec,const btif_a2dp_callback_parms_t *Info);

void a2dp_callback_source(a2dp_stream_t *Stream, const a2dp_callback_parms_t *Info);
void app_a2dp_source_init(void);
void app_source_init(void);

void app_a2dp_source_find_sink(void);
void avrcp_source_callback_TG(btif_avrcp_chnl_handle_t chnl, const avrcp_callback_parms_t *Parms);
void app_a2dp_start_stream(void);
void app_a2dp_suspend_stream(void);
#endif

/**
 * Convert BES BD_ADDR to virtual
 * BES Device ID
 */
bool a2dp_id_from_bdaddr(bt_bdaddr_t *bd_addr, uint8_t *id);

void bt_key_handle_func_click();
void bt_key_handle_func_doubleclick();
void bt_key_handle_func_longpress();


void bt_sbc_player_set_codec_type(uint8_t type);
uint8_t bt_sbc_player_get_codec_type(void);
uint8_t bt_sbc_player_get_sample_bit(void);
#if defined(A2DP_LDAC_ON)
int bt_ldac_player_get_channelmode(void);
int bt_get_ladc_sample_rate(void);
#endif

uint8_t app_bt_avrcp_get_notify_trans_id(void);
void app_bt_avrcp_set_notify_trans_id(uint8_t trans_id);
uint8_t app_bt_avrcp_get_ctl_trans_id(void);
void app_bt_avrcp_set_ctl_trans_id(uint8_t trans_id);


#if defined(IBRT)
#if defined(ENHANCED_STACK)
uint32_t app_avrcp_save_ctxs(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_avrcp_restore_ctxs(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
#endif /* ENHANCED_STACK */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BTAPP_H__ */
