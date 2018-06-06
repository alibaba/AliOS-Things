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
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);

static int boot_table_update( boot_table_t * boot_table )
{
  int ret = 0;
  uint32_t offset;

  boot_table_t *boot_table_dump = malloc(sizeof(boot_table_t));
  hal_flash_erase(HAL_PARTITION_PARAMETER_1, 0x0, hal_flash_get_info( HAL_PARTITION_PARAMETER_1 )->partition_length);
  offset = 0x0;
  hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, boot_table, sizeof(boot_table_t));
  offset = 0x0;
  hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, boot_table_dump, sizeof(boot_table_t));
  if(memcmp(boot_table, boot_table_dump, sizeof(boot_table_t)) != 0)
  {
    ret = -1;
  }

  free(boot_table_dump);
  return ret;
}

int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc )
{
    int ret = 0;
    boot_table_t* boot_table = malloc(sizeof(boot_table_t));
    memset(boot_table, 0, sizeof(boot_table_t));
    boot_table->length = ota_data_len;
    boot_table->start_address = hal_flash_get_info( HAL_PARTITION_OTA_TEMP )->partition_start_addr;
    boot_table->type = 'A';
    boot_table->upgrade_type = 'U';
    boot_table->crc = ota_data_crc;
    if(boot_table_update(boot_table) != 0)
    {
        ret = -1;
    }
    else
    {
        /* reboot */
        hal_reboot();
    }

    free(boot_table);
    return ret;
}

static  CRC16_Context contex;

unsigned int _off_set = 0;

static int ota_init(hal_ota_module_t *m, void *something)
{
    hal_logic_partition_t *partition_info;

    LOG("stm32f4xx ota init\n");
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

static int ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
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

static int ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int ota_set_boot(hal_ota_module_t *m, void *something)
{

    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH)
    {
        CRC16_Final( &contex, &ota_info.ota_crc );
        LOG("moc108 set boot\n");
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

struct hal_ota_module_s stm32f4xx_ota_module = {
    .init = ota_init,
    .ota_write = ota_write,
    .ota_read = ota_read,
    .ota_set_boot = ota_set_boot,
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
