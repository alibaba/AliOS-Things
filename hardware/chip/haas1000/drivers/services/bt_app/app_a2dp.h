/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_A2DP_H__
#define __APP_A2DP_H__
#include "btapp.h"

#if defined(__cplusplus)
extern "C" {
#endif

bool a2dp_is_music_ongoing(void);
int a2dp_volume_set(enum BT_DEVICE_ID_T id, uint8_t vol);
int avrcp_get_current_media_status(enum BT_DEVICE_ID_T device_id);
void ag_adapter_send_panel_key(uint8_t device_id, avrcp_panel_operation_t op);
int avctp_adapter_get_media_info(uint8_t device_id);
int avctp_adapter_set_absolute_volume(uint8_t device_id, uint8_t volume);
#if defined(A2DP_LDAC_ON)
void app_ibrt_restore_ldac_info(uint8_t sample_freq);
#endif
#ifdef __cplusplus
}
#endif

#endif
