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
#include <vfsdev/flash_dev.h>
#if defined(BOARD_HAAS200)
#include "aos/mtdpart.h"
#include "aos/mtd.h"
#endif
unsigned char *ota_cache           = NULL;
unsigned int ota_cache_len         = 0;
unsigned int ota_fw_size           = 0;
unsigned int ota_receive_total_len = 0;
#if defined(BOARD_HAAS200)
static int boot_part               = MTD_PART_ID_APP2;
#else
static int boot_part               = HAL_PARTITION_OTA_TEMP;
#endif
static ota_crc16_ctx  ctx          = {0};
static int ota_fd                  = -1;

#if defined(BOARD_HAAS200)
unsigned int ota_get_cur_index(void);
#endif
OTA_WEAK int ota_hal_init(ota_boot_param_t *param)
{
    int ret = OTA_INIT_FAIL;
    unsigned int len = 0;
    unsigned int off = 0;
    unsigned int block_size = 0;
    char dev_str[16] = {0};
    #if defined(BOARD_HAAS200)
    // nothing
    #else
    hal_logic_partition_t part_info;
    #endif
    #if defined(BOARD_HAAS200)
    struct mtd_erase_info erase_info = {0};
    int partition_id =boot_part;
    mtd_partition_t *tmp_part_info;
    #endif
    if (param == NULL) {
        goto OTA_HAL_INIT_EXIT;
    }
    #if defined(BOARD_HAAS200)
    #if defined(HAAS200_AB_OTA)
    if(ota_get_cur_index()==0)
        partition_id = MTD_PART_ID_APP2;
    else
        partition_id = MTD_PART_ID_APP;
    #elif defined(HAAS200_XZ_OTA)
        // do nothing
    #else
        #error "Please define Haas200 update mode!!"
    #endif
    OTA_LOG_I("update partition %d\r\n", partition_id);
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", boot_part);
    #endif
    ota_fd = open(dev_str, 0);
    if (ota_fd < 0) {
        ota_fd = -1;
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_HAL_INIT_EXIT;
    }
    #if defined(BOARD_HAAS200)
    extern const mtd_partition_t mtd_partitions[];
    tmp_part_info = &mtd_partitions[partition_id];
    if ((param->len == 0) || (param->len > tmp_part_info->partition_length)) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("bin file size err!");
        goto OTA_HAL_INIT_EXIT;
    }
    #else
    memset(&part_info, 0x00, sizeof(hal_logic_partition_t));
    ret = ioctl(ota_fd, IOC_FLASH_INFO_GET, (unsigned long)&part_info);
    if (ret < 0) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("ioctl failed!");
        goto OTA_HAL_INIT_EXIT;
    }
    if ((param->len == 0) || (param->len > part_info.partition_length)) {
        ret = OTA_INIT_FAIL;
        OTA_LOG_E("bin file size err!");
        goto OTA_HAL_INIT_EXIT;
    }
    #endif
    ota_receive_total_len = 0;
    ota_fw_size = param->len;
    ota_cache = ota_malloc(OTA_FLASH_WRITE_CACHE_SIZE);
    if (NULL == ota_cache) {
        ret = OTA_INIT_FAIL;
        goto OTA_HAL_INIT_EXIT;
    }
    len = param->len;
    while (len > 0) {
        block_size = (len > OTA_FLASH_BLOCK_SIZE) ? OTA_FLASH_BLOCK_SIZE : len;
        ret = lseek(ota_fd, (off_t)off, SEEK_SET);
        if (ret < 0) {
            ret = OTA_INIT_FAIL;
            OTA_LOG_E("lseek fail! ");
            goto OTA_HAL_INIT_EXIT;
        }
        #if defined(BOARD_HAAS200)
        erase_info.offset = off;
        erase_info.length = block_size;
        ret = ioctl(ota_fd, IOC_MTD_ERASE, &erase_info);
        #else
        ret = ioctl(ota_fd, IOC_FLASH_ERASE_FLASH, block_size);
        #endif
        if (ret < 0) {
            ret = OTA_INIT_FAIL;
            OTA_LOG_E("erase fail! ");
            goto OTA_HAL_INIT_EXIT;
        }
        off += block_size;
        len -= block_size;
        ota_msleep(10);
    }
    ota_crc16_init(&ctx);
    OTA_LOG_I("ota init part:%d len:%d\n", boot_part, param->len);
