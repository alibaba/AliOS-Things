/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef	__SDP_API_H__
#define	__SDP_API_H__

#include "me_api.h"

#define BTIF_SDP_RECORD_NUM_MAX (10)
#define BTIF_SDP_QUERY_TOKEN_NUM_MAX (4)

typedef void btif_sdp_record_t;

typedef uint8_t sdp_query_type;
#define BTIF_BSQT_ATTRIB_REQ                 0x04
#define BTIF_BSQT_SERVICE_SEARCH_ATTRIB_REQ  0x06
typedef uint8_t sdp_query_resp;
#define BTIF_BSQR_ERROR_RESP                 0x01
#define BTIF_BSQR_SERVICE_SEARCH_RESP        0x03
#define BTIF_BSQR_ATTRIB_RESP                0x05
#define BTIF_BSQR_SERVICE_SEARCH_ATTRIB_RESP 0x07

#if SDP_SERVER_SUPPORT == XA_ENABLED
#define BTIF_SDP_SERVER_ATTRIBS 3
#define BTIF_SDP_SERVER_TX_SIZE (SDP_SERVER_SEND_SIZE - 5)
#define BTIF_SDP_SERV_OWN_HANDLE     0x00000000
#define BTIF_SDP_SERV_FIRST_HANDLE   0x00010000
#define BTIF_SDP_SERV_BOGUS_HANDLE   0xffffffff
#define BTIF_SDP_ZERO_SERVICE_DATABASE_STATE_SIZE 5
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

typedef uint16_t sdp_service_class_uuid;
#define BTIF_SC_SERVICE_DISCOVERY_SERVER             0x1000
#define BTIF_SC_BROWSE_GROUP_DESC                    0x1001
#define BTIF_SC_PUBLIC_BROWSE_GROUP                  0x1002
#define BTIF_SC_SERIAL_PORT                          0x1101
#define BTIF_SC_LAN_ACCESS_PPP                       0x1102
#define BTIF_SC_DIALUP_NETWORKING                    0x1103
#define BTIF_SC_IRMC_SYNC                            0x1104
#define BTIF_SC_OBEX_OBJECT_PUSH                     0x1105
#define BTIF_SC_OBEX_FILE_TRANSFER                   0x1106
#define BTIF_SC_IRMC_SYNC_COMMAND                    0x1107
#define BTIF_SC_HEADSET                              0x1108
#define BTIF_SC_CORDLESS_TELEPHONY                   0x1109
#define BTIF_SC_AUDIO_SOURCE                         0x110A
#define BTIF_SC_AUDIO_SINK                           0x110B
#define BTIF_SC_AV_REMOTE_CONTROL_TARGET             0x110C
#define BTIF_SC_AUDIO_DISTRIBUTION                   0x110D
#define BTIF_SC_AV_REMOTE_CONTROL                    0x110E
#define BTIF_SC_VIDEO_CONFERENCING                   0x110F
#define BTIF_SC_INTERCOM                             0x1110
#define BTIF_SC_FAX                                  0x1111
#define BTIF_SC_HEADSET_AUDIO_GATEWAY                0x1112
#define BTIF_SC_WAP                                  0x1113
#define BTIF_SC_WAP_CLIENT                           0x1114
#define BTIF_SC_PANU                                 0x1115
#define BTIF_SC_NAP                                  0x1116
#define BTIF_SC_GN                                   0x1117
#define BTIF_SC_DIRECT_PRINTING                      0x1118
#define BTIF_SC_REFERENCE_PRINTING                   0x1119
#define BTIF_SC_IMAGING                              0x111A
#define BTIF_SC_IMAGING_RESPONDER                    0x111B
#define BTIF_SC_IMAGING_AUTOMATIC_ARCHIVE            0x111C
#define BTIF_SC_IMAGING_REFERENCED_OBJECTS           0x111D
#define BTIF_SC_HANDSFREE                            0x111E
#define BTIF_SC_HANDSFREE_AUDIO_GATEWAY              0x111F
#define BTIF_SC_DIRECT_PRINTING_REF_OBJECTS          0x1120
#define BTIF_SC_REFLECTED_UI                         0x1121
#define BTIF_SC_BASIC_PRINTING                       0x1122
#define BTIF_SC_PRINTING_STATUS                      0x1123
#define BTIF_SC_HUMAN_INTERFACE_DEVICE               0x1124
#define BTIF_SC_HCR                                  0x1125
#define BTIF_SC_HCR_PRINT                            0x1126
#define BTIF_SC_HCR_SCAN                             0x1127
#define BTIF_SC_ISDN                                 0x1128
#define BTIF_SC_VIDEO_CONFERENCING_GW                0x1129
#define BTIF_SC_UDI_MT                               0x112A
#define BTIF_SC_UDI_TA                               0x112B
#define BTIF_SC_AUDIO_VIDEO                          0x112C
#define BTIF_SC_SIM_ACCESS                           0x112D
#define BTIF_SC_PBAP_CLIENT                          0x112E
#define BTIF_SC_PBAP_SERVER                          0x112F
#define BTIF_SC_PBAP_PROFILE                         0x1130
#define BTIF_SC_MAP_SERVER                           0x1132
#define BTIF_SC_MAP_NOTIFY_SERVER                    0x1133
#define BTIF_SC_MAP_PROFILE                          0x1134
#define BTIF_SC_PNP_INFO                             0x1200
#define BTIF_SC_GENERIC_NETWORKING                   0x1201
#define BTIF_SC_GENERIC_FILE_TRANSFER                0x1202
#define BTIF_SC_GENERIC_AUDIO                        0x1203
#define BTIF_SC_GENERIC_TELEPHONY                    0x1204
#define BTIF_SC_UPNP_SERVICE                         0x1205
#define BTIF_SC_UPNP_IP_SERVICE                      0x1206
#define BTIF_SC_ESDP_UPNP_IP_PAN                     0x1300
#define BTIF_SC_ESDP_UPNP_IP_LAP                     0x1301
#define BTIF_SC_ESDP_UPNP_L2CAP                      0x1302
#define BTIF_SC_VIDEO_SOURCE                         0x1303
#define BTIF_SC_VIDEO_SINK                           0x1304
#define BTIF_SC_VIDEO_DISTRIBUTION                   0x1305

