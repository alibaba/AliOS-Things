#include "ota/ota_slaver_upgrade.h"
#include <stdbool.h>
#include "ubus/in_comm_task.h"
#include "ota_log.h"
#include "ubus/ubus_profile.h"
#include "ota_agent.h"
#include "aos/hal/flash.h"
#include "ota_import.h"
#include "ota_hal_os.h"

#define FILE_RECVD_BEGIN          (0x00)
#define FILE_RECVD_DOING          (0x01)
#define FILE_RECVD_DONE           (0x02)

#define OTA_ROUTINE_STACK_SIZE    (4096 * 2)
#define OTA_ROUTINE_QUEUE_NUMBER  (5)
#define OTA_PAYLOAD_DATA_SIZE     (1100)
#define OTA_UBUS_QUEUE_SIZE       sizeof(ubus_data_unit_t)
#define OTA_SLAVER_WAIT_TIMEOUT   (2000)

#define OTA_SLAVER_QUEUE_SIZE     (1100)
#define OTA_SLAVER_QUEUE_NUMB     (2)

#define DEBUG_PINFACE             (1)

static unsigned int slaver_image_total_erase_page_numbs = 0;
static unsigned int slaver_image_has_erase_page_numbs = 0;
static unsigned int slaver_image_erase_sector_size = 0;

static aos_queue_t ota_slaver_queue;
static unsigned char ota_slaver_recvd_queue[OTA_SLAVER_QUEUE_SIZE * OTA_SLAVER_QUEUE_NUMB];

static int ota_slaver_data_process(unsigned char *buf, unsigned int len);
static void handle_ubus_ota_updrade(const dbus_data_t data_type, const uint16_t len,
                                const void *data, const void *user_data);
static void ota_slaver_upgrade_proc(void *data);
int ota_slaver_init(unsigned int image_size, int flash_partition);
int ota_slaver_write(int flash_partition, unsigned int *off, char *in_buf ,int in_buf_len);
int ota_slaver_set_boot(unsigned int image_len, unsigned int ota_type, unsigned short image_crc);
void ui_start_data_snyc_gif_view(void);
void ui_stop_data_snyc_gif_view(void);
void *pinface_update_create(const char *filename);
int pinface_update_data(void *fp, void *buf, int size);
void pinface_update_abort(void *fp);
int pinface_update_finish(void *fp);
uint64_t pinface_inquire(void);
const char *ota_slaver_version(void);
int ota_slaver_update_parameter(ota_boot_param_t *ota_param);
int ota_slaver_check_upgrade_result(unsigned char *ota_result);
static int ota_slave_save_pinface(unsigned char *file_name, hal_partition_t file_store_partion,
    unsigned int offset, unsigned int size);

static rx_handler_node_t ota_rx_handler_node[] = {
    {ubus_data_upgrade_header, handle_ubus_ota_updrade, NULL},
    {ubus_data_upgrade_text,   handle_ubus_ota_updrade, NULL},
};

const static async_task_interface_t ota_slaver_async_task_list =
{ ASYNC_ID_OTA_UPGRADE, "ota-slaver", 0, ota_slaver_upgrade_proc, OTA_ROUTINE_STACK_SIZE, 32, OTA_ROUTINE_QUEUE_NUMBER, OTA_UBUS_QUEUE_SIZE, {}, NULL, {} };

static bool routine_initd = false;

static unsigned int slaver_image_flash_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

int ota_slaver_msg_queue_init()
{
    int ret = 0;
    ret = aos_queue_new(&ota_slaver_queue, ota_slaver_recvd_queue, OTA_SLAVER_QUEUE_SIZE * OTA_SLAVER_QUEUE_NUMB, OTA_SLAVER_QUEUE_SIZE);
    if(ret != 0) {
        OTA_LOG_E("ota ble creat queue fail");
    }
    return ret;
}

