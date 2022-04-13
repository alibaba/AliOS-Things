#if defined OTA_CONFIG_MCU_CAN_SLAVE
#include "string.h"
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_mcu_upgrade.h"
#include "ota_hal_param.h"
#include "canopen/canopen.h"
#include "canopen/canopen_od.h"
#include "board.h"

#define FRAME_PAYLOAD_START_INDEX   (9)
#define FRAME_FILE_SIZE_START_INDEX (9)

#define SYS_STATUS_WAITING_UPGRADE  (0x00)
#define SYS_STATUS_UPGRADE_READY    (0x01)
#define SYS_STATUS_UPGRADE_DOING    (0x02)

#define OTA_CAN_CMD_UPGRADE         (0x20)
#define OTA_CAN_CMD_REBOOT          (0x30)
#define OTA_CAN_CMD_DATA            (0x40)


#define OTA_CAN_RECV_DATA_BUF_SIZE  (2048)
#define OTA_CAN_RECV_QUEUE_NUM      (2)

#define OTA_CAN_CMD_UPGRADE         (0x20)
#define OTA_CAN_CMD_REBOOT          (0x30)
#define OTA_CAN_CMD_ROLLBACK        (0x60)

#define OTA_CAN_UPGRADE_READY       (0xFDFD)
#define OTA_CAN_UPGRADE_ROLLBACK    (0xFBFB)

#define OTA_CAN_UPGRADE_TIMEOUT     (10000)//(0xFFFFFF00)

typedef struct _ota_can_slave_receive_data_{
    unsigned short len;
    unsigned char rec_buf[OTA_CAN_RECV_DATA_BUF_SIZE];
} ota_can_slave_receive_t;

ota_can_slave_receive_t ota_can_slave_recv_queue[OTA_CAN_RECV_QUEUE_NUM];
static unsigned char queue_in_ptr = 0;
static unsigned char queue_out_ptr = 0;
static unsigned char is_thread_creat = 0;
static void *thread = NULL;
static ota_can_slave_receive_t recv_data;

static int ota_can_slave_data_process(void);
void ota_can_slave_data_product(const uint16_t len, const void *data, const bool recorded, void *user_data);

int ota_can_salve_upgrade_ready(void)
{
    int ret = 0;
    ret = ota_update_upg_flag(OTA_CAN_UPGRADE_READY);
    return ret;
}

int ota_can_salve_set_rollback(void)
{
    int ret = 0;
    ret = ota_update_upg_flag(OTA_CAN_UPGRADE_ROLLBACK);
    return ret;
}

int ota_can_slave_init()
{
    int ret = -1;
    int ver_len = 0;
    uint8_t index = 0;
    const char *ver = NULL;
    can_node_id_t *can_io_node = NULL;
    if(ota_hal_rollback()< 0) {
        OTA_LOG_E("can slave upgrade rollback failed");
    }
    can_io_node = foreach_can_node(PORT_CAN_IO, &index);
    if(NULL != can_io_node) {
        ret = 0;
        ver = ota_hal_version(0, NULL);
        if(ver == NULL) {
            OTA_LOG_E("can slave ver get failed");
            ver = "test_1.0.0";
        }
        ver_len = strlen((const char*)ver) + 1;
        co_sdo_set_od(can_io_node, CO_SW_VERSION_INDEX, CO_SW_VERSION_SUB_INDEX, ver, ver_len);
        co_sdo_register_modify_event(can_io_node, CO_UPGRADE_INDEX, CO_UPGRADE_SUB_INDEX, ota_can_slave_data_product, NULL);
    }
    else {
        OTA_LOG_E("Not found any CAN ID");
    }
    OTA_LOG_I("ver = %s", ver);
    return ret;
}

void ota_can_slave_data_product(const uint16_t len, const void *data, const bool recorded, void *user_data)
{
    if((NULL != data) && (len > 0) && (len < OTA_CAN_RECV_DATA_BUF_SIZE)) {
        ota_can_slave_recv_queue[queue_in_ptr].len = len;
        memcpy(ota_can_slave_recv_queue[queue_in_ptr].rec_buf, (unsigned char*)data, len);
        queue_in_ptr++;
        queue_in_ptr = queue_in_ptr % OTA_CAN_RECV_QUEUE_NUM;
        if(is_thread_creat == 0) {
            int ret = 0;
            is_thread_creat = 1;
            ret = ota_thread_create(&thread, (void *)ota_can_slave_data_process, (void *)NULL, NULL, 1024 * 4);
            if(ret < 0) {
                OTA_LOG_E("ota can slave task creat failed");
            }
            else {
                OTA_LOG_I("ota can slave task creat success!");
            }
        }
    }
}

