#include <aos/hal/flash.h>
#include "ota_breeze.h"
#include "ota_hal_os.h"
#include "ota_breeze_plat.h"

static unsigned int has_erase_page_numbs = 0;
static unsigned int total_erase_page_numbs = 0;
static unsigned int erase_sector_size = 0;
static int ota_part = HAL_PARTITION_OTA_TEMP;
static ota_settings_t breeze_ota_settings;

static unsigned short ota_breeze_crc16_calculate(unsigned char const* p_data, unsigned int size, unsigned short const* p_crc)
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
static unsigned int ota_breeze_crc32_calculate(unsigned char const* p_data, unsigned int size, unsigned int const* p_crc)
{
    unsigned int crc;
    crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (unsigned int i = 0; i < size; i++) {
        crc = crc ^ p_data[i];
        for (unsigned int j = 8; j > 0; j--) {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}
void ota_breeze_set_image_info_crc16(unsigned char* data, unsigned int len)
{
    breeze_ota_settings.image_info_crc16 = ota_breeze_crc16_calculate(data, len, NULL);
}
static unsigned short ota_breeze_get_image_info_crc16()
{
    return breeze_ota_settings.image_info_crc16;
}
static unsigned short ota_breeze_get_image_crc16()
{
    unsigned int   tmp_offset = 0;
    unsigned char  tmp_buf[32];
    unsigned int   read_len = 0;
    unsigned short crc = 0xFFFF;
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return crc;
    }
    crc = ota_breeze_get_image_info_crc16();
    while(tmp_offset < p_ota->valid_fw_size) {
        if(p_ota->valid_fw_size - tmp_offset > sizeof(tmp_buf)) {
            read_len = sizeof(tmp_buf);
        }
        else {
            read_len = p_ota->valid_fw_size - tmp_offset;
        }
        if(ota_breeze_read(&tmp_offset, tmp_buf, read_len) != 0) {
            break;
        }
        crc = ota_breeze_crc16_calculate(tmp_buf, read_len, &crc);
    }
    return crc;
}

static unsigned int ota_breeze_get_image_crc32()
{
    unsigned int  tmp_offset = 0;
    unsigned char tmp_buf[32];
    unsigned int  read_len = 0;
    unsigned int  crc = 0xFFFFFFFF;
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        return crc;
    }

    while(tmp_offset < p_ota->valid_fw_size) {
        if(p_ota->valid_fw_size - tmp_offset > sizeof(tmp_buf)) {
            read_len = sizeof(tmp_buf);
        }
        else {
            read_len = p_ota->valid_fw_size - tmp_offset;
        }
        if(ota_breeze_read(&tmp_offset, tmp_buf, read_len) != 0) {
            break;
        }
        crc = ota_breeze_crc32_calculate(tmp_buf, read_len, &crc);
    }
    return crc;
}

bool ota_breeze_check_if_resume(unsigned char * p_data, unsigned short length)
{
    bool ret = false;
    int i;
    if(ota_breeze_get_image_info(&breeze_ota_settings) == 0) {
        if(memcmp(breeze_ota_settings.version_store_buf, p_data, length) == 0) {
            ret = true;
        }
    }
    return ret;
}

void ota_breeze_update_fw_version(unsigned char * p_data, unsigned short length)
{
    memcpy(breeze_ota_settings.version_store_buf, p_data, length);
}


void ota_breeze_set_bin_type(unsigned char type)
{
   breeze_ota_settings.bin_type = type;
}

unsigned char ota_breeze_get_bin_type()
{
    return breeze_ota_settings.bin_type;
}

static int ota_breeze_get_parttion_id(unsigned char bin_type)
{
    int part_id = 0xff;
    switch(bin_type) {
        case OTA_BIN_TYPE_APP:
            part_id = HAL_PARTITION_MBINS_APP;
            break;
        case OTA_BIN_TYPE_KERNEL:
            part_id = HAL_PARTITION_MBINS_KERNEL;
            break;
        case OTA_BIN_TYPE_APP_KERNEL:
            part_id = HAL_PARTITION_APPLICATION;
            break;
        case OTA_BIN_TYPE_SINGLE:
            part_id = HAL_PARTITION_APPLICATION;
            break;
        default:
            break;
    }
    return part_id;
}