typedef uint8_t sdp_parsing_mode;
#define BTIF_BSPM_BEGINNING   0x00
#define BTIF_BSPM_RESUME      0x01
#define BTIF_BSPM_CONT_STATE  0x02
#define BTIF_BSPM_NO_SKIP   0x04

typedef uint8_t sdp_query_mode;
#define BTIF_BSQM_FIRST     0x00
#define BTIF_BSQM_CONTINUE  0x01
#define BTIF_BSQM_DONT_CARE 0xFF

typedef U8 SdpDataElemType;
#define DETD_NIL  0x00
#define DETD_UINT 0x08
#define DETD_SINT 0x10
#define DETD_UUID 0x18
#define DETD_TEXT 0x20
#define DETD_BOOL 0x28
#define DETD_SEQ  0x30
#define DETD_ALT  0x38
#define DETD_URL  0x40
#define DETD_MASK 0xf8

typedef uint8_t sdp_data_elem_size;
#define DESD_1BYTE      0x00
#define DESD_2BYTES     0x01
#define DESD_4BYTES     0x02
#define DESD_8BYTES     0x03
#define DESD_16BYTES    0x04
#define DESD_ADD_8BITS  0x05
#define DESD_ADD_16BITS 0x06
#define DESD_ADD_32BITS 0x07
#define DESD_MASK       0x07

#define SDP_ATTRIBUTE(attribId, attrib) \
          { attribId,           \
            sizeof(attrib),     \
            attrib,             \
            0x0000 }

#define SDP_ATTRIBUTE_ARM(attribId, attrib) \
          { attribId,           \
            sizeof(attrib),     \
            0,                  \
            0x0000 }

#define SDP_ATTRIB_HEADER_8BIT(size) \
            DETD_SEQ + DESD_ADD_8BITS,      \
            size

#define SDP_ATTRIB_HEADER_16BIT(size) \
            DETD_SEQ + DESD_ADD_16BITS,      \
            (U8)(((size) & 0xff00) >> 8),    \
            (U8)((size) & 0x00ff)

#define SDP_ATTRIB_HEADER_32BIT(size) \
            DETD_SEQ + DESD_ADD_32BITS,         \
            (U8)(((size) & 0xff000000) >> 24),  \
            (U8)(((size) & 0x00ff0000) >> 16),  \
            (U8)(((size) & 0x0000ff00) >> 8),   \
            (U8)((size) & 0x000000ff)

