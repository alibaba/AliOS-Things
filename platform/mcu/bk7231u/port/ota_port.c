#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"
typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} __attribute__((packed)) ota_hdl_t;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);

extern char app_download_addr;
extern char kernel_download_addr;

int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc )
{
    uint32_t offset;
    ota_hdl_t ota_hdl,ota_hdl_rb;
    hal_logic_partition_t* ota_partition;

    ota_partition = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );

   
    memset( &ota_hdl, 0, sizeof(ota_hdl_t) );
    ota_hdl.dst_adr = parti == OTA_PARTITION_KERNEL ? &kernel_download_addr :
                                   parti == OTA_PARTITION_APP ? &app_download_addr : &kernel_download_addr;
    
    if(ota_hdl.dst_adr < 0x11000 || ota_hdl.dst_adr > 0x100000) {
        LOG("download_addr 0x%08x invalid", ota_hdl.dst_adr);
        hal_reboot();
        return;
    }
    ota_hdl.src_adr = ota_partition->partition_start_addr;
    ota_hdl.siz = ota_data_len;
    ota_hdl.crc = ota_data_crc;

    LOG("OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n",
    ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.siz, ota_hdl.crc);

    offset = 0x00;
    hal_flash_erase( HAL_PARTITION_PARAMETER_1, offset, sizeof(ota_hdl_t) );

    offset = 0x00;
    hal_flash_write( HAL_PARTITION_PARAMETER_1, &offset, (const void *)&ota_hdl, sizeof(ota_hdl_t));

    offset = 0x00;
    memset(&ota_hdl_rb, 0, sizeof(ota_hdl_t));
    hal_flash_read( HAL_PARTITION_PARAMETER_1, &offset, &ota_hdl_rb, sizeof(ota_hdl_t));

    if(memcmp(&ota_hdl, &ota_hdl_rb, sizeof(ota_hdl_t)) != 0)
    {
        LOG("OTA header compare failed, OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n",
        ota_hdl_rb.dst_adr, ota_hdl_rb.src_adr, ota_hdl_rb.siz, ota_hdl_rb.crc);
        return -1;
    }

    /* reboot */
    hal_reboot();

    return 0;
}

static  CRC16_Context contex;

unsigned int _off_set = 0;

static int bk7231_ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    LOG("bk7231 ota init\n");
    _off_set = *(uint32_t*)something;
    ota_info.ota_len=_off_set;
    if(_off_set==0)
    {
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0 ,partition_info->partition_length);
        CRC16_Init( &contex );
    }
    else
    {
        contex.crc=hal_ota_get_crc16();
        LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
    }

    return 0;
}

static int bk7231_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    if(ota_info.ota_len == 0) {
        _off_set = 0;
        CRC16_Init( &contex );
        memset(&ota_info, 0 , sizeof ota_info);
    }
    CRC16_Update( &contex, in_buf, in_buf_len);
    int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
    ota_info.ota_len += in_buf_len;
    return ret;
}

static int bk7231_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int bk7231_ota_set_boot(hal_ota_module_t *m, void *something)
{

    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH)
    {
        CRC16_Final( &contex, &ota_info.ota_crc );
        LOG("bk7231 set boot\n");
        hal_ota_switch_to_new_fw(param->update_type, ota_info.ota_len, ota_info.ota_crc);
        memset(&ota_info, 0 , sizeof ota_info);
    }
    else if (param->result_type==OTA_BREAKPOINT)
    {
        unsigned int save_env_addr;
        hal_logic_partition_t *partition_info;
        LOG("-------save breakpoint to flash- crc=%d--------------\n",contex.crc);
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        hal_ota_save_crc16(contex.crc);
    }
    return 0;
}

struct hal_ota_module_s bk7231_ota_module = {
    .init = bk7231_ota_init,
    .ota_write = bk7231_ota_write,
    .ota_read = bk7231_ota_read,
    .ota_set_boot = bk7231_ota_set_boot,
};


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