int ota_breeze_get_image_info(ota_settings_t* tmp_info)
{
    int ret = 0;
    unsigned int off = 0;
    unsigned int tmp_crc = 0;
    if(tmp_info == NULL) {
        return -1;
    }
    ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, (unsigned int*)&off, (void*)tmp_info, sizeof(ota_settings_t));
    if(ret == 0) {
        tmp_crc = ota_breeze_crc32_calculate((unsigned char*)tmp_info + 4, sizeof(ota_settings_t)  - 4, NULL);
        if(tmp_crc == tmp_info->settings_crc32) {
            ret = 0;
        }
        else {
            ret = -1;
        }
    }
    return ret;

}

static int ota_breeze_save_image_info(ota_settings_t* tmp_info)
{
    int ret = -1;
    unsigned int off = 0;
    if(tmp_info == NULL) {
        return -1;
    }
    tmp_info->settings_crc32 = ota_breeze_crc32_calculate((unsigned char*)tmp_info + 4, sizeof(ota_settings_t)  - 4, NULL);
    ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, 0x00, erase_sector_size);
    if(ret == 0) {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &off, (void*)tmp_info, sizeof(ota_settings_t));
    }
    return ret;
}

static unsigned int ota_breeze_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

int ota_breeze_breakpoint_process(unsigned int iamge_size, unsigned int* break_point, bool breakpoint_valid)
{
    int ret = 0;
    unsigned int num_pages = 0;
    int err_code = 0;
    if(iamge_size == 0 || break_point == NULL) {
        ret = -1;
        goto OTA_BREEZE_PROCESS_BREAKPOINT_OVER;
    }
    if (breakpoint_valid) {
        if(iamge_size == breeze_ota_settings.break_point_offset) {
            *break_point = breeze_ota_settings.break_point_offset;
        }
        else {
            *break_point = breeze_ota_settings.break_point_offset & ~(erase_sector_size - 1);
        }
    }
    else {
        breeze_ota_settings.break_point_offset = 0;
        *break_point = 0;
    }
    num_pages = ota_breeze_align_to_page(iamge_size - *break_point, erase_sector_size) / erase_sector_size;
    if (num_pages == 0) {
        ret = 0;
        goto OTA_BREEZE_PROCESS_BREAKPOINT_OVER;
    }
    else {
        has_erase_page_numbs = *break_point / erase_sector_size;
        total_erase_page_numbs = num_pages + has_erase_page_numbs;
        err_code = hal_flash_erase(ota_part, *break_point, erase_sector_size);
        if (err_code != 0) {
            printf(" f-erase failed\r\n");
            ret = -1;
            goto OTA_BREEZE_PROCESS_BREAKPOINT_OVER;
        }
        has_erase_page_numbs++;
    }
OTA_BREEZE_PROCESS_BREAKPOINT_OVER:

    return ret;
}

int ota_breeze_save_breakpoint(unsigned int break_point)
{
    breeze_ota_settings.break_point_offset = break_point;
    return ota_breeze_save_image_info(&breeze_ota_settings);
}

int ota_breeze_write(unsigned int* off, char* in_buf ,int in_buf_len)
{
    int ret = 0;
    if((has_erase_page_numbs <= total_erase_page_numbs) && (*off % erase_sector_size + in_buf_len >= erase_sector_size)) {
        ret = hal_flash_erase(ota_part, *off + in_buf_len, erase_sector_size);
        if(ret == 0) {
            has_erase_page_numbs++;
        }
    }
    if(ret == 0) {
        ret = hal_flash_write(ota_part, off, in_buf, in_buf_len);
    }
    return ret;
}

int ota_breeze_read(unsigned int* off, char* out_buf, int out_buf_len)
{
    return hal_flash_read(ota_part, (unsigned int*)off, out_buf, out_buf_len);
}