#define SDP_ATTRIB_HEADER_ALT_8BIT(size) \
            DETD_ALT + DESD_ADD_8BITS,      \
            size

#define SDP_ATTRIB_HEADER_ALT_16BIT(size) \
            DETD_ALT + DESD_ADD_16BITS,      \
            (U8)(((size) & 0xff00) >> 8),    \
            (U8)((size) & 0x00ff)

#define SDP_ATTRIB_HEADER_ALT_32BIT(size) \
            DETD_ALT + DESD_ADD_32BITS,         \
            (U8)(((size) & 0xff000000) >> 24),  \
            (U8)(((size) & 0x00ff0000) >> 16),  \
            (U8)(((size) & 0x0000ff00) >> 8),   \
            (U8)((size) & 0x000000ff)

#define SDP_UUID_16BIT(uuid) \
            DETD_UUID + DESD_2BYTES,         \
            (U8)(((uuid) & 0xff00) >> 8),    \
            (U8)((uuid) & 0x00ff)

#define SDP_UUID_32BIT(uuid) \
            DETD_UUID + DESD_4BYTES,            \
            (U8)(((uuid) & 0xff000000) >> 24),  \
            (U8)(((uuid) & 0x00ff0000) >> 16),  \
            (U8)(((uuid) & 0x0000ff00) >> 8),   \
            (U8)((uuid) & 0x000000ff)

#define SDP_UUID_128BIT(uuid)                \
            DETD_UUID + DESD_16BYTES,        \
            (U8)(uuid[15]), \
            (U8)(uuid[14]), \
            (U8)(uuid[13]), \
            (U8)(uuid[12]), \
            (U8)(uuid[11]), \
            (U8)(uuid[10]), \
            (U8)(uuid[9]),  \
            (U8)(uuid[8]),  \
            (U8)(uuid[7]),  \
            (U8)(uuid[6]),  \
            (U8)(uuid[5]),  \
            (U8)(uuid[4]),  \
            (U8)(uuid[3]),  \
            (U8)(uuid[2]),  \
            (U8)(uuid[1]),  \
            (U8)(uuid[0])

#define SDP_UINT_8BIT(uint) \
            DETD_UINT + DESD_1BYTE,          \
            (U8)(uint)

#define SDP_UINT_16BIT(uint) \
            DETD_UINT + DESD_2BYTES,         \
            (U8)(((uint) & 0xff00) >> 8),    \
            (U8)((uint) & 0x00ff)

#define SDP_UINT_32BIT(uint) \
            DETD_UINT + DESD_4BYTES,            \
            (U8)(((uint) & 0xff000000) >> 24),  \
            (U8)(((uint) & 0x00ff0000) >> 16),  \
            (U8)(((uint) & 0x0000ff00) >> 8),   \
            (U8)((uint) & 0x000000ff)

#define SDP_UINT_64BIT(uint)                    \
            DETD_UINT + DESD_8BYTES,            \
            uint

#define SDP_UINT_128BIT(uint)                   \
            DETD_UINT + DESD_16BYTES,           \
            uint

#define SDP_TEXT_8BIT(size) \
            DETD_TEXT + DESD_ADD_8BITS,\
            (U8)(size)

#define SDP_TEXT_16BIT(size) \
            DETD_TEXT + DESD_ADD_16BITS,\
            (U8)(((size) & 0xff00) >> 8),\
            (U8)((size) & 0x00ff)

#define SDP_TEXT_32BIT(size) \
            DETD_TEXT + DESD_ADD_32BITS,        \
            (U8)(((size) & 0xff000000) >> 24),  \
            (U8)(((size) & 0x00ff0000) >> 16),  \
            (U8)(((size) & 0x0000ff00) >> 8),   \
            (U8)((size) & 0x000000ff)

#define SDP_BOOL(value) \
            DETD_BOOL + DESD_1BYTE,          \
            (U8)(value)

