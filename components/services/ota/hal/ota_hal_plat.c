#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "ota_log.h"
#include "ota_hal.h"
#include <aos/vfs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ota_import.h"
#include "ota_hal_os.h"
#include "aos/kernel.h"
#include "aos/hal/flash.h"
#include <hal2vfs/io_flash.h>
#if defined OTA_CONFIG_MCU_CAN
#include "canopen/canopen.h"
#include "canopen/canopen_od.h"
#endif

unsigned char *ota_cache           = NULL;
unsigned int ota_cache_len         = 0;
unsigned int ota_fw_size           = 0;
unsigned int ota_receive_total_len = 0;
static int boot_part               = HAL_PARTITION_OTA_TEMP;
static ota_crc16_ctx  ctx          = {0};
static int ota_fd                  = -1;

#if defined OTA_CONFIG_BLE
static unsigned int   has_erase_page_numbs = 0;
static unsigned int   total_erase_page_numbs = 0;
extern unsigned int hal_flash_erase_sector_size(void);

unsigned int ota_ble_align_to_page(unsigned int val, unsigned int page_size)
{
    return ((val + page_size - 1) & ~(page_size - 1));
}

int ota_ble_breakpoint_process(unsigned int image_size, unsigned int *break_point)
{
    int ret = 0;
    unsigned int num_pages = 0;
    unsigned int erase_sector_size = 0;
    if(break_point == NULL) {
        return -1;
    }
    erase_sector_size = hal_flash_erase_sector_size();
    if(erase_sector_size == 0) {
        return -1;
    }
    if(image_size != *break_point) {
        *break_point = *break_point & ~(erase_sector_size - 1);
    }
    num_pages = ota_ble_align_to_page(image_size - *break_point, erase_sector_size) / erase_sector_size;
    if (num_pages > 0) {
        has_erase_page_numbs   = *break_point / erase_sector_size;
        total_erase_page_numbs = num_pages + has_erase_page_numbs;
        ret = hal_flash_erase(boot_part, *break_point, erase_sector_size);
        if (ret < 0) {
            OTA_LOG_E(" f-erase failed");
            ret = -1;
        }
        else {
            has_erase_page_numbs++;
        }
    }
    return ret;
}

OTA_WEAK int ota_ble_write(unsigned int *off, char *in_buf, int in_buf_len)
{
    int ret = -1;
    unsigned int erase_sector_size = 0;
    erase_sector_size = hal_flash_erase_sector_size();
    if(erase_sector_size != 0) {
        ret = 0;
        if((has_erase_page_numbs <= total_erase_page_numbs) &&
           (*off % erase_sector_size + in_buf_len >= erase_sector_size)) {
            if(has_erase_page_numbs < total_erase_page_numbs) {
                ret = hal_flash_erase(boot_part, *off + in_buf_len, erase_sector_size);
            }
            if(ret == 0) {
                has_erase_page_numbs++;
            }
        }
        else if(has_erase_page_numbs > total_erase_page_numbs) {
            ret = -1;
        }
        if(ret == 0) {
            ret = hal_flash_write(boot_part, off, in_buf, in_buf_len);
        }
    }
    return ret;
}

OTA_WEAK int ota_ble_check_upgrade_result(int *ota_result)
{
    int ret                 = -1;
    unsigned int offset     = 0;
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    ret = hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &param, sizeof(ota_boot_param_t));
    if(ret >= 0) {
        ret = -1;
        if((param.boot_count != 0) && (param.boot_count != 0xff)) {
            const char *sys_ver = ota_hal_version(0, NULL);
            ret = 0;
            OTA_LOG_I("ble has ota");
            if(sys_ver != NULL) {
                *ota_result = strncmp(param.ver, sys_ver, strlen(sys_ver));
            }
        }
        else {
            OTA_LOG_I("ble isn't ota");
        }
    }
    return ret;
}

