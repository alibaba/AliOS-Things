/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _AVDTP_API_H
#define _AVDTP_API_H

#include "stdint.h"
#include "bluetooth.h"

/* Signal Commands */
#define BTIF_AVDTP_SIG_DISCOVER             0x01
#define BTIF_AVDTP_SIG_GET_CAPABILITIES     0x02
#define BTIF_AVDTP_SIG_SET_CONFIG           0x03
#define BTIF_AVDTP_SIG_GET_CONFIG           0x04
#define BTIF_AVDTP_SIG_RECONFIG             0x05
#define BTIF_AVDTP_SIG_OPEN                 0x06
#define BTIF_AVDTP_SIG_START                0x07
#define BTIF_AVDTP_SIG_CLOSE                0x08
#define BTIF_AVDTP_SIG_SUSPEND              0x09
#define BTIF_AVDTP_SIG_ABORT                0x0A
#define BTIF_AVDTP_SIG_SECURITY_CTRL        0x0B
#define BTIF_AVDTP_SIG_GET_ALL_CAPABILITIES 0x0C
#define BTIF_AVDTP_SIG_DELAYREPORT          0x0D

#ifndef  avdtp_codec_t
#define   avdtp_codec_t void
#endif
#ifndef avdtp_channel_t
#define   avdtp_channel_t void
#endif

typedef uint8_t btif_avdtp_codec_type_t;
typedef uint16_t btif_avdtp_content_prot_type_t;
typedef uint8_t btif_avdtp_capability_type_t;

#define BTIF_AVDTP_CP_TYPE_DTCP      0x0001

#define BTIF_AVDTP_CP_TYPE_SCMS_T    0x0002

#define BTIF_AVDTP_SRV_CAT_MEDIA_TRANSPORT      0x01
#define BTIF_AVDTP_SRV_CAT_REPORTING            0x02
#define BTIF_AVDTP_SRV_CAT_RECOVERY             0x03
#define BTIF_AVDTP_SRV_CAT_CONTENT_PROTECTION   0x04
#define BTIF_AVDTP_SRV_CAT_HEADER_COMPRESSION   0x05
#define BTIF_AVDTP_SRV_CAT_MULTIPLEXING         0x06
#define BTIF_AVDTP_SRV_CAT_MEDIA_CODEC          0x07
#define BTIF_AVDTP_SRV_CAT_DELAY_REPORTING      0x08

typedef uint16_t btif_avdtp_codec_sample_rate_t;

#define BTIF_AVDTP_CODEC_TYPE_SBC           0x00

#define BTIF_AVDTP_CODEC_TYPE_MPEG1_2_AUDIO 0x01

#define BTIF_AVDTP_CODEC_TYPE_MPEG2_4_AAC   0x02

#define BTIF_AVDTP_CODEC_TYPE_ATRAC         0x04

#define BTIF_AVDTP_CODEC_TYPE_OPUS          0x08

#define BTIF_AVDTP_CODEC_TYPE_H263          0x01

#define BTIF_AVDTP_CODEC_TYPE_MPEG4_VSP     0x02

#define BTIF_AVDTP_CODEC_TYPE_H263_PROF3    0x03

#define BTIF_AVDTP_CODEC_TYPE_H263_PROF8    0x04

#define BTIF_AVDTP_CODEC_TYPE_LHDC          0xFF

#define BTIF_AVDTP_CODEC_TYPE_NON_A2DP      0xFF

#define BTIF_AVDTP_MAX_CODEC_ELEM_SIZE 10

#define BTIF_AVDTP_MAX_CP_VALUE_SIZE 10

typedef uint8_t btif_avdtp_stream_state_t;

/** The stream is idle and not configured.  Streaming is not possible.
 */
#define BTIF_AVDTP_STRM_STATE_IDLE           0

/** A stream is configured, but not open.  This state will only occur in
 *  certain cases where a request to open the stream is rejected, and the
 *  operation cannot be aborted.
 *
 *  AVDTP_AbortStream() must be called to exit this state.
 */
#define BTIF_AVDTP_STRM_STATE_CONFIGURED     1

