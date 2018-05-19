#include <stdint.h>
#include <hal/ais_ota.h>

#include "nordic_common.h"
//#include "sdk_macros.h"
#include "nrf_soc.h"
//#include "crc16.h"
//#include "crc32.h"
#include "nrf_dfu_types.h"
#include "nrf_dfu_flash.h"
#include "nrf_dfu_settings.h"

#define UPDATE_FINISHED         0xAB            /**< Information in s_dfu_settings.init_command[0], which indicates whether application upgrade has     been successful. */

static flash_event_handler_t flash_handler;
static settings_event_handler_t settings_hanlder;

uint32_t ais_ota_get_setting_fw_offset()
{
    return s_dfu_settings.progress.firmware_image_offset;
}

void ais_ota_set_setting_fw_offset(uint32_t offset)
{
    s_dfu_settings.progress.firmware_image_offset = offset;
}

uint32_t ais_ota_get_page_size()
{
    return CODE_PAGE_SIZE;
}

/**@brief Callback function for flash write/erase operations. */
static void flash_event_handler_helper(fs_evt_t const * const evt, fs_ret_t result)
{
    if (evt->id == FS_EVT_ERASE && result == FS_SUCCESS) {
        flash_handler(ALI_OTA_FLASH_ERASE_OK);
    } else if (evt->id == FS_EVT_STORE && result == FS_SUCCESS) {
        flash_handler(ALI_OTA_FLASH_STORE_OK);
    } else {
        flash_handler(ALI_OTA_FLASH_ERROR);
    }
}

ali_ota_flash_err_t ais_ota_flash_erase(uint32_t const *addr, uint32_t num_pages, flash_event_handler_t cb)
{
    uint32_t err_code;

    flash_handler = cb;
    err_code = nrf_dfu_flash_erase(addr, num_pages, flash_event_handler_helper);

    return err_code == FS_SUCCESS ? ALI_OTA_FLASH_CODE_SUCCESS : ALI_OTA_FLASH_CODE_ERROR;
}

ali_ota_flash_err_t ais_ota_flash_store(uint32_t const *addr, uint32_t uint32_t const * p_data, uint16_t len, flash_event_handler_t cb)
{
    uint32_t err_code;

    flash_handler = cb;
    err_code = nrf_dfu_flash_store(addr, p_data, len, flash_event_handler_helper);

    return err_code == FS_SUCCESS ? ALI_OTA_FLASH_CODE_SUCCESS : ALI_OTA_FLASH_CODE_ERROR;
}

void ais_ota_flash_init()
{
    nrf_dfu_flash_init(true);
}

void ais_ota_settings_init()
{
    nrf_dfu_settings_init();
}

uint32_t ais_ota_get_dst_addr()
{
    return CODE_REGION_1_START + align_to_page(s_dfu_settings.bank_0.image_size, ais_ota_get_page_size());
}

static void bootloader_settings_event_handler_helper(fs_evt_t const * const evt, fs_ret_t result)
{
    if (evt->id == FS_EVT_STORE && result == FS_SUCCESS) {
        settings_handler(ALI_OTA_FLASH_STORE_OK);
    } else {
        settings_handler(ALI_OTA_FLASH_STORE_FAIL);
    }
}

ali_ota_settings_err_t ais_ota_settings_write(settings_event_handler_t cb)
{
    uint32_t err_code;

    settings_handler = cb;
    err_code = nrf_dfu_settings_write(bootloader_settings_event_handler_helper);

    return err_code == NRF_SUCCESS ? ALI_OTA_SETTINGS_CODE_SUCCESS : ALI_OTA_SETTINGS_CODE_ERROR;
}

bool ais_ota_check_if_resume(uint8_t * p_data, uint16_t length)
{
    return memcmp(s_dfu_settings.init_command+1, p_data, length) == 0;
}

void ais_ota_update_fw_version(uint8_t * p_data, uint16_t length)
{
    memcpy(s_dfu_settings.init_command+1, p_data, length);
}

bool ais_ota_check_if_update_finished()
{
    return s_dfu_settings.init_command[0] == UPDATE_FINISHED;
}

void ais_ota_update_settings_after_update_finished()
{
    s_dfu_settings.init_command[0] = 0xFF;
    memset(s_dfu_settings.init_command+1, 0xFF, ALI_OTA_MAX_FW_VERSION_LEN + sizeof(uint16_t) + sizeof(uint32_t) + 1);
    s_dfu_settings.progress.firmware_image_offset = 0;
}

void ais_ota_update_setting_after_xfer_finished(uint32_t img_size, uint32_t img_crc)
{
    /* Update bootloader settings */
    s_dfu_settings.bank_current = 1;    // Swap bank becomes valid.
    s_dfu_settings.bank_1.image_size = img_size;
    s_dfu_settings.bank_1.bank_code  = NRF_DFU_BANK_VALID_APP;
    s_dfu_settings.bank_1.image_crc  = img_crc;
}
