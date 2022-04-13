/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _A2DP_API_H
#define _A2DP_API_H
#include "bluetooth.h"
#include "avrcp_api.h"

#include "avdtp_api.h"
#include "conmgr_api.h"
#include "codec_sbc.h"

typedef enum{
    BTIF_DEVICE_ID_1 = 0,
#ifdef __BT_ONE_BRING_TWO__
    BTIF_DEVICE_ID_2,
#endif
    BTIF_DEVICE_NUM
}btif_dev_it_e;

typedef void a2dp_stream_t;
typedef void a2dp_callback_parms_t;

typedef uint8_t btif_a2dp_event_t;

#define A2DP_AAC_OCTET_NUMBER                     (6)
#define A2DP_AAC_OCTET0_MPEG2_AAC_LC              0x80
#define A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100  0x01
#define A2DP_AAC_OCTET2_CHANNELS_1                0x08
#define A2DP_AAC_OCTET2_CHANNELS_2                0x04
#define A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000  0x80
#define A2DP_AAC_OCTET3_VBR_SUPPORTED             0x80
/**  Accept or reject by calling btif_a2dp_open_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_OPEN_IND           1

/** This event is  the response to a call from   btif_a2dp_open_stream() or
 *  btif_a2dp_open_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_OPEN               2

/** This event is received during the establishment of an
 *  outgoing stream (btif_a2dp_open_stream()) to indicate the capabilities of the
 *  remote device.  This event may also be received after calling
 *  btif_a2dp_get_stream_capabilities().
 */
#define BTIF_A2DP_EVENT_CODEC_INFO                3

/** This event is received  during the establishment of an
 *  outgoing stream (btif_a2dp_open_stream()) to indicated the capabilities of the
 *  remote device.  This event may also be received after calling
 *  btif_a2dp_get_stream_capabilities().
 *
 */
#define BTIF_A2DP_EVENT_CP_INFO                   4

/**  If this event is received, then the remote device
 *  supports the Delay Reporting feature.
 */
#define BTIF_A2DP_EVENT_DELAY_REPORTING_INFO      5

/** This event is received  when a stream is opening and
 *  all matching capabilities have been found.  The application must call
 *  btif_a2dp_set_stream_config() to configure the stream.  If successful, the
 *  stream will be opened.  btif_a2dp_close_stream() can also be called to close
 *  the stream.  This event is only received on outgoing connections.
 */
#define BTIF_A2DP_EVENT_GET_CONFIG_IND            6

/** This event is received after calling btif_a2dp_get_stream_capabilities().  This
 *  event is received after all the capabilities of the remote device have
 *  been indicated (see BTIF_A2DP_EVENT_CODEC_INFO and BTIF_A2DP_EVENT_CP_INFO).
 */
#define BTIF_A2DP_EVENT_GET_CAP_CNF               7

/** This event is received  when an open stream has been
 *  closed.  This can happen as a result of a call to btif_a2dp_close_stream(), if
 *  the stream has been closed by the remote device, if a link loss has been
 *  detected, or if the remote device rejects a request to open the stream.
 *
 */
#define BTIF_A2DP_EVENT_STREAM_CLOSED             8

/** This event is received  when an open stream has been
 *  set to the idle state.   result of a call to btif_a2dp_idle_stream().
 */
#define BTIF_A2DP_EVENT_STREAM_IDLE               9

/** When the remote device requests streaming to begin, this event will be
 *  received .  The application can accept or reject
 *  this request by calling btif_a2dp_start_stream_rsp(). (Note: this event
 *  will be received only after BTIF_A2DP_EVENT_STREAM_OPEN_IND but it may arrive
 *  before BTIF_A2DP_EVENT_STREAM_OPEN.)
 */
#define BTIF_A2DP_EVENT_STREAM_START_IND         10

/** When streaming has been started, this event will be received.  This can happen as the result to a call to
 *  btif_a2dp_start_stream() or btif_a2dp_start_stream_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_STARTED           11

/** When streaming has been suspended, this event will be received .  as the result to a call to
 *  btif_a2dp_suspend_stream(), or when the remote device suspends the stream.
 */
#define BTIF_A2DP_EVENT_STREAM_SUSPENDED         12

