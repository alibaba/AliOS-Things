#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "aos/kernel.h"
#include "aos/kv.h"
#include "aos/hal/flash.h"
#include "ota/ota_service.h"

#define OTA_IMAGE_A    0
#define OTA_IMAGE_B    1
#define KV_HAL_OTA_HDR  "hal_ota_get_hdr"
#define OTA_TWO_IMAGE_SWITCH_RUN       1

typedef struct {
    unsigned short crc;
} crc16_ctx;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

typedef struct
{
    uint32_t version;
    uint32_t hdr_num;
    struct
    {
        uint32_t img_ID;
        uint32_t hdr_len;
        uint32_t checksum;
        uint32_t img_len;
        uint32_t img_offset;
        uint32_t flash_offset;
    }ota_file[2];
}ota_hdr_t;

static int boot_part = HAL_PARTITION_OTA_TEMP;
static ota_reboot_info_t ota_info = {0};
static crc16_ctx contex = {0};
static uint32_t _off_set = 0;
static ota_hdr_t ota_hdr_info = {0};
static uint8_t ota_index = 0;
static uint8_t ota_hdr_check = 0;

void crc16_init(crc16_ctx *inCtx);
void crc16_update(crc16_ctx *inCtx, const void *inSrc, unsigned int inLen);
void crc16_final(crc16_ctx *inCtx, unsigned short *outResult );

static void hal_get_ota_index(void)
{
    uint32_t offset, boot_addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info;
    offset = 0x00;
    hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (void *)&boot_addr, sizeof(boot_addr));

    partition_info = &info;

    if (hal_flash_info_get(HAL_PARTITION_OTA_TEMP, partition_info) != 0) {
        return -1;
    }

    if(boot_addr == partition_info->partition_start_addr) {
        ota_index = OTA_IMAGE_A; //update to image A
    }
    else {
        ota_index = OTA_IMAGE_B; //update to image B
    }
    return;
}

static int hal_verify_ota_head(void)
{
    char imgid[5];

    if(~ota_hdr_info.version) {
        printf("check ota version error %x", ota_hdr_info.version);
        return 1;
    }
    if(ota_hdr_info.hdr_num != 2) {
        printf("check ota number error %d", ota_hdr_info.hdr_num);
        return 1;
    }
    memset(imgid, 0, sizeof(imgid));
    memcpy(imgid, (char *)&ota_hdr_info.ota_file[0].img_ID, 4);
    if(strcmp(imgid, "OTA1")) {
        printf("OTA1 image id error.");
        return 1;
    }
    memcpy(imgid, (char *)&ota_hdr_info.ota_file[1].img_ID, 4);
    if(strcmp(imgid, "OTA2")) {
        printf("OTA2 image id error.");
        return 1;
    }

    if(ota_hdr_info.ota_file[0].checksum == 0) {
        printf("OTA1 checksum is zaro.");
        return 1;
    }
    if(ota_hdr_info.ota_file[1].checksum == 0) {
        printf("OTA2 checksum is zaro.");
        return 1;
    }
    return 0;
}

static int hal_verify_ota_checksum(uint8_t crc_partition_idx, uint32_t crc_len)
{
    uint8_t *f_data;
    crc16_ctx ctx;
    uint16_t ota_crc;
    uint32_t i = 0, j = 0, len = 0, checksum = 0;

    crc16_final(&contex, &ota_info.ota_crc);//download crc
    if(ota_info.ota_crc == 0) {
        printf("crc is zaro.");
        return -1;
    }
    f_data = (uint8_t *)aos_malloc(0x1000);
    if(f_data == NULL) {
        printf("hal_ota_crc16_compute malloc fail.");
        return -1;
    }
    _off_set = 0;
    crc16_init(&ctx);
    while(i < crc_len) {
        if((0x1000 + i) < crc_len) {
            len = 0x1000;
        }
        else {
            len = crc_len - i;
        }
        hal_flash_read(crc_partition_idx, &_off_set, f_data, len);
        /*update checksum*/
        for(j = 0; j < len; j++) {
            checksum += f_data[j];
        }
        /*update crc16*/
        crc16_update(&ctx, f_data, len);
        i += len;
    }
    crc16_final(&ctx, &ota_crc);

    if(f_data) {
        aos_free(f_data);
    }
    /*verify crc*/
    if(ota_info.ota_crc != ota_crc) {
        printf("crc check error: download_crc=0x%x, comput_crc=0x%x.", ota_info.ota_crc, ota_crc);
        return -1;
    }

    /*verify checksum*/
    if(ota_hdr_info.ota_file[ota_index].checksum != checksum) {
        printf("checksum error: gen_chcksum=0x%x, comput_checksum=0x%x.",
            ota_hdr_info.ota_file[ota_index].checksum, checksum);
        return -1;
    }

    return 0;
}

