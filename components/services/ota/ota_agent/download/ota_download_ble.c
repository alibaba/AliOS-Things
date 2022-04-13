#include <string.h>
#include "ota_ble.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal_trans.h"
#include "ota_hal_digest.h"
#include "ota_hal.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "ota_agent.h"

extern volatile char g_ota_file_type;

static void *thread = NULL;
static unsigned int off_bp = 0;
static aos_queue_t ble_ota_queue;
ota_ble_global_dat_t g_ctx = {0};
static ota_ble_receive_t tmp_queue;
static char image_version[64] = {0};
static unsigned int image_has_recvd = 0;
static unsigned char image_head_is_get = 0;
static unsigned short image_crc16 = 0xffff;
static ota_boot_param_t ota_ble_param = {0};
static ota_ble_receive_t cmd_data_tmp_cache;
static unsigned short image_crc16_bp = 0xffff;
static unsigned char ota_ble_msg_creat_flag = 0;
static ota_image_info_t image_tail_data_content;
static unsigned int image_information_buf_ptr = 0;
static unsigned int image_inforation_buf_size = 0;
static unsigned char  l_data_new[OTA_VER_LEN + 1]; /* +1 for trailing zero */
static unsigned char *image_information_buf = NULL;
static volatile char ota_ble_subdev_upgrade_flag = 0;
static unsigned char ota_ble_recvd_buf[OTA_BLE_CMD_SIZE * OTA_BLE_CAHCH_CMD_NUMB];

on_ota_bool_pfunc ota_ble_status_get_cb = NULL;
on_ota_int_pfunc ota_ble_user_store_data_process_cb = NULL;

unsigned int hal_flash_erase_sector_size(void);
int ota_ble_write(unsigned int *off, char *in_buf, int in_buf_len);
int ota_ble_breakpoint_process(unsigned int image_size, unsigned int *break_point);

unsigned int ota_ble_tail_get_len(unsigned int image_size, unsigned int tail_len)
{
    unsigned int tmp_len = 0;
    unsigned int want_len = 0;
    if(image_size - tail_len > 16) {
        tmp_len = image_size - tail_len;
        (tmp_len % 16 == 0) ? (want_len = tmp_len) : (want_len = tmp_len - tmp_len % 16);
    }
    return want_len;
}

static unsigned short ota_ble_crc16_calculate(unsigned char const *p_data, unsigned int size, unsigned short const *p_crc)
{
    unsigned short crc = (p_crc == NULL) ? 0xFFFF : *p_crc;
    for (unsigned int i = 0; i < size; i++) {
        crc  = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (unsigned char)(crc & 0xFF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xFF) << 4) << 1;
    }
    return crc;
}

static void ota_ble_init_image_crc16()
{
    image_crc16 = 0xffff;
    image_crc16_bp = 0xffff;
}

void ota_ble_calculate_image_crc16(unsigned char *data, unsigned int len)
{
    unsigned int erase_sector_size = 0;
    unsigned int tmp_len = 0;
    erase_sector_size = hal_flash_erase_sector_size();
    if (image_has_recvd % erase_sector_size + len >= erase_sector_size) {
        tmp_len = erase_sector_size - image_has_recvd % erase_sector_size;
        image_crc16 = ota_ble_crc16_calculate(data, tmp_len, &image_crc16);
        image_crc16_bp = image_crc16;
        image_crc16 = ota_ble_crc16_calculate(data + tmp_len, len - tmp_len, &image_crc16);
    }
    else {
        image_crc16 = ota_ble_crc16_calculate(data, len, &image_crc16);
    }
    image_has_recvd += len;
}

static int ota_ble_check_image_crc16()
{
    int ret = -1;
    (image_crc16 == g_ctx.crc) ? (ret = 0) : (ret = -1);
    return ret;
}

int ota_ble_msg_queue_init()
{
    int ret = 0;
    if(ota_ble_msg_creat_flag == 0) {
        ret = aos_queue_new(&ble_ota_queue, ota_ble_recvd_buf, OTA_BLE_CMD_SIZE * OTA_BLE_CAHCH_CMD_NUMB, OTA_BLE_CMD_SIZE);
        if(ret != 0) {
            OTA_LOG_E("ota ble creat queue fail");
        }
        else {
            ota_ble_msg_creat_flag = 1;
        }
    }
    return ret;
}

