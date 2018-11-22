#include "ota_breeze.h"
#include "ota_hal_os.h"
#include "ota_log.h"

static ota_breeze_rec_t* ota_breeze_queue = NULL;
static uint8_t ota_breeze_queue_ptr_in = 0;
static uint8_t ota_breeze_queue_ptr_out = 0;

static uint32_t ota_breeze_creat_receive_buf()
{
    if(ota_breeze_queue != NULL) {
        OTA_LOG_E("ota breeze init:cmd buf is not null");
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    ota_breeze_queue = ota_malloc(OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    if(ota_breeze_queue == NULL) {
        OTA_LOG_E("ota breeze init:cmd buf malloc failed");
        return OTA_BREEZE_ERROR_NULL;
    }
    memset(ota_breeze_queue, 0x00, OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    return OTA_BREEZE_SUCCESS;
}

static void ota_breeze_destroy_receive_buf()
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
        OTA_LOG_E("ota breeze queue is null");
        return;
    }
//    OTA_LOG_I("ota_cmd:%x, num_frame:%x, length:%x", ota_cmd, num_frame, length);
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

static uint32_t ota_breeze_receive_data_consume(ota_breeze_rec_t* out_queue)
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

static uint32_t ota_breeze_init()
{
    ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
    ota_breeze_set_task_active_ctrl(true);
    ota_breeze_set_task_active_flag(true);
    return OTA_BREEZE_SUCCESS;
}

static uint32_t ota_breeze_start()
{
    ota_breeze_rec_t tmp_queue;
    ota_breeze_state_t cur_breeze_status;
    ota_breeze_version_t* tmp_verion = NULL;
    uint32_t error_code = 0;
    uint8_t send_err = false;
    if(ota_breeze_init() != OTA_BREEZE_SUCCESS) {
        OTA_LOG_E("ota breeze init failed");
        return OTA_BREEZE_ERROR_NULL;
    }

    while(1) {
        if(ota_breeze_get_task_active_ctrl_status() == false) {
            OTA_LOG_I("ota breeze task need to over!");
            goto OTA_BREEZE_OVER;
        }
        if(ota_breeze_receive_data_consume(&tmp_queue) == OTA_BREEZE_SUCCESS) {
            if ((tmp_queue.cmd & OTA_BREEZE_CMD_TYPE_MASK) != OTA_BREEZE_CMD_TYPE_FW_UPGRADE) {
                    OTA_LOG_E("ota breeze receive error cmd");
            }
            else {
                cur_breeze_status = ota_breeze_get_status();
                switch(cur_breeze_status) {
                    case OTA_BREEZE_STATE_IDLE:
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_VERSION_REQ) { // cmd=0x20
                            tmp_verion = ota_breeze_get_version();
                            error_code = ota_breeze_send_fw_version_rsp(OTA_BREEZE_CMD_FW_VERSION_RSP, tmp_verion->fw_ver, tmp_verion->fw_ver_len);     
                            if(error_code != OTA_BREEZE_SUCCESS) {
                                OTA_LOG_E("ota breeze send verion failed");
                                goto OTA_BREEZE_OVER;
                            }
                        }
                        else if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_UPGRADE_REQ) { // cmd=0x22
                            error_code = ota_breeze_on_fw_upgrade_req(tmp_queue.rec_buf, tmp_queue.length);
                            if(error_code != OTA_BREEZE_SUCCESS) {
                                OTA_LOG_E("ota breeze ota permission report failed");
                                goto OTA_BREEZE_OVER;
                            }
                        }
                        else {
                            send_err = true;
                        }
                        break;

                    case OTA_BREEZE_STATE_RECEIVE:
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_DATA) { // cmd=0x2F
                            ota_breeze_on_fw_data(tmp_queue.rec_buf, tmp_queue.length, tmp_queue.num_frames);
                        }
                        else if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_GET_INIT_FW_SIZE) { // cmd=0x27
                            if(ota_breeze_get_init_fw_size() != OTA_BREEZE_SUCCESS) {
                              send_err = true;
                            }
                        }
                        else {
                            send_err = true;
                        }
                        break;
                    case OTA_BREEZE_STATE_FW_CHECK:
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_XFER_FINISH) { // cmd=0x28
                            //char flag[5] = "Yes";
                            //int  len     = sizeof(flag);
                            ota_breeze_on_xfer_finished(tmp_queue.rec_buf, tmp_queue.length);
                            OTA_LOG_I("Firmware download completed, let's set the flag.\r\n");
                        }
                        else {
                            send_err = true;
                        }
                        break;
                    case OTA_BREEZE_STATE_UPGRADE_REPORT:
                    case OTA_BREEZE_STATE_RESET_PREPARE:
                    case OTA_BREEZE_STATE_OFF:
                    case OTA_BREEZE_STATE_WRITE:
                        send_err = true;
                        break;
                    default:
                        break;
                    }
                }
                if (send_err) {
                    (void)ota_breeze_send_error();
                }
        }
        ota_msleep(1);
    }
OTA_BREEZE_OVER:
    OTA_LOG_I("ota breeze task is over!!!!!!!!!!!");
    ota_breeze_destroy_receive_buf();
    ota_breeze_set_task_active_flag(false);
    return 0;
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

void ota_breeze_get_data(uint8_t ota_cmd, uint8_t num_frame, uint8_t *buffer, uint32_t length)
{
    volatile uint8_t tm_cnt = 0;
    if((ota_breeze_get_status() == OTA_BREEZE_STATE_IDLE) && (ota_cmd == OTA_BREEZE_CMD_FW_VERSION_REQ)) {
        if(ota_breeze_get_task_active_flag() == true) {
            ota_breeze_set_task_active_ctrl(false);
            while(ota_breeze_get_task_active_flag() == true) {
                ota_breeze_set_task_active_ctrl(false);
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
    else if((ota_breeze_get_task_active_flag() == true) && (ota_breeze_get_status() != OTA_BREEZE_STATE_OFF)) {
        ota_breeze_receive_data_product(ota_cmd, num_frame, buffer, length);
    }
    else {
        ota_breeze_set_task_active_ctrl(false);
        OTA_LOG_E("ota breeze status error");
    }

}

void ota_breeze_relate_event(uint8_t event_type, uint8_t sub_status)
{
    //OTA_LOG_I("event:%d, sub_event:%d", event_type, sub_status);
    switch(event_type) {
        case ALI_OTA_ON_AUTH_EVT:
            ota_breeze_on_auth(sub_status);
            break;
        case ALI_OTA_ON_TX_DONE:
            ota_breeze_on_tx_done(sub_status);
            break;
        case ALI_OTA_ON_DISCONNECTED:
            ota_msleep(1000);
            ota_breeze_set_task_active_ctrl(false);
            ota_msleep(1000);
            ota_breeze_reset();
            break;
        case ALI_OTA_ON_DISCONTINUE_ERR:
            ota_msleep(1000);
            ota_breeze_set_task_active_ctrl(false);
            ota_msleep(1000);
            ota_breeze_on_discontinuous_frame();
            break;
        default:
            OTA_LOG_I("ota breeze get a unknow event");
            break;
    }
}


