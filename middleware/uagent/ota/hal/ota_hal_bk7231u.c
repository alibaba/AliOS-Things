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

#define OTA_CRC16  "hal_ota_get_crc16"
static int boot_part = HAL_PARTITION_OTA_TEMP;
const char ota_board_string[] = "board "SYSINFO_PRODUCT_MODEL;  /*don't delete it, used for ota diff recovery*/


#define OTA_TWO_IMAGE_SWITCH_RUN       1
#define KV_HAL_OTA_CRC16               "hal_ota_get_crc16"

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info = {0};
static ota_crc16_ctx contex = {0};
static uint32_t _off_set = 0;

static uint16_t hal_ota_get_crc16(void);
static void hal_ota_save_crc16(uint16_t crc16);

#define OTA_IMAGE_A    0
#define OTA_IMAGE_B    1
#define KV_HAL_OTA_HDR  "hal_ota_get_hdr"

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

static ota_hdr_t ota_hdr_info = {0};
static uint8_t ota_index = 0;
static uint8_t ota_hdr_check = 0;

static int hal_get_ota_hdr(void)
{
    int len, ret;

    len = sizeof(ota_hdr_t);
    ret = aos_kv_get(KV_HAL_OTA_HDR, &ota_hdr_info, &len);

    return ret;
}

static int hal_ota_save_hdr(void)
{
    int len, ret;

    len = sizeof(ota_hdr_t);
    ret = aos_kv_set(KV_HAL_OTA_HDR, &ota_hdr_info, len, 1);

    return ret;
}

static void hal_get_ota_index(void)
{
    uint32_t offset, boot_addr;
    hal_logic_partition_t *partition_info;
    offset = 0x00;
    hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, (void *)&boot_addr, sizeof(boot_addr));
    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
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
        OTA_LOG_E("check ota version error %x", ota_hdr_info.version);
        return 1;
    }
    if(ota_hdr_info.hdr_num != 2) {
        OTA_LOG_E("check ota number error %d", ota_hdr_info.hdr_num);
        return 1;
    }
    memset(imgid, 0, sizeof(imgid));
    memcpy(imgid, (char *)&ota_hdr_info.ota_file[0].img_ID, 4);
    if(strcmp(imgid, "OTA1")) {
        OTA_LOG_E("OTA1 image id error.");
        return 1;
    }
    memcpy(imgid, (char *)&ota_hdr_info.ota_file[1].img_ID, 4);
    if(strcmp(imgid, "OTA2")) {
        OTA_LOG_E("OTA2 image id error.");
        return 1;
    }

    if(ota_hdr_info.ota_file[0].checksum == 0) {
        OTA_LOG_E("OTA1 checksum is zaro.");
        return 1;
    }
    if(ota_hdr_info.ota_file[1].checksum == 0) {
        OTA_LOG_E("OTA2 checksum is zaro.");
        return 1;
    }
    return 0;
}

static int hal_verify_ota_checksum(uint8_t crc_partition_idx, uint32_t crc_len)
{
    uint8_t *f_data;
    ota_crc16_ctx ctx;
    uint16_t ota_crc;
    uint32_t i = 0, j = 0, len = 0, checksum = 0;

    ota_crc16_final(&contex, &ota_info.ota_crc);//download crc
    if(ota_info.ota_crc == 0) {
        OTA_LOG_E("crc is zaro.");
        return -1;
    }
    f_data = (uint8_t *)aos_malloc(0x1000);
    if(f_data == NULL) {
        OTA_LOG_E("hal_ota_crc16_compute malloc fail.");
        return -1;
    }
    _off_set = 0;
    ota_crc16_init(&ctx);
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
        ota_crc16_update(&ctx, f_data, len);
        i += len;
    }
    ota_crc16_final(&ctx, &ota_crc);

    if(f_data) {
        aos_free(f_data);
    }
    /*verify crc*/
    if(ota_info.ota_crc != ota_crc) {
        OTA_LOG_E("crc check error: download_crc=0x%x, comput_crc=0x%x.", ota_info.ota_crc, ota_crc);
        return -1;
    }

    /*verify checksum*/
    if(ota_hdr_info.ota_file[ota_index].checksum != checksum) {
        OTA_LOG_E("checksum error: gen_chcksum=0x%x, comput_checksum=0x%x.",
            ota_hdr_info.ota_file[ota_index].checksum, checksum);
        return -1;
    }

    return 0;
}