int ota_breeze_set_boot()
{
    int ret = 0;
    unsigned short crc = 0;
    unsigned int img_crc = 0;
    unsigned int tmp_offset = 0;
    int dest_part = 0;
    hal_logic_partition_t *dest_part_info = NULL;
    hal_logic_partition_t *ota_part_info = hal_flash_get_info(ota_part);
    dest_part = ota_breeze_get_parttion_id(ota_breeze_get_bin_type());
    if(dest_part == 0xff) {
        ret = -1;
        goto OTA_BREEZE_SET_BOOT_OVER;
    }
    dest_part_info = hal_flash_get_info(dest_part);
    _ota_ble_global_dat_t* p_ota = ota_breeze_get_global_data_center();
    if(p_ota == NULL) {
        ret = -1;
        goto OTA_BREEZE_SET_BOOT_OVER;
    }
    crc = ota_breeze_get_image_crc16();
    if (crc == p_ota->crc) {
        printf("crc16 ok\r\n");
        img_crc = ota_breeze_get_image_crc32();
        breeze_ota_settings.ota_flag = OTA_ONLY_UPGRADE_FW;
        breeze_ota_settings.backup_dest_addr = breeze_ota_settings.dest_addr;
        breeze_ota_settings.backup_store_addr = breeze_ota_settings.src_addr;
        breeze_ota_settings.backup_image_size = breeze_ota_settings.image_size;
        breeze_ota_settings.backup_image_crc32 = breeze_ota_settings.image_crc32;
        breeze_ota_settings.backup_has_copy_len = 0;

        breeze_ota_settings.image_size = p_ota->valid_fw_size;
        breeze_ota_settings.image_crc32  = img_crc;
        breeze_ota_settings.dest_addr  = dest_part_info->partition_start_addr;
        breeze_ota_settings.src_addr = ota_part_info->partition_start_addr;
        breeze_ota_settings.image_has_copy_len = 0;
        breeze_ota_settings.settings_crc32 = ota_breeze_crc32_calculate((unsigned char*)&breeze_ota_settings + 4, sizeof(breeze_ota_settings)  - 4, NULL);
        ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, 0x00, erase_sector_size);
        if(ret == 0) {
            ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &tmp_offset, &breeze_ota_settings, sizeof(breeze_ota_settings));
        }
    }
    else {
        printf("crc16 error!\r\n");
        breeze_ota_settings.break_point_offset = 0;
        breeze_ota_settings.image_info_crc16 = 0;
        breeze_ota_settings.bin_type = 0xff;
        memset(breeze_ota_settings.version_store_buf, 0x00, sizeof(breeze_ota_settings.version_store_buf));
        ota_breeze_save_image_info(&breeze_ota_settings);
        ret = -1;
    }
OTA_BREEZE_SET_BOOT_OVER:
    return ret;
}

int ota_breeze_rollback()
{
    unsigned int tmp_crc = 0;
    unsigned int tmp_offset = 0;
    int ret = -1;
    erase_sector_size = hal_flash_erase_sector_size();
    memset(&breeze_ota_settings, 0x00, sizeof(ota_settings_t));
    if(hal_flash_read(HAL_PARTITION_PARAMETER_1, &tmp_offset, &breeze_ota_settings, sizeof(ota_settings_t)) != 0) {
        ret = -1;
        goto OTA_BREEZE_ROLLBACK_OVER;
    }
    tmp_crc = ota_breeze_crc32_calculate((unsigned char*)&breeze_ota_settings + 4, sizeof(breeze_ota_settings)  - 4, NULL);
    if(breeze_ota_settings.settings_crc32 == tmp_crc) {
        if(breeze_ota_settings.ota_flag == OTA_UPGRADE_FW_SUCC) {
            breeze_ota_settings.ota_flag = OTA_UPGRADE_FINISH;
            breeze_ota_settings.break_point_offset = 0;
            breeze_ota_settings.image_info_crc16 = 0;
            breeze_ota_settings.bin_type = 0xff;
            memset(breeze_ota_settings.version_store_buf, 0x00, sizeof(breeze_ota_settings.version_store_buf));
            breeze_ota_settings.settings_crc32 = ota_breeze_crc32_calculate((unsigned char*)&breeze_ota_settings + 4, sizeof(breeze_ota_settings)  - 4, NULL);
            ret = hal_flash_erase(HAL_PARTITION_PARAMETER_1, 0x00, erase_sector_size);
            if(ret == 0) {
                ret = hal_flash_write(HAL_PARTITION_PARAMETER_1, &tmp_offset, &breeze_ota_settings, sizeof(breeze_ota_settings));
            }
        }
    }
OTA_BREEZE_ROLLBACK_OVER:
    return ret;
}

