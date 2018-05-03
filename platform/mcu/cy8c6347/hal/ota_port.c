#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 

#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"


typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static  CRC16_Context contex;

static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);
static int cy8c6347_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len);
static int cy8c6347_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len);
static int cy8c6347_ota_set_boot(hal_ota_module_t *m, void *something);


int hal_ota_switch_to_new_fw()
{
#if 0
    if (0 == FLASH_set_boot_bank(FLASH_BANK_BOTH)) {
        LOG("Default boot bank switched successfully.\n");
        return 0;
    } else {
        LOG("Error: failed changing the boot configuration\n");
        return -1;
    }
#endif
}


unsigned int _off_set = 0;
static int cy8c6347_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    LOG("ota module init\n");
    _off_set = *(uint32_t*)something;
    ota_info.ota_len=_off_set;
    
    if(_off_set==0) {
        partition_info = hal_flash_get_info( pno );
        hal_flash_erase(pno, 0 ,partition_info->partition_length);
        CRC16_Init( &contex );
    } else {
        contex.crc=hal_ota_get_crc16();
        LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
    }

    return 0;
}


static int cy8c6347_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
    int ret;

    if (ota_info.ota_len == 0) {
        _off_set = 0;
        CRC16_Init( &contex );
        memset(&ota_info, 0 , sizeof ota_info);
    }
    CRC16_Update( &contex, in_buf, in_buf_len);
		
    ret = hal_flash_write(pno, &_off_set, in_buf, in_buf_len);
    ota_info.ota_len += in_buf_len;
    return ret;
}

static int cy8c6347_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;
		
    hal_flash_read(pno, (uint32_t*)off_set, out_buf, out_buf_len);
    return 0;
}

static int cy8c6347_ota_set_boot(hal_ota_module_t *m, void *something)
{
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH)
    {
        CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );
        LOG("switch boot bank\n");
        hal_ota_switch_to_new_fw();
        memset(&ota_info, 0 , sizeof ota_info);
    } else if (param->result_type==OTA_BREAKPOINT) {
        LOG("OTA package is incomplete!\n");
        hal_ota_save_crc16(contex.crc);
    }
    return 0;
}

static uint16_t hal_ota_get_crc16(void)
{
    int len = 2;
    uint16_t crc16=0;
    aos_kv_get(KV_HAL_OTA_CRC16, &crc16, &len);
    return crc16;
}

static void  hal_ota_save_crc16(uint16_t crc16)
{
    aos_kv_set(KV_HAL_OTA_CRC16, &crc16, 2, 1);
}

struct hal_ota_module_s cy8c6347_ota_module = {
    .init = cy8c6347_ota_init,
    .ota_write = cy8c6347_ota_write,
    .ota_read = cy8c6347_ota_read,
    .ota_set_boot = cy8c6347_ota_set_boot,
};

