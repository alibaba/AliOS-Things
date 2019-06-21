#include "esp_ota_ops.h"
#include <string.h>
#include <malloc.h>
#include "esp_spi_flash.h"
#include "rom/spi_flash.h"
#include "aos/hal/flash.h"
#include "ota/ota_service.h"

static esp_partition_t  operate_partition;
static esp_ota_handle_t out_handle;
static esp_err_t esp_write_error;
extern void esp_restart(void);

bool esp_ota_prepare()
{
    esp_err_t err;
    const esp_partition_t *esp_current_partition = esp_ota_get_boot_partition();
    if (esp_current_partition == NULL) {
        printf("Got null partition.");
        return false;
    }
    if (esp_current_partition->type != ESP_PARTITION_TYPE_APP) {
        printf("Err part type");
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

    printf("ota_begin part type 0x%x sub:0x%x addr:0x%x size:0x%x label:%s encry:0x%x\n", partition->type,
             partition->subtype, partition->address, partition->size, partition->label, partition->encrypted);
    esp_write_error = ESP_OK;
    err = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &out_handle);
    if (err != ESP_OK) {
        printf("begin err=0x%x!", err);
        return false;
    } else {
        memcpy(&operate_partition, partition, sizeof(esp_partition_t));
        printf("ota init OK");
        return true;
    }
    return false;
}

int ota_hal_init(ota_boot_param_t *param)
{
    if (param == NULL) {
        return OTA_INIT_FAIL;
    }
    printf("esp32 ota init.\n");
    /* prepare to os update  */
    if (esp_ota_prepare() != true) {
            return OTA_INIT_FAIL;
    }
    return 0;
}

int ota_hal_write(unsigned int *off_set, char *in_buf, unsigned int in_buf_len)
{
    esp_err_t err = ESP_OK;
    err = esp_ota_write(out_handle, (const void *)in_buf, (size_t)in_buf_len);
    if (err != ESP_OK) {
        esp_write_error = err;
        printf("write err=%x", err);
        return OTA_INIT_FAIL;
    }
    return 0;
}

int ota_hal_boot(ota_boot_param_t *param)
{
    esp_err_t err = ESP_OK;
    if (param == NULL) {
        return -1;
    }
    if (param->upg_status == OTA_FINISH) {
        if (esp_ota_end(out_handle) != ESP_OK) {
            printf("end failed!");
            return OTA_INIT_FAIL;
        }
        if(esp_write_error > 0)  {
            printf("write_error %d!", esp_write_error);
            return OTA_INIT_FAIL;
        }
        err = esp_ota_set_boot_partition(&operate_partition);
        if (err != ESP_OK) {
            printf("set part failed! err=0x%x", err);
            return OTA_INIT_FAIL;
        }
        printf("restart system!");
        esp_restart();
    }
    else {
        return OTA_INIT_FAIL;
    }

    return 0;
}

int ota_hal_read(unsigned int *off_set, char *out_buf, unsigned int out_buf_len)
{
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t *)off_set, out_buf, out_buf_len);
}

int ota_hal_rollback(void)
{
    return 0;
}

const char *ota_get_version(unsigned char dev_type, char* dn)
{
    if(dev_type > 0) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return SYSINFO_APP_VERSION;
    }
}