int32_t ota_slaver_upgrade_init(void)
{
    int  ret = -1;
    const uint8_t rx_list_size = sizeof(ota_rx_handler_node) / sizeof(ota_rx_handler_node[0]);
    if(!routine_initd) {
        ret = ota_slaver_msg_queue_init();
        if(ret == 0) {
            ret = async_task_init(&ota_slaver_async_task_list);
            if(ret == 0) {
                if (rx_list_size != ubus_register_rx_handler_list(rx_list_size, ota_rx_handler_node)) {
                    OTA_LOG_E("subscribe data Failed");
                    async_task_uninit(ASYNC_ID_OTA_UPGRADE);
                }
                else {
                    routine_initd = true;
                }
            }
            else {
                OTA_LOG_E("start Async Failed");
            }
        }
        else {
            OTA_LOG_E("ota slaver queue init failed");
        }
    }
    return ret;
}

static unsigned char tmp_slaver_recvd_buf[1100];
static void handle_ubus_ota_updrade(const dbus_data_t data_type, const uint16_t len, const void *data, const void *user_data)
{
    int ret = 0;
    if((data != NULL) && (len <= sizeof(tmp_slaver_recvd_buf))) {
        memcpy(tmp_slaver_recvd_buf, data, len);
        ret = aos_queue_send(&ota_slaver_queue, (void*)tmp_slaver_recvd_buf, len);
        if(ret != 0) {
            OTA_LOG_E("ota ble queue send err");
        }
    }
}

static int ota_slaver_send_data(unsigned char *buf, unsigned int len)
{
    int tmp_len = 0;
    int ret = -1;
    unsigned char tmp_send_buf[64];
    memset(tmp_send_buf, 0, sizeof(tmp_send_buf));
    tmp_send_buf[0] = 1;
    if(buf != NULL) {
        tmp_send_buf[1] = ubus_data_slave_bin_resp;
        SET_CUBE_LEN(&tmp_send_buf[1], len);
        memcpy(&tmp_send_buf[sizeof(ubus_data_cube_t) + 1], buf, len);
        ret = ubus_send(ubus_comm_process, len + 4, tmp_send_buf);
    }
    if(ret != 0) {
          OTA_LOG_E("Send uBUS Failed");
    }
    return ret;
}

int ota_slaver_report_upgrade_result(unsigned char result)
{
    int ret = -1;
    unsigned short src_crc16 = 0;
    unsigned short frame_crc16 = 0;
    unsigned char tmp_send_buf[20] = {0};
    tmp_send_buf[0] = 0xBA;
    tmp_send_buf[1] = 0x00;
    tmp_send_buf[2] = 0x09;
    tmp_send_buf[3] = 0x00;
    tmp_send_buf[4] = 0xF0;
    tmp_send_buf[5] = 0x00;
    tmp_send_buf[6] = 0x01;
    if(result == 0) {
        tmp_send_buf[6] = 0x00;
    }
    src_crc16 = ota_get_data_crc16(tmp_send_buf, 7);
    tmp_send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
    tmp_send_buf[8] = (char)(src_crc16 & 0x00ff);
    ret = ota_slaver_send_data(tmp_send_buf, 9);
    if(ret < 0) {
        OTA_LOG_E("ubus send error!");
    }
    else if(tmp_send_buf[6] == 0){
        OTA_LOG_I("upgrade success!");
    }
    return ret;
}

