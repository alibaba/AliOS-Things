/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BT_IF_H_
#define __BT_IF_H_
#include "btif_sys_config.h"

//Application ID,indentify profle app context
#define BTIF_APP_INVALID_ID                    (0x00000000)
#define BTIF_APP_HFP_PROFILE_ID                (0x00000001)
#define BTIF_APP_A2DP_PROFILE_ID               (0x00000002)
#define BTIF_APP_AVRCP_PROFILE_ID              (0x00000004)

#define BTIF_APP_SPP_SERVER_DEFAULT_ID         (0x00000010)
#define BTIF_APP_SPP_CLIENT_AI_VOICE_ID        (0x00000020)
#define BTIF_APP_SPP_SERVER_AI_VOICE_ID        (0x00000040)
#define BTIF_APP_SPP_SERVER_INTERACTION_ID     (0x00000080)
#define BTIF_APP_SPP_CLIENT_CCMP_ID            (0x00000100)
#define BTIF_APP_SPP_CLIENT_INTERCONNECTION_ID (0x00000200)
#define BTIF_APP_SPP_SERVER_INTERCONNECTION_ID (0x00000400)
#define BTIF_APP_SPP_SERVER_SMARTVOICE_ID      (0x00000800)
#define BTIF_APP_SPP_SERVER_TOTAD_ID           (0x00001000)
#define BTIF_APP_SPP_SERVER_GSOUND_CTL_ID      (0x00002000)
#define BTIF_APP_SPP_SERVER_GSOUND_AUD_ID      (0x00004000)
#define BTIF_APP_SPP_SERVER_OTA_ID             (0x00008000)
#define BTIF_APP_SPP_SERVER_FP_RFCOMM_ID       (0x00010000)

#define BTIF_TWS_LINK_CONNECTED                (1 << 0)
#define BTIF_TWS_L2CAP_CONNECTED               (1 << 1)

#ifdef __cplusplus
extern "C" {
#endif                          /*  */

    enum pair_event {
        PAIR_EVENT_NUMERIC_REQ,
        PAIR_EVENT_COMPLETE,
    };
    typedef void (*pairing_callback_t) (enum pair_event, const void *);
    typedef void (*authing_callback_t) (void);

    int bt_stack_initilize(void);
    int bt_pairing_init(pairing_callback_t pair_cb);
#ifdef BTIF_SECURITY
    int bt_authing_init(authing_callback_t auth_cb);
#else
    static inline int bt_authing_init(authing_callback_t auth_cb) {
        return 0;
    }
#endif
    int a2dp_codec_init(void);
    int bt_stack_config(void);
    int bt_set_local_dev_name(const unsigned char *dev_name, u8 len);
    void bt_process_stack_events(void);
    void bt_generate_ecdh_key_pair(void);

    uint8_t bt_get_sco_number(void);
    void bt_set_sco_number(uint8_t sco_num);
    void bt_fast_init(uint8_t* bt_addr, uint8_t* ble_addr);

    void btif_set_btstack_chip_config(void *config);

#if defined(IBRT)
uint32_t btif_save_app_bt_device_ctx(uint8_t *ctx_buffer,uint32_t app_id);
uint32_t btif_set_app_bt_device_ctx(uint8_t *ctx_buffer,uint32_t app_id,uint8_t bt_devices_idx, uint8_t rm_detbl_idx, uint8_t avd_ctx_device_idx);
#endif

#ifdef __cplusplus
}
#endif /*  */

#endif /*__BT_IF_H_*/
