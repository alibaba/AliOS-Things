
#include <string.h>
#include <malloc.h>
#include "hal/ota.h"
#include "esp_common.h"
#include "upgrade.h"
#include "hal/soc/flash.h"
#include <CheckSumUtils.h>
#include "rec_hal.h"

static const char *TAG = "esp8266_ota";
static int esp_write_error;

extern void hal_reboot(void);
extern void esp_restart(void);
extern void system_upgrade_erase();
extern void system_upgrade_deinit();
extern uint16 system_get_fw_start_sec();
extern bool upgrade_crc_check(uint16 fw_bin_sec, unsigned int sumlength);
extern void rec_start(void);
/*********************global param define start ******************************/
LOCAL uint32 totallength = 0;
//LOCAL uint32 sumlength = 0;
LOCAL bool flash_erased = false;
/*********************global param define end *******************************/

typedef PatchStatus ota_hdl_t;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
    uint32_t ota_type;
    uint32_t update_type;
    uint32_t splict_size;
    uint8_t  diff_version;
    char sign_enable;
    char hash_method;
    int sign_bitnumb;
    unsigned char sign_value[256];
} ota_reboot_info_t;

__attribute__((section(".bss"))) ota_hdl_t ota_hdl,ota_hdl_rb;
int hal_ota_switch_to_new_fw(ota_reboot_info_t *ota_info_t)
{
    if(!ota_info_t) {
        LOG("hal_ota_switch_to_new_fw ota_info_t invalid");
        return -1;
    }
    uint32_t offset;
    hal_logic_partition_t* ota_partition;
    int parti;

    ota_partition = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
    memset( &ota_hdl, 0x00, sizeof(ota_hdl_t) );
    if (ota_info_t->ota_type == OTA_DIFF) {
        ota_hdl.dst_adr = HAL_PARTITION_APPLICATION;
        ota_hdl.src_adr = HAL_PARTITION_OTA_TEMP;
        ota_hdl.siz = 0;
        ota_hdl.crc = ota_info_t->ota_crc;
        ota_hdl.patch_size = ota_info_t->ota_len;
        ota_hdl.splict_size = ota_info_t->splict_size;
        ota_hdl.diff_version = (ota_info_t->ota_type == OTA_DIFF)?1:0;
        CRC16_Context contex;
        uint16_t crc;
        CRC16_Init(&contex);
        CRC16_Update(&contex, &ota_hdl, sizeof(ota_hdl_t) - sizeof(uint16_t));
        CRC16_Final(&contex, &crc);
        ota_hdl.patch_crc = crc;
        /*TODO: set HAL_PARTITION_PARAMETER_1 will cause parm2 data clear, this bug need be fixed later ;
        so set HAL_PARTITION_PARAMETER_3 temply */
        parti = HAL_PARTITION_PARAMETER_1;
    }
    else {

    }

    LOG("OTA des:0x%08x, src:0x%08x, size:0x%08x, CRC:0x%04x patch_CRC:0x%04x patch.size:%d splict:%d diff:%d \r\n",
    ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.siz, ota_hdl.crc, ota_hdl.patch_crc, ota_hdl.patch_size, ota_hdl.splict_size, ota_hdl.diff_version );

    offset = 0x00;
    hal_flash_erase( parti, offset, sizeof(ota_hdl_t) );

    offset = 0x00;
    hal_flash_write( parti, &offset, (const void *)&ota_hdl, sizeof(ota_hdl_t));

    offset = 0x00;
    memset(&ota_hdl_rb, 0, sizeof(ota_hdl_t));
    hal_flash_read( parti, &offset, &ota_hdl_rb, sizeof(ota_hdl_t));

    if(memcmp(&ota_hdl, &ota_hdl_rb, sizeof(ota_hdl_t)) != 0)
    {
        LOG("OTA header compare failed, OTA destination = 0x%08x, source address = 0x%08x, size = 0x%08x, CRC = 0x%04x\r\n",
        ota_hdl_rb.dst_adr, ota_hdl_rb.src_adr, ota_hdl_rb.siz, ota_hdl_rb.crc);
        return -1;
    }

    /* reboot */
    if (ota_info_t->ota_type == OTA_DIFF) {
        rec_start();
    }else {
        hal_reboot();
    }
    hal_reboot();
    return 0;
}

void ota_set_param(uint32_t ota_len, uint32_t ota_crc, uint32_t splict_size )
{
    ota_reboot_info_t ota_info;

    memset(&ota_info, 0, sizeof(ota_reboot_info_t));
    ota_info.ota_len = ota_len;
    ota_info.ota_crc = ota_crc;

    ota_info.ota_type = OTA_DIFF;
    ota_info.splict_size = splict_size;

    hal_ota_switch_to_new_fw(&ota_info);
}

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