OTA_WEAK int ota_ble_mcu_upgrade(unsigned int partition_offset, unsigned char file_type, unsigned int size,
                         char *ver, unsigned short file_crc16)
{
#if defined OTA_CONFIG_MCU_UBUS
    return ota_mcu_ubus_upgrade(partition_offset, file_type, size, ver, file_crc16);
#else
   return 0;
#endif
}
#endif

OTA_WEAK int ota_hal_init(ota_boot_param_t *param)
{
    int ret = OTA_INIT_FAIL;
    unsigned int len = 0;
    unsigned int off = 0;
    unsigned int block_size = 0;
    char dev_str[16];
    hal_logic_partition_t part_info;
    if(param == NULL) {
        goto OTA_HAL_INIT_EXIT;
    }
    snprintf(dev_str, 15, "/dev/flash%d", boot_part);
    ota_fd = open(dev_str, 0);
    if (ota_fd < 0) {
        ota_fd = -1;
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_HAL_INIT_EXIT;
    }
    memset(&part_info, 0x00, sizeof(hal_logic_partition_t));
    ret = ioctl(ota_fd, IOC_FLASH_INFO_GET, (unsigned long)&part_info); 
    if(ret < 0) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("ioctl failed!");
        goto OTA_HAL_INIT_EXIT;
    }
    if((param->len == 0) || (param->len > part_info.partition_length)) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("bin file size err!");
        goto OTA_HAL_INIT_EXIT;
    }
    ota_receive_total_len = 0;
    ota_fw_size = param->len;
    ota_cache = ota_malloc(OTA_FLASH_WRITE_CACHE_SIZE);
    if (NULL == ota_cache) {
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }
    len = param->len;
    while(len > 0) {
        block_size = (len > OTA_FLASH_BLOCK_SIZE) ? OTA_FLASH_BLOCK_SIZE: len;
        ret = lseek(ota_fd, (off_t)off, SEEK_SET);
        if(ret < 0) {
            ret = OTA_INIT_FAIL;
            OTA_LOG_E("lseek fail! ");
            goto OTA_HAL_INIT_EXIT;
        }
        ret = ioctl(ota_fd, IOC_FLASH_ERASE_FLASH, block_size);
        if(ret < 0) {
            ret = OTA_INIT_FAIL;
            OTA_LOG_E("erase fail! ");
            goto OTA_HAL_INIT_EXIT;
        }
        off += block_size;
        len -= block_size;
        ota_msleep(10);
    }
    ota_crc16_init(&ctx);
    OTA_LOG_I("ota init part:%d len:%d \n", boot_part, param->len);

OTA_HAL_INIT_EXIT:
    if(ret != 0) {
        if(NULL != ota_cache) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if(ota_fd >= 0) {
            close(ota_fd);
            ota_fd = -1;
        }
        OTA_LOG_E("ota init fail!");
    }
    return ret;
}

