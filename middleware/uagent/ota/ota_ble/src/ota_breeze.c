#include <string.h>
#include "ota_breeze.h"
#include "ota_breeze_os.h"
#include "ota_breeze_plat.h"
#include "ota_breeze_transport.h"

static ota_breeze_rec_t *ota_breeze_queue     = NULL;
static unsigned char ota_breeze_queue_ptr_in  = 0;
static unsigned char ota_breeze_queue_ptr_out = 0;

static unsigned int ota_breeze_creat_receive_buf()
{
    if(ota_breeze_queue != NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    ota_breeze_queue = ota_breeze_malloc(OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    if(ota_breeze_queue == NULL) {
        return OTA_BREEZE_ERROR_NULL;
    }
    memset(ota_breeze_queue, 0x00, OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    return OTA_BREEZE_SUCCESS;
}

static void ota_breeze_destroy_receive_buf()
{
    if(ota_breeze_queue != NULL) {
        ota_breeze_free(ota_breeze_queue);
    }
    ota_breeze_queue = NULL;
}

static unsigned int ota_breeze_init_receive_buf()
{
    unsigned int error_code = 0;
    ota_breeze_destroy_receive_buf();
    error_code = ota_breeze_creat_receive_buf();
    if(error_code != OTA_BREEZE_SUCCESS) {
        return error_code;
    }
    ota_breeze_queue_ptr_in  = 0;
    ota_breeze_queue_ptr_out = 0;
    return OTA_BREEZE_SUCCESS;
}

static void ota_breeze_receive_data_product(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buffer, unsigned int length)
{
    if(ota_breeze_queue == NULL) {
        return;
    }
    if(length > OTA_BREEZE_REC_PER_FRAME_LEN) {
        return;
    }

    ota_breeze_queue[ota_breeze_queue_ptr_in].cmd        = ota_cmd;
    ota_breeze_queue[ota_breeze_queue_ptr_in].length     = length;
    ota_breeze_queue[ota_breeze_queue_ptr_in].num_frames = num_frame;
    if(buffer != NULL) {
        memcpy(ota_breeze_queue[ota_breeze_queue_ptr_in].rec_buf, buffer, length);
    }
    ota_breeze_queue_ptr_in++;
    ota_breeze_queue_ptr_in = ota_breeze_queue_ptr_in % OTA_BREEZE_RECEIVE_BUF_NUMB;
}

static unsigned int ota_breeze_receive_data_consume(ota_breeze_rec_t *out_queue)
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

static unsigned int ota_breeze_init()
{
    ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
    ota_breeze_set_task_active_ctrl(true);
    ota_breeze_set_task_active_flag(true);
    return OTA_BREEZE_SUCCESS;
}

static unsigned int ota_breeze_start()
{
    ota_breeze_rec_t tmp_queue;
    ota_breeze_state_t cur_breeze_status;
    ota_breeze_version_t *tmp_verion = NULL;
    unsigned int error_code          = 0;
    unsigned char send_err           = false;
    if(ota_breeze_init() != OTA_BREEZE_SUCCESS) {
        return OTA_BREEZE_ERROR_NULL;
    }
    while(1) {
        if(ota_breeze_get_task_active_ctrl_status() == false) {
            goto OTA_BREEZE_OVER;
        }
        if(ota_breeze_receive_data_consume(&tmp_queue) == OTA_BREEZE_SUCCESS) {
            if ((tmp_queue.cmd & OTA_BREEZE_CMD_TYPE_MASK) != OTA_BREEZE_CMD_TYPE_FW_UPGRADE) {
                OTA_BREEZE_LOG_E("cmd err");
            }
            else {
                cur_breeze_status = ota_breeze_get_status();
                switch(cur_breeze_status) {
                    case OTA_BREEZE_STATE_IDLE:
                        send_err = true;
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_VERSION_REQ) { /*cmd=0x20*/
                            tmp_verion = ota_breeze_get_version();
                            error_code = ota_breeze_send_fw_version_rsp(OTA_BREEZE_CMD_FW_VERSION_RSP, tmp_verion->fw_ver, tmp_verion->fw_ver_len);
                            if(error_code != OTA_BREEZE_SUCCESS) {
                                OTA_BREEZE_LOG_E("send ver failed");
                                goto OTA_BREEZE_OVER;
                            }
                            send_err = false;
                        }
                        else if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_UPGRADE_REQ) { /*cmd=0x22*/
                            error_code = ota_breeze_on_fw_upgrade_req(tmp_queue.rec_buf, tmp_queue.length);
                            if (error_code == OTA_BREEZE_SUCCESS) {
                                error_code = ota_breeze_send_fw_upgrade_rsp(true);
                                if (error_code == OTA_BREEZE_SUCCESS) {
                                    ota_breeze_set_status(OTA_BREEZE_STATE_RECEIVE);
                                    send_err = false;
                                }
                            }
                            else {
                                (void)ota_breeze_send_fw_upgrade_rsp(false);
                                goto OTA_BREEZE_OVER;
                            }
                        }
                        break;

                    case OTA_BREEZE_STATE_RECEIVE:
                        send_err = true;
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_DATA) { /*cmd=0x2F*/
                            error_code = ota_breeze_on_fw_data(tmp_queue.rec_buf, tmp_queue.length, tmp_queue.num_frames);
                            if(error_code == OTA_BREEZE_SUCCESS) {
                                if(ota_breeze_is_in_check_status() == OTA_BREEZE_SUCCESS) {
                                    ota_breeze_set_status(OTA_BREEZE_STATE_FW_CHECK);
                                }
                                error_code = ota_breeze_send_bytes_received();
                                if(error_code == OTA_BREEZE_SUCCESS) {
                                    send_err = false;
                                }
                            }
                            else {
                                OTA_BREEZE_LOG_E("rece data failed");
                            }
                        }
                        else if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_GET_INIT_FW_SIZE) { /*cmd=0x27*/
                            error_code = ota_breeze_send_bytes_received();
                            if(error_code == OTA_BREEZE_SUCCESS) {
                                if(ota_breeze_is_in_check_status() == OTA_BREEZE_SUCCESS) {
                                    ota_breeze_set_status(OTA_BREEZE_STATE_FW_CHECK);
                                }
                                send_err = false;
                            }
                        }
                        break;
                    case OTA_BREEZE_STATE_FW_CHECK:
                        send_err = true;
                        if (tmp_queue.cmd == OTA_BREEZE_CMD_FW_XFER_FINISH) { /*cmd=0x28*/
                            if(ota_breeze_set_boot() == 0) {
                                ota_breeze_msleep(2000);
                                ota_breeze_send_crc_result(true);
                                ota_breeze_set_status(OTA_BREEZE_STATE_RESET_PREPARE);
                                OTA_BREEZE_LOG_I("setboot over");
                                send_err = false;
                            }
                            else {
                                (void)ota_breeze_send_crc_result(false);
                                if (error_code == OTA_BREEZE_SUCCESS) {
                                    ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
                                }
                                OTA_BREEZE_LOG_E("setboot failed");
                            }
                        }
                        break;
                    case OTA_BREEZE_STATE_RESET_PREPARE:
                    case OTA_BREEZE_STATE_OFF:
                        send_err = true;
                        break;
                    default:
                        break;
                }
                if(send_err) {
                    ota_breeze_send_error();
                    OTA_BREEZE_LOG_E("send err report");
                    goto OTA_BREEZE_OVER;
                }
            }
        }
        ota_breeze_msleep(1);
    }