static unsigned char ota_slaver_recvd_buf[1100];
static void ota_slaver_upgrade_proc(void *data)
{
    int ret = -1;
    unsigned int offset = 0;
    unsigned int recvd_len = 0;
    unsigned short src_crc16 = 0;
    unsigned short frame_crc16 = 0;
    unsigned char frame_header = 0;
    unsigned short frame_len = 0;
    unsigned char send_buf[20];
    unsigned char status = FILE_RECVD_BEGIN;

    unsigned int image_size = 0;
    unsigned short image_crc16 = 0;
    unsigned int image_upgrade_type = 0;
    unsigned int has_recvd_len = 0;
    unsigned char image_type = 0x00;
    unsigned short recvd_index = 0;
    unsigned short last_index = 0;
    unsigned char image_ver[32] = {0};
    unsigned char upgrade_result = 0;
    ota_crc16_ctx  ctx    = {0};
    unsigned short cal_image_crc16 = 0;
    void *fptr = NULL;

    if (ota_slaver_check_upgrade_result(&upgrade_result) == 0) {
        unsigned char fwup_success = 0x01;
        if(ota_slaver_report_upgrade_result(upgrade_result) != 0) {
             OTA_LOG_E("send err!");
        }
    }

    if(ota_slaver_rollback() < 0) {
        OTA_LOG_E("ota slaver clear flag faild!");
    }

    while (1) {
        ret = aos_queue_recv(&ota_slaver_queue, AOS_WAIT_FOREVER, (void*)ota_slaver_recvd_buf, &recvd_len);
        if((ret == 0) && (recvd_len >= 9)) {
            ret = OTA_SUCCESS;
        }
        frame_header = ota_slaver_recvd_buf[0];
        recvd_index = L_EXTRACT_U16(&ota_slaver_recvd_buf[5]);
        src_crc16 = L_EXTRACT_U16(&ota_slaver_recvd_buf[recvd_len - 2]);
        frame_crc16 = ota_get_data_crc16(ota_slaver_recvd_buf, recvd_len - 2);
        if((frame_header != 0xAB) && (src_crc16 != frame_crc16)) {
            send_buf[0] = 0xBA;
            send_buf[1] = 0x00;
            send_buf[2] = 0x09;
            send_buf[3] = 0x00;
            send_buf[4] = 0x01;
            send_buf[5] = 0x00;
            send_buf[6] = 0x02;
            src_crc16 = ota_get_data_crc16(send_buf, 7);
            send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
            send_buf[8] = (char)(src_crc16 & 0x00ff);
            ret = ota_slaver_send_data(send_buf, 9);
            if(ret < 0) {
                OTA_LOG_E("ubus send error!");
            }
        }
        else if(recvd_index == 0) {
            status = FILE_RECVD_BEGIN;
        }
        switch (status) {
            case FILE_RECVD_BEGIN:
                if(recvd_len == 47) {
                    offset = 0;
                    recvd_index = 0;
                    last_index = 0;
                    has_recvd_len = 0;
                    last_index = 0;
                    status = FILE_RECVD_DOING;
                    image_type = ota_slaver_recvd_buf[3];
                    frame_len = L_EXTRACT_U16(&ota_slaver_recvd_buf[1]);
                    image_size = L_EXTRACT_U32(&ota_slaver_recvd_buf[7]);
                    memcpy(image_ver, &ota_slaver_recvd_buf[11], sizeof(image_ver));
                    image_crc16 = L_EXTRACT_U16(&ota_slaver_recvd_buf[recvd_len - 4]);
                    send_buf[6] = 0x00;
                    OTA_LOG_E("file:%s", image_ver);
                    recvd_len = 0;
                    ota_crc16_init(&ctx);
                    if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) || (image_type == OTA_RECVD_BLE_SLAVER_IMAGE)) {
                        ret = ota_slaver_init(image_size, HAL_PARTITION_OTA_TEMP);
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            status = FILE_RECVD_BEGIN;
                            OTA_LOG_E("slaver ota init failed!");
                        }
                    }
                    else if(image_type == OTA_RECVD_SOURCE_FILE) {
                        OTA_LOG_E("image type = 0x%04X", image_type);
                        ret = 0;
                    }
                    else if(image_type == OTA_RECVD_SOURCE_PIC_FILE) {
                        ret = ota_slaver_init(image_size, HAL_PARTITION_CUSTOM_1);
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            status = FILE_RECVD_BEGIN;
                            OTA_LOG_E("slaver pic resource init failed!");
                        }
                    }
                    else if(image_type == OTA_RECVD_PINFACE) {
#if DEBUG_PINFACE
                        fptr = pinface_update_create(image_ver);
                        if(fptr == NULL) {
                            send_buf[6] = 0x01;
                            status = FILE_RECVD_BEGIN;
                            OTA_LOG_E("ota slaver creat pinface file failed");
                        }
#endif
                    }
                    else if(image_type == OTA_RCVD_XZ_PINFACE) {
                        ret = ota_slaver_init(image_size, HAL_PARTITION_OTA_TEMP);
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            status = FILE_RECVD_BEGIN;
                            OTA_LOG_E("slaver xz pinface init failed!");
                        }
                    }
                    else {
                        send_buf[6] = 0x01;
                        status = FILE_RECVD_BEGIN;
                    }
                }
                else {
                    send_buf[6] = 0x03;
                }
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0x01;
                send_buf[5] = 0x00;
                src_crc16 = ota_get_data_crc16(send_buf, 7);
                send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[8] = (char)(src_crc16 & 0x00ff);
                ret = ota_slaver_send_data(send_buf, 9);
                if(ret < 0) {
                    OTA_LOG_E("ota slaver rspons head failed");
                }
                else if(send_buf[6] == 0){
                    OTA_LOG_I("head ok\r\n");
                }
                if((ret < 0) || (send_buf[6] != 0)) {
#if DEBUG_PINFACE
                    if((image_type == OTA_RECVD_PINFACE) && (send_buf[6] = 0x00)) {
                        pinface_update_abort(fptr);
                    }
#endif
                    status = FILE_RECVD_BEGIN;
                }
                break;
            case FILE_RECVD_DOING:
                if(has_recvd_len < image_size) {
                    frame_len = L_EXTRACT_U16(&ota_slaver_recvd_buf[1]);
                    image_type = ota_slaver_recvd_buf[3];
                    send_buf[6] = 0x01;
                    if(recvd_index == last_index + 1) {
                        last_index = recvd_index;
                        send_buf[6] = 0x00;
                        if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                            (image_type == OTA_RECVD_BLE_SLAVER_IMAGE)) {
                            if(offset == 0) {
                                image_upgrade_type = ota_parse_ota_type(&ota_slaver_recvd_buf[7], frame_len - 9);
                            }
                            ret = ota_slaver_write(HAL_PARTITION_OTA_TEMP, &offset, &ota_slaver_recvd_buf[7], frame_len - 9);
                            if(ret != 0) {
                                send_buf[6] = 0x01;
                                OTA_LOG_E("slaver ota write flash failed!");
                            }
                        }
                        else if(image_type == OTA_RECVD_SOURCE_FILE) {
                            ret = 0;
                        }
                        else if(image_type == OTA_RECVD_SOURCE_PIC_FILE) {
                            ret = ota_slaver_write(HAL_PARTITION_CUSTOM_1, &offset, &ota_slaver_recvd_buf[7], frame_len - 9);
                            if(ret != 0) {
                                send_buf[6] = 0x01;
                                OTA_LOG_E("slaver pic resource write flash failed!");
                            }
                        }
                        else if(image_type == OTA_RECVD_PINFACE) {
#if DEBUG_PINFACE
                            ret = pinface_update_data(fptr, &ota_slaver_recvd_buf[7], frame_len - 9);
                            if(ret < 0) {
                                send_buf[6] = 0x01;
                                OTA_LOG_E("pinface update failed");
                            }
#endif
                        }
                        else if(image_type == OTA_RCVD_XZ_PINFACE) {
                            ret = ota_slaver_write(HAL_PARTITION_OTA_TEMP, &offset, &ota_slaver_recvd_buf[7], frame_len - 9);
                            if(ret != 0) {
                                send_buf[6] = 0x01;
                                OTA_LOG_E("slaver xz pinface write flash failed!");
                            }
                        }
                    }
                    else {
                        send_buf[6] = 0x03;
                    }
                    send_buf[0] = 0xBA;
                    send_buf[1] = 0x00;
                    send_buf[2] = 0x09;
                    send_buf[3] = 0x00;
                    send_buf[4] = 0x01;
                    send_buf[5] = 0x00;
                    src_crc16 = ota_get_data_crc16(send_buf, 7);
                    send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
                    send_buf[8] = (char)(src_crc16 & 0x00ff);
                    ret = ota_slaver_send_data(send_buf, 9);
                    if(ret < 0) {
                        OTA_LOG_E("ota slaver rspons text failed");
                    }
                    else if(send_buf[6] == 0) {
                        has_recvd_len += frame_len - 9;
                        ota_crc16_update(&ctx, &ota_slaver_recvd_buf[7], frame_len - 9);
                        if(has_recvd_len == image_size) {
                            status = FILE_RECVD_DONE;
                        }
                        break;
                    }
                    if((ret < 0) || (send_buf[6] != 0)) {
#if DEBUG_PINFACE
                        if(image_type == OTA_RECVD_PINFACE) {
                            pinface_update_abort(fptr);
                        }
#endif
                        status = FILE_RECVD_BEGIN;
                        OTA_LOG_E("ota slaver send status err!");
                    }
                }
                else {
                    OTA_LOG_E("ota slaver has recvd len err!");
                }
                break;
            case FILE_RECVD_DONE:
                frame_len = L_EXTRACT_U16(&ota_slaver_recvd_buf[1]);
                image_type = ota_slaver_recvd_buf[3];
                send_buf[6] = 0x01;
                if(ota_slaver_recvd_buf[4] == 0xF0) {
                    send_buf[6] = 0x00;
                    status = FILE_RECVD_BEGIN;
                    ota_crc16_final(&ctx, &cal_image_crc16);
                    if((image_type == OTA_RECVD_LOCAL_SLAVER_IMAGE) ||
                        (image_type == OTA_RECVD_BLE_SLAVER_IMAGE)) {
                        if(cal_image_crc16 == image_crc16) {
                            ret = ota_slaver_set_boot(image_size, image_upgrade_type, image_crc16);
                        }
                        else {
                            ret = -1;
                            OTA_LOG_E("image crc err!");
                        }
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            OTA_LOG_E("ota slaver set boot failed");
                        }
                        else {
                            aos_reboot();
                        }
                    }
                    else if(image_type == OTA_RECVD_SOURCE_FILE) {
                        ret = 0;
                    }
                    else if(image_type == OTA_RECVD_SOURCE_PIC_FILE) {
                        if(cal_image_crc16 == image_crc16) {
                            ret = 0;
                            OTA_LOG_E("pic resource download success!\r\n");
                        }
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            OTA_LOG_E("ota slaver download pic resource failed");
                        }
                    }
                    else if(image_type == OTA_RECVD_PINFACE) {
#if DEBUG_PINFACE
                        ret = pinface_update_finish(fptr);
                        if(ret != 0 ) {
                            send_buf[6] = 0x01;
                            OTA_LOG_E("pinface finish failed\r\n");
                        }
#endif
                    }
                    else if(image_type == OTA_RCVD_XZ_PINFACE) {
                        int release_len = 0;
                        release_len = xz_file_uncompress(HAL_PARTITION_OTA_SUB, HAL_PARTITION_OTA_TEMP, image_size, cal_image_crc16);
                        if(release_len > 0) {
                            ret = ota_slave_save_pinface(image_ver, HAL_PARTITION_OTA_SUB, 0x00, release_len);
                            if(ret != 0) {
                                OTA_LOG_E("pinface save failed!");
                            }
                        }
                        else {
                            ret = -1;
                            OTA_LOG_E("xz pinface decopress failed!");
                        }
                        if(ret != 0) {
                            send_buf[6] = 0x01;
                            OTA_LOG_E("download xz pinface failed!");
                        }
                    }
                }
                else {
                    send_buf[6] = 0x03;
#if DEBUG_PINFACE
                    if(image_type == OTA_RECVD_PINFACE) {
                        pinface_update_abort(fptr);
                    }
#endif
                }
                send_buf[0] = 0xBA;
                send_buf[1] = 0x00;
                send_buf[2] = 0x09;
                send_buf[3] = 0x00;
                send_buf[4] = 0xF0;
                send_buf[5] = 0x00;
                src_crc16 = ota_get_data_crc16(send_buf, 7);
                send_buf[7] = (char)((src_crc16 >> 8) & 0x00ff);
                send_buf[8] = (char)(src_crc16 & 0x00ff);
                ret = ota_slaver_send_data(send_buf, 9);
                if(ret < 0) {
                    OTA_LOG_E("ota slaver rspons text failed");
                }
                status = FILE_RECVD_BEGIN;
                break;
            default:
                OTA_LOG_E("err status");
                break;
        }
    }
}