static int ota_init(void *something)
{
    uint32_t offset = 0, delta = 0;
    hal_logic_partition_t *partition_info;
    hal_flash_dis_secure(0, 0, 0);//disable flash protect
    hal_get_ota_index();
    offset = *(uint32_t*)something;
    if(offset == 0) {
        OTA_LOG_I("ota start :%d  off:%d", ota_index, offset);
        if(ota_index == OTA_IMAGE_A) {
            partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
            hal_flash_erase(HAL_PARTITION_APPLICATION, 0, partition_info->partition_length);
        }
        else {
            partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
            hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, partition_info->partition_length);
        }
        _off_set = 0;
        ota_hdr_check = 0;
        memset(&ota_info, 0, sizeof(ota_info));
        memset(&ota_hdr_info, 0, sizeof(ota_hdr_info));
        ota_crc16_init(&contex);
    }
    else {
        OTA_LOG_I("header:%d off:%d", ota_hdr_check, offset);
        ota_info.ota_len = offset;
        ota_info.ota_crc = 0;
        if(hal_get_ota_hdr()) {
            OTA_LOG_E("get ota header fail");
            return -1;
        }
        if(offset < sizeof(ota_hdr_t)) {
             ota_hdr_check = 0;
        }
        else {
            OTA_LOG_I("verfiy header");
            ota_hdr_check = hal_verify_ota_head();
            if(ota_hdr_check == 0) {
                if(offset < ota_hdr_info.ota_file[1].img_offset) {
                    delta = offset - sizeof(ota_hdr_t);
                    if(ota_index == OTA_IMAGE_A) {
                        _off_set = delta;
                    }
                }
                else {
                    delta = offset - ota_hdr_info.ota_file[1].img_offset;
                    if(ota_index == OTA_IMAGE_B) {
                        _off_set = delta;
                    }
                }
            }
            else {
                OTA_LOG_E("check ota header error.");
                return -1;
            }
        }
        contex.crc = hal_ota_get_crc16();
    }
    OTA_LOG_I("ota init over");
    return 0;
}

static int ota_read(int* off, char* out_buf, int out_buf_len)
{
    if(ota_index == OTA_IMAGE_A) {
         hal_flash_read(HAL_PARTITION_APPLICATION, off, out_buf, out_buf_len);
    }
    else {
         hal_flash_read(HAL_PARTITION_OTA_TEMP, off, out_buf, out_buf_len);
    }
    return 0;
}

static int ota_write(int* off, char* in_buf, int in_buf_len)
{
    uint32_t offset = 0;
    if(ota_hdr_check) {
        OTA_LOG_E("check ota header fail.");
        return -1;
    }
    else {
        /*OTA_LOG_I(".");*/
    }
    if(ota_info.ota_len < sizeof(ota_hdr_t)) {
        OTA_LOG_I("ota header w check:%d", ota_hdr_check);
        offset = sizeof(ota_hdr_t) - ota_info.ota_len;
        if(in_buf_len < offset) {
            memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, in_buf_len);
        }
        else {
            memcpy((uint8_t *)&ota_hdr_info + ota_info.ota_len, in_buf, offset);
            ota_hdr_check = hal_verify_ota_head();
            if(ota_hdr_check == 0 && ota_index == OTA_IMAGE_A) {
                ota_crc16_update(&contex, in_buf + offset, in_buf_len - offset);
                hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf + offset, in_buf_len - offset);
            }
        }
    }
    else {
        if(ota_index == OTA_IMAGE_A) {
           if(ota_info.ota_len + in_buf_len < ota_hdr_info.ota_file[1].img_offset) {
                ota_crc16_update(&contex, in_buf, in_buf_len);
                hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, in_buf_len);
           }
           else {
                if(ota_info.ota_len < ota_hdr_info.ota_file[1].img_offset) {
                    offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
                    ota_crc16_update(&contex, in_buf, offset);
                    hal_flash_write(HAL_PARTITION_APPLICATION, &_off_set, in_buf, offset);
                }
           }
        }
        else {
            if(ota_info.ota_len > ota_hdr_info.ota_file[1].img_offset) {
                ota_crc16_update(&contex, in_buf, in_buf_len);
                hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
            }
            else {
                if(ota_info.ota_len + in_buf_len > ota_hdr_info.ota_file[1].img_offset) {
                    offset = ota_hdr_info.ota_file[1].img_offset - ota_info.ota_len;
                    ota_crc16_update(&contex, in_buf + offset, in_buf_len - offset);
                    hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf + offset, in_buf_len - offset);
                }
            }
       }
    }
    ota_info.ota_len += in_buf_len;
    return 0;
}

