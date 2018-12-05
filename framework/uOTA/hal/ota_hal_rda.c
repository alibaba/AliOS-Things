#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/soc/soc.h>
#include "ota_hal_plat.h"
#include "rda_def.h"
#include "rda_flash.h"

r_u32 g_crc_result = ~0UL;
r_u32 g_ota_off_set = 0;

#define CRC32_TABLE_ADDR    0xbb5c
#define CRC32_ADDR          0x8dff
#define CRC32_TABLE_ADDR_4  0xbbd8
#define CRC32_ADDR_4        0x8e33
#define IMAGE_MAGIC         0xAEAE

static r_u32 bootrom_crc32(r_u8 *p, r_u32 len)
{
    r_u32 func = CRC32_ADDR;
    if (rda_ccfg_hwver() >= 4) {
        func = CRC32_ADDR_4;
    }
    return ((r_u32(*)(r_u8 *, r_u32))func)(p, len);
}

static r_u32 crc32(const r_u8 *p, r_u32 len, r_u32 crc)
{
    const r_u32 *crc32_tab = (const r_u32 *)CRC32_TABLE_ADDR;

    if (rda_ccfg_hwver() >= 4) {
        crc32_tab = (const r_u32 *)CRC32_TABLE_ADDR_4;
    }
    /* Calculate CRC */
    while (len--)
    {
         crc = crc32_tab[((crc & 0xFF) ^ *p++)] ^ (crc >> 8);
    }

    return crc;
}

static int ota_init(void *something)
{
    printf("========= OTA init ========= \r\n");
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    g_ota_off_set = param->off_bp;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    hal_logic_partition_t *partition_info;
    partition_info = hal_flash_get_info(pno);
    if (g_ota_off_set == 0) {
        hal_flash_erase(pno, 0, partition_info->partition_length);
    } else {
        printf("OTA init invalid, offset = %d \r\n", g_ota_off_set);
    }
    return 0;
}

static int ota_write(volatile int* off_set, char* in_buf ,int in_buf_len)
{
    int ret = 0;

    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    printf("========= OTA write start: len: %d ==========\r\n", in_buf_len);
    g_crc_result = crc32(in_buf, in_buf_len, g_crc_result);
    ret = hal_flash_write(pno, &g_ota_off_set, in_buf, in_buf_len);
    g_ota_off_set += in_buf_len;
    return ret;
}

static int ota_read(volatile int* off_set, char* out_buf, int out_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    hal_flash_read(pno, off_set, out_buf, out_buf_len);
    return 0;
}

static int ota_boot(void *something)
{
    printf("========= OTA boot set ========= \r\n");
    hal_logic_partition_t *partition_info;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param==NULL) {
        return -1;
    }
    
    if (param->res_type == OTA_FINISH) {
        printf("upgrade finish.");
        partition_info = hal_flash_get_info(pno);
        core_util_critical_section_enter();
        spi_flash_flush_cache();
        r_u32 crc32_check = bootrom_crc32((r_u8 *)partition_info->partition_start_addr, g_ota_off_set);
        core_util_critical_section_exit();
        printf("rda5981_write_partition_end:0x%08x:0x%08x\r\n", g_crc_result, crc32_check);
        if (crc32_check == g_crc_result) {
            hal_reboot();
        } else {
            printf("ERROR!!! check crc32 error\r\n");
        }
    } else if (param->res_type==OTA_BREAKPOINT) {
        printf("OTA package is incomplete!\r\n");
    }
    return 0;
}

static int ota_rollback(void *something)
{
    printf("OTA rollback. \r\n");
    return 0;
}

static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return aos_get_app_version();
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_boot,
    .rollback = ota_rollback,
    .version  = ota_get_version,
};