int ota_slaver_init(unsigned int image_size, int flash_partition)
{
    int ret = -1;
    unsigned int num_pages = 0;
    unsigned int offset = 0;
    slaver_image_has_erase_page_numbs = 0;
    slaver_image_erase_sector_size = 4096;
    if((flash_partition >= HAL_PARTITION_MAX) ||
       (flash_partition < 0) || (image_size <= 0)) {
        OTA_LOG_E("slaver init para err!");
        ret = -1;
    }
    else {
        slaver_image_total_erase_page_numbs = slaver_image_flash_align_to_page(image_size, slaver_image_erase_sector_size) / slaver_image_erase_sector_size;
        if (slaver_image_total_erase_page_numbs > 0) {
            ret = hal_flash_erase(flash_partition, offset, slaver_image_erase_sector_size);
            if (ret == 0) {
                slaver_image_has_erase_page_numbs++;
            }
        }
    }
    return ret;
}

int ota_slaver_write(int flash_partition, unsigned int *off, char *in_buf ,int in_buf_len)
{
    int ret = 0;
    if((slaver_image_has_erase_page_numbs <= slaver_image_total_erase_page_numbs) &&
       (*off % slaver_image_erase_sector_size + in_buf_len >= slaver_image_erase_sector_size)) {
        if(slaver_image_has_erase_page_numbs == slaver_image_total_erase_page_numbs) {
            slaver_image_has_erase_page_numbs++;
        }
        else {
            ret = hal_flash_erase(flash_partition, *off + in_buf_len, slaver_image_erase_sector_size);
            if(ret == 0) {
                slaver_image_has_erase_page_numbs++;
            }
        }
    }
    else if(slaver_image_has_erase_page_numbs > slaver_image_total_erase_page_numbs) {
        ret = -1;
    }
    if(ret == 0) {
        ret = hal_flash_write(flash_partition, off, in_buf, in_buf_len);
    }
    return ret;
}

