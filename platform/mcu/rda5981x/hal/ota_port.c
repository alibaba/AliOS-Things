#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>

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

static r_s32 rda59xx_ota_init(hal_ota_module_t *m, void *something)
{
    printf("========= OTA init ========= \r\n");
    g_ota_off_set = *(r_u32*)something;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    hal_logic_partition_t *partition_info;
    partition_info = hal_flash_get_info(pno);
    //g_ota_partition_len = partition_info->partition_length;
    if (g_ota_off_set == 0) {
        hal_flash_erase(pno, 0, partition_info->partition_length);
    } else {
        printf("OTA init invalid, offset = %d \r\n", g_ota_off_set);
    }

    return 0;
}

static r_s32 rda59xx_ota_write(hal_ota_module_t *m, volatile r_u32* off_set, r_u8* in_buf ,r_u32 in_buf_len)
{
    r_s32 ret;

    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    printf("========= OTA write start: len: %d ==========\r\n", in_buf_len);
    g_crc_result = crc32(in_buf, in_buf_len, g_crc_result);
    ret = hal_flash_write(pno, &g_ota_off_set, in_buf, in_buf_len);
    g_ota_off_set += in_buf_len;
    return ret;
}

static r_s32 rda59xx_ota_read(hal_ota_module_t *m,  volatile r_u32* off_set, r_u8* out_buf, r_u32 out_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    hal_flash_read(pno, off_set, out_buf, out_buf_len);

    return 0;
}

static r_s32 rda59xx_ota_set_boot(hal_ota_module_t *m, void *something)
{
    printf("========= OTA boot set ========= \r\n");
    hal_logic_partition_t *partition_info;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL) {
        return -1;
    }
    if (param->result_type == OTA_FINISH) {
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
    } else if (param->result_type==OTA_BREAKPOINT) {
        printf("OTA package is incomplete!\r\n");
    }
    return 0;
}

struct hal_ota_module_s rda59xx_ota_module = {
    .init = rda59xx_ota_init,
    .ota_write = rda59xx_ota_write,
    .ota_read = rda59xx_ota_read,
    .ota_set_boot = rda59xx_ota_set_boot,
};
