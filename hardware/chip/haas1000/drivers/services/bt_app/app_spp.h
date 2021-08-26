/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_SPP_H__
#define __APP_SPP_H__

#include "spp_api.h"
#include "sdp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__3M_PACK__)
#define L2CAP_MTU                           980
#else
#define L2CAP_MTU                           672
#endif

#define SPP_RECV_BUFFER_SIZE   3072
#define SPP_MAX_DATA_PACKET_SIZE    L2CAP_MTU

#ifdef ENHANCED_STACK
#define    BTIF_APP_SPP_SERVER_GSOUND_CTL_ID    BTIF_APP_SPP_SERVER_ID_1
#define    BTIF_APP_SPP_SERVER_GSOUND_AUD_ID    BTIF_APP_SPP_SERVER_ID_2
#define    BTIF_APP_SPP_SERVER_TOTAD_ID         BTIF_APP_SPP_SERVER_ID_3
#define    BTIF_APP_SPP_SERVER_BES_OTA_ID       BTIF_APP_SPP_SERVER_ID_4
#define    BTIF_APP_SPP_SERVER_AI_VOICE_ID      BTIF_APP_SPP_SERVER_ID_5
#define    BTIF_APP_SPP_SERVER_GREEN_ID         BTIF_APP_SPP_SERVER_ID_6
#define    BTIF_APP_SPP_SERVER_RED_ID           BTIF_APP_SPP_SERVER_ID_7
#define    BTIF_APP_SPP_SERVER_FP_RFCOMM_ID     BTIF_APP_SPP_SERVER_ID_8

#define    BTIF_APP_SPP_CLIENT_AI_VOICE_ID      BTIF_APP_SPP_CLIENT_ID_1
#define    BTIF_APP_SPP_CLIENT_CCMP_ID          BTIF_APP_SPP_CLIENT_ID_2
#define    BTIF_APP_SPP_CLIENT_RED_ID           BTIF_APP_SPP_CLIENT_ID_3

/*---------------------------------------------------------------------------
 * rfcomm channel number
 *      should be from 1 to 30
 */
enum RFCOMM_CHANNEL_NUM {
    RFCOMM_CHANNEL_GS_CONTROL   = RFCOMM_CHANNEL_1,
    RFCOMM_CHANNEL_GS_AUDIO     = RFCOMM_CHANNEL_2,
    RFCOMM_CHANNEL_TOTA         = RFCOMM_CHANNEL_3,
    RFCOMM_CHANNEL_BES_OTA      = RFCOMM_CHANNEL_4,
    RFCOMM_CHANNEL_AI_VOICE     = RFCOMM_CHANNEL_5,
    RFCOMM_CHANNEL_GREEN        = RFCOMM_CHANNEL_6,
    RFCOMM_CHANNEL_RED          = RFCOMM_CHANNEL_7,
    RFCOMM_CHANNEL_FP           = RFCOMM_CHANNEL_8,
};
#endif

struct spp_device *app_create_spp_device(void);
#if 0
void app_spp_register_connect_callback(struct spp_device *osDev_t, spp_event_callback_t callback);
void app_spp_register_disconnect_callback(struct spp_device *osDev_t, spp_event_callback_t callback);
void app_spp_register_tx_done(struct spp_device *osDev_t, spp_event_callback_t callback);
#endif
bt_status_t app_spp_send_data(struct spp_device *osDev_t, uint8_t* ptrData, uint16_t *length);
void app_spp_open(struct spp_device *osDev_t, btif_remote_device_t  *btDevice, btif_sdp_record_param_t *param, osMutexId mid, uint8_t service_id, spp_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif

