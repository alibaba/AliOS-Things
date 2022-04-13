#if defined OTA_CONFIG_LOCAL_UART_DOWNLOAD
#include <stdio.h>
#include "ota_log.h"
#include "aos/hal/flash.h"
#include "ubus/in_comm_task.h"
#include "ubus/ubus_profile.h"
#include "ubus/ubus.h"
#include "aos/kernel.h"
#include "ota_import.h"
#include "aos/hal/uart.h"
#include "board.h"
#include "ota_hal_os.h"

#define UART_CMD_TIME_SYNC           (0xF1)
#define UART_CMD_IMAGE_UPDATE        (0xF2)
#define UART_DOWNLOAD_BIGIN          (0x00)
#define UART_DOWNLOAD_DOING          (0x01)
#define UART_DOWNLOAD_DONE           (0x02)
#define EXPECT_RECVD_IMAGE_INFOR     (47)
#define EXPECT_RECVD_SIZE            (1024)
#define EXPECT_RECVD_SEND_OVER       (13)
#define OTA_UART_UBUS_CMD_SIZE       (64)
#define OTA_UART_UBUS_RECVD_CMD_NUM  (4)
#define OTA_UART_UBUS_QUEUE_TMOUT    (1000)

#define MONITO_LEN     (11)
static unsigned int monit_cmd_index = 0;
static unsigned char monit_cmd_buf[20];

static unsigned char ota_uart_ubus_cmd_buf[OTA_UART_UBUS_CMD_SIZE * OTA_UART_UBUS_RECVD_CMD_NUM];
static aos_queue_t ota_uart_ubus_queue;

extern uart_dev_t uart_0;

unsigned char send_buf[100];
unsigned char recvd_buf[1100];

static unsigned int image_has_rcvd_len = 0;
static unsigned int image_total_erase_page_numbs = 0;
static unsigned int image_has_erase_page_numbs = 0;
static int image_ota_part = HAL_PARTITION_OTA_TEMP;
static unsigned int image_erase_sector_size = 0;
static void *ota_uart_thread = NULL;

static char monitor_flag = 0;
static unsigned int utc_time = 0;
static ota_boot_param_t image_boot_param;

int ota_uart_translate_ubus_init(void);
static void ota_uart_image_download(void);
uint32_t hal_flash_erase_sector_size(void);
static void ota_uart_data_product(const dbus_data_t data_type, const uint16_t len, const void *data, const void *user_data);
int ota_uart_ubus_send(unsigned short data_type, unsigned char *buf, unsigned short send_len);
static int ota_uart_ubus_recvd(unsigned char *buf, unsigned int *recvd_len, unsigned int timeout);

static bool std_uart_inited = false;

static uint8_t USER_HINTS[128] = "\r\n***SUPPORT Local Upgrade***\r\n";

static rx_handler_node_t ota_uart_ubus_rx_handler_node[] = {
    {ubus_data_slave_bin_resp, ota_uart_data_product, NULL},
};

int uart_send(const void *data, uint32_t size, uint32_t timeout)
{
    return hal_uart_send(&uart_0, data, size, timeout);
}

int uart_recvd(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    return hal_uart_recv_II(&uart_0, data, expect_size, recv_size, timeout);
}

unsigned int image_flash_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

static void stduart_init(void)
{
    if(!std_uart_inited) {
        uart_0.port = 0;
        uart_0.config.baud_rate = 460800;
        uart_0.config.data_width = DATA_WIDTH_8BIT;
        uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
        uart_0.config.mode = MODE_TX_RX;
        uart_0.config.parity = NO_PARITY;
        uart_0.config.stop_bits = STOP_BITS_1;

        hal_uart_init(&uart_0);
        std_uart_inited = true;
    }

}

