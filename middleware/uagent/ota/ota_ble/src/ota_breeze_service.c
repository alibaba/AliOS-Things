#include "ota_breeze.h"
#include "ota_breeze_os.h"
#include "breeze_export.h"
#include "ota_breeze_plat.h"
#include "ota_breeze_export.h"

static _ota_ble_global_dat_t g_ctx;

_ota_ble_global_dat_t* ota_breeze_get_global_data_center()
{
    return &g_ctx;
}

void ota_breeze_set_task_active_ctrl(volatile unsigned char is_enable)
{
    g_ctx.ota_breeze_task_active_ctrl = is_enable;
}

volatile unsigned char ota_breeze_get_task_active_ctrl_status()
{
    return g_ctx.ota_breeze_task_active_ctrl;
}

void ota_breeze_set_task_active_flag(volatile unsigned char flag)
{
    g_ctx.ota_breeze_task_active_flag = flag;
}

volatile unsigned char ota_breeze_get_task_active_flag()
{
    return g_ctx.ota_breeze_task_active_flag;
}


void ota_breeze_set_status(ota_breeze_state_t status)
{
    g_ctx.ota_breeze_status = status;
}

ota_breeze_state_t ota_breeze_get_status()
{
    return g_ctx.ota_breeze_status;
}

static int ota_breeze_parse_firmware_version(unsigned char *data, unsigned char len)
{
    int err_code = 0;
    unsigned int v[3];
    unsigned char  l_data[OTA_BREEZE_FW_VER_LEN + 1]; // +1 for trailing zero
    if ((data == NULL) || (len == 0)) {
        return -1;
    }
    memcpy(l_data, data, len);
    l_data[len] = 0;

    if(ota_breeze_split_sw_ver((char *)l_data, v, v + 1, v + 2) < 0) {
        return -1;
    }
    memset(l_data, 0, sizeof(l_data));
    sprintf((char *)l_data, "%d.%d.%d", (int)v[0], (int)v[1], (int)v[2]);

    if (memcmp(l_data, data, len) == 0) {
        return 0;
    } else {
        return -1;
    }
}

static int ota_breeze_set_version(unsigned char* fw_ver, unsigned char fw_ver_len)
{
    memset(&g_ctx.verison, 0x00, sizeof(ota_breeze_version_t));
    if (fw_ver_len == 0 || fw_ver_len > OTA_BREEZE_FW_VER_LEN) {
        return -1;
    }
    g_ctx.verison.fw_ver_len = fw_ver_len;
    memcpy(g_ctx.verison.fw_ver, fw_ver, fw_ver_len);
    return 0;
}

ota_breeze_version_t* ota_breeze_get_version()
{
    return &g_ctx.verison;
}

void ota_breeze_disconnect()
{
    /* still have data feedback to app, so do disconnection after a * while */
    ota_breeze_msleep(2000);
    breeze_disconnect_ble();
}

void ota_breeze_process_message(breeze_otainfo_t* breeze_info)
{
    if(breeze_info != NULL) {
        if(breeze_info->type == OTA_CMD) {
            ota_breeze_get_data(breeze_info->cmd_evt.m_cmd.cmd, breeze_info->cmd_evt.m_cmd.frame,
                breeze_info->cmd_evt.m_cmd.data, breeze_info->cmd_evt.m_cmd.len);
        }
        else if(breeze_info->type == OTA_EVT) {
            ota_breeze_relate_event(breeze_info->cmd_evt.m_evt.evt, breeze_info->cmd_evt.m_evt.d);
        }
    }
}

int ota_breeze_service_init(ota_breeze_service_manage_t* ota_manage)
{
    if(ota_manage == NULL) {
        return -1;
    }
    if(ota_breeze_parse_firmware_version(ota_manage->verison.fw_ver, ota_manage->verison.fw_ver_len) < 0) {
        OTA_BREEZE_LOG_E("ver parse failed\r\n");
        return -1;
    }
    memset(&g_ctx, 0x00, sizeof(_ota_ble_global_dat_t));
    if(ota_breeze_set_version(ota_manage->verison.fw_ver, ota_manage->verison.fw_ver_len) < 0) {
        OTA_BREEZE_LOG_E("ver set failed");
        return -1;
    }
    g_ctx.feature_enable = ota_manage->is_ota_enable;
    ota_manage->get_dat_cb = ota_breeze_process_message;

    ota_breeze_set_status(OTA_BREEZE_STATE_OFF);
    return 0;
}