/** When the remote device wishes to reconfigure an open stream, this event
 *  is received by the application.  The application can accept or reject
 *  the request by calling btif_a2dp_reconfig_stream_rsp().
 *
 */
#define BTIF_A2DP_EVENT_STREAM_RECONFIG_IND      13

/** When an open stream is reconfigured, this event is received by the
 *  application.
 *
 */
#define BTIF_A2DP_EVENT_STREAM_RECONFIG_CNF      14

/** This event is received when the remote device requests the security
 *  process to begin.  The application responds to this request by calling
 *  btif_a2dp_security_control_rsp().
 */
#define BTIF_A2DP_EVENT_STREAM_SECURITY_IND      15

/** This event is received  when the remote device responds
 *  to the security process request.
 */
#define BTIF_A2DP_EVENT_STREAM_SECURITY_CNF      16

/** When the stream is aborted, this event is received by the application.
 *  This can happen in response to a request from the remote device to abort
 *  the stream, or as the result of a call to btif_a2dp_abort_stream().  When a
 *  stream is aborted, the stream is closed.
 */
#define BTIF_A2DP_EVENT_STREAM_ABORTED           17

/** This event is received when stream data has been received from the remote
 *  device.  The data is raw and is not parsed by A2DP.  It should contain
 *  a single media packet.
 */
#define BTIF_A2DP_EVENT_STREAM_DATA_IND          18

/** This event is received when raw data has been sent to the remote device.
 *  This happens as the result of a call to btif_a2dp_stream_send_raw_packet().
 *
 */
#define BTIF_A2DP_EVENT_STREAM_PACKET_SENT       19

/** This event is received when SBC data has been sent to the remote device.
 *  This happens as the result of a call to btif_a2dp_stream_send_sbc_packet().
 */
#define BTIF_A2DP_EVENT_STREAM_SBC_PACKET_SENT   20

/** This event is received by a Source when the Sink reports the value of its
 *  buffer/processing delay.  This may happen when the stream is configured
 *  (or reconfigured), and when the stream is in the streaming state.
 *
 */
#define BTIF_A2DP_EVENT_DELAY_REPORT_IND         21

/** This event is received by a Sink when the Source acknowldeges the
 *  transmitted buffer/processing delay.
 */
#define BTIF_A2DP_EVENT_DELAY_REPORT_CNF         22

/** When the the lower level AVDTP connection is established between the
 *  local and remote devices, this event will be generated.
 *
 *  During this callback, the 'p.device' parameter contains a pointer to the
 *  device that was connected.
 */
#define BTIF_A2DP_EVENT_AVDTP_CONNECT            23

/** When the the lower level AVDTP connection is disconnected, this event
 *  will be generated.
 *
 */
#define BTIF_A2DP_EVENT_AVDTP_DISCONNECT         24

#if  BTIF_AV_WORKER
/* Confirms  has received configuration information after an
 * btif_avdtp_get_config() request.
 */
#define BTIF_A2DP_AVDTP_EVENT_GET_CFG_CNF        27

/* Confirms  has sent configuration information after an
 * btif_avdtp_open_stream() request.
 */
#define BTIF_A2DP_AVDTP_EVENT_SET_CFG_CNF        28

#endif

#define BTIF_A2DP_EVENT_STREAM_STARTED_MOCK      29

//user define @biao
#define BTIF_A2DP_EVENT_AVDTP_CLOSE_IND          81
#define BTIF_A2DP_EVENT_AVDTP_DISCOVER_IND       82
#define BTIF_A2DP_EVENT_AVDTP_DISCOVER_RSP       83
//#define AVDTP_EVENT_STREAM_CLOSE_IND

/* event that let app level to see a stream is good to select */
#define BTIF_A2DP_EVENT_STREAM_SELECT            84
/* event that let app level to confirm stream(sep) state */
#define BTIF_A2DP_EVENT_STREAM_STATE_CFM         85

typedef uint8_t btif_a2dp_error_t;

#define BTIF_A2DP_ERR_NO_ERROR                         0x00

#define BTIF_A2DP_ERR_BAD_SERVICE                      0x80

#define BTIF_A2DP_ERR_INSUFFICIENT_RESOURCE            0x81

#define BTIF_A2DP_ERR_INVALID_CODEC_TYPE               0xC1

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE   AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE

#define BTIF_A2DP_ERR_INVALID_SAMPLING_FREQUENCY       0xC3