OTA_WEAK int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    int tocopy = 0;

    if(off == NULL || in_buf_len > OTA_FLASH_WRITE_CACHE_SIZE) {
        return OTA_UPGRADE_WRITE_FAIL;
    }
    if (in_buf_len <= OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len) {
        tocopy = in_buf_len;
    } else {
        tocopy = OTA_FLASH_WRITE_CACHE_SIZE - ota_cache_len;
    }
    /*Start from last byte of remaing data*/
    memcpy(ota_cache + ota_cache_len, in_buf, tocopy);
    ota_cache_len += tocopy;
    if (ota_cache_len == OTA_FLASH_WRITE_CACHE_SIZE) {
        ret = lseek(ota_fd, (off_t)*off, SEEK_SET);
        if(ret < 0) {
            goto EXIT;
        }
        ret = write(ota_fd, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        if(ret < 0) {
            goto EXIT;
        }
        *off += OTA_FLASH_WRITE_CACHE_SIZE;
        ota_cache_len = 0;
        ota_crc16_update(&ctx, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
    }
    /*keep remaining data*/
    if (in_buf_len - tocopy > 0) {
        /*Now ota_cache only contains remaing data*/
        memcpy(ota_cache, in_buf + tocopy, in_buf_len - tocopy);
        ota_cache_len = in_buf_len - tocopy;
    }
    ota_receive_total_len += in_buf_len;
    if(ota_receive_total_len == ota_fw_size) {
        if (ota_cache_len != 0) {
            ret = lseek(ota_fd, (off_t)*off, SEEK_SET);
            if(ret < 0) {
                goto EXIT;
            }
            ret = write(ota_fd, ota_cache, ota_cache_len);
            if(ret < 0) {
                goto EXIT;
            }
            *off += ota_cache_len;
            ota_crc16_update(&ctx, ota_cache, ota_cache_len);
        }
        if(ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if(ota_fd >= 0) {
            close(ota_fd);
            ota_fd = -1;
        }
    }
EXIT:
    if(ret < 0) {
        if(ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if(ota_fd >= 0) {
            close(ota_fd);
            ota_fd = -1;
        }
        OTA_LOG_E("ota_write err:%d", ret);
    }
    return ret;
}

OTA_WEAK int ota_hal_rollback(void)
{
    int ret = 0;
    int temp_fd = -1;
    char dev_str[16];
    ota_boot_param_t param;
    memset(&param, 0, sizeof(ota_boot_param_t));
    snprintf(dev_str, 15, "/dev/flash%d", HAL_PARTITION_PARAMETER_1);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_ROLLBACK;
    }
    ret = read(temp_fd, (void*)&param, sizeof(ota_boot_param_t));
    if(ret < 0) {
        OTA_LOG_E("rollback err:%d", ret);
        goto OTA_ROLLBACK;
    }
    if((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ret = ota_update_parameter(&param);
        if(ret < 0) {
            OTA_LOG_E("rollback err:%d", ret);
            goto OTA_ROLLBACK;
        }
        ret = ioctl(temp_fd, IOC_FLASH_CLEAR_BOOT_COUNT, (unsigned long)0x00);
        if(ret < 0) {
            OTA_LOG_E("clear user boot count fail!");
            goto OTA_ROLLBACK;
        }
    }
 OTA_ROLLBACK:
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    if(ret != 0) {
        OTA_LOG_E("rollback err:%d", ret);
    }
    return ret;
}

OTA_WEAK int ota_hal_reboot_bank(void)
{
    return 0;
}

OTA_WEAK void ota_hal_image_crc16(unsigned short *outResult )
{
    ota_crc16_final(&ctx, outResult);
}

OTA_WEAK int ota_hal_boot_type()
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    int part_id = HAL_PARTITION_PARAMETER_1;
    snprintf(dev_str, 15, "/dev/flash%d", part_id);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open partition %d failed", part_id);
    }
    else {
        ret = ioctl(temp_fd, IOC_FLASH_GET_BOOT_TYPE, (unsigned long)0x00); 
        if(ret < 0) {
            OTA_LOG_E("get boot type ioctl failed!");
        }
    }
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

OTA_WEAK unsigned char ota_hal_set_user_bootinfo(void *param)
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    int part_id = HAL_PARTITION_PARAMETER_1;
    snprintf(dev_str, 15, "/dev/flash%d", part_id);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open partition %d failed", part_id);
    }
    else {
        ret = ioctl(temp_fd, IOC_FLASH_SET_BOOT_INFO, (unsigned long)param);
        if(ret < 0) {
            OTA_LOG_E("set user boot ioctl failed!");
        }
    }
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

int ota_get_partition_info(int part_id, hal_logic_partition_t *tmp_info)
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    snprintf(dev_str, 15, "/dev/flash%d", part_id);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota temp partition failed");
    }
    else {
        ret = ioctl(temp_fd, IOC_FLASH_INFO_GET, (unsigned long)tmp_info); 
        if(ret < 0) {
            OTA_LOG_E("ioctl failed!");
        }
    }
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