static int ota_boot(void *something)
{
    int ret = 0;
    uint32_t offset, addr_rb, boot_addr;
    uint32_t crc_partition_idx, crc_len;
    hal_logic_partition_t *partition_info;
    ota_boot_param_t *param;

    param = (ota_boot_param_t *)something;
    if (param == NULL) {
        OTA_LOG_E("finish type is null.");
        return -1;
    }

    if(ota_hdr_check) {
        OTA_LOG_E("check ota head fail.");
        return -1;
    }

    if(param->res_type == OTA_FINISH) {
        if(ota_index == OTA_IMAGE_A) {
            crc_partition_idx = HAL_PARTITION_APPLICATION;
            crc_len = ota_hdr_info.ota_file[0].img_len;
            partition_info = hal_flash_get_info(HAL_PARTITION_APPLICATION);
        }
        else {
            crc_partition_idx = HAL_PARTITION_OTA_TEMP;
            crc_len = ota_hdr_info.ota_file[1].img_len;
            partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
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
            OTA_LOG_E("write boot address fail.");
            return -1;
        }
    }
    else {
        ret = hal_ota_save_hdr();
        if(ret) {
            OTA_LOG_E("save ota header fail.");
            return -1;
        }
        hal_ota_save_crc16(contex.crc);
    }

    hal_reboot();
    return ret;
}

static uint16_t hal_ota_get_crc16(void)
{
    int len = 2;
    uint16_t crc16 = 0;
    aos_kv_get(KV_HAL_OTA_CRC16, &crc16, &len);
    return crc16;
}

static void  hal_ota_save_crc16(uint16_t crc16)
{
    aos_kv_set(KV_HAL_OTA_CRC16, &crc16, 2, 1);
}

static int ota_rollback(void *something)
{
    int offset = 0x00;
    int param_part = HAL_PARTITION_PARAMETER_1;
    ota_boot_param_t param_w, param_r;
    memset(&param_w, 0, sizeof(ota_boot_param_t));
    hal_flash_read(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
    if((param_w.boot_count != 0) && (param_w.boot_count != 0xff)) {
        ota_crc16_ctx ctx1;
        unsigned short crc;
        param_w.upg_flag = 0;
        param_w.boot_count = 0; /*Clear bootcount to avoid rollback*/
        ota_crc16_init(&ctx1);
        ota_crc16_update(&ctx1, &param_w, sizeof(ota_boot_param_t) - sizeof(unsigned short));
        ota_crc16_final(&ctx1, &crc);
        param_w.param_crc = crc;
        offset = 0x00;
        hal_flash_erase(param_part, offset, sizeof(ota_boot_param_t));
        offset = 0x00;
        hal_flash_write(param_part, (uint32_t*)&offset, &param_w, sizeof(ota_boot_param_t));
        offset = 0x00;
        memset(&param_r, 0, sizeof(ota_boot_param_t));
        hal_flash_read(param_part, (uint32_t*)&offset, &param_r, sizeof(ota_boot_param_t));
        if(memcmp(&param_w, &param_r, sizeof(ota_boot_param_t)) != 0) {
            OTA_LOG_E("rollback failed\n");
            return -1;
        }
    }
    return 0;
}


static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    }
    else {
        return SYSINFO_APP_VERSION;
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

