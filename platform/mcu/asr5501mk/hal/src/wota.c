#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include "aos/kernel.h"
#include "aos/kv.h"
#include "aos/hal/flash.h"

#include "ota_hal_plat.h"
#include "ota_hal_os.h"
#include "ota_log.h"

#define OTA_CRC16 "ota_file_crc16"

static unsigned int _offset = 0;

#define TMP_BUF_LEN 1024

int ota_image_check(uint32_t image_size)
{
    uint32_t filelen, flashaddr, len = 0, left;
    uint8_t md5_recv[16];
    uint8_t md5_calc[16];
    ota_md5_context ctx;
    uint8_t *tmpbuf;

    tmpbuf = (uint8_t *)malloc(TMP_BUF_LEN);

    filelen = image_size - 16;
    flashaddr = filelen;
    hal_flash_read(HAL_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)md5_recv, 16);

    ota_md5_init(&ctx);
    ota_md5_starts(&ctx);
    flashaddr = 0;
    left = filelen;

    while (left > 0)
    {
        if (left > TMP_BUF_LEN)
        {
            len = TMP_BUF_LEN;
        }
        else
        {
            len = left;
        }
        left -= len;
        hal_flash_read(HAL_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)tmpbuf, len);
        ota_md5_update(&ctx, (uint8_t *)tmpbuf, len);
    }

    ota_md5_finish(&ctx, md5_calc);
    ota_md5_free(&ctx);

    if (memcmp(md5_calc, md5_recv, 16) != 0)
    {
        OTA_LOG_I("RX:   %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                  md5_recv[0], md5_recv[1], md5_recv[2], md5_recv[3],
                  md5_recv[4], md5_recv[5], md5_recv[6], md5_recv[7],
                  md5_recv[8], md5_recv[9], md5_recv[10], md5_recv[11],
                  md5_recv[12], md5_recv[13], md5_recv[14], md5_recv[15]);
        OTA_LOG_I("Need: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                  md5_calc[0], md5_calc[1], md5_calc[2], md5_calc[3],
                  md5_calc[4], md5_calc[5], md5_calc[6], md5_calc[7],
                  md5_calc[8], md5_calc[9], md5_calc[10], md5_calc[11],
                  md5_calc[12], md5_calc[13], md5_calc[14], md5_calc[15]);
        OTA_LOG_I("user crc check fail\r\n");
        goto exit;
    }

    OTA_LOG_I("OTA image md5 check success");

    return 0;
exit:
    return -1;
}

int ota_image_crc(uint32_t image_size)
{
    uint16_t crcout;
    uint32_t filelen, flashaddr, len = 0, left;
    ota_crc16_ctx ctx = {0};
    uint8_t *tmpbuf;

    tmpbuf = (uint8_t *)malloc(TMP_BUF_LEN);

    filelen = image_size - 16;

    ota_crc16_init(&ctx);

    flashaddr = 0;
    left = filelen;

    while (left > 0)
    {
        if (left > TMP_BUF_LEN)
        {
            len = TMP_BUF_LEN;
        }
        else
        {
            len = left;
        }
        left -= len;
        hal_flash_read(HAL_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)tmpbuf, len);
        ota_crc16_update(&ctx, tmpbuf, len);
    }

    ota_crc16_final(&ctx, &crcout);

    return crcout;
}

static int ota_init(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    _offset = param->off_bp;

    hal_logic_partition_t *part_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);

    OTA_LOG_I("ota init off:0x%08x part:%d len:0x%08x\n", param->off_bp, HAL_PARTITION_OTA_TEMP, param->len);
    if (part_info->partition_length < param->len || param->len == 0)
    {
        ret = OTA_PARAM_FAIL;
        return ret;
    }

    if (param->off_bp == 0)
    {
        int ret = 0;
        int len = part_info->partition_length;
        int offset = _offset;
        while (len > 0)
        {
            ret = hal_flash_erase(HAL_PARTITION_OTA_TEMP, offset, 4096);
            if (ret != 0)
            {
                ret = OTA_INIT_FAIL;
                return ret;
            }
            offset += 4096;
            len -= 4096;
            aos_msleep(1);
        }
    }
    OTA_LOG_I("ota erase completed\r\n");
    return ret;
}

static int ota_write(int *off, char *in_buf, int in_buf_len)
{
    return hal_flash_write(HAL_PARTITION_OTA_TEMP, (uint32_t *)&_offset, (uint8_t *)in_buf, in_buf_len);
}

static int ota_read(int *off, char *out_buf, int out_buf_len)
{
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t *)off, (uint8_t *)out_buf, out_buf_len);
}

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} ota_hdr_t;

static int hal_ota_switch(uint32_t ota_len, uint16_t ota_crc)
{
    uint32_t addr = 0;
    ota_hdr_t ota_hdr = {
        .dst_adr = 0xA000,
        .src_adr = 0x100000,
        .siz = ota_len,
        .crc = ota_crc,
    };

    hal_flash_write(HAL_PARTITION_PARAMETER_1, &addr, (uint8_t *)&ota_hdr, sizeof(ota_hdr));

    return 0;
}

static int ota_boot(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL)
    {
        ret = OTA_REBOOT_FAIL;
        return ret;
    }
    if (param->res_type == OTA_FINISH)
    {
        if (param->upg_flag == OTA_DIFF)
        {
        }
        else
        {
            if (ota_image_check(param->len) != 0)
            {
                return -1;
            }
            param->crc = ota_image_crc(param->len);
            OTA_LOG_I("ota finished, switch to new firmware ...");
            hal_ota_switch(param->len - 16, param->crc);
        }
        ota_reboot();
    }
    return ret;
}

static int ota_rollback(void *something)
{
    return 0;
}

const char *aos_get_app_version(void);
static const char *ota_get_version(unsigned char dev_type)
{
    if (dev_type)
    {
        return "v1.0.0-20180101-1000"; //SYSINFO_APP_VERSION;
    }
    else
    {
        return SYSINFO_APP_VERSION;
    }
}

ota_hal_module_t ota_hal_module = {
    .init = ota_init,
    .write = ota_write,
    .read = ota_read,
    .boot = ota_boot,
    .rollback = ota_rollback,
    .version = ota_get_version,
};
