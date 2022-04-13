#if defined OTA_CONFIG_MCU_YMODEM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ota_hal_os.h"
#include "ota_log.h"
#include "ota_mcu_upgrade.h"

#define SOH                          0x01
#define STX                          0x02
#define EOT                          0x04
#define ACK                          0x06
#define NAK                          0x15
#define SOH_DATA_LEN                 128
#define STX_DATA_LEN                 1024

#define YMODEM_BEGIN                 (0x00)
#define YMODEM_RECEIVE_HEAD_ACK      (0x01)
#define YMODEM_RECEIVE_DATA_REQ      (0x02)
#define YMODE_SEND_DATA              (0x03)
#define YMODE_SEND_OVER_FLAG1        (0x04)
#define YMODE_SEND_OVER_FLAG2        (0x05)
#define YMODE_RECEIVE_OVER_ACK       (0x06)
#define YMODE_SEND_OVER_FRAME        (0x07)
#define YMODE_SEND_FINISH            (0x08)

#define YMODE_SEND_BUF_SIZE          (1029)

#define YMODE_MALLOC_ERR             (-1)
#define YMODE_SEND_DATA_ERR          (-2)
#define YMODE_MAKE_HEAD_ERR          (-3)
#define YMODE_MAKE_CONTENT_ERR       (-4)
#define YMODE_GET_IMAGE_DATA_ERR     (-5)
#define YMODE_RECEIVE_ACK_ERR        (-6)
#define YMODE_GET_IMAGE_INFO_ERR     (-7)
#define YMODE_IMAGE_IS_UPGRADING     (-8)
#define YMODE_PARAM_ERR              (-9)

void ymode_int2str(unsigned int data, unsigned char *buf, unsigned int buf_len)
{
    int power = 0;
    int i = 0;
    unsigned int tmp_data = 0;
    if((data > 0) && (buf != NULL) && (buf_len > 0)) {
        tmp_data = data;
        for(power = 1; tmp_data > 10; tmp_data /= 10) {
            power *= 10;
        }
        tmp_data = data;
        for(i = 0; power > 0; power /= 10) {
            buf[i++] = '0' + tmp_data / power;
            tmp_data %= power;

        }
        OTA_LOG_D("data:%d, buf:%s\n", data, buf);
    }
}

int ymode_make_msg_head(unsigned int image_size, unsigned char *buf, unsigned int buf_size)
{
    int len = -1;
    unsigned short c_crc16 = 0;
    if(image_size > 0 && buf != NULL && buf_size >= 133 ) {
        
        if(image_size > 0) {
            memset(buf, 0x00, buf_size);
            buf[0] = SOH;
            buf[1] = 0x00;
            buf[2] = 0xFF;
            buf[3] = 0x00;
            ymode_int2str(image_size, &buf[4], 127);
            c_crc16 = ota_get_data_crc16(&buf[3], SOH_DATA_LEN);
            buf[131] = ((c_crc16 & 0xff00) >> 8);
            buf[132] = (c_crc16 & 0x00ff);
            len = 133;
        }
    }
    return len;
}

int ymode_make_msg_content(unsigned char msg_id, unsigned char *buf, unsigned int buf_size, unsigned int data_len)
{
    int len = -1;
    unsigned short c_crc16 = 0;
    if((buf != NULL) && (buf_size >= YMODE_SEND_BUF_SIZE) && (data_len > 0)) {
        if(data_len <= 128) {
            data_len = 128;
            buf[0] = SOH;
        }
        else {
            data_len  = 1024;
            buf[0] = STX;   
        }
        buf[1] = msg_id;
        buf[2] = ~msg_id;
        c_crc16 = ota_get_data_crc16(&buf[3], data_len);
        buf[data_len + 3] = ((c_crc16 & 0xff00) >> 8);
        buf[data_len + 4] = (c_crc16 & 0x00ff);
        len = data_len + 5;
    }
    return len;
}

