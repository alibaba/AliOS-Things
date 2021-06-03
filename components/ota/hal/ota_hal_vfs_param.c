#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <aos/vfs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "aos/hal/flash.h"
#if defined(BOARD_HAAS200)
#include "aos/mtd.h"
#endif
#include <vfsdev/flash_dev.h>

int ota_is_download_mode(void)
{
    int ret = 0;
    ota_boot_param_t param = {0};
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = ota_read_parameter(&param);
    if(ret < 0) {
         return 0;
    }
    if((param.upg_status == OTA_TRANSPORT) && (param.upg_flag != 0xffff)){
         OTA_LOG_I("OTA running status :0x%04x \n",param.upg_status);
         return 1;
    }
    OTA_LOG_I("ota is status:%d  param crc:0x%04x \n",param.upg_status, param.param_crc);
    return ret;
}

int ota_read_parameter(ota_boot_param_t *ota_param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    unsigned short patch_crc = 0;
    int temp_fd = -1;
    char dev_str[16] = {0};

    if(ota_param == NULL) {
        goto OTA_READ_PARAM_OVER;
    }
    #if defined(BOARD_HAAS200)
    snprintf(dev_str, 15, "/dev/mtdblock%d", MTD_PART_ID_ENV);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", HAL_PARTITION_PARAMETER_1);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota param1 partition failed");
        goto OTA_READ_PARAM_OVER;
    }
    memset(ota_param, 0, sizeof(ota_boot_param_t));
    ret = read(temp_fd, (void*)ota_param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        OTA_LOG_E("read failed:%d", ret);
        goto OTA_READ_PARAM_OVER;
    }
    patch_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota param crc:0x%04x cal:0x%04x \n", ota_param->param_crc, patch_crc);
    if(patch_crc != ota_param->param_crc) {
        OTA_LOG_E("ota param crc err");
        ret = OTA_UPGRADE_PARAM_FAIL;
    }
OTA_READ_PARAM_OVER:
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

int ota_update_parameter(ota_boot_param_t *ota_param)
{
    int ret = OTA_UPGRADE_PARAM_FAIL;
    int temp_fd = -1;
    char dev_str[16] = {0};
    ota_boot_param_t comp_buf;
    unsigned int offset = 0x00;
    unsigned int len = sizeof(ota_boot_param_t);
    #if defined(BOARD_HAAS200)
    struct mtd_erase_info erase_info = {0};
    #endif
    if(ota_param == NULL) {
        goto OTA_UPGRADE_PARAN_OVER;
    }
    ota_param->param_crc = ota_get_data_crc16((const unsigned char *)ota_param, sizeof(ota_boot_param_t) - sizeof(unsigned short));
    OTA_LOG_I("ota update param crc:0x%04x flag:0x%04x boot_type:%d len = %d\n", ota_param->param_crc, ota_param->upg_flag, ota_param->boot_type, len);
    memset(&comp_buf, 0, len);
    #if defined(BOARD_HAAS200)
    snprintf(dev_str, 15, "/dev/mtdblock%d", MTD_PART_ID_ENV);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", HAL_PARTITION_PARAMETER_1);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota param1 partition failed");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    #if defined(BOARD_HAAS200)
    // do nothing
    #else
    ret = ioctl(temp_fd, IOC_FLASH_ENABLE_SECURE, len);
    if(ret < 0) {
        OTA_LOG_E("flash secure failed!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    #endif
    ret = lseek(temp_fd, (off_t)offset, SEEK_SET);
    if(ret < 0) {
        OTA_LOG_E("lseek fail!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    #if defined(BOARD_HAAS200)
    erase_info.offset = 0;
    erase_info.length = len;
    ret = ioctl(temp_fd, IOC_MTD_ERASE, &erase_info);
    #else
    ret = ioctl(temp_fd, IOC_FLASH_ERASE_FLASH, len);
    #endif
    if(ret < 0) {
        OTA_LOG_E("erase fail!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    offset = 0;
    ret = lseek(temp_fd, (off_t)offset, SEEK_SET);
    if(ret < 0) {
        OTA_LOG_E("lseek fail!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    ret = write(temp_fd, (void*)ota_param, len);
    if(ret < 0) {
        OTA_LOG_E("write fail!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    offset = 0x00;
    ret = lseek(temp_fd, (off_t)offset, SEEK_SET);
    if(ret < 0) {
        OTA_LOG_E("lseek fail!");
        goto OTA_UPGRADE_PARAN_OVER;
    }
    ret = read(temp_fd, (void*)&comp_buf, len);
    if(ret < 0) {
        OTA_LOG_E("read failed:%d", ret);
        goto OTA_UPGRADE_PARAN_OVER;
    }
    if(memcmp(ota_param, (unsigned char*)&comp_buf, len) != 0) {
        ret = OTA_UPGRADE_PARAM_FAIL;
        OTA_LOG_E("ota save param failed\r");
    }
OTA_UPGRADE_PARAN_OVER:
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    OTA_LOG_I("ota update param over\n");
    return 0;
}

unsigned short ota_get_upgrade_flag(void)
{
    int ret = 0;
    unsigned short flag = 0;
    ota_boot_param_t ota_param = {0};
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        return 0xffff;
    }
    flag = ota_param.upg_flag;
    return flag;
}

int ota_update_upg_flag(unsigned short flag)
{
    int ret = 0;
    ota_boot_param_t ota_param = {0};
    OTA_LOG_I("upg_flag:0x%x \n", flag);
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        return ret;
    }
    ota_param.upg_flag = flag;
    ret = ota_update_parameter(&ota_param);
    return 0;
}

int ota_clear_paramters()
{
    int ret = 0;
    int temp_fd = -1;
    char dev_str[16] = {0};
    unsigned int offset = 0x00;
    #if defined(BOARD_HAAS200)
    struct mtd_erase_info erase_info = {0};
    #endif
    #if defined(BOARD_HAAS200)
    snprintf(dev_str, 15, "/dev/mtdblock%d", MTD_PART_ID_ENV);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", HAL_PARTITION_PARAMETER_1);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota param1 partition failed");
        goto OTA_CLEAR_PARAM_OVER;
    }
    ret = lseek(temp_fd, (off_t)offset, SEEK_SET);
    if(ret < 0) {
        OTA_LOG_E("lseek fail! ");
        goto OTA_CLEAR_PARAM_OVER;
    }
    #if defined(BOARD_HAAS200)
    erase_info.offset = 0;
    erase_info.length = sizeof(ota_boot_param_t);
    ret = ioctl(temp_fd, IOC_MTD_ERASE , &erase_info);
    #else
    ret = ioctl(temp_fd, IOC_FLASH_ERASE_FLASH, sizeof(ota_boot_param_t));
    #endif
    if(ret < 0) {
        OTA_LOG_E("erase fail! ");
        goto OTA_CLEAR_PARAM_OVER;
    }
    OTA_LOG_I("ota clear ret = %d\r\n", ret);
OTA_CLEAR_PARAM_OVER:
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

unsigned int ota_parse_ota_type(unsigned char *buf, unsigned int len)
{
    int upg_flag = 0;
    unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    if((buf != NULL) && (len >= 8)) {
        if (memcmp(buf, "BSDIFF40", 8) == 0) {
            upg_flag = OTA_UPGRADE_DIFF;
        }
        else if (memcmp(buf, xz_header, 6) == 0) {
            upg_flag = OTA_UPGRADE_XZ;
        }
        else {
            upg_flag = OTA_UPGRADE_ALL;
        }
    }
    OTA_LOG_I("ota header:0x%x", upg_flag);
    return upg_flag;
}