typedef uint16_t sdp_service_class_uuid;
#define SC_SERVICE_DISCOVERY_SERVER             0x1000
#define SC_BROWSE_GROUP_DESC                    0x1001
#define SC_PUBLIC_BROWSE_GROUP                  0x1002
#define SC_SERIAL_PORT                          0x1101
#define SC_LAN_ACCESS_PPP                       0x1102
#define SC_DIALUP_NETWORKING                    0x1103
#define SC_IRMC_SYNC                            0x1104
#define SC_OBEX_OBJECT_PUSH                     0x1105
#define SC_OBEX_FILE_TRANSFER                   0x1106
#define SC_IRMC_SYNC_COMMAND                    0x1107
#define SC_HEADSET                              0x1108
#define SC_CORDLESS_TELEPHONY                   0x1109
#define SC_AUDIO_SOURCE                         0x110A
#define SC_AUDIO_SINK                           0x110B
#define SC_AV_REMOTE_CONTROL_TARGET             0x110C
#define SC_AUDIO_DISTRIBUTION                   0x110D
#define SC_AV_REMOTE_CONTROL                    0x110E
#define SC_VIDEO_CONFERENCING                   0x110F
#define SC_INTERCOM                             0x1110
#define SC_FAX                                  0x1111
#define SC_HEADSET_AUDIO_GATEWAY                0x1112
#define SC_WAP                                  0x1113
#define SC_WAP_CLIENT                           0x1114
#define SC_PANU                                 0x1115
#define SC_NAP                                  0x1116
#define SC_GN                                   0x1117
#define SC_DIRECT_PRINTING                      0x1118
#define SC_REFERENCE_PRINTING                   0x1119
#define SC_IMAGING                              0x111A
#define SC_IMAGING_RESPONDER                    0x111B
#define SC_IMAGING_AUTOMATIC_ARCHIVE            0x111C
#define SC_IMAGING_REFERENCED_OBJECTS           0x111D
#define SC_HANDSFREE                            0x111E
#define SC_HANDSFREE_AUDIO_GATEWAY              0x111F
#define SC_DIRECT_PRINTING_REF_OBJECTS          0x1120
#define SC_REFLECTED_UI                         0x1121
#define SC_BASIC_PRINTING                       0x1122
#define SC_PRINTING_STATUS                      0x1123
#define SC_HUMAN_INTERFACE_DEVICE               0x1124
#define SC_HCR                                  0x1125
#define SC_HCR_PRINT                            0x1126
#define SC_HCR_SCAN                             0x1127
#define SC_ISDN                                 0x1128
#define SC_VIDEO_CONFERENCING_GW                0x1129
#define SC_UDI_MT                               0x112A
#define SC_UDI_TA                               0x112B
#define SC_AUDIO_VIDEO                          0x112C
#define SC_SIM_ACCESS                           0x112D
#define SC_PBAP_CLIENT                          0x112E
#define SC_PBAP_SERVER                          0x112F
#define SC_PBAP_PROFILE                         0x1130
#define SC_MAP_SERVER                           0x1132
#define SC_MAP_NOTIFY_SERVER                    0x1133
#define SC_MAP_PROFILE                          0x1134
#define SC_PNP_INFO                             0x1200
#define SC_GENERIC_NETWORKING                   0x1201
#define SC_GENERIC_FILE_TRANSFER                0x1202
#define SC_GENERIC_AUDIO                        0x1203
#define SC_GENERIC_TELEPHONY                    0x1204
#define SC_UPNP_SERVICE                         0x1205
#define SC_UPNP_IP_SERVICE                      0x1206
#define SC_ESDP_UPNP_IP_PAN                     0x1300
#define SC_ESDP_UPNP_IP_LAP                     0x1301
#define SC_ESDP_UPNP_L2CAP                      0x1302
#define SC_VIDEO_SOURCE                         0x1303
#define SC_VIDEO_SINK                           0x1304
#define SC_VIDEO_DISTRIBUTION                   0x1305

typedef uint16_t sdp_protocol_uuid;
#define PROT_SDP                     0x0001
#define PROT_UDP                     0x0002
#define PROT_RFCOMM                  0x0003
#define PROT_TCP                     0x0004
#define PROT_TCS_BIN                 0x0005
#define PROT_TCS_AT                  0x0006
#define PROT_OBEX                    0x0008
#define PROT_IP                      0x0009
#define PROT_FTP                     0x000A
#define PROT_HTTP                    0x000C
#define PROT_WSP                     0x000E
#define PROT_BNEP                    0x000F
#define PROT_UPNP                    0x0010
#define PROT_HIDP                    0x0011
#define PROT_HCR_CONTROL_CHANNEL     0x0012
#define PROT_HCR_DATA_CHANNEL        0x0014
#define PROT_HCR_NOTIFICATION        0x0016
#define PROT_AVCTP                   0x0017
#define PROT_AVDTP                   0x0019
#define PROT_AVCTP_BROWSING          0x001B
#define PROT_UDI_C                   0x001D
#define PROT_L2CAP                   0x0100