OTA_WEAK int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    int des_part = HAL_PARTITION_APPLICATION;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    memset(&ota_info, 0, sizeof(hal_logic_partition_t));
    memset(&app_info, 0, sizeof(hal_logic_partition_t));
    if(param == NULL) {
        OTA_LOG_E("hal boot input param err!");
        goto OTA_HAL_BOOT_OVER;
    }
    ret = ota_get_partition_info(boot_part, &ota_info);
    if(ret < 0) {
        OTA_LOG_E("get ota partiton info err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if(param->upg_flag == OTA_UPGRADE_FS) {
        des_part = HAL_PARTITION_LITTLEFS;
    }
    ret = ota_get_partition_info(des_part, &app_info);
    if(ret < 0) {
        OTA_LOG_E("get app partiton info err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if(param->crc == 0 || param->crc == 0xffff) {
        OTA_LOG_I("calculate image crc");
        ota_hal_image_crc16(&param->crc);
    }
    param->src_adr = ota_info.partition_start_addr;
    if(param->upg_flag == OTA_UPGRADE_FS) {
        ota_image_info_t tmp_info;
        ret = ota_get_image_info(param->len, &tmp_info);
        if(ret < 0) {
            OTA_LOG_E("get image info err!");
            goto OTA_HAL_BOOT_OVER;
        }
        param->len = tmp_info.image_size;
        param->crc = tmp_info.image_crc16;
    }
    param->dst_adr = app_info.partition_start_addr;
    param->old_size = app_info.partition_length;
    if(param->upg_flag == OTA_UPGRADE_ALL) {
        int tmp_type = ota_hal_boot_type();
        if(tmp_type < 0) {
            OTA_LOG_E("get platform boot type err!");
            goto OTA_HAL_BOOT_OVER;
        }
        param->boot_type = (unsigned char)tmp_type;
    }
    else {
        param->boot_type = 0;
    }
    ret = ota_update_parameter(param);
    if(ret < 0) {
        OTA_LOG_E("update param err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if(param->upg_flag == OTA_UPGRADE_ALL) {
        ret = ota_hal_set_user_bootinfo(param);
        if(ret < 0) {
            OTA_LOG_E("clear user boot count fail!");
            goto OTA_HAL_BOOT_OVER;
        }
    }
    OTA_LOG_I("OTA after finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
OTA_HAL_BOOT_OVER:
    if(ret < 0) {
        OTA_LOG_E("set boot info failed!");
    }
    return ret;
}

OTA_WEAK int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    snprintf(dev_str, 15, "/dev/flash%d", boot_part);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_HAL_READ;
    }
    ret = lseek(temp_fd, (off_t)*off, SEEK_SET);
    if(ret < 0) {
        OTA_LOG_E("read lseek failed:%d", ret);
        goto OTA_HAL_READ;
    }
    ret = read(temp_fd, (void*)out_buf, out_buf_len);
    if(ret < 0) {
        OTA_LOG_E("read failed:%d", ret);
        goto OTA_HAL_READ;
    }

OTA_HAL_READ:
    if(temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

#if 0
OTA_WEAK int ota_get_fs_version(char *ver_buf, int ver_buf_len)
{
    int ret = -1;
    FILE *fptr = NULL;
    char read_buf[128];
    int read_len = 0;
    char verfile_name[] = "/data/fs_ver.txt";
    if((ver_buf == NULL) || (ver_buf_len < 32)) {
        OTA_LOG_E("fs ver input param err!");
        goto OTA_GET_FS_VER_OVER;
    }
    fptr = ota_fopen(verfile_name, "rb");
    if(fptr == NULL) {
        OTA_LOG_I("can't find fs_ver.txt file");
        goto OTA_GET_FS_VER_OVER;
    }
    memset(read_buf, 0x00, sizeof(read_buf));
    read_len = ota_fread(read_buf, 1, sizeof(read_buf), fptr);
    if(read_len < ver_buf_len) {
        ret = 0;
        memcpy(ver_buf, read_buf, read_len);
    }
    else {
        OTA_LOG_E("fs version too long!");
    }
OTA_GET_FS_VER_OVER:
    if(fptr != NULL) {
        ota_fclose(fptr);
    }
    return ret;
}
#else
OTA_WEAK int ota_get_fs_version(char *ver_buf, int ver_buf_len)
{
    int ret = -1;
    int fd = -1;
    char read_buf[128];
    int read_len = 0;
    char verfile_name[] = "/data/fs_ver.txt";
    if((ver_buf == NULL) || (ver_buf_len < 32)) {
        OTA_LOG_E("fs ver input param err!");
        goto OTA_GET_FS_VER_OVER;
    }
    fd = open(verfile_name, O_RDONLY, 0666);
    if(fd < 0) {
        OTA_LOG_I("can't find fs_ver.txt file");
        goto OTA_GET_FS_VER_OVER;
    }
    memset(read_buf, 0x00, sizeof(read_buf));
    read_len = read(fd, read_buf, sizeof(read_buf));
    OTA_LOG_I("read_len = %d", read_len);
    if((read_len > 0) && (read_len < ver_buf_len)) {
        ret = 0;
        memcpy(ver_buf, read_buf, read_len);
    }
    else {
        OTA_LOG_E("fs version too long!");
    }
OTA_GET_FS_VER_OVER:
    if(fd >= 0) {
        close(fd);
    }
    return ret;
}
#endif

#define DEFAULT_ERR_VER   "0.0.0"
#define SUBDEV_VER        "subdev-1.0.0"
static char tmp_version[65];
OTA_WEAK const char *ota_hal_version(unsigned char dev_type, char* dn)
{
    char *ver_result = NULL;
    if(dev_type == 1) {//subdev version
        strncpy(tmp_version, SUBDEV_VER, sizeof(tmp_version));
    }
    else {
        memset(tmp_version, 0x00, sizeof(tmp_version));
        if(dn == NULL) {
            ver_result = aos_version_str_get(tmp_version, sizeof(tmp_version));
            if(ver_result == NULL) {
                OTA_LOG_E("ota get kernel version failed\r\n");
                strncpy(tmp_version, DEFAULT_ERR_VER, sizeof(tmp_version));
            }
        }
        else {
            if(ota_get_fs_version(tmp_version, sizeof(tmp_version)) < 0) {
                strncpy(tmp_version, DEFAULT_ERR_VER, sizeof(tmp_version));
            }
        }
    }
    tmp_version[sizeof(tmp_version) - 1] = 0;
    return tmp_version;
}

OTA_WEAK int ota_mcu_hal_init(void)
{
    int ret = 0;
    return ret;
}

OTA_WEAK int ota_mcu_hal_read(unsigned int *off, unsigned char *buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;   
    return ret;
}

OTA_WEAK int ota_mcu_hal_write(unsigned int* off, unsigned char* buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;   
    return ret;
}

OTA_WEAK int ota_mcu_hal_version(void *device, char *version)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    if(version == NULL) {
        OTA_LOG_E("mcu version err.");
        return ret;
    }
#if defined OTA_CONFIG_MCU_CAN
    ret = co_sdo_read_od(device, CO_SW_VERSION_INDEX, CO_SW_VERSION_SUB_INDEX, version, OTA_VER_LEN);
    OTA_LOG_I("MCU version get:%s ret:%d ", version, ret);
#endif
    return ret;
}

OTA_WEAK int ota_mcu_hal_recv(void *device, unsigned char *buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    return ret;
}

OTA_WEAK int ota_mcu_hal_send(void *device, unsigned char* buf, unsigned int len)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    return ret;
}

OTA_WEAK int ota_mcu_upgrade_start(unsigned int size, char* ver, char* md5)
{
    int ret = OTA_MCU_UPGRADE_FAIL;
    return ret;
}