#define BTIF_A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ          0xC4

/** Channel mode not valid
 *
 *  SBC
 *  MPEG-1,2 Audio
 *  ATRAC family
 */
#define BTIF_A2DP_ERR_INVALID_CHANNEL_MODE             0xC5

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE       0xC6

#define BTIF_A2DP_ERR_INVALID_SUBBANDS                 0xC7

#define BTIF_A2DP_ERR_NOT_SUPPORTED_SUBBANDS           0xC8

#define BTIF_A2DP_ERR_INVALID_ALLOCATION_METHOD        0xC9

#define BTIF_A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD       0xCA

#define BTIF_A2DP_ERR_INVALID_MIN_BITPOOL_VALUE        0xCB

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE  0xCC

#define BTIF_A2DP_ERR_INVALID_MAX_BITPOOL_VALUE        0xCD

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE  0xCE

#define BTIF_A2DP_ERR_INVALID_LAYER                    0xCF

#define BTIF_A2DP_ERR_NOT_SUPPORTED_LAYER              0xD0

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CRC                0xD1

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MPF                0xD2

#define BTIF_A2DP_ERR_NOT_SUPPORTED_VBR                0xD3

#define BTIF_A2DP_ERR_INVALID_BIT_RATE                 0xD4

#define BTIF_A2DP_ERR_NOT_SUPPORTED_BIT_RATE           0xD5

#define BTIF_A2DP_ERR_INVALID_OBJECT_TYPE              0xD6

#define BTIF_A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE        0xD7

#define BTIF_A2DP_ERR_INVALID_CHANNELS                 0xD8

#define BTIF_A2DP_ERR_NOT_SUPPORTED_CHANNELS           0xD9

#define A2DP_SCALABLE_OCTET_NUMBER (7)

#define BTIF_A2DP_ERR_INVALID_VERSION                  0xDA

#define BTIF_A2DP_ERR_NOT_SUPPORTED_VERSION            0xDB

#define BTIF_A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL        0xDC

#define BTIF_A2DP_ERR_INVALID_BLOCK_LENGTH             0xDD

#define BTIF_A2DP_ERR_INVALID_CP_TYPE                  0xE0

#define BTIF_A2DP_ERR_INVALID_CP_FORMAT                0xE1

#define BTIF_A2DP_ERR_UNKNOWN_ERROR                    AVDTP_ERR_UNKNOWN_ERROR

typedef U16 btif_a22dp_version_t;

typedef U16 btif_a2dp_features_t;

/* Audio Player */
#define BTIF_A2DP_SRC_FEATURE_PLAYER    0x01

/* Microphone */
#define BTIF_A2DP_SRC_FEATURE_MIC       0x02

/* Tuner */
#define BTIF_A2DP_SRC_FEATURE_TUNER     0x04

/* Mixer */
#define BTIF_A2DP_SRC_FEATURE_MIXER     0x08

/* Headphones */
#define BTIF_A2DP_SNK_FEATURE_HEADPHONE 0x01

/* Loudspeaker */
#define BTIF_A2DP_SNK_FEATURE_SPEAKER   0x02

/* Audio Recorder */
#define BTIF_A2DP_SNK_FEATURE_RECORDER  0x04

/* Amplifier */
#define BTIF_A2DP_SNK_FEATURE_AMP       0x08

typedef U8 btif_a2dp_endpoint_type_t;

/* The stream is a source */
#define BTIF_A2DP_STREAM_TYPE_SOURCE  0

/* The stream is a sink */
#define BTIF_A2DP_STREAM_TYPE_SINK    1

typedef uint8_t btif_a2dp_stream_state_t;

/* The stream is closed */
#define BTIF_A2DP_STREAM_STATE_CLOSED     0

/* The stream is idle */
#define BTIF_A2DP_STREAM_STATE_IDLE       1

/* The stream is open */
#define BTIF_A2DP_STREAM_STATE_OPEN       2

/* The stream is streaming */
#define BTIF_A2DP_STREAM_STATE_STREAMING  3

/* Unknown state */
#define BTIF_A2DP_STREAM_STATE_UNKNOWN    0xFF

typedef void (*btif_a2dp_callback) (a2dp_stream_t * Stream, const a2dp_callback_parms_t * Info);


typedef void btif_av_device_t;