int ota_hal_init(ota_boot_param_t *param)
{
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;
    hal_flash_dis_secure(0, 0, 0);//disable flash protect
    hal_get_ota_index();
    printf("ota start :%d  off:%d", ota_index);
    if(ota_index == OTA_IMAGE_A) {
        hal_flash_info_get(HAL_PARTITION_APPLICATION, partition_info);
        hal_flash_erase(HAL_PARTITION_APPLICATION, 0, partition_info->partition_length);
    }
    else {
        hal_flash_info_get(HAL_PARTITION_OTA_TEMP, partition_info);
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, partition_info->partition_length);
    }
    _off_set = 0;
    ota_hdr_check = 0;
    memset(&ota_info, 0, sizeof(ota_info));
    memset(&ota_hdr_info, 0, sizeof(ota_hdr_info));
    crc16_init(&contex);
    printf("ota init over");
    return 0;
}

int ota_hal_read(unsigned int *off, char *out_buf, unsigned int out_buf_len)
{
    if(ota_index == OTA_IMAGE_A) {
         hal_flash_read(HAL_PARTITION_APPLICATION, off, out_buf, out_buf_len);
    }
    else {
         hal_flash_read(HAL_PARTITION_OTA_TEMP, off, out_buf, out_buf_len);
    }
    return 0;
}

int ota_hal_write(unsigned int *off, char *in_buf, unsigned int in_buf_len)
{
    uint32_t offset = 0;
    if(ota_hdr_check) {
        printf("check ota header fail.");
        return -1;
    }
    else {
        /*OTA_LOG_I(".");*/
    }
    if(ota_info.ota_len < sizeof(ota_hdr_t)) {
        printf("ota header w check:%d", ota_hdr_check);
        offset = sizeof(ota_hdr_t) - ota_info.ota_len;
        if(in_buf_len < offset) {
            memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, in_buf_len);
        }
        else {
            memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, offset);
            ota_hdr_check = hal_verify_ota_head();
            if(ota_hdr_check == 0 && ota_index == OTA_IMAGE_A) {
                crc16_update(&contex, in_buf + offset, in_buf_len - offset);
                hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf + offset, in_buf_len - offset);
            }
        }
    }
    else {
        if(ota_index == OTA_IMAGE_A) {
           if(ota_info.ota_len + in_buf_len < ota_hdr_info.ota_file[1].img_offset) {
                crc16_update(&contex, in_buf, in_buf_len);
                hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, in_buf_len);
           }
           else {
                if(ota_info.ota_len < ota_hdr_info.ota_file[1].img_offset) {
                    offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
                    crc16_update(&contex, in_buf, offset);
                    hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, offset);
                }
           }
        }
        else {
            if(ota_info.ota_len > ota_hdr_info.ota_file[1].img_offset) {
                crc16_update(&contex, in_buf, in_buf_len);
                hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
            }
            else {
                if(ota_info.ota_len + in_buf_len > ota_hdr_info.ota_file[1].img_offset) {
                    offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
                    crc16_update(&contex, in_buf + offset, in_buf_len - offset);
                    hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf + offset, in_buf_len - offset);
                }
            }
       }
    }
    ota_info.ota_len += in_buf_len;
    return 0;
}

int ota_hal_boot(ota_boot_param_t *param)
{
    int ret = 0;
    uint32_t offset, addr_rb, boot_addr;
    uint32_t crc_partition_idx, crc_len;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;
    if (param == NULL) {
        printf("finish type is null.");
        return -1;
    }

    if(ota_hdr_check) {
        printf("check ota head fail.");
        return -1;
    }

    if(param->upg_status == OTA_FINISH) {
        if(ota_index == OTA_IMAGE_A) {
            crc_partition_idx = HAL_PARTITION_APPLICATION;
            crc_len = ota_hdr_info.ota_file[0].img_len;
            hal_flash_info_get(HAL_PARTITION_APPLICATION, partition_info);
        }
        else {
            crc_partition_idx = HAL_PARTITION_OTA_TEMP;
            crc_len = ota_hdr_info.ota_file[1].img_len;
            hal_flash_info_get(HAL_PARTITION_OTA_TEMP, partition_info);
        }
        boot_addr = partition_info->partition_start_addr;
        ret = hal_verify_ota_checksum(crc_partition_idx, crc_len);
        if(ret) {
            return -1;
        }
        offset = 0x00;
        hal_flash_erase(HAL_PARTITION_PARAMETER_1, offset, sizeof(boot_addr));
        offset = 0x00;
        hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, (const void *)&boot_addr, sizeof(boot_addr));
        offset = 0x00;
        hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, &addr_rb, sizeof(addr_rb));
        if(addr_rb != boot_addr) {
            printf("write boot address fail.");
            return -1;
        }
    }
    hal_reboot();
    return ret;
}

const char *ota_get_version(unsigned char dev_type, char* dn)
{
    if(dev_type > 0) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return SYSINFO_APP_VERSION;
    }
}

/*CRC16*/
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

void crc16_init(crc16_ctx *inCtx)
{
    inCtx->crc = 0;
}

void crc16_update(crc16_ctx *inCtx, const void *inSrc, unsigned int inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while (src < srcEnd) {
        inCtx->crc = update_crc16(inCtx->crc, *src++);
    }
}

void crc16_final(crc16_ctx *inCtx, unsigned short *outResult )
{
    inCtx->crc = update_crc16(inCtx->crc, 0);
    inCtx->crc = update_crc16(inCtx->crc, 0);
    *outResult = inCtx->crc & 0xffffu;
}