OTA_HAL_INIT_EXIT:
    if (ret != 0) {
        if (NULL != ota_cache) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (ota_fd >= 0) {
            close(ota_fd);
            ota_fd = -1;
        }
        OTA_LOG_E("ota init fail!");
    }
    return ret;
}
#if defined(BOARD_HAAS200)
OTA_WEAK int ota_hal_final(void)
{
    int ret = OTA_INIT_FAIL;
    char dev_str[16] = {0};
    int partition_id =5;
    int fd;
    unsigned char buf[8] = {0};
    unsigned char signature[8] = {0x38, 0x31, 0x39, 0x35, 0x38, 0x37, 0x31, 0x31};
    memset(dev_str, 0, sizeof(dev_str));
    if(ota_get_cur_index()==0)
        partition_id = MTD_PART_ID_APP2;
    else
        partition_id = MTD_PART_ID_APP;
    printf("ota_hal_final update partition[%d] signature to valid\r\n", partition_id);
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    fd = open(dev_str, 0);
    if (fd < 0) {
        OTA_LOG_E("open %s failed\r\n", dev_str);
        return fd;
    }
    lseek(fd, 0 ,SEEK_SET);
    ret = write(fd, signature, sizeof(signature));
    printf("ota_hal_final1 ret %d\r\n" ,ret);
    close(fd);

    if(ota_get_cur_index()==0)
        partition_id = MTD_PART_ID_APP;
    else
        partition_id = MTD_PART_ID_APP2;
    printf("ota_hal_final update partition[%d] signature to invalid\r\n", partition_id);
    memset(dev_str, 0 , sizeof(dev_str));
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    fd = open(dev_str, 0);
    if (fd < 0) {
        OTA_LOG_E("open %s failed\r\n", dev_str);
        return fd;
    }
    lseek(fd, 0 ,SEEK_SET);
    memset(buf, 0 ,sizeof(buf));
    ret = write(fd, buf, sizeof(buf));
    printf("ota_hal_final2 ret %d\r\n" ,ret);
    close(fd);
    return ret;
}
#endif