void uart_pc_request_monitor(void)
{
    int ret = 0;
    if(!std_uart_inited) {
        if(!if_std_uart_inited()) {
            stduart_init();
            if(std_uart_inited) {
                char buffer[64];
                memset(buffer, 0, sizeof(buffer));
                snprintf(buffer, sizeof(buffer), "***Build: %s***\r\n", __TIME__);
                strcat(USER_HINTS, buffer);
                uart_send(USER_HINTS, strlen(USER_HINTS), 1000);
            }
        }
    }

    if(0 != std_uart_inited) {
        unsigned char retry_tm = 0;
        unsigned short src_crc16 = 0;
        unsigned short frame_crc16 = 0;
        unsigned char want_size = MONITO_LEN;
        unsigned int tmp_rcvd_size = 0;
        unsigned int cmd_output_ptr = 0;
        if(monitor_flag == 0) {
            ret = uart_recvd((void*)recvd_buf, want_size, &tmp_rcvd_size, 300);
            if(tmp_rcvd_size > 0) {
                while(cmd_output_ptr < tmp_rcvd_size) {
                    monit_cmd_buf[monit_cmd_index++] = recvd_buf[cmd_output_ptr++];
                    if(monit_cmd_buf[0] != 0xAB) {
                        monit_cmd_index = 0;
                    }
                    else if(monit_cmd_index == MONITO_LEN) {
                        src_crc16 = L_EXTRACT_U16(&monit_cmd_buf[monit_cmd_index - 2]);
                        frame_crc16 = ota_get_data_crc16(monit_cmd_buf, monit_cmd_index - 2);
                        if(src_crc16 == frame_crc16) {
                            switch(recvd_buf[3]) {
                                case UART_CMD_TIME_SYNC:
                                    utc_time = L_EXTRACT_U16(&monit_cmd_buf[4]);
                                    send_buf[0] = 0xBA;
                                    send_buf[1] = 0x00;
                                    send_buf[2] = 0x09;
                                    send_buf[3] = UART_CMD_TIME_SYNC;
                                    send_buf[4] = 0x00;
                                    send_buf[5] = 0x00;
                                    send_buf[6] = 0x00;
                                    src_crc16 = ota_get_data_crc16(send_buf, 5);
                                    send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
                                    send_buf[8] = (char)(src_crc16 & 0x00ff);
                                    for(retry_tm = 0; retry_tm < 3; retry_tm++) {
                                        ret = uart_send((void*)send_buf, 9, 500);
                                        if(ret >= 0) {
                                            break;
                                        }
                                    }
                                    break;
                                case UART_CMD_IMAGE_UPDATE:
                                    monitor_flag = 1;
                                    ret = ota_uart_translate_ubus_init();
                                    if(ret >= 0) {
                                        ret = ota_thread_create(&ota_uart_thread, (void *)ota_uart_image_download, (void *)NULL, NULL, 1024 * 4);
                                    }
                                    if(ret < 0) {
                                        monitor_flag = 0;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        monit_cmd_index = 0;
                    }
                }
            }
        }
    }
}

int ota_uart_init(unsigned int image_size)
{
    int ret = -1;
    unsigned int num_pages = 0;
    unsigned int offset = 0;
    image_has_erase_page_numbs = 0;
    image_erase_sector_size = 4096;
    memset(&image_boot_param, 0x00, sizeof(ota_boot_param_t));
    if(image_size > 0) {
        image_total_erase_page_numbs = image_flash_align_to_page(image_size, image_erase_sector_size) / image_erase_sector_size;
        if (image_total_erase_page_numbs > 0) {
            ret = hal_flash_erase(image_ota_part, offset, image_erase_sector_size);
            if (ret == 0) {
                image_has_erase_page_numbs++;
            }
        }
    }
    return ret;
}

int ota_uart_write(unsigned int *off, char *in_buf ,int in_buf_len)
{
    int ret = 0;
    if((image_has_erase_page_numbs <= image_total_erase_page_numbs) &&
       (*off % image_erase_sector_size + in_buf_len >= image_erase_sector_size)) {
        if(image_has_erase_page_numbs < image_total_erase_page_numbs) {
            ret = hal_flash_erase(image_ota_part, *off + in_buf_len, image_erase_sector_size);
        }
        if(ret == 0) {
            image_has_erase_page_numbs++;
        }
    }
    else if(image_has_erase_page_numbs > image_total_erase_page_numbs) {
        ret = -1;
    }
    if(ret == 0) {
        ret = hal_flash_write(image_ota_part, off, in_buf, in_buf_len);
    }
    return ret;
}

static void ota_uart_image_download()
{
    int ret = 0;
    int retry_tm = 0;
    int status = 0;
    int recvd_tm = 0;

    unsigned int image_size = 0;
    unsigned short image_crc16 = 0;
    unsigned short cal_image_crc16 = 0;
    unsigned int has_recvd_len = 0;
    unsigned char frame_header = 0;
    unsigned short frame_len = 0;
    unsigned char cmd[2] = {0, 0};
    unsigned char image_type = 0x00;
    unsigned short src_crc16 = 0x00;
    unsigned short frame_crc16 = 0x00;
    unsigned char *tmp_data = NULL;
    unsigned char fail_cnt = 0;
    unsigned short recvd_index = 0;
    unsigned short last_index = 0;

    unsigned char loop_lock = 1;
    unsigned int expect_size = 0;
    unsigned int recvd_size = 0;
    unsigned int timeout = 300;

    unsigned int send_frame_len = 0;
    unsigned int offset = 0;

    ota_crc16_ctx  ota_uart_ctx    = {0};

    send_buf[0] = 0xBA;
    send_buf[1] = 0x00;
    send_buf[2] = 0x09;
    send_buf[3] = UART_CMD_IMAGE_UPDATE;
    send_buf[4] = 0x00;
    send_buf[5] = 0x00;
    send_buf[6] = 0x00;
    src_crc16 = ota_get_data_crc16(send_buf, 7);
    send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
    send_buf[8] = (char)(src_crc16 & 0x00ff);
    for(retry_tm = 0; retry_tm < 3; retry_tm++) {
        ret = uart_send((void*)send_buf, 9, 500);
        if(ret >= 0) {
            break;
        }
    }
    if(ret < 0) {
        monitor_flag = 0;
        return;
    }
    status = UART_DOWNLOAD_BIGIN;
    expect_size = EXPECT_RECVD_IMAGE_INFOR;
    while(loop_lock) {
        for(retry_tm = 0; retry_tm < 2; retry_tm++) {
            ret = uart_recvd((void*)recvd_buf, expect_size, &recvd_size, timeout);
            if(ret >= 0) {
                break;
            }
        }
        if(ret < 0) {
            loop_lock = 0;
            break;
        }
        recvd_tm++;
        if(recvd_tm > 1) {
            loop_lock = 0;
            break;
        }
        if(expect_size == recvd_size) {
            recvd_tm = 0;
            frame_header = recvd_buf[0];
            src_crc16 = L_EXTRACT_U16(&recvd_buf[recvd_size - 2]);
            frame_crc16 = ota_get_data_crc16(recvd_buf, recvd_size - 2);
            if(status == UART_DOWNLOAD_DOING) {
                send_buf[0] = 0xBA;
            }
            if((frame_header != 0xAB) || (src_crc16 != frame_crc16)) {
                fail_cnt++;
                if(fail_cnt > 3) {
                    loop_lock = 0;
                    break;
                }
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0x01;
                send_buf[5] = 0x00;
                send_buf[6] = 0x01;
                src_crc16 = ota_get_data_crc16(send_buf, 7);
                send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[8] = (char)(src_crc16 & 0x00ff);
                for(retry_tm = 0; retry_tm < 3; retry_tm++) {
                    ret = uart_send((void*)send_buf, 9, 500);
                    if(ret >= 0) {
                        break;
                    }
                }
                if(ret < 0) {
                    loop_lock = 0;
                    break;
                }
                continue;
            }
            fail_cnt = 0;
        }
        else {
            continue;
        }
        switch(status) {
            case UART_DOWNLOAD_BIGIN:
                recvd_index = 0;
                last_index = 0;
                has_recvd_len = 0;
                offset = 0;
                send_frame_len = 0;
                image_type = recvd_buf[3];
                timeout = 2000;
                frame_len = L_EXTRACT_U16(&recvd_buf[1]);
                recvd_index = L_EXTRACT_U16(&recvd_buf[5]);
                image_size = L_EXTRACT_U32(&recvd_buf[7]);
                image_crc16 = L_EXTRACT_U16(&recvd_buf[recvd_size - 4]);
                image_size > EXPECT_RECVD_SIZE ? (expect_size = EXPECT_RECVD_SIZE + 9) : (expect_size = image_size);
                status = UART_DOWNLOAD_DOING;

                send_frame_len = 9;
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0x01;
                send_buf[5] = 0x00;
                send_buf[6] = 0x01;
                if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                    (image_type == OTA_RECVD_SOURCE_FILE) ||
                    (image_type == OTA_RECVD_SOURCE_PIC_FILE)) {
                    ret = ota_uart_ubus_send(0, recvd_buf, frame_len);
                    if(ret == 0) {
                        aos_msleep(1);
                        ret = ota_uart_ubus_recvd(send_buf, &send_frame_len, OTA_UART_UBUS_QUEUE_TMOUT);
                        if(send_frame_len == 0) {
                            ret = -1;
                            send_buf[6] = 0x01;
                            OTA_LOG_E("ota uart ubus recvd err");
                        }
                    }
                }
                else if(image_type == OTA_RECVD_LOCAL_MASTER_IMAGE) {
                    send_buf[6] = 0x00;
                    ret = ota_uart_init(image_size);
                    if(ret < 0) {
                        loop_lock = 0;
                        send_buf[6] = 0x01;
                    }
                    else {
                        ota_crc16_init(&ota_uart_ctx);
                    }
                }
                else{
                    status = UART_DOWNLOAD_BIGIN;
                    expect_size = EXPECT_RECVD_IMAGE_INFOR;
                    timeout = 300;
                }
                src_crc16 = ota_get_data_crc16(send_buf, send_frame_len - 2);
                send_buf[send_frame_len - 2] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[send_frame_len - 1] = (char)(src_crc16 & 0x00ff);
                for(retry_tm = 0; retry_tm < 3; retry_tm++) {
                    ret = uart_send((void*)send_buf, send_frame_len, 500);
                    if(ret >= 0) {
                        break;
                    }
                }
                if(ret < 0) {
                    loop_lock = 0;
                }
                break;
            case UART_DOWNLOAD_DOING:
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0x01;
                send_buf[5] = 0x00;
                send_buf[6] = 0x01;
                send_frame_len = 9;
                frame_len = L_EXTRACT_U16(&recvd_buf[1]);
                recvd_index = L_EXTRACT_U16(&recvd_buf[5]);
                image_type = recvd_buf[3];
                if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                    (image_type == OTA_RECVD_SOURCE_FILE) ||
                    (image_type == OTA_RECVD_SOURCE_PIC_FILE)) {
                    ret = ota_uart_ubus_send(1, recvd_buf, frame_len);
                    if(ret == 0) {
                        aos_msleep(1);
                        ret = ota_uart_ubus_recvd(send_buf, &send_frame_len, OTA_UART_UBUS_QUEUE_TMOUT);
                        if(send_frame_len == 0) {
                            ret = -1;
                            send_buf[6] = 0x01;
                        }
                    }
                    else {
                        ret = -1;
                    }
                }
                else if(image_type == OTA_RECVD_LOCAL_MASTER_IMAGE){
                    if(recvd_index == last_index + 1) {
                        last_index = recvd_index;
                        send_buf[6] = 0x00;
                        if(offset == 0) {
                            image_boot_param.upg_flag = ota_parse_ota_type(&recvd_buf[7], frame_len - 9);
                        }
                        ret = ota_uart_write(&offset, &recvd_buf[7], frame_len - 9);
                        if(ret < 0) {
                            loop_lock = 0;
                            send_buf[6] = 0x01;
                        }
                    }
                    else {
                        send_buf[6] = 0x03;
                    }
                }
                else {
                    send_buf[6] = 0x02;
                }
                src_crc16 = ota_get_data_crc16(send_buf, send_frame_len - 2);
                send_buf[send_frame_len - 2] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[send_frame_len - 1] = (char)(src_crc16 & 0x00ff);
                for(retry_tm = 0; retry_tm < 3; retry_tm++) {
                    ret = uart_send((void*)send_buf, send_frame_len, 500);
                    if(ret >= 0) {
                        break;
                    }
                }
                if(ret < 0) {
                    loop_lock = 0;
                }
                else if(send_buf[6] == 0){
                    has_recvd_len += frame_len - 9;
                    offset = has_recvd_len;
                    if(image_type == OTA_RECVD_LOCAL_MASTER_IMAGE) {
                        ota_crc16_update(&ota_uart_ctx, &recvd_buf[7], frame_len - 9);
                    }
                    if(has_recvd_len == image_size) {
                        if(image_type == OTA_RECVD_LOCAL_MASTER_IMAGE) {
                            ota_crc16_final(&ota_uart_ctx, &cal_image_crc16);
                            if(cal_image_crc16 == image_crc16) {
                                image_boot_param.len = image_size;
                                image_boot_param.crc = cal_image_crc16;
                                ret = ota_hal_boot(&image_boot_param);
                            }
                            loop_lock = 0;
                            aos_reboot();
                        }
                        else if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                                (image_type == OTA_RECVD_SOURCE_FILE) ||
                                (image_type == OTA_RECVD_SOURCE_PIC_FILE)) {
                            status = UART_DOWNLOAD_DONE;
                            expect_size = EXPECT_RECVD_SEND_OVER;
                            timeout = 300;
                        }
                    }
                    else {
                        (image_size - has_recvd_len > EXPECT_RECVD_SIZE) ? (expect_size = EXPECT_RECVD_SIZE + 9) : (expect_size = image_size - has_recvd_len + 9);
                    }
                }
                break;
            case UART_DOWNLOAD_DONE:
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0x01;
                send_buf[5] = 0x00;
                send_buf[6] = 0x01;
                send_frame_len = 9;
                frame_len = L_EXTRACT_U16(&recvd_buf[1]);
                recvd_index = L_EXTRACT_U16(&recvd_buf[5]);
                image_type = recvd_buf[3];
                if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                    (image_type == OTA_RECVD_SOURCE_FILE) ||
                    (image_type == OTA_RECVD_SOURCE_PIC_FILE)) {
                    ret = ota_uart_ubus_send(1, recvd_buf, frame_len);
                    if(ret == 0) {
                        aos_msleep(1);
                        ret = ota_uart_ubus_recvd(send_buf, &send_frame_len, OTA_UART_UBUS_QUEUE_TMOUT);
                        if(send_frame_len == 0) {
                            send_buf[6] = 0x01;
                        }
                    }
                    else {
                        ret = -1;
                    }
                }
                src_crc16 = ota_get_data_crc16(send_buf, send_frame_len - 2);
                send_buf[send_frame_len - 2] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[send_frame_len - 1] = (char)(src_crc16 & 0x00ff);
                for(retry_tm = 0; retry_tm < 3; retry_tm++) {
                    ret = uart_send((void*)send_buf, send_frame_len, 500);
                    if(ret >= 0) {
                        break;
                    }
                }
                if(ret < 0) {
                    loop_lock = 0;
                }
                status = UART_DOWNLOAD_BIGIN;
                expect_size = EXPECT_RECVD_IMAGE_INFOR;
                timeout = 300;
                break;
            default:
                break;
        }
    }
    monitor_flag = 0;
}