static ota_boot_param_t image_boot_param;
int ota_slaver_set_boot(unsigned int image_len, unsigned int ota_type, unsigned short image_crc)
{
    int ret = -1;
    unsigned int offset = 0;
    ota_boot_param_t comp_buf;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    hal_logic_partition_t *p_ota_info = &ota_info;
    hal_logic_partition_t *p_app_info = &app_info;
    memset(p_ota_info, 0, sizeof(hal_logic_partition_t));
    memset(p_app_info, 0, sizeof(hal_logic_partition_t));
    memset(&image_boot_param, 0x00, sizeof(ota_boot_param_t));
    hal_flash_info_get(HAL_PARTITION_OTA_TEMP, p_ota_info);
    hal_flash_info_get(HAL_PARTITION_APPLICATION, p_app_info);
    image_boot_param.src_adr = p_ota_info->partition_start_addr;
    ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, slaver_image_erase_sector_size);
    image_boot_param.dst_adr = p_app_info->partition_start_addr;
    image_boot_param.len = image_len;
    image_boot_param.upg_flag = ota_type;
    image_boot_param.crc = image_crc;
    image_boot_param.param_crc = ota_get_data_crc16((const unsigned char *)&image_boot_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    memset(&comp_buf, 0, sizeof(ota_boot_param_t));
    if(ret >= 0) {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, &image_boot_param, sizeof(ota_boot_param_t));
        offset = 0x00;
        if(ret >= 0) {
            ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (unsigned char*)&comp_buf, sizeof(ota_boot_param_t));
            if(ret >= 0) {
                 if(memcmp(&image_boot_param, (unsigned char*)&comp_buf, sizeof(ota_boot_param_t)) != 0) {
                     ret = OTA_UPGRADE_PARAM_FAIL;
                 }
            }
        }
    }
    return ret;
}


