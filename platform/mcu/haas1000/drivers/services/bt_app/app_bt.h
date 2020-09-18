/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BT_H__
#define __APP_BT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "me_api.h"
#include "a2dp_api.h"
#include "hfp_api.h"
#include "btapp.h"

enum APP_BT_REQ_T {
    APP_BT_REQ_ACCESS_MODE_SET,
    APP_BT_REQ_AUTO_TEST,
    APP_BT_REQ_NUM
};

enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T {
    APP_BT_GOLBAL_HANDLE_HOOK_USER_0 = 0,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_1,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY
};

typedef void (*APP_BT_REQ_CONNECT_PROFILE_FN_T)(void *, void *);
typedef bt_status_t  (*APP_BT_REQ_HF_OP_FN_T)(void *);
typedef void (*APP_BT_GOLBAL_HANDLE_HOOK_HANDLER)(const btif_event_t*Event);

#define app_bt_accessmode_set_req(accmode) do{app_bt_send_request(APP_BT_REQ_ACCESS_MODE_SET, accmode, 0, 0);}while(0)
typedef enum
{
    APP_BT_IDLE_STATE = 0,
    APP_BT_IN_CONNECTING_PROFILES_STATE	// acl link is created and in the process of connecting profiles
} APP_BT_CONNECTING_STATE_E;

typedef enum
{
    // always in active mode during a2dp streaming
    ACTIVE_MODE_KEEPER_A2DP_STREAMING = 0,
    // always in active mode during ai voice up-streaming
    ACTIVE_MODE_KEEPER_AI_VOICE_STREAM,
    // always in active mode during OTA
    ACTIVE_MODE_KEEPER_OTA,
    // stay in active mode for 15 seconds after role switch
    ACTIVE_MODE_KEEPER_ROLE_SWITCH,
    // stay in active mode during synchronous voice prompt playing
    ACTIVE_MODE_KEEPER_SYNC_VOICE_PROMPT,
    // always in active mode during phone call
    ACTIVE_MODE_KEEPEER_SCO_STREAMING,


} BT_LINK_ACTIVE_MODE_KEEPER_USER_E;

#define MAX_ACTIVE_MODE_MANAGED_LINKS       3  
#define UPDATE_ACTIVE_MODE_FOR_ALL_LINKS    MAX_ACTIVE_MODE_MANAGED_LINKS

void app_bt_active_mode_manager_init(void);

void app_bt_active_mode_reset(uint32_t linkIndex);

void app_bt_active_mode_set(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex);

void app_bt_active_mode_clear(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex);

void app_bt_global_handle(const btif_event_t *Event);
bool is_app_bt_pairing_running(void);

void PairingTransferToConnectable(void);

void app_bt_global_handle_init(void);

void app_bt_opening_reconnect(void);

void app_bt_accessmode_set(  btif_accessible_mode_t mode);

void app_bt_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t ptr);

void app_bt_init(void);

int app_bt_state_checker(void);

void *app_bt_profile_active_store_ptr_get(uint8_t *bdAddr);

void app_bt_profile_connect_manager_open(void);

void app_bt_profile_connect_manager_opening_reconnect(void);

BOOL app_bt_profile_connect_openreconnecting(void *ptr);

int app_bt_global_handle_hook_set(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user, APP_BT_GOLBAL_HANDLE_HOOK_HANDLER handler);

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_global_handle_hook_get(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user);

bool app_is_hfp_service_connected(void);
#if defined(IBRT)
void app_bt_ibrt_reconnect_mobile_profile_flag_set(void);
void app_bt_ibrt_reconnect_mobile_profile_flag_clear(void);
bool app_bt_ibrt_reconnect_mobile_profile_flag_get(void);
#endif
#if defined(__BT_SELECT_PROF_DEVICE_ID__)
int8_t app_bt_a2dp_is_same_stream(a2dp_stream_t *src_Stream, a2dp_stream_t *dst_Stream);

void app_bt_a2dp_find_same_unused_stream(a2dp_stream_t *in_Stream, a2dp_stream_t **out_Stream, uint32_t device_id);

int8_t app_bt_a2dp_is_stream_on_device_id(a2dp_stream_t *in_Stream, uint32_t device_id);

int8_t app_bt_hfp_is_chan_on_device_id(hf_chan_handle_t chan, uint32_t device_id);

int8_t app_bt_is_any_profile_connected(uint32_t device_id);

