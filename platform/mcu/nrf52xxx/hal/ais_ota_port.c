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

/**< Information in s_dfu_settings.init_command[0], which indicates whether application upgrade has been successful. */
#define UPDATE_FINISHED            0xAB
#ifdef AOS_BINS
#define UPDATE_ONGOING             0xBC /* Single bin update onging flag */
#define BINS_KERNEL_UPDATE_ONGOING 0xCD /* Multiple BINS kernel bin update ongoing flag */
#define BINS_APP_UPDATE_ONGOING    0xDE /* Multiple BINS app bin update onging flag*/
#endif

#ifndef ALI_OTA_MAX_FW_VERSION_LEN
#define ALI_OTA_MAX_FW_VERSION_LEN 8
#endif

/* Flash memory layout */
#define BOOTLOADER_OFFSET 0
#define BOOTLOADER_SIZE 0x10000 //64k
#ifdef AOS_BINS
#define BIN_OFFSET 0x10000
#define BIN_SIZE 0x50000
#define BIN2_OFFSET 0x60000
#define BIN2_SIZE 0x20000
#define OTA_OFFSET 0x80000
#define OTA_SIZE 0x7f000
#endif
#ifdef BOARD_PCA10056
#define SETTINGS_ADDR 0xff000
#define SETTINGS_SIZE 0x1000 // 4k
#else 
#ifdef BOARD_PCA10040
#define SETTINGS_ADDR 0x7F000
#define SETTINGS_SIZE 0x1000 //4K
#endif
#endif 

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
    static uint8_t flash_inited = 0;

    if (!flash_inited) {
        nrf_dfu_flash_init(false);
        flash_inited = 1;
    }
}

void ais_ota_settings_init()
{
    static uint8_t settings_inited = 0;

    if (!settings_inited) {
        nrf_dfu_settings_init(false);
        settings_inited = 1;
    }
}

uint32_t ais_ota_get_dst_addr()
{
#ifdef AOS_BINS
    return OTA_OFFSET;
#else
    //return CODE_REGION_1_START + align_to_page(s_dfu_settings.bank_0.image_size, ais_ota_get_page_size()) + 0x1000;
    // The 0x1000 fixes up the nrfutil tool issue.
    return BOOTLOADER_SIZE + align_to_page(s_dfu_settings.bank_0.image_size, ais_ota_get_page_size()) + 0x1000;
#endif
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
    LOG("Writing settings...");

    // Not setting the callback function because ERASE is required before STORE
    // Only report completion on successful STORE.
    err_code = nrf_dfu_flash_erase((uint32_t)SETTINGS_ADDR, 1, NULL);

    if (err_code != NRF_SUCCESS)
    {
        LOG("Could not erase the settings page!");
        return NRF_ERROR_INTERNAL;
    }

    s_dfu_settings.crc = nrf_dfu_settings_crc_get();

    nrf_dfu_settings_t temp_dfu_settings;
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
    /* Bootload will update this field after OTA copy finished. */
    return s_dfu_settings.init_command[0] == UPDATE_FINISHED;
}

#ifdef AOS_BINS
bool ais_ota_check_if_bins_kernel_update_ongoing()
{
    return s_dfu_settings.init_command[0] == BINS_KERNEL_UPDATE_ONGOING;
}

bool ais_ota_check_if_bins_app_update_ongoing()
{
    return s_dfu_settings.init_command[0] == BINS_APP_UPDATE_ONGOING;
}

bool ais_ota_check_if_update_ongoing()
{
    return s_dfu_settings.init_command[0] == UPDATE_ONGOING;
}

void ais_ota_set_upgrade_bin_type_info(ali_ota_bin_type_t type)
{
    if (type == ALI_OTA_BIN_TYPE_APP)
        s_dfu_settings.init_command[0] = BINS_APP_UPDATE_ONGOING;
    else if (type == ALI_OTA_BIN_TYPE_KERNEL)
        s_dfu_settings.init_command[0] = BINS_KERNEL_UPDATE_ONGOING;
    else if (type == ALI_OTA_BIN_TYPE_SINGLE)
        s_dfu_settings.init_command[0] = UPDATE_ONGOING;
}
#endif