struct btif_get_codec_cap_t
{
    uint8_t ** cap;
    uint16_t * cap_len;
    bool     done;
};

typedef struct {
    btif_a2dp_event_t event;
    uint16_t len;
    I8 status;
    btif_a2dp_error_t error;
    btif_a2dp_error_t discReason;
    union {
        btif_avdtp_content_prot_t *cp;
        btif_avdtp_codec_t *codec;
        btif_avdtp_capability_t *capability;
        btif_avdtp_config_request_t *configReq; /* AVDTP Config request          */
        btif_bt_packet_t *btPacket; /* Raw Transmit packet           */
        btif_a2dp_sbc_packet_t *sbcPacket;  /* SBC Transmit packet           */
        uint8_t *data;          /* Receive data                  */
        uint16_t delayMs;       /* Buffering/processing delay of
                                 * the Sink.
                                 */
        btif_avdtp_stream_info_t *streamInfo;   /* Stream information */
        btif_av_device_t *device;   /* The connecting or disconnectin device. */
        struct btif_get_codec_cap_t *get_cap;

        a2dp_stream_t    *dstStream;  /* Stream to switch */
    } p;
     btif_remote_device_t   *remDev;
} btif_a2dp_callback_parms_t;

typedef struct {
    list_entry_t node;          /* Used internally by A2DP. */
    btif_avdtp_stream_info_t info;  /* Stream information */
} btif_a2dp_streamInfo_t;

typedef struct {
#if defined(__TWS__)
    btif_avdtp_codec_t setconfig_codec;
#endif /*  */
    bool free;
    uint8_t state;
    a2dp_stream_t *a2dp_stream; //stack A2dpStream  object
} btif_a2dp_stream_t;



