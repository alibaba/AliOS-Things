#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include "aos/kernel.h"
#include "aos/kv.h"
#include "aos/hal/flash.h"
#include "ota/ota_service.h"

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} ota_hdr_t;

#define TMP_BUF_LEN           (1024)
#define IMAGE_INFORMATION_LEN (28)

static unsigned short update_crc16(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;

    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

static void crc16_update(unsigned short *crc, const void *inSrc, unsigned int inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while (src < srcEnd) {
        *crc = update_crc16(*crc, *src++);
    }
}

static void crc16_final(unsigned short *crc, unsigned short *outResult )
{
    *crc = update_crc16(*crc, 0);
    *crc = update_crc16(*crc, 0);
    *outResult = *crc & 0xffffu;
}

int ota_image_crc(uint32_t image_size)
{
    uint16_t crcout;
    uint32_t filelen, flashaddr, len = 0, left;
    unsigned short crc = 0x00;
    uint8_t *tmpbuf = NULL;

    tmpbuf = (uint8_t *)aos_malloc(TMP_BUF_LEN);
    filelen = image_size;
    flashaddr = 0;
    left = filelen;
    while (left > 0) {
        if (left > TMP_BUF_LEN) {
            len = TMP_BUF_LEN;
        }
        else {
            len = left;
        }
        left -= len;
        hal_flash_read(HAL_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)tmpbuf, len);
        crc16_update(&crc, tmpbuf, len);
    }
    crc16_final(&crc, &crcout);
    if(tmpbuf != NULL) {
        aos_free(tmpbuf);
        tmpbuf = NULL;
    }
    return crcout;
}

int ota_hal_init(ota_boot_param_t *param)
{
    int ret = -1;
    int len = 0;
    int offset = 0;
    hal_logic_partition_t part_info;
    if(param != NULL) {
        ret = hal_flash_info_get(HAL_PARTITION_OTA_TEMP, &part_info);
        printf("ota init part:%d len:0x%08x\n", HAL_PARTITION_OTA_TEMP, param->len);
        if (ret != 0 || part_info.partition_length < param->len || param->len == 0) {
            ret  = -1;
            return ret;
        }
        len = part_info.partition_length;
        while (len > 0) {
            ret = hal_flash_erase(HAL_PARTITION_OTA_TEMP, offset, 4096);
            if (ret != 0) {
                ret = -1;
                return ret;
            }
            offset += 4096;
            len -= 4096;
            aos_msleep(1);
        }
        printf("ota erase completed\r\n");
    }
    return ret;
}

int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    return hal_flash_write(HAL_PARTITION_OTA_TEMP, off, (uint8_t *)in_buf, in_buf_len);
}

int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t *)off, (uint8_t *)out_buf, out_buf_len);
}

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

int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = 0;
    if (param != NULL) {
        param->crc = ota_image_crc(param->len - IMAGE_INFORMATION_LEN);
        printf("ota finished, switch to new firmware ...\r\n");
        hal_ota_switch(param->len - IMAGE_INFORMATION_LEN, param->crc);
    }
    return ret;
}

int ota_hal_rollback(void)
{
    return 0;
}

const char *ota_hal_version(unsigned char dev_type, char* dn)
{
    if (dev_type) {
        return "v1.0.0-20180101-1000"; //SYSINFO_APP_VERSION;
    }
    else {
        return SYSINFO_APP_VERSION;
    }
}