static unsigned char ota_uart_ubus_send_buf[1100];
int ota_uart_ubus_send(unsigned short data_type, unsigned char *buf, unsigned short send_len)
{
     int ret = -1;
     ota_uart_ubus_send_buf[0] = 1;
     if((buf != NULL) && (send_len > 0) && (send_len <= 1050)) {
          if(data_type == 0) {
               ota_uart_ubus_send_buf[1] = ubus_data_upgrade_header;
          }
          else {
               ota_uart_ubus_send_buf[1] = ubus_data_upgrade_text;
          }
          SET_CUBE_LEN(&ota_uart_ubus_send_buf[1], send_len);
          memcpy(&ota_uart_ubus_send_buf[sizeof(ubus_data_cube_t)+1], buf, send_len);
          LOGI("Pre", "len %d 0x%x 0x%x 0x%x 0x%x 0x%x", send_len + 4, *((uint32_t *)(&ota_uart_ubus_send_buf[0])),
          *((uint32_t *)(&ota_uart_ubus_send_buf[4])), *((uint32_t *)(&ota_uart_ubus_send_buf[8])),
          *((uint32_t *)(&ota_uart_ubus_send_buf[12])), *((uint32_t *)(&ota_uart_ubus_send_buf[16])));
          ret = ubus_send(ubus_comm_service, send_len + 4, ota_uart_ubus_send_buf);
    }
    if(ret != 0) {
          OTA_LOG_E("Send uBUS Failed %d %d 0x%x", send_len, data_type, buf);
    }
    return ret;
}

