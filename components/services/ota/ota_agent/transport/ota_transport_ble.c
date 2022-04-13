#include <string.h>
#include "ota_ble.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal_trans.h"
#include "ota_hal.h"

extern ota_ble_global_dat_t g_ctx;

int ota_ble_check_upgrade_result(int *ota_result);
int ble_file_translate_ubus_init(void);
int ota_ble_transport_auth(unsigned char is_auth)
{
    int ret = 0;
    int ota_result = -1;
    unsigned char fwup_success = 0x00;
    ret = ota_ble_check_upgrade_result(&ota_result);
    if (ret == 0) {
        if(ota_result == 0) {
            fwup_success = 0x01;
            OTA_LOG_I("OTA OK!");
        }
        else {
            OTA_LOG_I("OTA ver cmp failed!");
        }
        if(ota_ble_transport(OTA_BLE_CMD_FW_UPDATE_PROCESS, &fwup_success, 1) == OTA_SUCCESS) {
             OTA_LOG_I("OTA result send ok!");
        }
        else {
             OTA_LOG_E("OTA result send failed");
        }
    }
    if(ota_hal_rollback() < 0) {
        OTA_LOG_E("ble ota clear flag faild!");
    }
    if (is_auth != 0) {
        g_ctx.status = OTA_BLE_STATE_IDLE;
    }
#ifndef BOARD_PCA10056
    ret = ble_file_translate_ubus_init();
    if(ret != 0) {
        OTA_LOG_E("ble ubus init failed");
    }
#endif
    OTA_LOG_I("auth over");
    return ret;
}

int ota_ble_send_bytes_received()
{
    int err_code = -1;
    unsigned char tx_buff[16];
    memset(tx_buff, 0x00, sizeof(tx_buff));
    ENCODE_U16(tx_buff, g_ctx.frames_recvd);
    ENCODE_U32(tx_buff + sizeof(unsigned short), g_ctx.bytes_recvd);
    err_code = breeze_post_ext(OTA_BLE_CMD_FW_BYTES_RECEIVED, tx_buff, sizeof(unsigned short) + sizeof(unsigned int));
    if (err_code != OTA_SUCCESS) {
        OTA_LOG_E("send rec err");
    }
    return err_code;
}

int ota_ble_transport_done(unsigned char cmd)
{
    int ret = 0;
    if(g_ctx.status == OTA_BLE_STATE_RESET_PREPARE) {
        if (cmd == OTA_BLE_CMD_FW_CHECK_RESULT) {
            ota_msleep(2000);
            ota_ble_disconnect();
        }
    }
    return ret;
}

void ota_ble_transport_msg(breeze_otainfo_t *ble_info)
{
    int ret = 0;
    if(ble_info != NULL) {
        if(ble_info->type == OTA_CMD) {
            ret = ota_ble_download(ble_info->cmd_evt.m_cmd.cmd, ble_info->cmd_evt.m_cmd.frame, ble_info->cmd_evt.m_cmd.data, ble_info->cmd_evt.m_cmd.len);
        }
        else if(ble_info->type == OTA_EVT) {
            switch(ble_info->cmd_evt.m_evt.evt) {
                case ALI_OTA_ON_AUTH_EVT:
                   ota_ble_transport_auth(ble_info->cmd_evt.m_evt.d);
                   break;
                case ALI_OTA_ON_TX_DONE:
                   ota_ble_transport_done(ble_info->cmd_evt.m_evt.d);
                   break;
                case ALI_OTA_ON_DISCONNECTED:
                    if(g_ctx.status == OTA_BLE_STATE_RESET_PREPARE) {
                        ota_msleep(1000);
                        ota_reboot();
                    }
                   break;
                case ALI_OTA_ON_DISCONTINUE_ERR:
                   ret = ota_ble_send_bytes_received();
                   if(ret != OTA_SUCCESS) {
                       OTA_LOG_E("err-frame resent fail");
                   }
                   break;
                default:
                   break;
            }
        }
    }
}
