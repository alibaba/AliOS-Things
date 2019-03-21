#include "esp_ota_ops.h"
#include "ota_log.h"
#include "ota_hal_plat.h"
#include <string.h>
#include <malloc.h>
#include "esp_spi_flash.h"
#include "rom/spi_flash.h"
#include "aos/hal/flash.h"

static esp_partition_t  operate_partition;
static esp_ota_handle_t out_handle;
static esp_err_t esp_write_error;

extern void esp_restart(void);
extern const char *aos_get_app_version(void);

bool esp_ota_prepare()
{
    esp_err_t err;
    const esp_partition_t *esp_current_partition = esp_ota_get_boot_partition();
    if (esp_current_partition == NULL) {
        OTA_LOG_E("Got null partition.");
        return false;
    }
    if (esp_current_partition->type != ESP_PARTITION_TYPE_APP) {
        OTA_LOG_E("Err part type");
        return false;
    }
    esp_partition_t find_partition;
    /*choose which OTA image should we write to*/
    switch (esp_current_partition->subtype) {
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;
        case  ESP_PARTITION_SUBTYPE_APP_OTA_0:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
            break;
        case ESP_PARTITION_SUBTYPE_APP_OTA_1:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;
        default:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;
    }

    find_partition.type = ESP_PARTITION_TYPE_APP;
    const esp_partition_t *partition = esp_partition_find_first(find_partition.type, find_partition.subtype, NULL);
    assert(partition != NULL);
    memset(&operate_partition, 0, sizeof(esp_partition_t));

    OTA_LOG_I("ota_begin part type 0x%x sub:0x%x addr:0x%x size:0x%x label:%s encry:0x%x\n", partition->type, partition->subtype, partition->address, partition->size, partition->label, partition->encrypted);
    esp_write_error = ESP_OK;
    err = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &out_handle);
    if (err != ESP_OK) {
        OTA_LOG_E("begin err=0x%x!", err);
        return false;
    } else {
        memcpy(&operate_partition, partition, sizeof(esp_partition_t));
        OTA_LOG_I("ota init OK");
        return true;
    }
    return false;
}

static int ota_init(void *something)
{
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    uint32_t offset = param->off_bp;
    OTA_LOG_I("ota_init off:0x%x \n", offset);
    if(offset == 0) {
        /* prepare to os update  */
        if (esp_ota_prepare() != true) {
            return -1;
        }
    }
    return 0;
}

static int ota_write(int *off, char *buf, int buf_len)
{
    esp_err_t err = ESP_OK;
    err = esp_ota_write(out_handle, (const void *)buf, (size_t)buf_len);
    if (err != ESP_OK) {
        esp_write_error = err;
        OTA_LOG_E("write err=%x", err);
        return -1;
    }
    return 0;
}

static int ota_boot(void *something)
{
    esp_err_t err = ESP_OK;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL) {
        return -1;
    }
    if (param->res_type == OTA_FINISH) {
        if (esp_ota_end(out_handle) != ESP_OK) {
            OTA_LOG_E("end failed!");
            return -1;
        }
        if(esp_write_error > 0)  {
            OTA_LOG_E("write_error %d!", esp_write_error);
            return -1;
        }
        err = esp_ota_set_boot_partition(&operate_partition);
        if (err != ESP_OK) {
            OTA_LOG_E("set part failed! err=0x%x", err);
            return -1;
        }
        OTA_LOG_I("restart system!");
        esp_restart();
    }
    else if (param->res_type == OTA_BREAKPOINT) {

    }
    else {
        return -1;
    }

    return 0;
}

static int ota_read(int *off, char *out_buf, int out_buf_len)
{
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t *)off, out_buf, out_buf_len);
}

static int ota_rollback(void *something)
{
    return 0;
}

static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type > 0) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
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
