/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _L2CAP_API_H
#define _L2CAP_API_H
#include "bluetooth.h"

#define BTIF_NULL_IDENTIFIER_CID      0x0000
#define BTIF_SIGNALING_CHNL_CID       0x0001
#define BTIF_CONNLESS_CHNL_CID        0x0002
#define BTIF_BASE_DYNAMIC_CID         0x0040
#define BTIF_LAST_DYNAMIC_CID         (BTIF_BASE_DYNAMIC_CID + L2CAP_NUM_CHANNELS - 1)
#define BTIF_CID_TO_INDEX(_CID)       ((_CID) - BTIF_BASE_DYNAMIC_CID)
#define BTIF_BASE_GROUP_CID           ((BTIF_LAST_DYNAMIC_CID+16) & 0xFFF0)
#define BTIF_LAST_GROUP_CID           (BTIF_BASE_GROUP_CID + L2CAP_NUM_GROUPS - 1)
#define BTIF_GROUP_CID_TO_INDEX(_CID) ((_CID) - BTIF_BASE_GROUP_CID)

#define BTIF_LLC_COMMAND_REJ     0x01
#define BTIF_LLC_CONN_REQ        0x02
#define BTIF_LLC_CONN_RSP        0x03
#define BTIF_LLC_CONFIG_REQ      0x04
#define BTIF_LLC_CONFIG_RSP      0x05
#define BTIF_LLC_DISC_REQ        0x06
#define BTIF_LLC_DISC_RSP        0x07
#define BTIF_LLC_ECHO_REQ        0x08
#define BTIF_LLC_ECHO_RSP        0x09
#define BTIF_LLC_INFO_REQ        0x0A
#define BTIF_LLC_INFO_RSP        0x0B
#define  BTIF_LLC_TWS_DATA_XFER	0xFE

typedef U16 btif_l2cap_psm_value_t;

#define BTIF_BT_CLIENT_ONLY_PSM      0x0000

/* Value for a Service Discovery Protocol server */
#define BTIF_BT_PSM_SDP              0x0001

/* Value for an RFCOMM server */
#define BTIF_BT_PSM_RFCOMM           0x0003

/* Value for a TCS Binary server */
#define BTIF_BT_PSM_TCS              0x0005

/* Value for a TCS Binary group */
#define BTIF_BT_PSM_TCS_CORDLESS     0x0007

/* Value for the BNEP service */
#define BTIF_BT_PSM_BNEP             0x000F

/* Value for the HID Control Channel */
#define BTIF_BT_PSM_HID_CTRL         0x0011

/* Value for the HID Interrupt Channel */
#define BTIF_BT_PSM_HID_INTR         0x0013

/* Value for the UPnP/ESDP service */
#define BTIF_BT_PSM_UPNP             0x0015

/* Value for the A/V Control Transport Protocol signal channel */
#define BTIF_BT_PSM_AVCTP            0x0017

/* Value for the A/V Distribution Transport protocol */
#define BTIF_BT_PSM_AVDTP            0x0019

/* Value for the A/V Control Transport Protocol browsing channel*/
#define BTIF_BT_PSM_AVCTP_BROWSING   0x001B

/* Value for Unrestricted Digital Information Control Plane protocol */
#define BTIF_BT_PSM_UDI_C            0x001D

#define BTIF_BT_DYNAMIC_PSM          0x1101

typedef void btif_l2cap_channel_t;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(IBRT)
	uint32_t btif_save_l2cap_channel_ctxs(uint8_t *ctxs_buffer,uint16_t hci_handle,uint8_t psm_context_mask);
	uint32_t btif_set_l2cap_channels_ctx(uint8_t *ctxs_buffer, uint8_t dev_tbl_idx);
    bool btif_l2cap_is_profile_channel_connected(uint8_t psm_context_mask);    
#endif
	void btif_btm_register_get_ibrt_role_callback(btif_callback_ext cb);
    void btif_l2cap_register_sdp_disconnect_callback(btif_callback cb);
    btif_l2cap_channel_t *btif_l2cap_alloc_channel_instance(uint16_t index);
    btif_l2cap_channel_t *btif_l2cap_get_l2cap_channel_instance(uint16_t index);
    uint16_t btif_l2cap_cid_to_index(uint16_t index);
    uint32_t btif_l2cap_get_channel_size(void);
    void btif_l2cap_set_channel_local_cid(btif_l2cap_channel_t * channel, uint16_t cid);
    void btif_l2cap_set_channel_remote_cid(btif_l2cap_channel_t * channel, uint16_t cid);
    uint16_t btif_l2cap_get_channel_local_cid(btif_l2cap_channel_t * channel);
    uint16_t btif_l2cap_get_channel_remote_cid(btif_l2cap_channel_t * channel);
    bool btif_l2cap_is_tws_reserved_channel_in_use(void);
    //void btif_L2CAP_BuildConnectionToTwsReservedChannel(btif_remote_device_t * Device);
    void btif_l2cap_close_tws_reserved_channel(void);

#if IS_USE_INTERNAL_ACL_DATA_PATH

#else
    bt_status_t btif_l2cap_send_data_to_peer_tws(U16 connHandle, U8 dataLen, U8 * data);

#endif

#ifdef __cplusplus
}
#endif
#endif
