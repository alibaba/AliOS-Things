/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BT_FUNC_H__
#define __APP_BT_FUNC_H__

#include "cmsis_os.h"
#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "me_api.h"
#include "a2dp_api.h"
#include "hfp_api.h"
#include "dip_api.h"

typedef enum _bt_fn_req {
        Me_switch_sco_req                           = 0,
        ME_SwitchRole_req                           = 1,
        ME_SetConnectionRole_req                    = 2,
        MeDisconnectLink_req                        = 3,
        ME_StopSniff_req                            = 4,
        ME_SetAccessibleMode_req                    = 5,
        Me_SetLinkPolicy_req                        = 6,
        CMGR_SetSniffTimer_req                      = 7,
        CMGR_SetSniffInofToAllHandlerByRemDev_req   = 8,
        A2DP_OpenStream_req                         = 9,
        A2DP_CloseStream_req                        = 10,
        A2DP_SetMasterRole_req                      = 11,
        HF_CreateServiceLink_req                    = 12,
        HF_DisconnectServiceLink_req                = 13,
        HF_CreateAudioLink_req                      = 14,
        HF_DisconnectAudioLink_req                  = 15,
        HF_EnableSniffMode_req                      = 16,
        HF_SetMasterRole_req                        = 17,
#if defined (__HSP_ENABLE__)
        HS_CreateServiceLink_req                    = 18,
        HS_CreateAudioLink_req                      = 19,
        HS_DisconnectAudioLink_req                  = 20,
        HS_EnableSniffMode_req                      = 21,
        HS_DisconnectServiceLink_req                = 22,
#endif
        BT_Control_SleepMode_req                    = 23,
        BT_Custom_Func_req                          = 24,
        ME_StartSniff_req                           = 25,
#ifdef BTIF_DIP_DEVICE
        DIP_QuryService_req                         = 26,
#endif
        A2DP_Force_OpenStream_req                   = 27,
        HF_Force_CreateServiceLink_req              = 28,
        BT_Red_Ccmp_Client_Open                     = 29,
        BT_Set_Access_Mode_Test                     = 30,
        BT_Set_Adv_Mode_Test                     = 31,



}bt_fn_req;
typedef void (*APP_BT_REQ_CUSTOMER_CALl_CB_T)(void *, void *);