int ota_slaver_check_upgrade_result(unsigned char *ota_result)
{
    int ret                 = -1;
    unsigned int offset     = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    if(ota_result != NULL) {
        ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
        if(ret >= 0) {
            if((param.boot_count != 0) && (param.boot_count != 0xff)) {
                const char *sys_ver = ota_slaver_version();
                ret = 0;
                if(sys_ver != NULL) {
                    ota_result = strncmp(param.ver, sys_ver, strlen(sys_ver));
                }
            }
            else {
                ret = -1;
            }
        }
    }
    return ret;
}

int ota_slaver_rollback(void)
{
    int ret = 0;
    unsigned int offset = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        OTA_LOG_E("rollback err:%d", ret);
        return ret;
    }
    if((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ret = ota_slaver_update_parameter(&param);
    }
    if(ret != 0) {
        OTA_LOG_E("rollback err:%d", ret);
    }
    return ret;
}

const char *ota_slaver_version()
{
    return CLOUD_PIN_DEV_VER;
}

int ota_slaver_update_parameter(ota_boot_param_t *ota_param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    unsigned int offset = 0x00;
    unsigned int len = sizeof(ota_boot_param_t);
    ota_boot_param_t comp_buf;
    if(ota_param == NULL) {
        return ret;
    }
    ota_param->param_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota update param crc:0x%04x flag:0x%04x \n", ota_param->param_crc, ota_param->upg_flag);
    memset(&comp_buf, 0, len);
    ret = hal_flash_dis_secure(HAL_PARTITION_PARAMETER_1, 0, 0);
    if(ret != 0) {
        return OTA_UPGRADE_PARAM_FAIL;
    }
    ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, len);
    if(ret >= 0) {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, ota_param, len);
        offset = 0x00;
        if(ret >= 0) {
            ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (unsigned char*)&comp_buf, len);
            if(ret >= 0) {
                 if(memcmp(ota_param, (unsigned char*)&comp_buf, len) != 0) {
                     ret = OTA_UPGRADE_PARAM_FAIL;
                     printf("ble ota save param failed\r\n");
                 }
            }
        }
    }
    return 0;
}