static int ota_uart_ubus_recvd(unsigned char *buf, unsigned int *recvd_len, unsigned int timeout)
{
    int ret = -1;
    unsigned int tmp_len = 0;
    if((buf != NULL) && (recvd_len != NULL)) {
        ret = aos_queue_recv(&ota_uart_ubus_queue, timeout, buf, &tmp_len);
    }
    *recvd_len = tmp_len;
    return ret;
}

static void ota_uart_data_product(const dbus_data_t data_type, const uint16_t len, const void *data, const void *user_data)
{
     int ret = 0;
     unsigned char cmd_recvd_buf[100];
     if((data != NULL) && (len <= sizeof(cmd_recvd_buf))) {
          memcpy(cmd_recvd_buf, data, len);
          ret = aos_queue_send(&ota_uart_ubus_queue, cmd_recvd_buf, len);
          if(ret != 0) {
               OTA_LOG_E("ota msg queue send err");
          }
    }
}

static unsigned char ota_uart_ubus_init_flag = 0;
int ota_uart_translate_ubus_init()
{
     int ret = 0;
     uint8_t rx_list_size = sizeof(ota_uart_ubus_rx_handler_node) / sizeof(ota_uart_ubus_rx_handler_node[0]);
     if(ota_uart_ubus_init_flag == 0) {
        ota_uart_ubus_init_flag = 1;
     }
     else {
        return ret;
     }
     if (rx_list_size != ubus_register_rx_handler_list(rx_list_size, ota_uart_ubus_rx_handler_node)) {
          OTA_LOG_E("Subscribe data Failed");
          ret = -1;
     }
     else {
          ret = aos_queue_new(&ota_uart_ubus_queue, ota_uart_ubus_cmd_buf, OTA_UART_UBUS_CMD_SIZE * OTA_UART_UBUS_RECVD_CMD_NUM, OTA_UART_UBUS_CMD_SIZE);
          if(ret != 0) {
               OTA_LOG_E("ota creat ubus queue fail");
          }
     }
     return ret;
}
#endif