typedef union _bt_fn_param {
    // BtStatus Me_switch_sco(uint16_t  scohandle)
    struct {
        uint16_t  scohandle;
    } Me_switch_sco_param;

    // BtStatus ME_SwitchRole(btif_remote_device_t *remDev)
    struct {
        btif_remote_device_t* remDev;
    } ME_SwitchRole_param;

    //BtConnectionRole ME_SetConnectionRole(BtConnectionRole role)
    struct {
        btif_connection_role_t  role;
    } BtConnectionRole_param;

    // void MeDisconnectLink(btif_remote_device_t* remDev)
    struct {
        btif_remote_device_t* remDev;
    } MeDisconnectLink_param;

    //BtStatus ME_StopSniff(btif_remote_device_t *remDev)
    struct {
        btif_remote_device_t* remDev;
    } ME_StopSniff_param;

    struct {
        btif_remote_device_t* remDev;
        btif_sniff_info_t sniffInfo;
    } ME_StartSniff_param;

    struct {
        bool isEnable;
    } ME_BtControlSleepMode_param;

    struct {
        bool isEnable;
    } ME_BtSetAdvMode_param;
    
    //BtStatus ME_SetAccessibleMode(btif_accessible_mode_t mode, const btif_access_mode_info_t *info)
    struct {
        btif_accessible_mode_t mode;
        btif_access_mode_info_t info;
    } ME_SetAccessibleMode_param;

    //BtStatus Me_SetLinkPolicy(btif_remote_device_t *remDev, btif_link_policy_t policy)
    struct {
        btif_remote_device_t *remDev;
        btif_link_policy_t policy;
    } Me_SetLinkPolicy_param;

    /*BtStatus CMGR_SetSniffTimer(CmgrHandler *Handler,
                                btif_sniff_info_t* SniffInfo,
                                TimeT Time)
       */
    struct {
        btif_cmgr_handler_t *Handler;
        btif_sniff_info_t SniffInfo;
        TimeT Time;
    } CMGR_SetSniffTimer_param;

    /*BtStatus CMGR_SetSniffInofToAllHandlerByRemDev(btif_sniff_info_t* SniffInfo,
                                                                btif_remote_device_t *RemDev)
       */
    struct {
        btif_sniff_info_t SniffInfo;
        btif_remote_device_t *RemDev;
    } CMGR_SetSniffInofToAllHandlerByRemDev_param;

    //BtStatus A2DP_OpenStream(a2dp_stream_t *Stream, bt_bdaddr_t *Addr)
    struct {
        a2dp_stream_t *Stream;
        bt_bdaddr_t *Addr;
    } A2DP_OpenStream_param;

    //BtStatus A2DP_CloseStream(a2dp_stream_t *Stream);
    struct {
        a2dp_stream_t *Stream;
    } A2DP_CloseStream_param;

    //BtStatus A2DP_SetMasterRole(a2dp_stream_t *Stream, BOOL Flag);
    struct {
        a2dp_stream_t *Stream;
        BOOL Flag;
    } A2DP_SetMasterRole_param;

    //BtStatus HF_CreateServiceLink(HfChannel *Chan, bt_bdaddr_t *Addr)
    struct {
        hf_chan_handle_t Chan;
        bt_bdaddr_t *Addr;
    } HF_CreateServiceLink_param;

    //bt_status_t HF_DisconnectServiceLink(hf_chan_handle_t Chan)
    struct {
        hf_chan_handle_t Chan;
    } HF_DisconnectServiceLink_param;

    //bt_status_t HF_CreateAudioLink(hf_chan_handle_t Chan)
    struct {
        hf_chan_handle_t Chan;
    } HF_CreateAudioLink_param;

    //bt_status_t HF_DisconnectAudioLink(hf_chan_handle_t Chan)
    struct {
        hf_chan_handle_t Chan;
    } HF_DisconnectAudioLink_param;

    //bt_status_t HF_EnableSniffMode(hf_chan_handle_t Chan, BOOL Enable)
    struct {
        hf_chan_handle_t Chan;
        BOOL Enable;
    } HF_EnableSniffMode_param;

    //bt_status_t HF_SetMasterRole(hf_chan_handle_t Chan, BOOL Flag);
    struct {
        hf_chan_handle_t Chan;
        BOOL Flag;
    } HF_SetMasterRole_param;

#ifdef BTIF_DIP_DEVICE
    struct {
        btif_remote_device_t* remDev;
        btif_dip_client_t *dip_client;
    } DIP_QuryService_param;
#endif

#if defined (__HSP_ENABLE__)
    //bt_status_t HS_CreateServiceLink(HsChannel *Chan, bt_bdaddr_t *Addr)
    struct {
        HsChannel *Chan;
        bt_bdaddr_t *Addr;
    } HS_CreateServiceLink_param;

    //BtStatus HS_CreateAudioLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_CreateAudioLink_param;

    //BtStatus HS_DisconnectAudioLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_DisconnectAudioLink_param;

    //BtStatus HS_DisconnectServiceLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_DisconnectServiceLink_param;

    //BtStatus HS_EnableSniffMode(HsChannel *Chan, BOOL Enable)
    struct {
        HsChannel *Chan;
        BOOL Enable;
    } HS_EnableSniffMode_param;
#endif

    struct {
        uint32_t func_ptr;
        uint32_t param0;
        uint32_t param1;
    } CustomFunc_param;

} bt_fn_param;

typedef struct {
    uint32_t src_thread;
    uint32_t request_id;
    bt_fn_param param;
} APP_BT_MAIL;

typedef struct
{
    btif_remote_device_t  *remDev;
    btif_link_policy_t    policy;
} BT_SET_LINKPOLICY_REQ_T;

