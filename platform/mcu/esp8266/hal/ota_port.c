
#include <string.h>
#include <malloc.h>
#include "hal/ota.h"
#include "esp_common.h"
#include "upgrade.h"

static const char *TAG = "esp8266_ota";
//static esp_partition_t  operate_partition;
//static esp_ota_handle_t out_handle;
static int esp_write_error;

extern void esp_restart(void);

extern  void system_upgrade_erase();
/*********************global param define start ******************************/
LOCAL uint32 totallength = 0;
//LOCAL uint32 sumlength = 0;
LOCAL bool flash_erased = false;
/*********************global param define end *******************************/

/******************************************************************************
 * FunctionName : upgrade_recycle
 * Description  : recyle upgrade task, if OTA finish switch to run another bin
 * Parameters   :
 * Returns      : none
 *******************************************************************************/
LOCAL void upgrade_recycle(void)
{
    totallength = 0;
    //sumlength = 0;
    flash_erased = false;

    system_upgrade_deinit();
   // vTaskDelete(ota_task_handle);
    //ota_task_handle = NULL;
    if (system_upgrade_flag_check() == UPGRADE_FLAG_FINISH) {
        system_upgrade_reboot(); // if need
    }

}


static int esp_ota_init(hal_ota_module_t *m, void *something)
{    
        LOG("esp_ota_init\n");
        //uint32_t offset = *(uint32_t*)something;
        
        system_upgrade_flag_set(UPGRADE_FLAG_START);
        
        system_upgrade_init();
        system_upgrade_erase();
        return 0;
}

static int esp_ota_write_ota_cb(hal_ota_module_t *m, volatile uint32_t* off_set,uint8_t *buf,uint32_t buf_len)
{
    int         err = 0;
    totallength+=buf_len;
    if (false == system_upgrade(buf, buf_len)) {
    //err = esp_ota_write(out_handle, (const void *)buf, (size_t)buf_len);
   // if (err != 0) {
        esp_write_error = err;
        system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
        upgrade_recycle();
        LOGE(TAG, "======Error: esp_ota_write failed! err=%x", err);
        return -1;
    }

    return 0;
}

static int esp_ota_finish_cb(hal_ota_module_t *m, void *something)
{
    int         err = 0;
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH) {
        if (upgrade_crc_check(system_get_fw_start_sec(), totallength) != true) {
            printf("===upgrade crc check failed !===\n");
            system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
            
        }else{
            system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
        }
        upgrade_recycle();

    }
    else if (param->result_type==OTA_BREAKPOINT) {

    }
    else {
        return -1;
    }

    return 0;
}

static int esp_ota_read_ota_cb(hal_ota_module_t *m,  volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    return 0;
}
struct hal_ota_module_s esp8266_ota_module = {
    .init         = esp_ota_init,
    .ota_write    = esp_ota_write_ota_cb,
    .ota_read     = esp_ota_read_ota_cb,
    .ota_set_boot = esp_ota_finish_cb,
};
