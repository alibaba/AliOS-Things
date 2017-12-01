#include "esp_ota_ops.h"
#include "esp_log.h"
#include <string.h>
#include <malloc.h>

#include "esp_spi_flash.h"
#include "rom/spi_flash.h"
#include "hal/ota.h"

static const char *TAG = "esp32_ota";
static esp_partition_t  operate_partition;
static esp_ota_handle_t out_handle;
static esp_err_t esp_write_error;

extern void esp_restart(void);

bool esp_ota_prepare()
{
    esp_err_t err;
    const esp_partition_t *esp_current_partition = esp_ota_get_boot_partition();

    if (esp_current_partition == NULL) {
        ESP_LOGE(TAG, "esp_ota_get_boot_partition got null prt");
        return false;
    }

    if (esp_current_partition->type != ESP_PARTITION_TYPE_APP) {
        ESP_LOGE(TAG, "Error esp_current_partition->type != ESP_PARTITION_TYPE_APP");
        return false;
    }

    esp_partition_t find_partition;

    /*choose which OTA image should we write to*/
    switch (esp_current_partition->subtype) {
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:

            ESP_LOGE(TAG, "esp_ota ESP_PARTITION_SUBTYPE_APP_FACTORY!");
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;

        case  ESP_PARTITION_SUBTYPE_APP_OTA_0:

            ESP_LOGE(TAG, "esp_ota ESP_PARTITION_SUBTYPE_APP_OTA_0!");
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
            break;

        case ESP_PARTITION_SUBTYPE_APP_OTA_1:

            ESP_LOGE(TAG, "esp_ota ESP_PARTITION_SUBTYPE_APP_OTA_1!");
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;

        default:

            ESP_LOGE(TAG, "esp_ota cur partition=%d!",esp_current_partition->subtype);
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
            break;
    }

    find_partition.type = ESP_PARTITION_TYPE_APP;

    const esp_partition_t *partition = esp_partition_find_first(find_partition.type, find_partition.subtype, NULL);
    assert(partition != NULL);
    memset(&operate_partition, 0, sizeof(esp_partition_t));

    ESP_LOGI(TAG, "esp_ota_begin partition type 0x%x!", partition->type);
    ESP_LOGI(TAG, "esp_ota_begin partition subtype 0x%x!", partition->subtype);
    ESP_LOGI(TAG, "esp_ota_begin partition address 0x%x!", partition->address);
    ESP_LOGI(TAG, "esp_ota_begin partition size 0x%x!", partition->size);
    ESP_LOGI(TAG, "esp_ota_begin partition label %s!", partition->label);
    ESP_LOGI(TAG, "esp_ota_begin partition encrypted 0x%x!", partition->encrypted);
    esp_write_error = ESP_OK;
    err = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &out_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed err=0x%x!", err);
        return false;
    } else {
        memcpy(&operate_partition, partition, sizeof(esp_partition_t));
        ESP_LOGI(TAG, "esp_ota_begin init OK");
        return true;
    }

    return false;
}

static int esp_ota_init(hal_ota_module_t *m, void *something)
{    
        LOG("esp_ota_init\n");
        uint32_t offset = *(uint32_t*)something;

        if(offset==0) {
            /* prepare to os update  */
            if (esp_ota_prepare() != true) { 
                return -1; 
            }
            
        }
        else {

        }
    
        return 0;
}

static int esp_ota_write_ota_cb(hal_ota_module_t *m, volatile uint32_t* off_set,uint8_t *buf,uint32_t buf_len)
{
    esp_err_t         err = ESP_OK;

    err = esp_ota_write(out_handle, (const void *)buf, (size_t)buf_len);

    if (err != ESP_OK) {
        esp_write_error = err;
        ESP_LOGE(TAG, "Error: esp_ota_write failed! err=%x", err);
        return -1;
    }

    return 0;
}

static int esp_ota_finish_cb(hal_ota_module_t *m, void *something)
{
    esp_err_t         err = ESP_OK;
    ota_finish_param_t *param = (ota_finish_param_t *)something;
    if (param==NULL){
        return -1;
    }
    if (param->result_type==OTA_FINISH) {
        if (esp_ota_end(out_handle) != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_end failed!");
            return -1;
        }

        if(esp_write_error )  {
            ESP_LOGE(TAG, "esp_ota_write_ota_cb write_error %d!",esp_write_error);
            return -1;
        }
        err = esp_ota_set_boot_partition(&operate_partition);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
            return -1;
        }

        ESP_LOGI(TAG, "Prepare to restart system!");
        esp_restart();
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
struct hal_ota_module_s esp32_yos_ota_module = {
    .init         = esp_ota_init,
    .ota_write    = esp_ota_write_ota_cb,
    .ota_read     = esp_ota_read_ota_cb,
    .ota_set_boot = esp_ota_finish_cb,
};