#ifdef __cplusplus
extern "C" {
#endif

    void btif_a2dp_init(void);

    int a2dp_hid_init(void);

    btif_a2dp_stream_t *btif_a2dp_alloc_stream(void);

    uint16_t btif_avdtp_parse_mediaHeader(btif_media_header_t * header,
                                          btif_a2dp_callback_parms_t * Info, uint8_t avdtp_cp);

    void btif_app_a2dp_avrcpadvancedpdu_mempool_init();

    void btif_app_a2dp_avrcpadvancedpdu_mempool_calloc(void **buf);

    void btif_app_a2dp_avrcpadvancedpdu_mempool_free(void *buf);

    void a2dp_set_config_codec(btif_avdtp_codec_t * config_codec,
                               const btif_a2dp_callback_parms_t * Info);

    btif_avdtp_codec_type_t btif_a2dp_get_stream_register_codec_type(a2dp_stream_t *stream);

    bt_status_t btif_a2dp_register(a2dp_stream_t *Stream, 
                                   btif_a2dp_endpoint_type_t stream_type,
                                   btif_avdtp_codec_t *Codec,
                                   btif_avdtp_content_prot_t *Prot,                               
                                   uint8_t Priority,
                                   uint8_t Device_id,
                                   btif_a2dp_callback Callback);

    bt_status_t btif_a2dp_add_content_protection(a2dp_stream_t *Stream, btif_avdtp_content_prot_t *Prot);

    bt_status_t btif_a2dp_deregister(a2dp_stream_t * Stream);

    int8_t btif_a2dp_is_register_codec_same(a2dp_stream_t *src_stream, a2dp_stream_t *dst_stream);

    void btif_a2dp_lhdc_config_tws_audio(const a2dp_callback_parms_t * info);

    void btif_a2dp_aac_config_tws_audio(const a2dp_callback_parms_t * info);

    void btif_a2dp_sbc_config_tws_audio(const a2dp_callback_parms_t * info);

    void btif_a2dp_tws_set_mobile_codec_info(const a2dp_callback_parms_t * info);

    btif_remote_device_t *btif_a2dp_get_remote_device(a2dp_stream_t * stream);

    uint8_t *btif_a2dp_get_stream_devic_cmgrHandler_remdev_bdAddr(a2dp_stream_t * Stream);

    void *btif_a2dp_get_stream_device(a2dp_stream_t * Stream);

    void *btif_a2dp_get_stream_devic_cmgrHandler_bt_handler(a2dp_stream_t * Stream);

    void *btif_a2dp_get_stream_devic_cmgrHandler_remdev(a2dp_stream_t * Stream);

    uint8_t btif_a2dp_get_stream_devic_cmgrHandler_remdev_role(a2dp_stream_t * Stream);

    btif_cmgr_handler_t *btif_a2dp_get_stream_devic_cmgrHandler(a2dp_stream_t * Stream);

    void btif_me_set_sniffer_env(uint8_t sniffer_acitve, uint8_t sniffer_role,
                                 uint8_t * monitored_addr, uint8_t * sniffer_addr);

    uint16_t btif_me_get_remote_device_hci_handle(btif_remote_device_t * rdev);

    bt_bdaddr_t *btif_a2dp_stream_conn_remDev_bdAddr(a2dp_stream_t * Stream);

    uint8_t *btif_a2dp_get_remote_device_version(btif_remote_device_t * remDev);

    btif_a2dp_event_t btif_a2dp_get_cb_event(a2dp_callback_parms_t * info);

    bt_status_t btif_a2dp_set_sink_delay(a2dp_stream_t * Stream, U16 delayMs);

    bt_status_t btif_a2dp_set_stream_config(a2dp_stream_t * Stream,
                                            btif_avdtp_codec_t * Codec,
                                            btif_avdtp_content_prot_t * Cp);

    bt_status_t btif_a2dp_open_stream(a2dp_stream_t * Stream, bt_bdaddr_t * Addr);

    bt_status_t btif_a2dp_disc_stream(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_start_stream(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_idle_stream(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_suspend_stream(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_start_stream_rsp(a2dp_stream_t * Stream, btif_a2dp_error_t error);

    bt_status_t btif_a2dp_close_stream(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_reconfig_stream_rsp(a2dp_stream_t * Stream,
                                              btif_a2dp_error_t Error,
                                              btif_avdtp_capability_type_t Type);

    bt_status_t btif_a2dp_reconfig_stream(a2dp_stream_t * Stream,
                                          btif_avdtp_codec_t * codec_cfg,
                                          btif_avdtp_content_prot_t * cp);

    uint8_t btif_a2dp_security_control_req(a2dp_stream_t *stream, uint8_t *data, uint16_t len);
    uint8_t btif_a2dp_security_control_rsp(a2dp_stream_t *stream,uint8_t* data,uint16_t len, uint8_t error);

    bt_status_t btif_a2dp_open_stream_rsp(a2dp_stream_t * Stream,
                                          btif_a2dp_error_t Error,
                                          btif_avdtp_capability_type_t CapType);

    bool btif_a2dp_stream_has_remote_device(btif_a2dp_stream_t * stream);

    bt_bdaddr_t *btif_a2dp_stream_get_remote_bd_addr(btif_a2dp_stream_t * stream);

    btif_a2dp_stream_t *btif_get_a2dp_stream(a2dp_stream_t * stream);

    btif_a2dp_endpoint_type_t btif_a2dp_get_stream_type(a2dp_stream_t * Stream);

    btif_avdtp_stream_state_t btif_a2dp_get_stream_state(a2dp_stream_t * Stream);

    uint8_t btif_a2dp_get_stream_loc_strmId(a2dp_stream_t * Stream);

    uint8_t btif_a2dp_get_stream_rem_strmId(a2dp_stream_t * Stream);

    uint16_t btif_a2dp_get_stream_chnl_sigchnl_l2ChannelId(a2dp_stream_t * Stream);

    void btif_a2dp_set_stream_state(a2dp_stream_t * Stream, btif_avdtp_stream_state_t state);

    void btif_a2dp_set_stream_conn_l2ChannelId(a2dp_stream_t * Stream, uint16_t id);

    void btif_a2dp_set_stream_chnl_conn_l2ChannelId(a2dp_stream_t * Stream, uint16_t id);

    void btif_a2dp_set_stream_chnl_sigChnl_l2ChannelId(a2dp_stream_t * Stream, uint16_t id);

    void btif_a2dp_set_stream_loc_streamId(a2dp_stream_t * Stream, uint8_t id);

    void btif_a2dp_set_stream_remote_streamId(a2dp_stream_t * Stream, uint8_t id);

    void btif_a2dp_discover(a2dp_stream_t* Stream);

    BOOL btif_a2dp_is_stream_device_has_delay_reporting(a2dp_stream_t * Stream);

    btif_avdtp_codec_t *btif_a2dp_get_stream_codec(a2dp_stream_t * Stream);

    btif_avdtp_codec_t *btif_a2dp_get_stream_codecCfg(a2dp_stream_t * Stream);

    uint16_t btif_a2dp_get_stream_conn_remDev_hciHandle(a2dp_stream_t * Stream);

    uint16_t btif_a2dp_get_stream_device_cmgrhandler_remDev_hciHandle(a2dp_stream_t * Stream);

    btif_avdtp_codec_t *btif_a2dp_get_avdtp_setconfig_codec(a2dp_stream_t * stream);

    bt_status_t btif_a2dp_get_stream_capabilities(a2dp_stream_t * Stream);

    bt_status_t btif_a2dp_stream_send_sbc_packet(a2dp_stream_t * stream,
                                                 btif_a2dp_sbc_packet_t * Packet,
                                                 btif_sbc_stream_info_short_t * StreamInfo);

    void btif_a2dp_sync_avdtp_streaming_state(bt_bdaddr_t *addr);

    void btif_app_a2dp_source_init(void);

    uint8_t btif_a2dp_get_cb_error(const btif_a2dp_callback_parms_t * Info);

    uint8_t btif_a2dp_set_dst_stream(a2dp_callback_parms_t *Info, a2dp_stream_t *stream);

    btif_remote_device_t *btif_a2dp_get_stream_conn_remDev(a2dp_stream_t * stream);

    uint8_t btif_a2dp_get_stream_codec_element(a2dp_stream_t * stream,uint8_t index);

    bt_status_t btif_a2dp_set_master_role(a2dp_stream_t * Stream, BOOL Flag);

    uint16_t btif_a2dp_get_stream_conn_l2ChannelId(a2dp_stream_t * Stream);

    btif_media_header_t *btif_a2dp_get_stream_media_header(a2dp_stream_t * stream);

    int tws_if_get_a2dpbuff_available(void);

    int a2dp_codec_init(void);
    void  btif_a2dp_lock_same_deviceid_endpoint(a2dp_stream_t  * Stream,btif_remote_device_t   *remDev);
    void  btif_a2dp_unlock_same_deviceid_endpoint(a2dp_stream_t  * Stream,btif_remote_device_t   *remDev);
    void  btif_a2dp_unlock_the_connected_stream_byRemdev(btif_remote_device_t* RemDev);

    uint8_t btif_a2dp_trylock_the_other_id_by_configedid(btif_remote_device_t* RemDev);

    uint8_t btif_a2dp_confirm_stream_state(a2dp_stream_t *Stream, uint8_t old_state, uint8_t new_state);

    void  btif_a2dp_lock_deviceid_endpoint(uint8_t id);

    void  btif_a2dp_unlock_deviceid_endpoint(uint8_t unused_id);

    uint8_t btif_a2dp_get_avrcpadvancedpdu_trans_id(void* pdu);
    void btif_a2dp_set_avrcpadvancedpdu_trans_id(void* pdu, uint8_t trans_id);

    btif_remote_device_t *btif_a2dp_get_remote_device_from_cbparms(a2dp_stream_t *Stream, const a2dp_callback_parms_t *info);

    btif_avdtp_codec_type_t btif_a2dp_get_codec_type(const a2dp_callback_parms_t *info);
#if defined(IBRT)
    void btif_reset_app_bt_device_streaming(uint8_t bt_dev_idx, uint8_t rm_detbl_idx , uint8_t avd_ctx_device_idx);
    void btif_a2dp_set_codec_info(btif_dev_it_e dev_num, btif_avdtp_codec_type_t codec_type, uint8_t sample_rate, uint8_t sample_bit);
#endif
#if defined(IBRT)
#if defined(ENHANCED_STACK)
    uint32_t btif_a2dp_profile_save_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
    uint32_t btif_a2dp_profile_restore_ctx(uint8_t *buf, uint32_t buf_len);
#endif /* *ENHANCED_STACK */
#endif /* IBRT */

    /* Callout functions, do not call directly */
    uint8_t a2dp_stream_confirm_stream_state(uint8_t index, uint8_t old_state, uint8_t new_state);
    uint8_t a2dp_stream_locate_the_connected_dev_id(a2dp_stream_t *Stream);

#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /*  */