/*
 * This API updates below informations in settings:
 *   1. Clear upgrade udergoing flag (i.e. init_command[0] field in our case).
 *   2. Clear upgrade version information (i.e. init_command[1-len] in our case).
 *   3. Clear upgrade progress informations (i.e. firmware_image_offset in our case).
 */
void ais_ota_update_settings_after_update_finished()
{
    /* Update bootloader settings after the whole upgrade process completed. */
    s_dfu_settings.init_command[0] = 0xFF;
    memset(s_dfu_settings.init_command+1, 0xFF, ALI_OTA_MAX_FW_VERSION_LEN + sizeof(uint16_t) + sizeof(uint32_t) + 1);
    s_dfu_settings.progress.firmware_image_offset = 0;
}

/*
 * This API updates upgrade bank information.
 */
void ais_ota_update_setting_after_xfer_finished(uint32_t img_size, uint32_t img_crc)
{
#ifdef AOS_BINS
    if (ais_ota_check_if_bins_app_update_ongoing()) {
        s_dfu_settings.bank_app_current = 1;    // Swap bank becomes valid.
        s_dfu_settings.bank_app_1.image_size = img_size;
        s_dfu_settings.bank_app_1.bank_code  = NRF_DFU_BANK_VALID_APP;
        s_dfu_settings.bank_app_1.image_crc  = img_crc;
    } else
#endif
    {
        /* Update bootloader settings  after the file transfter/download complted. */
        s_dfu_settings.bank_current = 1;    // Swap bank becomes valid.
        s_dfu_settings.bank_1.image_size = img_size;
        s_dfu_settings.bank_1.bank_code  = NRF_DFU_BANK_VALID_APP;
        s_dfu_settings.bank_1.image_crc  = img_crc;
    }
}

static ssize_t storage_write(const bt_addr_le_t *addr, u16_t key,
                             const void *data, size_t length)
{
    uint8_t *mac = s_dfu_settings.mac;
    uint32_t err_code;

    memcpy(mac, ((bt_addr_le_t *)data)->a.val, sizeof(bt_addr_le_t));
    err_code = ota_dfu_settings_write();
    if (err_code != NRF_SUCCESS) {
        printf("%s failed.\r\n", __func__);
        return 0;
    }

    return sizeof(bt_addr_le_t);
}

static ssize_t storage_read(const bt_addr_le_t *addr, u16_t key, void *data,
                            size_t length)
{
    /* MAC[6] is stored at the end of settings struct. */
    uint8_t *mac = s_dfu_settings.mac;

    if (key != BT_STORAGE_ID_ADDR || !data) return 0;

    mac = s_dfu_settings.mac;

    if ((mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && \ 
        mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) ||
        (mac[0] == 0x00 && mac[1] == 0x00 && mac[2] == 0x00 && \ 
        mac[3] == 0x00 && mac[4] == 0x00 && mac[5] == 0x00)) {
        printf("%s: no valid mac read\r\n", __func__);
        return 0;
    } else {
        memcpy(((bt_addr_le_t *)data)->a.val, mac, 6);
        printf("%s: valid mac read - 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\r\n",
               __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return sizeof(bt_addr_le_t);
    }
}

static int storage_clear(const bt_addr_le_t *addr)
{

}

int ais_ota_bt_storage_init(void)
{
        static const struct bt_storage storage = {
                .read  = storage_read,
                .write = storage_write,
                .clear = storage_clear
        };

        ais_ota_flash_init();
        ais_ota_settings_init();
        bt_storage_register(&storage);

        return 0;
}

int ais_ota_get_local_addr(bt_addr_le_t *addr)
{
    struct bt_le_oob oob;
    if (!addr) return;

    if (bt_le_oob_get_local(&oob) != 0) {
        printf("Failed to get ble local address.\r\n");
        return -1;
    }

    memcpy(addr, &(oob.addr), sizeof(oob.addr));

    return 0;
}

bool ais_ota_check_if_bins_supported()
{
    return s_dfu_settings.bins_flag == 1 ? true : false;
}
