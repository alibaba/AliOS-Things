/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BT_MEDIA_MANAGER_H__
#define __APP_BT_MEDIA_MANAGER_H__

#include "resources.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "btapp.h"

#define  BT_STREAM_DIRECTION_OUT    0
#define  BT_STREAM_DIRECTION_IN     1

// direction is out
#define  BT_STREAM_SBC          0x1
#define  BT_STREAM_MEDIA        0x2
#define  BT_STREAM_VOICE        0x4
#ifdef AUDIO_LINEIN
#define  BT_STREAM_LINEIN       0x8
#endif
#ifdef RB_CODEC
#define  BT_STREAM_RBCODEC      0x10    //from rockbox decoder
#endif

// direction is in
#ifdef VOICE_DATAPATH
#define BT_STREAM_CAPTURE       0x100
#endif

#ifdef __AI_VOICE__
#define BT_STREAM_AI_VOICE      0x200
#endif

#define BT_STREAM_TYPE_MASK   (BT_STREAM_SBC | BT_STREAM_MEDIA | BT_STREAM_VOICE)


enum APP_BT_MEDIA_MANAGER_ID_T {
    APP_BT_STREAM_MANAGER_START = 0,
    APP_BT_STREAM_MANAGER_STOP,
    APP_BT_STREAM_MANAGER_SWITCHTO_SCO,
    APP_BT_STREAM_MANAGER_STOP_MEDIA,   
    APP_BT_STREAM_MANAGER_UPDATE_MEDIA,
    APP_BT_STREAM_MANAGER_SWAP_SCO,
    APP_BT_STREAM_MANAGER_CTRL_VOLUME,
    APP_BT_STREAM_MANAGER_TUNE_SAMPLERATE_RATIO,
    APP_BT_STREAM_MANAGER_NUM,
};

typedef struct {
    uint8_t id;
    uint16_t stream_type;
    uint8_t device_id;
    uint16_t aud_id;

}APP_AUDIO_MANAGER_MSG_STRUCT;

enum APP_AUDIO_MANAGER_VOLUME_CTRL_T {
    APP_AUDIO_MANAGER_VOLUME_CTRL_SET = 0,
    APP_AUDIO_MANAGER_VOLUME_CTRL_UP,
    APP_AUDIO_MANAGER_VOLUME_CTRL_DOWN,
    APP_AUDIO_MANAGER_VOLUME_CTRL_NUM,
};

typedef void (*APP_AUDIO_MANAGER_CALLBACK_T)(uint32_t status, uint32_t param);

#define APP_AUDIO_MANAGER_SET_MESSAGE(appevt, id, stream_type) (appevt = (((uint32_t)id&0xffff)<<16)|(stream_type&0xffff))
#define APP_AUDIO_MANAGER_SET_MESSAGE0(appmsg, device_id,aud_id) (appmsg = (((uint32_t)device_id&0xffff)<<16)|(aud_id&0xffff))
#define APP_AUDIO_MANAGER_GET_ID(appevt, id) (id = (appevt>>16)&0xffff)
#define APP_AUDIO_MANAGER_GET_STREAM_TYPE(appevt, stream_type) (stream_type = appevt&0xffff)
#define APP_AUDIO_MANAGER_GET_DEVICE_ID(appmsg, device_id) (device_id = (appmsg>>16)&0xffff)
#define APP_AUDIO_MANAGER_GET_AUD_ID(appmsg, aud_id) (aud_id = appmsg&0xffff)
#define APP_AUDIO_MANAGER_GET_CALLBACK(appmsg, callback) (callback = (APP_AUDIO_MANAGER_CALLBACK_T)appmsg)
#define APP_AUDIO_MANAGER_GET_CALLBACK_PARAM(appmsg, param) (param = appmsg)

int app_audio_manager_sendrequest(uint8_t massage_id,uint16_t stream_type, uint8_t device_id, uint16_t aud_id);
int app_audio_manager_sendrequest_need_callback(
                           uint8_t massage_id, uint16_t stream_type, uint8_t device_id, uint16_t aud_id, uint32_t cb, uint32_t cb_param);
void app_audio_manager_open(void);

void  bt_media_start(uint16_t stream_type,enum BT_DEVICE_ID_T device_id,AUD_ID_ENUM media_id);
void bt_media_stop(uint16_t stream_type,enum BT_DEVICE_ID_T device_id);
void bt_media_switch_to_voice(uint16_t stream_type,enum BT_DEVICE_ID_T device_id);
uint8_t bt_media_is_media_active_by_type(uint16_t media_type);
void bt_media_volume_ptr_update_by_mediatype(uint16_t stream_type);
int app_audio_manager_set_active_sco_num(enum BT_DEVICE_ID_T id);
int app_audio_manager_get_active_sco_num(void);
hf_chan_handle_t* app_audio_manager_get_active_sco_chnl(void);
int app_audio_manager_swap_sco(enum BT_DEVICE_ID_T id);
uint8_t bt_media_is_media_active_by_device(uint16_t media_type,enum BT_DEVICE_ID_T device_id);
bool bt_media_is_media_active(void);
int app_audio_manager_sco_status_checker(void);
int app_audio_manager_switch_a2dp(enum BT_DEVICE_ID_T id);
bool app_audio_manager_a2dp_is_active(enum BT_DEVICE_ID_T id);
bool app_audio_manager_capture_is_active(void);
bool app_audio_manager_media_is_active(void);
bool app_audio_manager_hfp_is_active(enum BT_DEVICE_ID_T id);
int app_audio_manager_set_scocodecid(enum BT_DEVICE_ID_T dev_id, uint16_t codec_id);
int app_audio_manager_get_scocodecid(void);
bool bt_media_is_media_idle(void);
uint8_t bt_media_device_enumerate_media_type_by_prior();
void bt_media_clean_up(void);

bool bt_media_is_media_active_by_sbc(void);
#ifdef RB_CODEC
bool app_rbplay_is_localplayer_mode(void);
bool app_rbplay_mode_switch(void);
void app_rbplay_set_player_mode(bool isInPlayerMode);
void app_rbcodec_ctr_play_onoff(bool on );
bool app_rbcodec_get_play_status(void);
void app_rbcodec_toggle_play_stop(void);
#endif

void app_stop_a2dp_media_stream(uint8_t devId);
void app_stop_sco_media_stream(uint8_t devId);
int app_audio_manager_ctrl_volume(APP_AUDIO_MANAGER_VOLUME_CTRL_T volume_ctrl, uint16_t volume_level);
int app_audio_manager_tune_samplerate_ratio(enum AUD_STREAM_T stream, float ratio);
bool bt_media_cur_is_bt_stream_media(void);
bool bt_media_is_sbc_media_active(void);
void app_ibrt_sync_mix_prompt_req_handler(uint8_t* ptrParam, uint16_t paramLen);

#ifdef __cplusplus
    }
#endif

#endif
