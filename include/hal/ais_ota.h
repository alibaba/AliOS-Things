#ifndef _AIS_OTA_H_
#define _AIS_OTA_H_

#include <stdbool.h>
#include <bluetooth/bluetooth.h>

typedef enum {
    ALI_OTA_FLASH_ERASE_OK = 0,
    ALI_OTA_FLASH_ERASE_FAIL,
    ALI_OTA_FLASH_STORE_OK,
    ALI_OTA_FLASH_STORE_FAIL,
    ALI_OTA_FLASH_ERROR = 0xff
} ali_ota_flash_evt_t;

typedef enum {
    ALI_OTA_FLASH_CODE_SUCCESS,
    ALI_OTA_FLASH_CODE_ERROR
} ali_ota_flash_err_t;

typedef enum {
    ALI_OTA_SETTINGS_CODE_SUCCESS,
    ALI_OTA_SETTINGS_CODE_ERROR
} ali_ota_settings_err_t;

typedef void (*flash_event_handler_t)(ali_ota_flash_evt_t t);
typedef void (*settings_event_handler_t)(ali_ota_flash_evt_t t);

uint32_t ais_ota_get_setting_fw_offset();

void ais_ota_set_setting_fw_offset(uint32_t offset);

uint32_t ais_ota_get_page_size();

ali_ota_flash_err_t ais_ota_flash_erase(uint32_t const *addr, uint32_t num_pages, flash_event_handler_t cb);

ali_ota_flash_err_t ais_ota_flash_store(uint32_t const *addr, uint32_t const * p_data, uint16_t len, flash_event_handler_t cb);

void ais_ota_flash_init();

void ais_ota_settings_init();

uint32_t ais_ota_get_dst_addr();

ali_ota_settings_err_t ais_ota_settings_write(settings_event_handler_t cb);

bool ais_ota_check_if_resume(uint8_t * p_data, uint16_t length);

void ais_ota_update_fw_version(uint8_t * p_data, uint16_t length);

bool ais_ota_check_if_update_finished();

void ais_ota_update_settings_after_update_finished();

void ais_ota_update_setting_after_xfer_finished(uint32_t img_size, uint32_t img_crc);

int ais_ota_bt_storage_init();

int ais_ota_get_local_addr(bt_addr_le_t *addr);

#endif
