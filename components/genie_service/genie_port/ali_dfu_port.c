#include <stdio.h>
#include <string.h>
#include "aos/hal/flash.h"
#include "ali_dfu_port.h"

/* 需要定义SPIFLAHS Sector 大小 */
#ifndef SPIF_SECTOR_SIZE
#define SPIF_SECTOR_SIZE 0x1000
#endif

extern uint16_t util_crc16_ccitt(uint8_t const *p_data, uint32_t size, uint16_t const *p_crc);
extern void drv_reboot(void);

#define LOG printf

#define ADDR_GRAN_MASK (0xFFF)
#define ADDR_ALIGN_UP(a) (((a) + ADDR_GRAN_MASK) & ~ADDR_GRAN_MASK)

static unsigned short image_crc16 = 0;

/**
 * @brief 镜像更新
 *
 * @param[in]  signature    暂时不使用
 * @param[in]  offset       当前buf代表的内容，从镜像bin文件从offset位置开始，比如为100，表示当前buffer是bin文件的第100字节开始
 * @param[in]  length       本次buffer的长度
 * @param[in]  buf          本次写入的具体内容
 *
 * @return 0:success, otherwise is failed
 */
int ali_dfu_image_update(short signature, int offset, int length, int *buf)
{
    int ret;
    hal_logic_partition_t partition_info;
    uint32_t wr_idx = offset;
    uint8_t *wr_buf = (uint8_t *)buf;

    if (offset == 0)
    {
        image_crc16 = util_crc16_ccitt(wr_buf, length, NULL);
    }
    else
    {
        image_crc16 = util_crc16_ccitt(wr_buf, length, &image_crc16);
    }

    ///get OTA temporary partition information
    hal_flash_info_get(HAL_PARTITION_OTA_TEMP, &partition_info);

    if (partition_info.partition_length < (offset + length))
    {
        LOG("The write range is over OTA temporary!\r\n");
        return -1;
    }

    /* For bootloader upgrade, we will reserve two sectors, then save the image */
    wr_idx += (SPIF_SECTOR_SIZE << 1);
    ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &wr_idx, (void *)wr_buf, length);
    if (ret < 0)
    {
        LOG("write flash error!!\r\n");
        return -1;
    }

    //LOG("write ok!\n");
    return 0;
}

/**
 * @brief 写入flash之前和之后checksum计算
 *
 * @param[in]  image_id 暂时不使用
 * @param[in]  crc16_output 计算出来的crc返回给调用者
 *
 * @return 1:success 0:failed
 */
unsigned char dfu_check_checksum(short image_id, unsigned short *crc16_output)
{
    *crc16_output = image_crc16;
    return 1;
}

/**
 * @brief 升级结束后重启
 *
 * @param[in]  -
 * @return -
 * @说明: 比如在此函数里面call 切换镜像分区的业务逻辑
 */
void dfu_reboot()
{
    drv_reboot();
}

#ifdef CONFIG_GENIE_OTA_PINGPONG
uint8_t genie_sal_ota_get_current_image_id(void)
{
    return 0;
}

uint8_t genie_sal_ota_change_image_id(uint8_t target_id)
{
    return 0;
}
#endif

int erase_dfu_flash(void)
{
    int ret;
    hal_logic_partition_t partition_info;
    uint32_t offset = (SPIF_SECTOR_SIZE << 1);
    uint32_t length = 0;
    uint8_t cmp_buf[32] = {0xFF};
    uint8_t wr_buf[32] = {0};

    ///get OTA temporary partition information
    hal_flash_info_get(HAL_PARTITION_OTA_TEMP, &partition_info);

    length = partition_info.partition_length - offset;

    memset(cmp_buf, 0xFF, sizeof(cmp_buf));
    ret = hal_flash_read(HAL_PARTITION_OTA_TEMP, &offset, (void *)wr_buf, sizeof(wr_buf));
    if (ret < 0)
    {
        LOG("read flash error!!\r\n");
        return -1;
    }
    if (memcmp(wr_buf, cmp_buf, sizeof(wr_buf)) == 0)
    {
        return 0;
    }

    LOG("OTA dirty\n");

    offset = (SPIF_SECTOR_SIZE << 1);

    /* For bootloader upgrade, we will reserve two sectors, then save the image */
    ret = hal_flash_erase(HAL_PARTITION_OTA_TEMP, offset, length);
    if (ret < 0)
    {
        LOG("Erase flash error!!\r\n");
        return -1;
    }

    return 0;
}

uint32_t ali_dfu_get_ota_partition_max_size(void)
{
    uint32_t reserve_size = (SPIF_SECTOR_SIZE << 1);
    hal_logic_partition_t partition_info;

    hal_flash_info_get(HAL_PARTITION_OTA_TEMP, &partition_info);

    if (partition_info.partition_length > reserve_size)
    {
        return partition_info.partition_length - reserve_size;
    }

    return 0;
}
