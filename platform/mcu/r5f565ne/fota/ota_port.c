#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>
#include <port.h>
#include "r_flash_rx_if.h"
#include "r_flash_rx_config.h"
#include "flash.h"


#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"

//extern int FLASH_set_boot_bank(uint32_t bank);
//extern int FLASH_bank1_enabled(void);
extern uint8_t g_buf_size;  //the data in the buf
typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static  CRC16_Context contex;

static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);
static int rx65n_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len);
static int rx65n_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len);
static int rx65n_ota_set_boot(hal_ota_module_t *m, void *something);


int hal_ota_switch_to_new_fw()
{
    int ret;
    flash_err_t status_flash;

    clrpsw_i();
    status_flash = R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
    setpsw_i();
    if (FLASH_SUCCESS == status_flash)
    {
        ret = 0;//FW_UP_SUCCESS;
    }
    else
    {
        ret = -1;//FW_UP_ERR_WRITE;
    }
    return ret;
}


unsigned int _off_set = 0;
static int rx65n_ota_init(hal_ota_module_t *m, void *something)
{
	flash_err_t reval;
	flash_access_window_config_t  accessInfo;
	int ret;
    hal_logic_partition_t *partition_info;
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    LOG("ota module init\n");
    _off_set = *(uint32_t*)something;
    ota_info.ota_len=_off_set;


    /*Open Code Flash Access Window*/
//	accessInfo.start_addr = FLASH_CF_BLOCK_75;
//	accessInfo.end_addr = FLASH_CF_BLOCK_37;
//	CPSR_ALLOC();
//	RHINO_CPU_INTRPT_DISABLE();
//	reval = R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, (void *)&accessInfo);
//	RHINO_CPU_INTRPT_ENABLE();
	g_buf_size =0;
    if(_off_set==0) {

        partition_info = hal_flash_get_info( pno );
        ret = hal_flash_erase(pno, 0 ,partition_info->partition_length);

        if(ret != 0){
        	return -1;
        }
        CRC16_Init( &contex );
    } else {
        contex.crc=hal_ota_get_crc16();
        LOG("--------get crc16 context.crc=%d!--------\n",contex.crc);
    }

    return 0;
}


static int rx65n_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
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

static int rx65n_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    hal_flash_read(pno, (uint32_t*)off_set, out_buf, out_buf_len);
    return 0;
}

static int rx65n_ota_set_boot(hal_ota_module_t *m, void *something)
{
	uint32_t banksel_val = *((uint32_t *)BANKSEL_ADDR);
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH)
    {
        CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );
        LOG("switch boot bank\n");
        LOG("BANKSWP IS: %x \n",banksel_val);
        hal_ota_switch_to_new_fw();
        banksel_val = *((uint32_t *)BANKSEL_ADDR);
        LOG("BANKSWP IS: %x \n",banksel_val);
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

struct hal_ota_module_s rx65n_ota_module = {
    .init = rx65n_ota_init,
    .ota_write = rx65n_ota_write,
    .ota_read = rx65n_ota_read,
    .ota_set_boot = rx65n_ota_set_boot,
};