static void ota_ble_receive_data_product(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buf, unsigned int len)
{
    int ret = 0;
    if(len <= OTA_BLE_CMD_SIZE) {
        cmd_data_tmp_cache.cmd = ota_cmd;
        cmd_data_tmp_cache.len = len;
        cmd_data_tmp_cache.num_frames = num_frame;
        if(buf != NULL) {
            memcpy(cmd_data_tmp_cache.rec_buf, buf, len);
        }
        ret = aos_queue_send(&ble_ota_queue, (void*)&cmd_data_tmp_cache, len + 4);
        if(ret != 0) {
            OTA_LOG_E("ota ble queue send err");
        }
    }
    else {
        OTA_LOG_E("ota ble queue too short!");
    }
}

static int ota_ble_receive_data_consume(ota_ble_receive_t *out_queue, unsigned int timeout)
{
    int ret = 0;
    unsigned int recvd_len = 0;
    if(out_queue != NULL) {
        ret = aos_queue_recv(&ble_ota_queue, timeout, (void*)out_queue, &recvd_len);
        if((ret == 0) && (recvd_len >= 4)) {
            ret = OTA_SUCCESS;
        }
        else {
            ret = OTA_INIT_FAIL;
        }
    }
    return ret;
}

static unsigned int ota_ble_creat_image_tail_data_buf()
{
    unsigned int ret = OTA_INIT_FAIL;
    if(image_information_buf == NULL) {
        image_inforation_buf_size = sizeof(ota_image_info_t) + 32;
        image_information_buf = ota_malloc(image_inforation_buf_size);
        if(image_information_buf != NULL) {
            ret = OTA_SUCCESS;
            memset(image_information_buf, 0x00, image_inforation_buf_size);
        }
        else {
            ret = OTA_INIT_FAIL;
        }
    }
    return ret;
}

static unsigned int ota_ble_get_image_data(unsigned char *buf, unsigned int len, unsigned char num_frames)
{
    unsigned int ret = OTA_INIT_FAIL;
    unsigned char *tmp_buf = image_information_buf + image_information_buf_ptr;
    if((image_information_buf_ptr + len) <= image_inforation_buf_size) {
        ret = OTA_SUCCESS;
        memcpy(tmp_buf, buf, len);
        g_ctx.frames_recvd += num_frames;
        g_ctx.bytes_recvd += len;
        image_information_buf_ptr += len;
    }
    return ret;
}

static int ota_ble_get_head_image_info(unsigned int offset, ota_image_info_t *image_info)
{
    unsigned int ret = OTA_INIT_FAIL;
    ota_image_info_t tmpinfo;
    if(image_info != NULL) {
        ret = ota_hal_read(&offset, (char*)&tmpinfo, sizeof(ota_image_info_t));
        if(ret >= 0) {
            ret = OTA_INIT_FAIL;
            if (tmpinfo.image_magic == OTA_BIN_MAGIC_ALL) {
                ret = OTA_SUCCESS;
                OTA_LOG_I("head image size = %d, crc16 = 0x%x", tmpinfo.image_size, tmpinfo.image_crc16);
                image_info->image_magic = tmpinfo.image_magic;
                image_info->image_crc16 = tmpinfo.image_crc16;
                image_info->image_num = tmpinfo.image_num;
                image_info->image_size = tmpinfo.image_size;
                memcpy(image_info->image_md5, tmpinfo.image_md5, sizeof(tmpinfo.image_md5));
            }
        }
        else {
            OTA_LOG_E("ota read param err!\r\n");
        }
    }
    return ret;
}