int8_t app_bt_is_a2dp_connected(uint32_t device_id);

btif_remote_device_t *app_bt_get_connected_profile_remdev(uint32_t device_id);
#endif

void app_bt_stay_active(uint8_t deviceId);

void app_bt_allow_sniff(uint8_t deviceId);

btif_remote_device_t* app_bt_get_remoteDev(uint8_t deviceId);

void app_bt_stop_sniff(uint8_t deviceId);

void app_bt_sniff_config(btif_remote_device_t *remDev);

void app_bt_allow_sniff_rem_dev(btif_remote_device_t* pRemDev);

void app_bt_stay_active_rem_dev(btif_remote_device_t* pRemDev);

void app_check_pending_stop_sniff_op(void);

void app_bt_reset_reconnect_timer(bt_bdaddr_t *pBdAddr);

uint8_t app_bt_get_num_of_connected_dev(void);

bool btapp_hfp_is_dev_call_active(uint8_t devId);

void app_bt_pause_media_player_again(uint8_t deviceId);

bool app_bt_is_music_player_working(uint8_t deviceId);

bool app_bt_pause_music_player(uint8_t deviceId);

void app_bt_resume_music_player(uint8_t deviceId);

bool app_bt_is_to_resume_music_player(uint8_t deviceId);

void app_bt_reset_music_player_resume_state(void);

bool app_bt_is_device_connected(uint8_t deviceId);

void app_bt_set_music_player_resume_device(uint8_t deviceId);

bool app_bt_is_a2dp_streaming(uint8_t deviceId);

void app_bt_suspend_a2dp_streaming(uint8_t deviceId);

bool app_bt_get_device_bdaddr(uint8_t deviceId, uint8_t* btAddr);

void fast_pair_enter_pairing_mode_handler(void);

void app_hfp_start_voice_media(uint8_t devId);

void app_hfp_resume_pending_voice_media(void);

bool app_hfp_is_starting_media_pending(void);

bool app_bt_is_in_reconnecting(void);

bool btapp_hfp_is_dev_sco_connected(uint8_t devId);

uint8_t a2dp_get_latest_paused_device(void);

bool app_bt_is_in_connecting_profiles_state(void);

void app_bt_clear_connecting_profiles_state(uint8_t devId);

void app_bt_set_connecting_profiles_state(uint8_t devId);

void app_a2dp_hold_mute();

void app_a2dp_unhold_mute();

#if defined(__INTERCONNECTION__)
btif_accessible_mode_t app_bt_get_current_access_mode(void);

bool app_bt_is_connected();
#endif

#ifdef VOICE_DATAPATH
bool bt_app_check_device_conneted_to_voicepath(uint8_t device_id);
#endif

bool app_bt_is_hfp_audio_on(void);

btif_remote_device_t* app_bt_get_connected_mobile_device_ptr(void);
void app_bt_set_spp_device_ptr(btif_remote_device_t* device);
btif_remote_device_t* app_bt_get_spp_device_ptr(void);

void app_bt_delay_connect_a2dp(uint32_t param1, uint32_t param2);

void app_bt_delay_connect_hfp(uint32_t param1, uint32_t param2);

bool app_bt_delay_connect_profile_working(void);

bool app_is_disconnecting_all_bt_connections(void);

void app_set_disconnecting_all_bt_connections(bool isEnable);

void app_bt_start_search(void);

#if defined(IBRT)
#if defined(ENHANCED_STACK)
uint32_t app_bt_save_a2dp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_a2dp_app_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_avrcp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_avrcp_app_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_hfp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_hfp_app_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_spp_app_ctx(uint32_t app_id,btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_spp_app_ctx(uint8_t *buf, uint32_t buf_len, uint32_t *app_id);
#ifdef __BTMAP_ENABLE__
uint32_t app_bt_save_map_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_map_app_ctx(uint8_t *buf, uint32_t buf_len);
#endif
#endif /* ENHANCED_STACK */
#endif

void app_stop_fast_connectable_ble_adv_timer(void);
int8_t app_bt_get_rssi(void);

#ifdef  TILE_DATAPATH
int8_t app_tile_get_ble_rssi(void);
#endif


a2dp_stream_t* app_bt_get_stream(enum BT_DEVICE_ID_T id);

int app_bt_get_bt_addr(enum BT_DEVICE_ID_T id, bt_bdaddr_t *bdaddr);

bool app_bt_a2dp_service_is_connected(void);

void app_wait_stack_ready(void);
#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