typedef uint16_t sdp_attribute_id;
#define AID_SERVICE_RECORD_HANDLE               0x0000
#define AID_SERVICE_CLASS_ID_LIST               0x0001
#define AID_SERVICE_RECORD_STATE                0x0002
#define AID_SERVICE_ID                          0x0003
#define AID_PROTOCOL_DESC_LIST                  0x0004
#define AID_BROWSE_GROUP_LIST                   0x0005
#define AID_LANG_BASE_ID_LIST                   0x0006
#define AID_SERVICE_INFO_TIME_TO_LIVE           0x0007
#define AID_SERVICE_AVAILABILITY                0x0008
#define AID_BT_PROFILE_DESC_LIST                0x0009
#define AID_DOC_URL                             0x000a
#define AID_CLIENT_EXEC_URL                     0x000b
#define AID_ICON_URL                            0x000c
#define AID_ADDITIONAL_PROT_DESC_LISTS          0x000d
#define AID_SERVICE_NAME                        0x0000
#define AID_SERVICE_DESCRIPTION                 0x0001
#define AID_PROVIDER_NAME                       0x0002
#define AID_IP_SUBNET                           0x0200
#define AID_GROUP_ID                            0x0200
#define AID_VERSION_NUMBER_LIST                 0x0200
#define AID_SERVICE_DATABASE_STATE              0x0201
#define AID_SERVICE_VERSION                     0x0300
#define AID_EXTERNAL_NETWORK                    0x0301
#define AID_SUPPORTED_DATA_STORES_LIST          0x0301
#define AID_FAX_CLASS_1_SUPPORT                 0x0302
#define AID_REMOTE_AUDIO_VOL_CONTROL            0x0302
#define AID_FAX_CLASS_20_SUPPORT                0x0303
#define AID_SUPPORTED_FORMATS_LIST              0x0303
#define AID_FAX_CLASS_2_SUPPORT                 0x0304
#define AID_AUDIO_FEEDBACK_SUPPORT              0x0305
#define AID_NETWORK_ADDRESS                     0x0306
#define AID_WAP_GATEWAY                         0x0307
#define AID_HOME_PAGE_URL                       0x0308
#define AID_WAP_STACK_TYPE                      0x0309
#define AID_SECURITY_DESC                       0x030A
#define AID_NET_ACCESS_TYPE                     0x030B
#define AID_MAX_NET_ACCESS_RATE                 0x030C
#define AID_IPV4_SUBNET                         0x030D
#define AID_IPV6_SUBNET                         0x030E
#define AID_SUPPORTED_CAPABILITIES              0x0310
#define AID_SUPPORTED_FEATURES                  0x0311
#define AID_SUPPORTED_FUNCTIONS                 0x0312
#define AID_TOTAL_IMAGE_DATA_CAPACITY           0x0313
#define AID_SUPPORTED_REPOSITORIES              0x0314
#define AID_MAS_INSTANCE_ID                     0x0315
#define AID_SUPPORTED_MESSAGE_TYPES             0x0316
#define AID_SUPPORTED_DOC_FORMATS               0x0350
#define AID_SUPPORTED_CHAR_REPERTOIRES          0x0352
#define AID_SUPPORTED_XHTML_IMAGE_FORMATS       0x0354
#define AID_COLOR_SUPPORTED                     0x0356
#define AID_PRINTER_1284ID                      0x0358
#define AID_DUPLEX_SUPPORTED                    0x035E
#define AID_SUPPORTED_MEDIA_TYPES               0x0360
#define AID_MAX_MEDIA_WIDTH                     0x0362
#define AID_MAX_MEDIA_LENGTH                    0x0364
#define AID_HID_PARSERVERSION                   0x201
#define AID_HID_DEVICESUBCLASS                  0x202
#define AID_HID_CONTRYCODE                      0x203
#define AID_HID_VIRTUALCABLE                    0x204
#define AID_HID_RECONNECTINITIATE               0x205
#define AID_HID_DESCRIPTORLIST                  0x206
#define AID_HID_LANGIDBASELIST                  0x207
#define AID_HID_BATTERYPOWER                    0x209
#define AID_HID_REMOTEWAKE                      0x20A
#define AID_HID_SUPERVISIONTIMEOUT              0x20C
#define AID_HID_NORMALLYCONNECTABLE             0x20D
#define AID_HID_BOOTDEVICE                      0x20E
#define AID_HID_SSRHOSTMAXLATENCY               0x20F
#define AID_HID_SSRHOSTMINTIMEOUT               0x210