static unsigned int ota_ble_parse_image_info()
{
    unsigned int read_point = 0;
    unsigned int ret = OTA_INIT_FAIL;
    if(image_information_buf_ptr >= sizeof(ota_image_info_t)) {
        ret = OTA_SUCCESS;
        read_point = image_information_buf_ptr - sizeof(ota_image_info_t);
        memcpy((unsigned char*)&image_tail_data_content, image_information_buf + read_point, sizeof(ota_image_info_t));
        if ((image_tail_data_content.image_magic == OTA_BIN_MAGIC_ALL) &&
            (image_tail_data_content.image_num == 2)) {
            g_ctx.slaver_image_crc16 = image_tail_data_content.image_crc16;
            g_ctx.slaver_image_size = image_tail_data_content.image_size + sizeof(ota_image_info_t);
        }
        else {
            g_ctx.slaver_image_crc16 = 0;
            g_ctx.slaver_image_size = 0;
            ret = OTA_DOWNLOAD_HEAD_FAIL;
        }
    }
    OTA_LOG_I("slaver image size = %d, crc16 = 0x%x", g_ctx.slaver_image_size, g_ctx.slaver_image_crc16);
    return ret;
}

static unsigned int ota_ble_check_version(const char *version, unsigned char *p_data, unsigned char len, unsigned char *file_type)
{
    unsigned int ret = OTA_INIT_FAIL;
    unsigned char  l_len;
    if((file_type != NULL) && (version != NULL) && (p_data != NULL) && (len > 7)) {
        *file_type = 0;
        l_len = len - sizeof(unsigned int) - sizeof(unsigned short) - 1;
        if(l_len > OTA_VER_LEN) {
            OTA_LOG_E("ver len too long");
            ret = OTA_UPGRADE_FW_SIZE_FAIL;
        }
        else {
            ret = OTA_SUCCESS;
            memcpy(l_data_new, p_data, l_len);
            l_data_new[l_len] = 0;
            if((strlen(l_data_new) == 4) && (l_data_new[0] == 'P') && (l_data_new[1] == 'C')) {
                if((l_data_new[2] >= '0') && (l_data_new[2] <= '9') &&
                 (l_data_new[3] >= '0') && (l_data_new[3] <= '9')) {
                    *file_type = 1;
                }
                else {
                    ret = OTA_TRANSPORT_VER_FAIL;
                    OTA_LOG_E("picture id err");
                }
            }
            else if((strlen(l_data_new) == 3) && (l_data_new[0] == 'C') &&
                   (l_data_new[1] == 'F') && (l_data_new[2] == 'G')) {
                    *file_type = 2;
            }
            else if(memcmp(version, (unsigned char*)l_data_new, l_len) == 0) {
                ret = OTA_TRANSPORT_VER_FAIL;
                OTA_LOG_E("ver same err!");
            }
        }
    }
    return ret;
}

static unsigned int ota_ble_update_data(unsigned char *buf, unsigned int len, unsigned char num_frames)
{
    unsigned short percent;
    int ret = OTA_SUCCESS;
    static unsigned short last_percent = 0;
    if((buf == NULL) || (len == 0)) {
        ret = OTA_INIT_FAIL;
        goto OTA_BLE_TRANS_ERR;
    }
    if(g_ctx.bytes_recvd == 0) {
        g_ctx.ota_type = ota_parse_ota_type(buf, len);
        if(g_ctx.ota_type == 0) {
            ret = OTA_UPGRTADE_TAG_ERR;
            OTA_LOG_E("ota type err.");
            goto OTA_BLE_TRANS_ERR;
        }
    }
    if(g_ctx.bytes_recvd + len < g_ctx.rx_fw_size) {
        if (len % 256 != 0) {
            ret = ota_ble_transport(OTA_BLE_CMD_ERROR, NULL, 0);
            if (ret != OTA_SUCCESS) {
                OTA_LOG_E("send err.");
            }
            ret = OTA_UPGRADE_FW_SIZE_FAIL;
            goto OTA_BLE_TRANS_ERR;
        }
    }
    if (ota_ble_write(&g_ctx.bytes_recvd, (char *)buf, len) != 0) {
        ret = OTA_UPGRADE_WRITE_FAIL;
        goto OTA_BLE_TRANS_ERR;
    }
    ota_ble_calculate_image_crc16(buf, len);
    g_ctx.frames_recvd += num_frames;
    off_bp = g_ctx.bytes_recvd;
    percent = g_ctx.bytes_recvd * 100 / g_ctx.rx_fw_size; /* Ensure no overflow */
    if(percent < last_percent) {
        /*breakpoint need to clear last_percent*/
        last_percent = 0;
    }
    if ((percent - last_percent) >= 5) {
        OTA_LOG_I("===>%dB\t%d%% ...", g_ctx.bytes_recvd, percent);
        last_percent = percent;
    }
OTA_BLE_TRANS_ERR:
    return ret;
}

