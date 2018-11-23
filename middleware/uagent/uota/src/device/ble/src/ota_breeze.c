#include "ota_breeze.h"
#include "ota_hal_os.h"

static ota_breeze_rec_t* ota_breeze_queue = NULL;
static uint8_t ota_breeze_queue_ptr_in = 0;
static uint8_t ota_breeze_queue_ptr_out = 0;
_ota_ble_global_dat_t g_ctx;

static int ota_breeze_set_version(uint8_t* fw_ver, uint8_t fw_ver_len)
{
    memset(&g_ctx.verison, 0x00, sizeof(ota_breeze_version_t));
    if (fw_ver_len == 0 || fw_ver_len > OTA_BREEZE_FW_VER_LEN) {
        return -1;
    }
    g_ctx.verison.fw_ver_len = fw_ver_len;
    memcpy(g_ctx.verison.fw_ver, fw_ver, fw_ver_len);
    return 0;
}

static uint32_t ota_breeze_creat_receive_buf()
{
    if(ota_breeze_queue != NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    ota_breeze_queue = ota_malloc(OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    if(ota_breeze_queue == NULL) {
        return OTA_BREEZE_ERROR_NULL;
    }
    memset(ota_breeze_queue, 0x00, OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    return OTA_BREEZE_SUCCESS;
}

void ota_breeze_destroy_receive_buf()
{
    if(ota_breeze_queue != NULL) {
        ota_free(ota_breeze_queue);
    }
    ota_breeze_queue = NULL;
}

static uint32_t ota_breeze_init_receive_buf()
{
    uint32_t error_code = 0;
    ota_breeze_destroy_receive_buf();
    error_code = ota_breeze_creat_receive_buf();
    if(error_code != OTA_BREEZE_SUCCESS) {
        return error_code;
    }
    ota_breeze_queue_ptr_in = 0;
    ota_breeze_queue_ptr_out = 0;
    return OTA_BREEZE_SUCCESS;
}

static void ota_breeze_receive_data_product(uint8_t ota_cmd, uint8_t num_frame, uint8_t *buffer, uint32_t length)
{
    if(ota_breeze_queue == NULL) {
        return;
    }
    if(length > OTA_BREEZE_REC_PER_FRAME_LEN) {
        OTA_LOG_E("ota breeze product data: input data length too long");
        return;
    }

    ota_breeze_queue[ota_breeze_queue_ptr_in].cmd = ota_cmd;
    ota_breeze_queue[ota_breeze_queue_ptr_in].length = length;
    ota_breeze_queue[ota_breeze_queue_ptr_in].num_frames = num_frame;
    if(buffer != NULL) {
        memcpy(ota_breeze_queue[ota_breeze_queue_ptr_in].rec_buf, buffer, length);
    }
    ota_breeze_queue_ptr_in++;
    ota_breeze_queue_ptr_in = ota_breeze_queue_ptr_in % OTA_BREEZE_RECEIVE_BUF_NUMB;
}

uint32_t ota_breeze_receive_data_consume(ota_breeze_rec_t* out_queue)
{
    if(ota_breeze_queue == NULL) {
        return OTA_BREEZE_ERROR_NULL;
    }
    if(ota_breeze_queue_ptr_out != ota_breeze_queue_ptr_in) {
        out_queue->cmd = ota_breeze_queue[ota_breeze_queue_ptr_out].cmd;
        out_queue->length = ota_breeze_queue[ota_breeze_queue_ptr_out].length;
        out_queue->num_frames = ota_breeze_queue[ota_breeze_queue_ptr_out].num_frames;
        memcpy(out_queue->rec_buf, ota_breeze_queue[ota_breeze_queue_ptr_out].rec_buf, out_queue->length);
        ota_breeze_queue_ptr_out++;
        ota_breeze_queue_ptr_out = ota_breeze_queue_ptr_out % OTA_BREEZE_RECEIVE_BUF_NUMB;
    }
    else {
        return OTA_BREEZE_ERROR_NULL;
    }
    return OTA_BREEZE_SUCCESS;
}

static void ota_breeze_init()
{
    g_ctx.ota_breeze_status = OTA_BREEZE_STATE_IDLE;
    g_ctx.ota_breeze_task_active_ctrl = true;
    g_ctx.ota_breeze_task_active_flag = true;
}

static uint32_t ota_breeze_start()
{
    ota_breeze_init();

    while(1) {
        if(g_ctx.ota_breeze_task_active_ctrl == false){
            OTA_LOG_I("ota breeze task need to over!");
        }
	extern void ota_breeze_cmd_disptacher(void);
	ota_breeze_cmd_disptacher();
        ota_msleep(1);
    }
}

static uint32_t ota_breeze_start_task()
{
    int ret = 0;
    uint32_t error_code = 0;
    void *thread = NULL;
    ret = ota_thread_create(&thread, (void *)ota_breeze_start, (void *)NULL, NULL, 0);
    if (ret != 0) {
        OTA_LOG_E("ota breeze creat task failed");
        error_code = OTA_BREEZE_ERROR_BUSY;
    }
    return error_code;
}

void ota_breeze_process_message(breeze_otainfo_t* breeze_info)
{
    if(breeze_info != NULL) {
        if(breeze_info->type == OTA_CMD) {
            ota_breeze_get_data(breeze_info->cmd_evt.m_cmd.cmd, breeze_info->cmd_evt.m_cmd.frame,
                breeze_info->cmd_evt.m_cmd.data, breeze_info->cmd_evt.m_cmd.len);
        }
        else if(breeze_info->type == OTA_EVT) {
            ota_breeze_event_dispatcher(breeze_info->cmd_evt.m_evt.evt, breeze_info->cmd_evt.m_evt.d);
        }
    }
}

int ota_breeze_service_init(ota_breeze_service_manage_t* ota_manage)
{
    if(ota_manage == NULL) {
        return -1;
    }
    memset(&g_ctx, 0x00, sizeof(_ota_ble_global_dat_t));
    if(ota_breeze_set_version(ota_manage->verison.fw_ver, ota_manage->verison.fw_ver_len) < 0) {
        OTA_LOG_E("ota breeze verison set failed");
        return -1;
    }
    g_ctx.feature_enable = ota_manage->is_ota_enable;
    ota_manage->get_dat_cb = ota_breeze_process_message;

    ais_ota_flash_init();
    ais_ota_settings_init();
    g_ctx.ota_breeze_status = OTA_BREEZE_STATE_OFF;
    g_ctx.bank_1_addr = ais_ota_get_dst_addr();
}

void ota_breeze_get_data(uint8_t ota_cmd, uint8_t num_frame, uint8_t *buffer, uint32_t length)
{
    volatile uint8_t tm_cnt = 0;
    if((g_ctx.ota_breeze_status == OTA_BREEZE_STATE_IDLE) && (ota_cmd == OTA_BREEZE_CMD_FW_VERSION_REQ)) {
        if(g_ctx.ota_breeze_task_active_flag == true) {
            g_ctx.ota_breeze_task_active_flag = false;
            while(g_ctx.ota_breeze_task_active_flag == true) {
                g_ctx.ota_breeze_task_active_flag = false;
                ota_msleep(1);
                if(tm_cnt > 2) {
                    OTA_LOG_E("be over last ota task failed");
                    return;
                }
                tm_cnt++;
            }
        }

        if(ota_breeze_init_receive_buf() != OTA_BREEZE_SUCCESS) {
            OTA_LOG_E("ota breeze init rececive buf failed");
            return;
        }

        ota_breeze_receive_data_product(ota_cmd, num_frame, buffer, length);
        if(ota_breeze_start_task() != OTA_BREEZE_SUCCESS) {
            OTA_LOG_E("ota breeze task creat failed");
            return;
        }
    }
    else if((g_ctx.ota_breeze_task_active_flag == true) && (g_ctx.ota_breeze_status != OTA_BREEZE_STATE_OFF)) {
        ota_breeze_receive_data_product(ota_cmd, num_frame, buffer, length);
    }
    else {
        g_ctx.ota_breeze_task_active_ctrl = false;
        OTA_LOG_E("ota breeze status error");
    }

}