static int ota_slave_save_pinface(unsigned char *file_name, hal_partition_t file_store_partion,
    unsigned int offset, unsigned int size)
{
    int ret = -1;
    unsigned int read_len = 0;
    unsigned int has_read_len = 0;
    void *fptr = NULL;
    unsigned char* tmp_buf = NULL;
    tmp_buf = ota_malloc(0x1000);
    if(tmp_buf != NULL) {
        memset(tmp_buf, 0x00, 0x1000);
        if(file_name != NULL) {
            fptr = pinface_update_create(file_name);
            if(fptr == NULL) {
                ret = -2;
                OTA_LOG_E("ota slaver creat pinface file failed");
            }
            else {
                OTA_LOG_E("facename = %s", file_name);
                for(has_read_len = 0; has_read_len < size; ) {
                    (size - has_read_len > 0x1000) ? (read_len = 0x1000) : (read_len = size - has_read_len);
                    ret = hal_flash_read(file_store_partion, &has_read_len, tmp_buf, read_len);
                    if(ret == 0) {
                        ret = pinface_update_data(fptr, tmp_buf, read_len);
                        if(ret < 0) {
                            pinface_update_abort(fptr);
                            OTA_LOG_E("pinface update failed");
                            break;
                        }
                    }
                    else {
                        ret = -1;
                        pinface_update_abort(fptr);
                        OTA_LOG_E("save pinface read flash failed");
                        break;
                    }
                    ota_msleep(10);
                }
                if(ret >= 0) {
                    ret = pinface_update_finish(fptr);
                    if(ret == 0 ) {
                        OTA_LOG_E("pinface finish ok\r\n");
                    }
                }
            }
        }
    }
    else {
        OTA_LOG_E("ota slave malloc failed");
        ret = -1;
    }
    if(tmp_buf != NULL) {
        ota_free(tmp_buf);
        tmp_buf = NULL;
    }
    return ret;
}