static int ota_ble_download_request(unsigned char *buffer, unsigned int len)
{
    int ret = 0;
    unsigned char l_len = 0;
    unsigned int image_tail_len = 0;
    const char *sys_ver = NULL;
    unsigned char tmp_type = 0;
    if((buffer == NULL) || (len <  sizeof(unsigned int) + sizeof(unsigned short))) {
        return OTA_INIT_FAIL;
    }
    image_tail_len = sizeof(ota_image_info_t);
    sys_ver = ota_hal_version(0, 0);
    ret = ota_ble_check_version(sys_ver, buffer, len, &tmp_type);
    if (ret == 0) {
        if(memcmp(image_version, buffer, len) != 0) {
            image_head_is_get = 0;
            off_bp = 0;
            g_ctx.ota_type = 0;
            g_ctx.slaver_image_crc16 = 0;
            g_ctx.slaver_image_size = 0;
        }
        else {
            if((tmp_type == 0) && (image_head_is_get == 0)) {
                off_bp = 0;
                g_ctx.ota_type = 0;
                g_ctx.slaver_image_crc16 = 0;
                g_ctx.slaver_image_size = 0;
            }
        }
        memcpy(image_version, buffer, len);
        l_len = len - sizeof(unsigned int) - sizeof(unsigned short);
        OTA_LOG_I("tmp_type = %d", tmp_type);
        g_ctx.file_type = tmp_type;
        g_ota_file_type = g_ctx.file_type;
        g_ctx.rx_fw_size = EXTRACT_U32(buffer + l_len);
        g_ctx.frames_recvd = 0;
        g_ctx.crc = EXTRACT_U16(buffer + l_len + sizeof(unsigned int));
        if((g_ctx.file_type == 0) && (image_head_is_get == 0)) {
            g_ctx.bytes_recvd = ota_ble_tail_get_len(g_ctx.rx_fw_size, image_tail_len);
            if(g_ctx.bytes_recvd == 0) {
                return OTA_DOWNLOAD_HEAD_FAIL;
            }
        }
        ret = ota_ble_breakpoint_process(g_ctx.rx_fw_size, &off_bp);
        if(ret == 0) {
            if(off_bp > 0) {
                g_ctx.bytes_recvd = off_bp;
                image_crc16 = image_crc16_bp;
                image_has_recvd = off_bp;
                OTA_LOG_I("begin off_bp = %d, bp_crc = 0x%04x\r\n", off_bp, image_crc16);
            }
            else {
                if(g_ctx.file_type != 0) {
                    g_ctx.bytes_recvd = 0;
                }
                image_has_recvd = 0;
                ota_ble_init_image_crc16();
            }
        }
        else {
            return OTA_INIT_FAIL;
        }
    }
    else {
        ret = OTA_TRANSPORT_VER_FAIL;
    }
    return ret;
}