OTA_WEAK int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    int ret = 0;
    int tocopy = 0;

    if (off == NULL || in_buf_len > OTA_FLASH_WRITE_CACHE_SIZE) {
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
        if (ret < 0) {
            goto EXIT;
        }
        ret = write(ota_fd, ota_cache, OTA_FLASH_WRITE_CACHE_SIZE);
        if (ret < 0) {
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
    if (ota_receive_total_len == ota_fw_size) {
        if (ota_cache_len != 0) {
            ret = lseek(ota_fd, (off_t)*off, SEEK_SET);
            if (ret < 0) {
                goto EXIT;
            }
            ret = write(ota_fd, ota_cache, ota_cache_len);
            if (ret < 0) {
                goto EXIT;
            }
            *off += ota_cache_len;
            ota_crc16_update(&ctx, ota_cache, ota_cache_len);
        }
        if (ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (ota_fd >= 0) {
            close(ota_fd);
            ota_fd = -1;
        }
    }
EXIT:
    if (ret < 0) {
        if (ota_cache != NULL) {
            ota_free(ota_cache);
            ota_cache = NULL;
        }
        if (ota_fd >= 0) {
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
    #if defined(BOARD_HAAS200)
    snprintf(dev_str, 15, "/dev/mtdblock%d", MTD_PART_ID_ENV);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", HAL_PARTITION_PARAMETER_1);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_ROLLBACK;
    }
    ret = read(temp_fd, (void *)&param, sizeof(ota_boot_param_t));
    if (ret < 0) {
        OTA_LOG_E("rollback err1:%d", ret);
        goto OTA_ROLLBACK;
    }
    if ((param.boot_count != 0) && (param.boot_count != 0xff)) {
        param.upg_flag = 0;
        param.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ret = ota_update_parameter(&param);
        if (ret < 0) {
            OTA_LOG_E("rollback err2:%d", ret);
            goto OTA_ROLLBACK;
        }
        #if defined(BOARD_HAAS200)
        // do nothing
        #else
        ret = ioctl(temp_fd, IOC_FLASH_CLEAR_BOOT_COUNT, (unsigned long)0x00);
        if (ret < 0) {
            OTA_LOG_E("clear user boot count fail!");
            goto OTA_ROLLBACK;
        }
        #endif
    }
 OTA_ROLLBACK:
    if (temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    if (ret != 0) {
        OTA_LOG_E("rollback err3:%d", ret);
    }
    return ret;
}

OTA_WEAK int ota_hal_reboot_bank(void)
{
    return 0;
}

OTA_WEAK void ota_hal_image_crc16(unsigned short *outResult)
{
    ota_crc16_final(&ctx, outResult);
}

OTA_WEAK int ota_hal_boot_type()
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    #if defined(BOARD_HAAS200)
    int part_id = MTD_PART_ID_ENV;
    snprintf(dev_str, 15, "/dev/mtdblock%d", part_id);
    #else
    int part_id = HAL_PARTITION_PARAMETER_1;
    snprintf(dev_str, 15, "/dev/flash%d", part_id);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open partition %d failed", part_id);
    } else { 
        #if defined(BOARD_HAAS200)
        ret = 1;
        #else
        ret = ioctl(temp_fd, IOC_FLASH_GET_BOOT_TYPE, (unsigned long)0x00);
        if (ret < 0) {
            OTA_LOG_E("get boot type ioctl failed!");
        }
        #endif
    }
    if (temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

OTA_WEAK unsigned char ota_hal_set_user_bootinfo(void *param)
{
    #if defined(BOARD_HAAS200)
    return 0;
    #else
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    int part_id = HAL_PARTITION_PARAMETER_1;
    snprintf(dev_str, 15, "/dev/flash%d", part_id);
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open partition %d failed", part_id);
    } else {
        ret = ioctl(temp_fd, IOC_FLASH_SET_BOOT_INFO, (unsigned long)param);
        if (ret < 0) {
            OTA_LOG_E("set user boot ioctl failed!");
        }
    }
    if (temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
    #endif
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
    } else {
        ret = ioctl(temp_fd, IOC_FLASH_INFO_GET, (unsigned long)tmp_info);
        if (ret < 0) {
            OTA_LOG_E("ioctl failed!");
        }
    }
    if (temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

OTA_WEAK int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = OTA_UPGRADE_WRITE_FAIL;
    #if defined(BOARD_HAAS200)
    int another_partition;
    mtd_partition_t *tmp_part_info;
    extern const mtd_partition_t mtd_partitions[];
    #if defined(HAAS200_AB_OTA)
    param->src_adr = 0x0;
    param->dst_adr = 0x0;
    param->old_size = 0x0;
    #elif defined(HAAS200_XZ_OTA)
    if(boot_part == MTD_PART_ID_APP2)
            another_partition = MTD_PART_ID_APP;
    else
            another_partition = MTD_PART_ID_APP2;
    tmp_part_info = &mtd_partitions[boot_part];
    param->src_adr = tmp_part_info->partition_start_addr;
    tmp_part_info = &mtd_partitions[another_partition];
    param->dst_adr = tmp_part_info->partition_start_addr;
    param->old_size = tmp_part_info->partition_length;
    if (param->crc == 0 || param->crc == 0xffff) {
        OTA_LOG_I("calculate image crc");
        ota_hal_image_crc16(&param->crc);
    }
    OTA_LOG_I("yxy_crc 0x%x\r\n", param->crc);
    #else
    #error "Please define Haas200 update mode!!"
    #endif
    #else
    int des_part = HAL_PARTITION_APPLICATION;
    hal_logic_partition_t  ota_info;
    hal_logic_partition_t  app_info;
    memset(&ota_info, 0, sizeof(hal_logic_partition_t));
    memset(&app_info, 0, sizeof(hal_logic_partition_t));
    if (param == NULL) {
        OTA_LOG_E("hal boot input param err!");
        goto OTA_HAL_BOOT_OVER;
    }
    ret = ota_get_partition_info(boot_part, &ota_info);
    if (ret < 0) {
        OTA_LOG_E("get ota partiton info err!");
        goto OTA_HAL_BOOT_OVER;
    }
    ret = ota_get_partition_info(des_part, &app_info);
    if (ret < 0) {
        OTA_LOG_E("get app partiton info err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if (param->crc == 0 || param->crc == 0xffff) {
        OTA_LOG_I("calculate image crc");
        ota_hal_image_crc16(&param->crc);
    }
    param->src_adr = ota_info.partition_start_addr;
    param->dst_adr = app_info.partition_start_addr;
    param->old_size = app_info.partition_length;
    #endif
    if (param->upg_flag == OTA_UPGRADE_ALL) {
        int tmp_type = ota_hal_boot_type();
        if (tmp_type < 0) {
            OTA_LOG_E("get platform boot type err!");
            goto OTA_HAL_BOOT_OVER;
        }
        param->boot_type = (unsigned char)tmp_type;
    } else {
        param->boot_type = 0;
    }
    ret = ota_update_parameter(param);
    if (ret < 0) {
        OTA_LOG_E("update param err!");
        goto OTA_HAL_BOOT_OVER;
    }
    if (param->upg_flag == OTA_UPGRADE_ALL) {
        ret = ota_hal_set_user_bootinfo(param);
        if (ret < 0) {
            OTA_LOG_E("clear user boot count fail!");
            goto OTA_HAL_BOOT_OVER;
        }
    }
    #if defined(BOARD_HAAS200)
    #if defined(HAAS200_AB_OTA)
    if(ota_hal_final()<0) {
        OTA_LOG_E("ota_hal_final failed\r\n");
    }
    #elif defined(HAAS200_XZ_OTA)
    //do nothing
    #else
    #error "Please define Haas200 update mode!!"
    #endif
    #endif
    OTA_LOG_I("OTA after finish dst:0x%08x src:0x%08x len:0x%08x, crc:0x%04x param crc:0x%04x upg_flag:0x%04x \r\n", param->dst_adr, param->src_adr, param->len, param->crc, param->param_crc, param->upg_flag);
OTA_HAL_BOOT_OVER:
    if (ret < 0) {
        OTA_LOG_E("set boot info failed!");
    }
    return ret;
}

OTA_WEAK int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    int ret = -1;
    int temp_fd = -1;
    char dev_str[16] = {0};
    #if defined(BOARD_HAAS200)
    int partition_id;
    #if defined(HAAS200_AB_OTA)
    if(ota_get_cur_index()==0)
        partition_id = 6;
    else
        partition_id = 5;
    #elif defined(HAAS200_XZ_OTA)
        partition_id = boot_part;
    #else
        #error "Please define Haas200 update mode!!"
    #endif

    boot_part = partition_id;
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    #else
    snprintf(dev_str, 15, "/dev/flash%d", boot_part);
    #endif
    temp_fd = open(dev_str, 0);
    if (temp_fd < 0) {
        OTA_LOG_E("open ota temp partition failed");
        goto OTA_HAL_READ;
    }
    ret = lseek(temp_fd, (off_t)*off, SEEK_SET);
    if (ret < 0) {
        OTA_LOG_E("read lseek failed:%d", ret);
        goto OTA_HAL_READ;
    }
    ret = read(temp_fd, (void *)out_buf, out_buf_len);
    if (ret < 0) {
        OTA_LOG_E("read failed:%d", ret);
        goto OTA_HAL_READ;
    }

OTA_HAL_READ:
    if (temp_fd >= 0) {
        close(temp_fd);
        temp_fd = -1;
    }
    return ret;
}