/** The stream is open and configured.  Streaming can be initiated after the
 *  stream is open by calling AVDTP_StartStream().
 */
#define BTIF_AVDTP_STRM_STATE_OPEN           2

/** The stream is active.  Stream data (media packets) can be sent only in
 *  this state.
 */
#define BTIF_AVDTP_STRM_STATE_STREAMING      3

#define BTIF_AVDTP_STRM_STATE_CLOSING        4

#define BTIF_AVDTP_STRM_STATE_ABORTING       5

typedef U8 btif_avdtp_error_t;

#define BTIF_AVDTP_ERR_NO_ERROR                    0x00

#define BTIF_AVDTP_ERR_BAD_HEADER_FORMAT           0x01

#define BTIF_AVDTP_ERR_BAD_LENGTH                  0x11

#define BTIF_AVDTP_ERR_BAD_ACP_SEID                0x12

#define BTIF_AVDTP_ERR_IN_USE                      0x13

#define BTIF_AVDTP_ERR_NOT_IN_USE                  0x14

#define BTIF_AVDTP_ERR_BAD_SERV_CATEGORY           0x17

#define BTIF_AVDTP_ERR_BAD_PAYLOAD_FORMAT          0x18

#define BTIF_AVDTP_ERR_NOT_SUPPORTED_COMMAND       0x19

#define BTIF_AVDTP_ERR_INVALID_CAPABILITIES        0x1A

#define BTIF_AVDTP_ERR_BAD_RECOVERY_TYPE           0x22

#define BTIF_AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT  0x23

#define BTIF_AVDTP_ERR_BAD_RECOVERY_FORMAT         0x25

#define BTIF_AVDTP_ERR_BAD_ROHC_FORMAT             0x26

#define BTIF_AVDTP_ERR_BAD_CP_FORMAT               0x27

#define BTIF_AVDTP_ERR_BAD_MULTIPLEXING_FORMAT     0x28

#define BTIF_AVDTP_ERR_UNSUPPORTED_CONFIGURATION   0x29

#define BTIF_AVDTP_ERR_BAD_STATE                   0x31

#define BTIF_AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE    0xC2

#define BTIF_AVDTP_ERR_UNKNOWN_ERROR               0xFF

typedef struct {
    btif_avdtp_content_prot_type_t cpType;
    uint8_t dataLen;
    uint8_t *data;
}__attribute__((packed))  btif_avdtp_content_prot_t;

typedef struct {
    btif_avdtp_codec_type_t codecType;
    uint8_t elemLen;
    uint8_t *elements;
    uint8_t *pstreamflags; 
    bool discoverable;
}__attribute__((packed)) btif_avdtp_codec_t ;

typedef struct {
    btif_avdtp_capability_type_t type;
    union {
        btif_avdtp_codec_t codec;
        btif_avdtp_content_prot_t cp;
    } p;
} btif_avdtp_capability_t;

typedef struct {
    btif_avdtp_codec_t codec;
    btif_avdtp_content_prot_t cp;
    BOOL delayReporting;
} btif_avdtp_config_request_t;


typedef uint8_t btif_avdtp_streamId_t;
typedef uint8_t btif_avdtp_media_type;
typedef uint8_t btif_avdtp_strm_endpoint_type_t;

typedef struct {
    btif_avdtp_streamId_t id;
    bool inUse;
    btif_avdtp_media_type mediaType;
    btif_avdtp_strm_endpoint_type_t streamType;
} btif_avdtp_stream_info_t;

typedef struct {
    U8 version;
    U8 padding;
    U8 marker;
    U8 payloadType;
    U16 sequenceNumber;
    U32 timestamp;
    U32 ssrc;
    U8 csrcCount;
    U32 csrcList[15];
} btif_avdtp_media_header_t;
typedef btif_avdtp_media_header_t btif_media_header_t;

typedef void btif_avdtp_stream_t;

#ifdef __cplusplus
extern "C" {

#endif
    btif_avdtp_codec_type_t btif_avdtp_get_stream_codec_type(btif_avdtp_stream_t * stream);

#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /*  */