static int ota_ble_download_start(void)
{
    int ret = 0;
    const char *sys_ver = NULL;
    unsigned char send_err  = 0;
    unsigned int wait_time = AOS_WAIT_FOREVER;
    unsigned char ack = 0;
    unsigned char download_status = 0;
    ota_image_info_t tmp_info;
    g_ctx.status = OTA_BLE_STATE_IDLE;
    ret = ota_ble_creat_image_tail_data_buf();
    if(ret == OTA_SUCCESS) {
        image_information_buf_ptr = 0;
    }
    else {
        image_information_buf = NULL;
        g_ctx.task_flag = 0;
        OTA_LOG_E("ble ota malloc tail data buf failed");
        return -1;
    }
    while(1) {
        ret = ota_ble_receive_data_consume(&tmp_queue, wait_time);
        if(ret != OTA_SUCCESS) {
            wait_time = AOS_WAIT_FOREVER;
            OTA_LOG_E("ota ble receive timeout!");
            if(ota_ble_status_get_cb != NULL) {
                ota_ble_status_get_cb(0x00);
            }
            download_status = 0;
            continue;
        }
        if ((tmp_queue.cmd & OTA_BLE_CMD_TYPE_MASK) != OTA_BLE_CMD_TYPE_FW_UPGRADE) {
            OTA_LOG_E("cmd err");
        }
        else {
            send_err = 1;
            ack = 1;
            ret = 0;
            wait_time = 2000;
            if((tmp_queue.cmd == OTA_BLE_CMD_FW_VERSION_REQ) ||
               (tmp_queue.cmd == OTA_BLE_CMD_FW_UPGRADE_REQ)) {
                g_ctx.status = OTA_BLE_STATE_IDLE;
            }
            printf("ble status = %d, cmd = 0x%02x", g_ctx.status, tmp_queue.cmd);
            switch(g_ctx.status) {
                case OTA_BLE_STATE_IDLE:
                    if (tmp_queue.cmd == OTA_BLE_CMD_FW_VERSION_REQ) { /*cmd=0x20*/
                        send_err = 0;
                        sys_ver = ota_hal_version(0, 0);
                        OTA_LOG_I("sys_ver = %s", sys_ver);
                        ret = ota_ble_transport(OTA_BLE_CMD_FW_VERSION_RSP, (unsigned char*)sys_ver, strlen(sys_ver)) ;
                        if(ret != OTA_SUCCESS) {
                            OTA_LOG_E("ble cmd = 0x21 response failed");
                        }
                        wait_time = AOS_WAIT_FOREVER;
                    }
                    else if (tmp_queue.cmd == OTA_BLE_CMD_FW_UPGRADE_REQ) { /*cmd=0x22*/
                        send_err = 0;
                        ret = ota_ble_download_request(tmp_queue.rec_buf, tmp_queue.len);
                        if (ret == OTA_SUCCESS) {
                            ret = ota_ble_transport(OTA_BLE_CMD_FW_UPGRADE_RSP, &ack, 1);
                            if (ret == OTA_SUCCESS) {
                                if(g_ctx.file_type == 0) {
                                    (image_head_is_get == 0) ? (g_ctx.status = OTA_BLE_STATE_RECEIVER_HEADER) :
                                        (g_ctx.status = OTA_BLE_STATE_RECEIVE);
                                }
                                else {
                                    g_ctx.status = OTA_BLE_STATE_RECEIVE;
                                }
                                if(ota_ble_status_get_cb != NULL) {
                                    if(ota_ble_status_get_cb(0x01)) {
                                        download_status = 1;
                                    }
                                    else {
                                        OTA_LOG_E("download abort, may the battery level is low");
                                    }
                                }
                            }
                        }
                        else {
                            ack = 0;
                            ret = ota_ble_transport(OTA_BLE_CMD_FW_UPGRADE_RSP, &ack, 1);
                        }
                        if(ret != OTA_SUCCESS) {
                            if(download_status == 1) {
                                download_status = 2;
                            }
                            OTA_LOG_E("ble cmd=0x23 response failed");
                        }
                    }
                    break;
                case OTA_BLE_STATE_RECEIVER_HEADER:
                    send_err = 0;
                    ret = -1;
                    if (tmp_queue.cmd == OTA_BLE_CMD_FW_DATA) { /*cmd=0x2F*/
                        ret = ota_ble_get_image_data(tmp_queue.rec_buf, tmp_queue.len, tmp_queue.num_frames);
                        if(ret == OTA_SUCCESS) {
                            if(g_ctx.bytes_recvd >= g_ctx.rx_fw_size) {
                                g_ctx.bytes_recvd = 0;
                                off_bp = 0;
                                ret = ota_ble_parse_image_info();
                                if(ret != OTA_SUCCESS) {
                                    OTA_LOG_E("ble parse image failed");
                                    break;
                                }
                                image_head_is_get = 1;
                                g_ctx.status = OTA_BLE_STATE_RECEIVE;
                            }
                            ret = ota_ble_send_bytes_received();
                        }
                        else {
                            OTA_LOG_E("rece data failed");
                            break;
                        }
                    }
                    else if (tmp_queue.cmd == OTA_BLE_CMD_FW_GET_INIT_FW_SIZE) { /*cmd=0x27*/
                        ret = ota_ble_send_bytes_received();
                        image_information_buf_ptr = 0;
                    }
                    if(ret != OTA_SUCCESS) {
                        send_err = 1;
                        if(download_status == 1) {
                            download_status = 2;
                        }
                        OTA_LOG_E("ble ota head cmd=0x27 response failed");
                    }
                    break;
                case OTA_BLE_STATE_RECEIVE:
                    ret = -1;
                    if (tmp_queue.cmd == OTA_BLE_CMD_FW_DATA) { /*cmd=0x2F*/
                        ret = ota_ble_update_data(tmp_queue.rec_buf, tmp_queue.len, tmp_queue.num_frames);
                        if(ret == OTA_SUCCESS) {
                            ret = ota_ble_send_bytes_received();
                        }
                        else {
                            OTA_LOG_E("rece data failed");
                        }
                    }
                    else if (tmp_queue.cmd == OTA_BLE_CMD_FW_GET_INIT_FW_SIZE) { /*cmd=0x27*/
                        ret = ota_ble_send_bytes_received();
                    }
                    if(ret == OTA_SUCCESS) {
                        send_err = 0;
                        if(g_ctx.bytes_recvd >= g_ctx.rx_fw_size) {
                            off_bp = 0;
                            if(g_ctx.file_type == 0) {
                                g_ctx.status = OTA_BLE_STATE_FW_CHECK;
                            }
                            else {
                                if(ota_ble_check_image_crc16() == 0) {
                                    if (g_ctx.file_type == 1) {
                                        unsigned char tmp_type = OTA_RECVD_PINFACE;
                                        if(g_ctx.ota_type == OTA_UPGRADE_XZ) {
                                            tmp_type = OTA_RCVD_XZ_PINFACE;
                                            OTA_LOG_I("XZ pinface");
                                        }
                                        OTA_LOG_I("image ok, size = %d", g_ctx.rx_fw_size);
                                        ota_ble_subdev_upgrade_flag = 1;
                                        ret = ota_ble_mcu_upgrade(0x00, tmp_type, g_ctx.rx_fw_size, l_data_new, g_ctx.crc);
                                        if(ret != 0) {
                                            ack = 0;
                                            OTA_LOG_E("ota_mcu_ubus_upgrade failed");
                                        }
                                        else {
                                            OTA_LOG_I("ota_mcu_ubus_upgrade success");
                                        }
                                    }
                                    else if(g_ctx.file_type == 2) {
                                        ret = -1;
                                        ota_ble_subdev_upgrade_flag = 1;
                                        OTA_LOG_I("cfg file ok, size = %d", g_ctx.rx_fw_size);
                                        if(ota_ble_user_store_data_process_cb != NULL) {
                                            ret = ota_ble_user_store_data_process_cb(0x00, g_ctx.rx_fw_size);
                                        }
                                        if(ret < 0) {
                                            ack = 0;
                                            OTA_LOG_E("ota cfg file failed");
                                        }
                                    }
                                }
                                else {
                                    ack = 0;
                                    OTA_LOG_E("image crc err");
                                }
                                aos_msleep(200);
                                g_ctx.status = OTA_BLE_STATE_IDLE;
                                ret = ota_ble_transport(OTA_BLE_CMD_FW_CHECK_RESULT, &ack, 1);
                                if(ret != OTA_SUCCESS) {
                                    OTA_LOG_E("file checksume send failed");
                                }
                                if(download_status == 1) {
                                    download_status = 2;
                                }
                                OTA_LOG_I("recvd over");
                                ota_ble_subdev_upgrade_flag = 0;
                            }
                        }
                    }
                    break;
                case OTA_BLE_STATE_FW_CHECK:
                    if (tmp_queue.cmd == OTA_BLE_CMD_FW_XFER_FINISH) { /*cmd=0x28*/
                        ack = 0;
                        if(ota_ble_check_image_crc16() == 0) {
                            if(g_ctx.file_type == 0) {
                                unsigned int read_offset = g_ctx.rx_fw_size - g_ctx.slaver_image_size;
                                ota_ble_subdev_upgrade_flag = 1;
                                OTA_LOG_I("read_offset = %d, read_size = %d", read_offset, g_ctx.slaver_image_size);
                                OTA_LOG_I("iamge crc16 = 0x%04x", g_ctx.slaver_image_crc16);
                                ret = ota_ble_mcu_upgrade(read_offset, OTA_RECVD_BLE_SLAVER_IMAGE, g_ctx.slaver_image_size - sizeof(ota_image_info_t), l_data_new, g_ctx.slaver_image_crc16);
                                if(ret != 0) {
                                    OTA_LOG_E("ota_mcu_ubus_upgrade failed");
                                }
                                else
                                {
                                    OTA_LOG_I("ota_mcu_ubus_upgrade success");
                                    ota_ble_param.upg_flag = g_ctx.ota_type;
                                    ota_ble_param.len = g_ctx.rx_fw_size - g_ctx.slaver_image_size - sizeof(ota_image_info_t);
                                    ret = ota_ble_get_head_image_info(ota_ble_param.len, &tmp_info);
                                    if(ret == 0) {
                                        ota_ble_param.crc = tmp_info.image_crc16;
                                        OTA_LOG_I("ota_ble_param.crc = 0x%x", ota_ble_param.crc);
                                        memcpy(ota_ble_param.ver, image_version, sizeof(ota_ble_param.ver));
                                        if(ota_hal_boot(&ota_ble_param) == 0) {
                                            ota_msleep(1000);
                                            g_ctx.status = OTA_BLE_STATE_RESET_PREPARE;
                                            OTA_LOG_I("setboot over");
                                            ack = 1;
                                        }
                                        else {
                                            OTA_LOG_E("setboot failed");
                                        }
                                    }
                                    else {
                                        OTA_LOG_E("get head image information err");
                                    }
                                }
                            }
                            else {
                               OTA_LOG_E("image type err");
                            }
                        }
                        else {
                            OTA_LOG_E("image check failed");
                        }
                        if(ack == 0) {
                            g_ctx.status = OTA_BLE_STATE_IDLE;
                            g_ota_file_type = -1;
                        }
                        ret = ota_ble_transport(OTA_BLE_CMD_FW_CHECK_RESULT, &ack, 1);
                        if(ack == 1) {
                            ota_msleep(1000);
                        }
                        if(download_status == 1) {
                            download_status = 2;
                        }
                        ota_ble_subdev_upgrade_flag = 0;
                        image_head_is_get = 0;
                        send_err = 0;
                        off_bp = 0;
                    }
                    break;
                default:
                    break;
            }
            if(send_err == 1) {
                ret = ota_ble_transport(OTA_BLE_CMD_ERROR, NULL, 0);
                if (ret != OTA_SUCCESS) {
                    OTA_LOG_E("send err.");
                }
                OTA_LOG_E("send err report");
                if(download_status == 1) {
                    download_status = 2;
                }
                wait_time = AOS_WAIT_FOREVER;
            }
            if(download_status == 2) {
                download_status = 0;
                if(ota_ble_status_get_cb != NULL) {
                    ota_ble_status_get_cb(0x00);
                }
                wait_time = AOS_WAIT_FOREVER;
            }
        }
    }
    OTA_LOG_I("task over!");
    if(image_information_buf != NULL) {
        ota_free(image_information_buf);
    }
    image_information_buf = NULL;
    g_ctx.task_flag = 0;
    return 0;
}

int ota_ble_download(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buf, unsigned int len)
{
    int ret = 0;
    if(g_ctx.task_flag == 0) {
        ret = ota_ble_msg_queue_init();
        if(ret == 0) {
            ret = ota_thread_create(&thread, (void *)ota_ble_download_start, (void *)NULL, NULL, 1024 * 8);
            if(ret >= 0) {
                g_ctx.task_flag = 1;
            }
            else {
                OTA_LOG_E("ota task creat failed");
                return OTA_DOWNLOAD_INIT_FAIL;
            }
        }
    }
    if((g_ctx.task_flag == 1) && (ota_ble_subdev_upgrade_flag == 0)) {
        printf("ota recvd len = %d\r\n", len);
        ota_ble_receive_data_product(ota_cmd, num_frame, buf, len);
    }
    else if(ota_ble_subdev_upgrade_flag == 1) {
        OTA_LOG_E("subdev upgrading");
    }
    return ret;
}
