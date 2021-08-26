/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BTMAP_SMS_H__
#define __APP_BTMAP_SMS_H__

#include "bluetooth.h"
#include "btapp.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_btmap_sms_init(void);
void app_btmap_sms_open(BT_DEVICE_ID_T id, bt_bdaddr_t *remote);
void app_btmap_sms_close(BT_DEVICE_ID_T id);
void app_btmap_sms_send(BT_DEVICE_ID_T id, char* telNum, char* msg);
bool app_btmap_check_is_connected(BT_DEVICE_ID_T id);
void app_btmap_sms_save(BT_DEVICE_ID_T id, char* telNum, char* msg);
void app_btmap_sms_resend(void);

struct SMS_MSG_T{
    char* telNum;
    char* msg;
    uint8_t telNumLen;
    uint32_t msgLen;
};

#ifdef __cplusplus
}
#endif

#endif /*__APP_BTMAP_SMS_H__*/

