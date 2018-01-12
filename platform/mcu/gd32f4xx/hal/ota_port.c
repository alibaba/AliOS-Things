#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 

#include <errno.h>
#include <hal/ota.h>
#include <aos/log.h>
#include <hal/soc/soc.h>

#include "gd32f4xx.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

typedef enum
{
    DEV_FMC_BANK0_START = 0,
    DEV_FMC_BANK1_START = 1,
}dev_fmc_start_enum; 

unsigned int g_dev_off_set = 0;

static void dev_fmc_boot_switch(void)
{
    /* unlock the option byte operation */
    ob_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
    
    /* set BB bit to enable boot from FMC bank1 */
    if (0 != ((uint8_t)FMC_OBCTL0 & (uint8_t)0x10)){
        ob_boot_mode_config(OB_BB_DISABLE);
    }
    else{
        ob_boot_mode_config(OB_BB_ENABLE);
    }
    /* send option byte change command */
    ob_start();
    /* lock the option byte operation */
    ob_lock();
}


static dev_fmc_start_enum dev_fmc_start_get(void)
{
    uint32_t tempreg;
    
    tempreg = FMC_OBCTL0;

    if((tempreg & FMC_OBCTL0_BB) == FMC_OBCTL0_BB){
        return DEV_FMC_BANK1_START;
    }else{
        return DEV_FMC_BANK0_START;
    }
}

static int dev_fmc_write(uint32_t start_addr, const uint8_t* buf ,uint32_t buf_size)
{
    int i;
    uint8_t *p_data;
    uint32_t address;
    
    p_data = buf;
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    address = start_addr;
    /* write data_8 to the corresponding address */
    for(i=0; i < buf_size; i++){
        if(FMC_READY == fmc_byte_program(address, buf[i])){
            address++;
//            fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        }else{ 
            while(1);
        }
    }

    /* lock the flash program erase controller */
    fmc_lock();
    /* Memory check */
    for (i = 0; i < buf_size; i++) {
        uint8_t *dst = (uint8_t *)(start_addr + i);
        uint8_t *src = (p_data + i);

        if ( *dst != *src ) {
            printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) dst, *dst, *src);
            return -1;
        }
    }

    return 0;
}
static int gd32f4xx_ota_init(hal_ota_module_t *m, void *something)
{
    printf("========= OTA init ========= \r\n");   
    g_dev_off_set = *(uint32_t*)something;
    
    if(0 == g_dev_off_set){
        if (DEV_FMC_BANK1_START == dev_fmc_start_get()) {
            fmc_bank0_erase();
        }else{
            fmc_bank1_erase();
        }
    }else{
        printf("OTA init invalid, offset = %d \r\n", g_dev_off_set);
    }

    return 0;
}

static int gd32f4xx_ota_write(hal_ota_module_t *m, volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    int ret;
    hal_logic_partition_t *partition_info;

    if (DEV_FMC_BANK1_START == dev_fmc_start_get()) {
        partition_info = hal_flash_get_info( HAL_PARTITION_APPLICATION );
        ret = dev_fmc_write(partition_info->partition_start_addr + g_dev_off_set, in_buf, in_buf_len);
    }else{
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        ret = dev_fmc_write(partition_info->partition_start_addr + g_dev_off_set, in_buf, in_buf_len);
    }
    
    printf("========= OTA write start: len: %d, start_addr = %08x ==========\r\n", in_buf_len, partition_info->partition_start_addr);
    g_dev_off_set += in_buf_len;

    return ret;
}

static int gd32f4xx_ota_read(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_partition_t pno = HAL_PARTITION_OTA_TEMP;

    if (DEV_FMC_BANK1_START == dev_fmc_start_get()) {
        pno = HAL_PARTITION_APPLICATION;
    }

    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);

    return 0;
}

static int gd32f4xx_ota_set_boot(hal_ota_module_t *m, void *something)
{
    printf("========= OTA boot set ========= \r\n");

    ota_finish_param_t *param = (ota_finish_param_t *)something;

    if (param==NULL){
        return -1;
    }
    if (param->result_type == OTA_FINISH)
    {
//        dev_fmc_boot_switch();
    } else if (param->result_type==OTA_BREAKPOINT) {
        printf("OTA package is incomplete!\r\n");
    }
    
    return 0;
}

struct hal_ota_module_s gd32f4xx_ota_module = {
    .init = gd32f4xx_ota_init,
    .ota_write = gd32f4xx_ota_write,
    .ota_read = gd32f4xx_ota_read,
    .ota_set_boot = gd32f4xx_ota_set_boot,
};