int ymode_make_over_frame_data(unsigned char *buf, unsigned int buf_size)
{
    int len = -1;
    unsigned short c_crc16 = 0;
    if((buf != NULL) && (buf_size >= 133)) {
        memset(buf, 0x00, buf_size);
        buf[0] = SOH;
        buf[1] = 0x00;
        buf[2] = 0xFF;
        c_crc16 = ota_get_data_crc16(&buf[3], SOH_DATA_LEN);
        buf[131] = ((c_crc16 & 0xff00) >> 8);
        buf[132] = (c_crc16 & 0x00ff);
        len = 133;
    }
    return len;
}

int ota_mcu_ymodem_upgrade(unsigned int size, char *ver, char *md5)
{
    int err_numb = 0;
    int ret = 0;
    int timeout_cnt = 0;
    unsigned char recv_ack = 0;
    int have_data = 0;
    char ymodem_status = 0;
    unsigned char *ymodem_send_buf = NULL;
    unsigned int ymodem_send_len = 0;
    unsigned int get_data_len = 0;
    unsigned int image_read_offset = 0;
    unsigned int image_size = 0;
    char msg_id = 0;
    void *device = NULL;

    if(size == 0) {
        err_numb = YMODE_PARAM_ERR;
        goto YMODEM_OVER;
    }

    image_size = size;
    while (1) {
        timeout_cnt++;
        ota_msleep(10);
        if (timeout_cnt > 500) {
            break;
        }
        have_data = ota_mcu_hal_recv(device, &recv_ack, 1);
        if (have_data > 0) {
            timeout_cnt = 0;
        }

        OTA_LOG_D("ymodem data recv %d %c\n", have_data, recv_ack);
        switch(ymodem_status) {
            case YMODEM_BEGIN:
                if(have_data > 0) {
                    if(recv_ack == 'C') {
                        if(ymodem_send_buf != NULL) {
                            ota_free(ymodem_send_buf);
                            ymodem_send_buf = NULL;
                        }
                        if(ymodem_send_buf == NULL) {
                            ymodem_send_buf = ota_malloc(YMODE_SEND_BUF_SIZE);
                            if(ymodem_send_buf == NULL ) {
                                err_numb = YMODE_MALLOC_ERR;
                                goto YMODEM_OVER;
                            }
                        }
                        ymodem_send_len = ymode_make_msg_head(image_size, ymodem_send_buf, YMODE_SEND_BUF_SIZE);
                        if(ymodem_send_len > 0) {
                            ret = ota_mcu_hal_send(device, ymodem_send_buf, ymodem_send_len);
                            if(ret != 0) {
                                err_numb = YMODE_SEND_DATA_ERR;
                                goto YMODEM_OVER;
                            }
                            else {
                                ymodem_status = YMODEM_RECEIVE_HEAD_ACK;
                            }
                        }
                        else {
                            err_numb = YMODE_MAKE_HEAD_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODEM_RECEIVE_HEAD_ACK:
                if(have_data > 0) {
                    if(recv_ack == ACK) {
                        ymodem_status = YMODEM_RECEIVE_DATA_REQ;
                    }
                }
                break;
            case YMODEM_RECEIVE_DATA_REQ:
                if(have_data > 0) {
                    if(recv_ack == 'C') {
                        msg_id++;
                        memset(ymodem_send_buf, 0x1A, YMODE_SEND_BUF_SIZE);
                        if(image_size >= STX_DATA_LEN) {
                            get_data_len = STX_DATA_LEN;
                        }
                        else {
                            get_data_len = image_size;
                        }
                        ret = ota_mcu_hal_read(&image_read_offset, &ymodem_send_buf[3], get_data_len);
                        if(ret < 0) {
                            err_numb = YMODE_GET_IMAGE_DATA_ERR;
                            goto YMODEM_OVER;
                        }
                        ymodem_send_len = ymode_make_msg_content(msg_id, ymodem_send_buf, YMODE_SEND_BUF_SIZE, get_data_len);
                        if(ymodem_send_len > 0) {
                            ret = ota_mcu_hal_send(device, ymodem_send_buf, ymodem_send_len);
                            if(ret != 0) {
                                err_numb = YMODE_SEND_DATA_ERR;
                                goto YMODEM_OVER;
                            }
                            else {
                                if(image_read_offset >= image_size) {
                                    ymodem_status = YMODE_SEND_OVER_FLAG1;
                                }
                                else {
                                    ymodem_status = YMODE_SEND_DATA;
                                }
                            }
                        }
                        else {
                            err_numb = YMODE_MAKE_CONTENT_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_SEND_DATA:
                if(have_data > 0) {
                    if(recv_ack == ACK) {
                        msg_id++;
                        if(image_size - image_read_offset >= STX_DATA_LEN) {
                            get_data_len = STX_DATA_LEN;
                        }
                        else {
                            get_data_len = image_size - image_read_offset;
                        }
                        ret = ota_mcu_hal_read(&image_read_offset, &ymodem_send_buf[3], get_data_len);
                        if(ret < 0) {
                            err_numb = YMODE_GET_IMAGE_DATA_ERR;
                            goto YMODEM_OVER;
                        }
                        ymodem_send_len = ymode_make_msg_content(msg_id, 
                            ymodem_send_buf, YMODE_SEND_BUF_SIZE, get_data_len);
                        if(ymodem_send_len > 0) {
                            ret = ota_mcu_hal_send(device, ymodem_send_buf, ymodem_send_len);
                            if(ret != 0) {
                                err_numb = YMODE_SEND_DATA_ERR;
                                goto YMODEM_OVER;
                            }
                            else {
                                if(image_read_offset >= image_size) {
                                    ymodem_status = YMODE_SEND_OVER_FLAG1;
                                }
                            }
                        }
                        else {
                            err_numb = YMODE_MAKE_CONTENT_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_SEND_OVER_FLAG1:
                if(have_data > 0) {
                    if(recv_ack == ACK) {
                        ymodem_send_buf[0] = EOT;
                        ret = ota_mcu_hal_send(device, ymodem_send_buf, 1);
                        if(ret == 0) {
                            ymodem_status = YMODE_SEND_OVER_FLAG2;
                        }
                        else {
                            err_numb = YMODE_SEND_DATA_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_SEND_OVER_FLAG2:
                if(have_data > 0) {
                    if(recv_ack == NAK) {
                        ymodem_send_buf[0] = EOT;
                        ret = ota_mcu_hal_send(device, ymodem_send_buf, 1);
                        if(ret == 0) {
                            ymodem_status = YMODE_RECEIVE_OVER_ACK;
                        }
                        else {
                            err_numb = YMODE_SEND_DATA_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_RECEIVE_OVER_ACK:
                if(have_data > 0) {
                    if(recv_ack == ACK) {
                        ymodem_status = YMODE_SEND_OVER_FRAME;
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_SEND_OVER_FRAME:
                if(have_data > 0) {
                    if(recv_ack == 'C') {
                        ymodem_send_len = ymode_make_over_frame_data(ymodem_send_buf, YMODE_SEND_BUF_SIZE);
                        ret = ota_mcu_hal_send(device, ymodem_send_buf, ymodem_send_len);
                        if(ret == 0) {
                            ymodem_status = YMODE_SEND_FINISH;
                        }
                        else {
                            err_numb = YMODE_SEND_DATA_ERR;
                            goto YMODEM_OVER;
                        }
                    }
                    else {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                        goto YMODEM_OVER;
                    }
                }
                break;
            case YMODE_SEND_FINISH:
                if(have_data > 0) {
                    if(recv_ack != ACK) {
                        err_numb = YMODE_RECEIVE_ACK_ERR;
                    }
                    goto YMODEM_OVER;
                }
                break;
            default:
                break;
        }
    }
YMODEM_OVER:
    if(ymodem_send_buf != NULL) {
        ota_free(ymodem_send_buf);
        ymodem_send_buf = NULL;
    }
    if(err_numb != 0) {
        OTA_LOG_E("ymodem send err:%d", err_numb);
    }
    else {
        OTA_LOG_I("ymodem send ok!");
    }
    return ret;
}
#endif
