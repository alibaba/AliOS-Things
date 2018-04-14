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
#include "nrf_fstorage.h"
#include <aos/aos.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/storage.h>
#include <bluetooth/hci.h>

#define UPDATE_FINISHED         0xAB            /**< Information in s_dfu_settings.init_command[0], which indicates whether application upgrade has     been successful. */

#ifndef ALI_OTA_MAX_FW_VERSION_LEN
#define ALI_OTA_MAX_FW_VERSION_LEN 8
#endif

#define SETTINGS_ADDR 0xff000

static flash_event_handler_t flash_handler;
static settings_event_handler_t settings_handler;

static uint32_t align_to_page(uint32_t val, uint32_t page_size)
{
    return ((val + page_size - 1 ) &~ (page_size - 1));
}

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
static void flash_event_handler_helper(nrf_fstorage_evt_t const * const evt)
{
    //LOGD("ble", "In %s, evt->id: %d, result: %d\r\n", __func__, evt->id, evt->result);
    if (evt->id == NRF_FSTORAGE_EVT_ERASE_RESULT && evt->result == NRF_SUCCESS) {
        if (flash_handler) flash_handler(ALI_OTA_FLASH_ERASE_OK);
    } else if (evt->id == NRF_FSTORAGE_EVT_WRITE_RESULT && evt->result == NRF_SUCCESS) {
        if (flash_handler) flash_handler(ALI_OTA_FLASH_STORE_OK);
    } else {
        if (flash_handler) flash_handler(ALI_OTA_FLASH_ERROR);
    }
}

ali_ota_flash_err_t ais_ota_flash_erase(uint32_t const *addr, uint32_t num_pages, flash_event_handler_t cb)
{
    uint32_t err_code;

    flash_handler = cb;
    err_code = nrf_dfu_flash_erase(addr, num_pages, flash_event_handler_helper);

    return err_code == NRF_SUCCESS ? ALI_OTA_FLASH_CODE_SUCCESS : ALI_OTA_FLASH_CODE_ERROR;
}

ali_ota_flash_err_t ais_ota_flash_store(uint32_t const *addr, uint32_t const * p_data, uint16_t len, flash_event_handler_t cb)
{
    uint32_t err_code;

    flash_handler = cb;
    err_code = nrf_dfu_flash_store(addr, p_data, len, flash_event_handler_helper);

    return err_code == NRF_SUCCESS ? ALI_OTA_FLASH_CODE_SUCCESS : ALI_OTA_FLASH_CODE_ERROR;
}

void ais_ota_flash_init()
{
    nrf_dfu_flash_init(false);
}

void ais_ota_settings_init()
{
    nrf_dfu_settings_init(false);
}

#define BOOTLOADER_SIZE 0x10000 //64k
uint32_t ais_ota_get_dst_addr()
{
    //return CODE_REGION_1_START + align_to_page(s_dfu_settings.bank_0.image_size, ais_ota_get_page_size()) + 0x1000;
    // The 0x1000 fixes up the nrfutil tool issue.
    return BOOTLOADER_SIZE + align_to_page(s_dfu_settings.bank_0.image_size, ais_ota_get_page_size()) + 0x1000;
}

static void bootloader_settings_event_handler_helper(nrf_fstorage_evt_t const * const evt)
{
    if (evt->id == NRF_FSTORAGE_EVT_WRITE_RESULT && evt->result == NRF_SUCCESS) {
        if (settings_handler) settings_handler(ALI_OTA_FLASH_STORE_OK);
    } else {
        if (settings_handler) settings_handler(ALI_OTA_FLASH_STORE_FAIL);
    }
}

ret_code_t ota_dfu_settings_write()
{
    ret_code_t err_code;
    //LOG("Writing settings...");
    //LOG("Erasing old settings at: 0x%08x", (uint32_t)m_dfu_settings_buffer);

    // Not setting the callback function because ERASE is required before STORE
    // Only report completion on successful STORE.
    err_code = nrf_dfu_flash_erase((uint32_t)SETTINGS_ADDR, 1, NULL);

    if (err_code != NRF_SUCCESS)
    {
        LOG("Could not erase the settings page!");
        return NRF_ERROR_INTERNAL;
    }

    s_dfu_settings.crc = nrf_dfu_settings_crc_get();

    static nrf_dfu_settings_t temp_dfu_settings;
    memcpy(&temp_dfu_settings, &s_dfu_settings, sizeof(nrf_dfu_settings_t));

    err_code = nrf_dfu_flash_store((uint32_t)SETTINGS_ADDR,
                                   &temp_dfu_settings,
                                   sizeof(nrf_dfu_settings_t),
                                   NULL);

    if (err_code != NRF_SUCCESS)
    {
        LOG("Could not write the DFU settings page!");
        return NRF_ERROR_INTERNAL;
    }

    return NRF_SUCCESS;
}

ali_ota_settings_err_t ais_ota_settings_write(settings_event_handler_t cb)
{
    uint32_t err_code;

    settings_handler = cb;
    //err_code = nrf_dfu_settings_write(bootloader_settings_event_handler_helper);
    err_code = ota_dfu_settings_write();

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

static ssize_t storage_write(const bt_addr_le_t *addr, u16_t key,
                             const void *data, size_t length)
{
    uint32_t err_code;
    uint8_t settings_content[CODE_PAGE_SIZE], *mac = (uint8_t *)(SETTINGS_ADDR + 0x1000 - 6);

    memcpy(settings_content, SETTINGS_ADDR, sizeof(nrf_dfu_settings_t));
    err_code = nrf_dfu_flash_erase(SETTINGS_ADDR, 1, NULL);
    if (err_code != NRF_SUCCESS) {
        return 0;
    } else {
        memcpy(SETTINGS_ADDR, settings_content, sizeof(nrf_dfu_settings_t));
        memcpy(mac, ((bt_addr_le_t *)data)->a.val, 6);
        return sizeof(bt_addr_le_t);
    }
}

static ssize_t storage_read(const bt_addr_le_t *addr, u16_t key, void *data,
                            size_t length)
{
    /* MAC[6] is stored at the end of settings page. */
    uint8_t *mac = (uint8_t *)(SETTINGS_ADDR + 0x1000 - 6);

    if (key != BT_STORAGE_ID_ADDR || !data) return 0;

    if (mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && \ 
        mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) {
        return 0;
    } else {
        memcpy(((bt_addr_le_t *)data)->a.val, mac, 6);
        return sizeof(bt_addr_le_t);
    }
}

static int storage_clear(const bt_addr_le_t *addr)
{

}

int ais_ota_bt_storage_init()
{
        static const struct bt_storage storage = {
                .read  = storage_read,
                .write = storage_write,
                .clear = storage_clear
        };

        nrf_dfu_flash_init(false);
        bt_storage_register(&storage);

        return 0;
}

