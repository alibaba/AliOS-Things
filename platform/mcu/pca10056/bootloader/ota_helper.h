#ifndef _OTA_HELPER_H_
#define _OTA_HELPER_H_

#include <stdint.h>
#include <stddef.h>

#define BTL_OFFSET 0
#define BTL_SIZE 0x10000 // 64k
#define SETTINGS_OFFSET 0xFF000
#define SETTINGS_SIZE 0x1000 // 4k

#define MAIN_APPLICATION_START_ADDR (BTL_OFFSET+BTL_SIZE)

#define PAGE_SIZE 0x1000
#define PROGRAM_UNIT 4
#define ERASE_UNIT PAGE_SIZE
#define INIT_COMMAND_MAX_SIZE       256     /**< Maximum size of the init command stored in dfu_settings. */

#define NRF_DFU_CURRENT_BANK_0 0x00
#define NRF_DFU_CURRENT_BANK_1 0x01

#define NRF_DFU_BANK_LAYOUT_DUAL   0x00
#define NRF_DFU_BANK_LAYOUT_SINGLE 0x01

#define UPDATE_FINISHED 0xAB

/** @brief DFU bank state codes.
 *
 * @details The DFU bank state indicates the content of a bank:
 *          A valid image of a certain type or an invalid image.
 */

#define NRF_DFU_BANK_INVALID     0x00 /**< Invalid image. */
#define NRF_DFU_BANK_VALID_APP   0x01 /**< Valid application. */
#define NRF_DFU_BANK_VALID_SD    0xA5 /**< Valid SoftDevice. */
#define NRF_DFU_BANK_VALID_BL    0xAA /**< Valid bootloader. */
#define NRF_DFU_BANK_VALID_SD_BL 0xAC /**< Valid SoftDevice and bootloader. */

    // Dummy type for nrf_dfu_peer_data_t if DFU SVCI is not in use
    typedef struct
    {
        uint32_t crc;
    } nrf_dfu_peer_data_t;

    // Dummy type for nrf_dfu_adv_name_t if DFU SVCI is not in use
    typedef struct
    {
        uint32_t crc;
    } nrf_dfu_adv_name_t;

        typedef struct
        {
            uint32_t firmware_image_crc;        /**< CRC value of the current firmware (continuously calculated as data is received). */
            uint32_t firmware_image_crc_last;   /**< The CRC of the last executed object. */
            uint32_t firmware_image_offset;     /**< The offset of the current firmware image being transferred. Note that this offset is the offset in the entire firmware image and not only the current object. */
            uint32_t firmware_image_offset_last;/**< The offset of the last executed object from the start of the firmware image. */
        } firmware_image_info_t;
		
/**@brief DFU progress.
 *
 * Be aware of the difference between objects and firmware images. A firmware image consists of multiple objects, each of a maximum size @ref DATA_OBJECT_MAX_SIZE.
 * 
 * @note The union inside this struct is cleared when CREATE_OBJECT of command type is executed, and when there is a valid post-validation.
 *       In DFU activation (after reset) the @ref sd_start_address will be used in case of a SD/SD+BL update.
 */
//ANON_UNIONS_ENABLE
typedef struct
{
    uint32_t command_size;              /**< The size of the current init command stored in the DFU settings. */
    uint32_t command_offset;            /**< The offset of the currently received init command data. The offset will increase as the init command is received. */
    uint32_t command_crc;               /**< The calculated CRC of the init command (calculated after the transfer is completed). */
    uint32_t data_object_size;          /**< The size of the last object created. Note that this size is not the size of the whole firmware image.*/
	  firmware_image_info_t fw_info;
} dfu_progress_t;
//ANON_UNIONS_DISABLE

/** @brief Description of a single bank. */
#pragma pack(4)
typedef struct
{
    uint32_t                image_size;         /**< Size of the image in the bank. */
    uint32_t                image_crc;          /**< CRC of the image. If set to 0, the CRC is ignored. */
    uint32_t                bank_code;          /**< Identifier code for the bank. */
} nrf_dfu_bank_t;

typedef struct
{
    uint32_t            crc;                /**< CRC for the stored DFU settings, not including the CRC itself. If 0xFFFFFFF, the CRC has never been calculated. */
    uint32_t            settings_version;   /**< Version of the currect DFU settings struct layout. */
    uint32_t            app_version;        /**< Version of the last stored application. */
    uint32_t            bootloader_version; /**< Version of the last stored bootloader. */

    uint32_t            bank_layout;        /**< Bank layout: single bank or dual bank. This value can change. */
    uint32_t            bank_current;       /**< The bank that is currently used. */

    nrf_dfu_bank_t      bank_0;             /**< Bank 0. */
    nrf_dfu_bank_t      bank_1;             /**< Bank 1. */

    uint32_t            write_offset;       /**< Write offset for the current operation. */
    uint32_t            sd_size;            /**< SoftDevice size (if combined BL and SD). */

    dfu_progress_t      progress;           /**< Current DFU progress. */

    uint32_t            enter_buttonless_dfu;
    uint8_t             init_command[INIT_COMMAND_MAX_SIZE];  /**< Buffer for storing the init command. */

    nrf_dfu_peer_data_t peer_data;          /**< Not included in calculated CRC. */
    nrf_dfu_adv_name_t  adv_name;           /**< Not included in calculated CRC. */
    uint8_t             mac[6];
} nrf_dfu_settings_t;

#define DFU_SETTINGS_INIT_COMMAND_OFFSET        offsetof(nrf_dfu_settings_t, init_command)          //<! Offset in the settings struct where the InitCommand is located.

#endif