int app_bt_mail_init(void);

int app_bt_Me_switch_sco(uint16_t  scohandle);

int app_bt_ME_SwitchRole(btif_remote_device_t* remDev);

int app_bt_ME_SetConnectionRole(btif_connection_role_t  role);

int app_bt_MeDisconnectLink(btif_remote_device_t* remDev);

int app_bt_ME_StopSniff(btif_remote_device_t *remDev);

int app_bt_ME_SetAccessibleMode(btif_accessible_mode_t mode, const btif_access_mode_info_t *info);

int app_bt_Me_SetLinkPolicy(btif_remote_device_t *remDev, btif_link_policy_t policy);

int app_bt_CMGR_SetSniffTimer(btif_cmgr_handler_t *Handler,
                                        btif_sniff_info_t* SniffInfo,
                                        TimeT Time);

int app_bt_CMGR_SetSniffInfoToAllHandlerByRemDev(btif_sniff_info_t* SniffInfo,
                                                                 btif_remote_device_t *RemDev);

int app_bt_A2DP_OpenStream(a2dp_stream_t *Stream, bt_bdaddr_t *Addr);

int app_bt_A2DP_CloseStream(a2dp_stream_t *Stream);

int app_bt_A2DP_SetMasterRole(a2dp_stream_t *Stream, BOOL Flag);

int app_bt_HF_CreateServiceLink(hf_chan_handle_t Chan, bt_bdaddr_t *Addr);

int app_bt_HF_DisconnectServiceLink(hf_chan_handle_t Chan);

int app_bt_HF_CreateAudioLink(hf_chan_handle_t Chan);

int app_bt_HF_DisconnectAudioLink(hf_chan_handle_t Chan);

int app_bt_HF_EnableSniffMode(hf_chan_handle_t Chan, BOOL Enable);

int app_bt_HF_SetMasterRole(hf_chan_handle_t Chan, BOOL Flag);

void app_bt_accessible_manager_process(const btif_event_t *Event);
void app_bt_role_manager_process(const btif_event_t* Event);
void app_bt_sniff_manager_process(const btif_event_t *Event);
//void app_bt_golbal_handle_hook(const btif_event_t *Event);

int app_bt_ME_ControlSleepMode(bool isEnabled);

#ifdef BTIF_DIP_DEVICE
int app_bt_dip_QuryService(btif_dip_client_t *client, btif_remote_device_t* rem);
#endif

#if defined (__HSP_ENABLE__)
int app_bt_HS_CreateServiceLink(HsChannel *Chan, bt_bdaddr_t *Addr);

int app_bt_HS_CreateAudioLink(HsChannel *Chan);

int app_bt_HS_DisconnectAudioLink(HsChannel *Chan);

int app_bt_HS_DisconnectServiceLink(HsChannel *Chan);

int app_bt_HS_EnableSniffMode(HsChannel *Chan, BOOL Enable);

#endif
bool app_is_access_mode_set_pending(void);
void app_set_pending_access_mode(void);
void app_bt_set_linkpolicy(btif_remote_device_t *remDev, btif_link_policy_t policy);
void app_check_pending_stop_sniff_op(void);
BT_SET_LINKPOLICY_REQ_T* app_bt_pop_pending_set_linkpolicy(void);
void app_retry_setting_access_mode(void);
void app_set_accessmode(btif_accessible_mode_t mode);

int app_bt_start_custom_function_in_bt_thread(
    uint32_t param0, uint32_t param1, uint32_t funcPtr);
int app_bt_ME_StartSniff(btif_remote_device_t *remDev, btif_sniff_info_t* sniffInfo);

int app_bt_Force_A2DP_OpenStream(a2dp_stream_t *Stream, bt_bdaddr_t *Addr);

int app_bt_Force_HF_CreateServiceLink(hf_chan_handle_t Chan, bt_bdaddr_t *Addr);


#ifdef __cplusplus
}
#endif
#endif /* __APP_BT_FUNC_H__ */