static int ota_can_slave_data_consume(ota_can_slave_receive_t *out_queue)
{
    int ret = -1;
    if(out_queue == NULL) {
        return ret;
    }
    if(queue_out_ptr != queue_in_ptr) {
        ret = 0;
        out_queue->len = ota_can_slave_recv_queue[queue_out_ptr].len;
        memcpy(out_queue->rec_buf, ota_can_slave_recv_queue[queue_out_ptr].rec_buf, out_queue->len);
        queue_out_ptr++;
        queue_out_ptr = queue_out_ptr % OTA_CAN_RECV_QUEUE_NUM;
    }
    return ret;
}

static int ota_can_slave_data_process(void)
{
#ifdef OTA_CAN_SLAVE_DW_DATA
    unsigned int recvd_image_len = 0;
    unsigned int cur_frame_index = 0;
    unsigned int last_frame_index = 0;
    unsigned short payload_len = 0;
    ota_boot_param_t param;
#endif
    uint8_t index = 0;
    int ret = -1;
    unsigned char reboot_flag = 0;
    unsigned char can_response = OTA_MCU_RES_FAIL;
    unsigned char upgrade_status = 0;
    unsigned int timeout_cnt = 0;
    unsigned char loop_lock = 1;
    unsigned char frame_header = 0;
    unsigned short frame_len = 0;
    unsigned char cmd[2] = {0x00, 0x00};
    unsigned short src_crc16 = 0x00;
    unsigned short frame_crc16 = 0x00;
    can_node_id_t *can_io_node = NULL;
    can_io_node = foreach_can_node(PORT_CAN_IO, &index);
    if(NULL == can_io_node) {
        OTA_LOG_E("can io node is null!");
        return ret;
    }
    while(loop_lock) {
        timeout_cnt++;
        if(timeout_cnt >= OTA_CAN_UPGRADE_TIMEOUT) {
            OTA_LOG_E("can slave time out");
            loop_lock = 0;
        }
        if(ota_can_slave_data_consume(&recv_data) == 0) {
            timeout_cnt = 0;
            if(recv_data.len >= 7) {
                frame_header = recv_data.rec_buf[0];
                frame_len = recv_data.rec_buf[1];
                frame_len = ((frame_len << 8) & 0xff00) + recv_data.rec_buf[2];
                cmd[0] = recv_data.rec_buf[3];
                cmd[1] = recv_data.rec_buf[4];
                src_crc16 = recv_data.rec_buf[recv_data.len - 2];
                src_crc16 = ((src_crc16 << 8) & 0xff00) + recv_data.rec_buf[recv_data.len - 1];
                frame_crc16 = ota_get_data_crc16((const unsigned char*)recv_data.rec_buf, recv_data.len - 2);
                printf("src_crc = 0x%02x, frame_crc = 0x%02x\r\n", src_crc16, frame_crc16);
                printf("recvd data = \r\n");
                for(int i = 0; i < 7; i++) {
                    printf("0x%02x, ", recv_data.rec_buf[i]);
                }
                printf("\r\n");
                if((frame_header == 0xAB) && (frame_len == recv_data.len) && (src_crc16 == frame_crc16)) {
                    switch(upgrade_status) {
                        case SYS_STATUS_WAITING_UPGRADE:
                            if(cmd[1] == OTA_CAN_CMD_UPGRADE) {
                                OTA_LOG_I("cmd upgrade");
                                ret = ota_can_salve_upgrade_ready();
                                if(ret >= 0) {
                                    can_response = OTA_MCU_RES_SUCCESS;
                                }
                                else {
                                    can_response = OTA_MCU_RES_FAIL;
                                }
#ifdef OTA_CAN_SLAVE_DW_DATA                          
                                cur_frame_index = 0;
                                upgrade_status = SYS_STATUS_UPGRADE_READY;
#endif
                            }
                            else if(cmd[1] == OTA_CAN_CMD_REBOOT) {
                                OTA_LOG_I("cmd reboot");
                                reboot_flag = 1;
                                loop_lock = 0;
                                can_response = OTA_MCU_RES_SUCCESS;
                            }
                            else if(cmd[1] == OTA_CAN_CMD_ROLLBACK) {
                                OTA_LOG_I("cmd rollback");
                                ret = ota_can_salve_set_rollback();
                                if(ret >= 0) {
                                    can_response = OTA_MCU_RES_SUCCESS;
                                    reboot_flag = 1;
                                    loop_lock = 0;
                                }
                                else {
                                    can_response = OTA_MCU_RES_FAIL;
                                }
                            }
                            else {
                                OTA_LOG_I("cmd don't support");
                                can_response = OTA_MCU_RES_FRAME_ERR;
                            }
                            break;
#ifdef OTA_CAN_SLAVE_DW_DATA
                        case SYS_STATUS_UPGRADE_READY:
                            if((cmd[1] == OTA_CAN_CMD_DATA) && (cur_frame_index == 0)) {
                                OTA_LOG_I("cmd data head");
                                if(frame_len < 15) {
                                    loop_lock = 0;
                                    can_response = OTA_MCU_RES_HEAD_ERR;
                                    OTA_LOG_E("ota image head length  = %d error", frame_len);
                                    break;
                                }
                                cur_frame_index = EXTRACT_U32(&recv_data.rec_buf[5]);
                                if(cur_frame_index != 0) {
                                    loop_lock = 0;
                                    can_response = OTA_MCU_RES_HEAD_ERR;
                                    OTA_LOG_E("ota image head index = %d error", cur_frame_index);
                                    break;
                                }
                                memset(&param, 0x00, sizeof(ota_boot_param_t));
                                last_frame_index = cur_frame_index;
                                param.len = EXTRACT_U32(&recv_data.rec_buf[9]);
                                recvd_image_len = 0;
                                if(ota_hal_init(&param) < 0) {
                                    OTA_LOG_E("ota can init failed");
                                    loop_lock = 0;
                                    can_response = OTA_MCU_RES_HEAD_ERR;
                                    break;
                                }
                                can_response = OTA_MCU_RES_SUCCESS;
                                upgrade_status = SYS_STATUS_UPGRADE_DOING;
                                OTA_LOG_I("image size = %d, index = %d", param.len, cur_frame_index);
                                reboot_flag = 1;
                            }
                            else {
                                OTA_LOG_E("recvd image head error\r\n");
                                can_response = OTA_MCU_RES_HEAD_ERR;
                            }
                        case SYS_STATUS_UPGRADE_DOING:
                            if(cmd[1] == OTA_CAN_CMD_DATA) {
                                OTA_LOG_I("get data\r\n");
                                can_response = OTA_MCU_RES_SUCCESS;
                                cur_frame_index = EXTRACT_U32(&recv_data.rec_buf[5]);
                                payload_len = frame_len - 11;
                                if((last_frame_index + 1) == cur_frame_index) {
                                    last_frame_index = cur_frame_index;
                                    ret = ota_hal_write(&recvd_image_len, (char*)&recv_data.rec_buf[9], payload_len);
                                    if(recvd_image_len == param.len) {
                                        OTA_LOG_I("recvd completed, recvd_len = %d, image_size = %d", recvd_image_len, param.len);
                                        param.crc = 0;
                                        if(ota_hal_boot(&param) < 0) {
                                            OTA_LOG_E("set boot failed");
                                            can_response = OTA_MCU_RES_WRITE_ERR;
                                        }
                                        loop_lock = 0;
                                    }
                                }
                                else if(last_frame_index == cur_frame_index){
                                    OTA_LOG_E("recvd same pack,ignore");
                                }
                                else {
                                    OTA_LOG_E("recvd index = %d error", cur_frame_index);
                                    can_response = OTA_MCU_RES_FAIL;
                                }
                            }
                            break;
#endif
                        default:
                            OTA_LOG_E("cmd don't support");
                            can_response = OTA_MCU_RES_FRAME_ERR;
                            break;
                    }
                }
                else {
                    can_response = OTA_MCU_RES_FRAME_ERR;
                    OTA_LOG_E("Invalid frame data!");
                }
            }
            else {
                can_response = OTA_MCU_RES_FRAME_ERR;
                OTA_LOG_E("Invalid frame len %d", recv_data.len);
            }
            co_upgrade_report(can_io_node, can_response);
            can_response = OTA_MCU_RES_FAIL;
        }
        ota_msleep(1);
    }
    if(reboot_flag == 1) {
        ota_msleep(1000);
        ota_reboot();
    }
    is_thread_creat = 0;
    return ret;
}
#endif