OTA_BREEZE_OVER:
    OTA_BREEZE_LOG_I("task over!");
    ota_breeze_destroy_receive_buf();
    ota_breeze_set_task_active_flag(false);
    ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
    return 0;
}

static unsigned int ota_breeze_start_task()
{
    int ret = 0;
    unsigned int error_code = 0;
    void *thread = NULL;
    ret = ota_breeze_thread_create(&thread, (void *)ota_breeze_start, (void *)NULL, NULL, 2048);
    if (ret != 0) {
        OTA_BREEZE_LOG_E("creat task failed");
        error_code = OTA_BREEZE_ERROR_BUSY;
    }
    return error_code;
}

void ota_breeze_get_data(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buffer, unsigned int length)
{
    volatile unsigned char tm_cnt = 0;
    if((ota_breeze_get_status() == OTA_BREEZE_STATE_IDLE) && (ota_cmd == OTA_BREEZE_CMD_FW_VERSION_REQ)) {
        if(ota_breeze_get_task_active_flag() == true) {
            ota_breeze_set_task_active_ctrl(false);
            while(ota_breeze_get_task_active_flag() == true) {
                ota_breeze_set_task_active_ctrl(false);
                ota_breeze_msleep(1);
                if(tm_cnt > 2) {
                    OTA_BREEZE_LOG_E("last task over failed\r\n");
                    return;
                }
                tm_cnt++;
            }
            ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
        }

        if(ota_breeze_init_receive_buf() != OTA_BREEZE_SUCCESS) {
            return;
        }

        ota_breeze_receive_data_product(ota_cmd, num_frame, buffer, length);
        if(ota_breeze_start_task() != OTA_BREEZE_SUCCESS) {
            OTA_BREEZE_LOG_E("ota task creat failed");
            return;
        }
    }
    else if((ota_breeze_get_task_active_flag() == true) && (ota_breeze_get_status() != OTA_BREEZE_STATE_OFF)) {
        ota_breeze_receive_data_product(ota_cmd, num_frame, buffer, length);
    }
    else {
        ota_breeze_set_task_active_ctrl(false);
        ota_breeze_set_status(OTA_BREEZE_STATE_IDLE);
        OTA_BREEZE_LOG_E("ota stus err");
    }

}

void ota_breeze_relate_event(unsigned char event_type, unsigned char sub_status)
{
    switch(event_type) {
        case ALI_OTA_ON_AUTH_EVT:
            ota_breeze_on_auth(sub_status);
            break;
        case ALI_OTA_ON_TX_DONE:
            ota_breeze_on_tx_done(sub_status);
            break;
        case ALI_OTA_ON_DISCONNECTED:
            ota_breeze_msleep(1000);
            ota_breeze_set_task_active_ctrl(false);
            ota_breeze_msleep(1000);
            ota_breeze_reset();
            break;
        case ALI_OTA_ON_DISCONTINUE_ERR:
            ota_breeze_msleep(1000);
            ota_breeze_set_task_active_ctrl(false);
            ota_breeze_msleep(1000);
            ota_breeze_on_discontinuous_frame();
            break;
        default:
            break;
    }
}