U16 sdp_get_u16(U8 * buff);
#define sdp_get_u16(buff) be_to_host16((buff))
U32 sdp_get_u32(U8 * buff);
#define sdp_get_u32(buff) BEtoHost32((buff))
void sdp_put_u16(U8 * buff, U16 val);
#define sdp_put_u16(buff,val) StoreBE16((buff),(val))
void sdp_put_u32(U8 * buff, U32 val);
#define sdp_put_u32(buff,val) StoreBE32((buff),(val))

typedef struct btif_sdp_attribute {
    uint16_t id;
    uint16_t len;
    const uint8_t *value;
    uint16_t flags;
} __attribute__ ((__packed__)) sdp_attribute_t;

typedef struct btif_sdp_query_info {
    const uint8_t *parms;
    uint16_t parm_len;
    sdp_query_type query_type;
    sdp_attribute_id attr_id;
    uint16_t uuid;
    sdp_parsing_mode mode;
    btif_callback callback;
    void *priv;
} __attribute__ ((__packed__)) btif_sdp_query_info_t;

typedef struct btif_sdp_record_param {
    sdp_attribute_t *attrs;
    uint32_t attr_count;
    uint32_t COD;
} __attribute__ ((__packed__)) btif_sdp_record_param_t;


typedef struct btif_sdp_query_token
{
    btif_remote_device_t *rm;  /* The remote device to query. An ACL
                               * connection must exist with this device
                               * before the query is started. */
   struct btif_sdp_query_info info;
    uint8_t         channel;
    uint8_t     response_buff[666];
    uint32_t    response_buff_len;
} __attribute__ ((__packed__)) btif_sdp_query_token_t;

#ifdef __cplusplus
extern "C" {
#endif

    bt_status_t btif_sdp_init(void);

    btif_sdp_record_t *btif_sdp_create_record(void);

    void btif_sdp_free_record(btif_sdp_record_t* record);

    void *btif_sdp_get_record_priv(btif_sdp_record_t *record);

    bt_status_t btif_sdp_record_setup(btif_sdp_record_t *record, btif_sdp_record_param_t *param);

    btif_sdp_query_token_t *btif_sdp_create_query_token(void);

    void btif_sdp_free_token(btif_sdp_query_token_t * query_token);

    int btif_sdp_set_query_token(btif_sdp_query_token_t * query_token, btif_sdp_query_info_t *info);

    int btif_sdp_set_remote_device(btif_sdp_query_token_t * query_token,
                                   btif_remote_device_t * r_device);

    bt_status_t btif_sdp_query(btif_sdp_query_token_t * query_token, sdp_query_mode mode);

    bt_status_t btif_sdp_parse_attrs(btif_sdp_query_token_t * query_token);

    btif_remote_device_t *btif_sdp_get_remote_device(btif_sdp_query_token_t * query_token);

    uint8_t btif_sdp_get_server_id(btif_sdp_query_token_t * query_token);

    void *btif_sdp_get_token_priv(btif_sdp_query_token_t * query_token);

    bt_status_t btif_sdp_add_record(btif_sdp_record_t * record);

    bt_status_t btif_sdp_remove_record(btif_sdp_record_t * record);

    void btif_sdp_set_parsing_mode(btif_sdp_query_token_t * query_token, sdp_parsing_mode mode);

    bt_status_t btif_spp_insert_rfcomm_chan(btif_sdp_record_t *record, uint8_t channel);

    btif_sdp_query_token_t *btif_me_get_callback_event_sdp_token(const btif_event_t *event);
#ifdef __cplusplus
}
#endif
#endif              /*__SDP_API_H__*/
