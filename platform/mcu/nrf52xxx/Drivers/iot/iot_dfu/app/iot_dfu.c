/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdint.h>
#include <string.h>
#include "iot_dfu.h"
#include "iot_file.h"
#include "iot_file_pstorage_raw.h"
#include "dfu_types.h"
#include "crc16.h"
#include "nrf_delay.h"

/**
 * @defgroup iot_sdk_dfu Module's Log Macros
 * @details Macros used for creating module logs which can be useful in understanding handling
 *          of events or actions on API requests. These are intended for debugging purposes and
 *          can be enabled by defining the IOT_DFU_ENABLE_LOGS.
 * @note That if ENABLE_DEBUG_LOG_SUPPORT is disabled, having IOT_DFU_ENABLE_LOGS has no effect.
 * @{
 */
#if (IOT_DFU_ENABLE_LOGS == 1)

#define IOT_DFU_TRC  app_trace_log                                                                  /**< Used for logging details. */
#define IOT_DFU_ERR  app_trace_log                                                                  /**< Used for logging errors in the module. */
#define IOT_DFU_DUMP app_trace_dump                                                                 /**< Used for dumping octet information to get details of bond information etc. */

#else  // IOT_DFU_ENABLE_LOGS

#define IOT_DFU_TRC(...)                                                                            /**< Diasbles detailed logs. */
#define IOT_DFU_ERR(...)                                                                            /**< Diasbles error logs. */
#define IOT_DFU_DUMP(...)                                                                           /**< Diasbles dumping of octet streams. */

#endif  // IOT_DFU_ENABLE_LOGS
/** @} */

/**
 * @defgroup api_param_check API Parameters check macros.
 *
 * @details Macros that verify parameters passed to the module in the APIs. These macros
 *          could be mapped to nothing in final versions of code to save execution and size.
 *          IOT_DFU_DISABLE_API_PARAM_CHECK should be set to 0 to enable these checks.
 *
 * @{
 */

#if (IOT_DFU_DISABLE_API_PARAM_CHECK == 0)

/**@brief Macro to check is module is initialized before requesting one of the module procedures. */
#define VERIFY_MODULE_IS_INITIALIZED()                              \
    if (m_settings.event_handler == false)                          \
    {                                                               \
        return (SDK_ERR_MODULE_NOT_INITIALZED | IOT_DFU_ERR_BASE);  \
    }

/**@brief Verify NULL parameters are not passed to API by application. */
#define NULL_PARAM_CHECK(PARAM)                                     \
    if ((PARAM) == NULL)                                            \
    {                                                               \
        return (NRF_ERROR_NULL | IOT_DFU_ERR_BASE);                 \
    }

#else // IOT_DFU_DISABLE_API_PARAM_CHECK

#define VERIFY_MODULE_IS_INITIALIZED()
#define NULL_PARAM_CHECK(PARAM)

#endif // IOT_DFU_DISABLE_API_PARAM_CHECK
/** @} */


/**
 * @defgroup iot_dfu_mutex_lock_unlock Module's Mutex Lock/Unlock Macros.
 *
 * @details Macros used to lock and unlock modules. Currently, SDK does not use mutexes but
 *          framework is provided in case need arises to use an alternative architecture.
 * @{
 */
#define IOT_DFU_MUTEX_LOCK()   SDK_MUTEX_LOCK(m_iot_dfu_mutex)                                      /**< Lock module using mutex */
#define IOT_DFU_MUTEX_UNLOCK() SDK_MUTEX_UNLOCK(m_iot_dfu_mutex)                                    /**< Unlock module using mutex */
/** @} */

/**@brief Clear bank mask. */
#define CLEAR_BANK_MASK(bank)                                \
    do {                                                     \
        bank = 0;                                            \
    }  while (0);

/**@brief Set proper bank mask. Call CLEAR_BANK_MASK before appending any mask. */
#define ADD_BANK_MASK(bank, mask)                            \
    if (bank == BANK_INVALID_APP) \
    {                                                        \
        bank = mask;                                         \
    }                                                        \
    else                                                     \
    {                                                        \
        bank |= mask;                                        \
    }

#define IOT_DFU_BOOT_SETTING_FILENAME  "settings.dat"                                               /**< Name of bootloader settings file. */
#define IOT_DFU_FIRMWARE_FILENAME      "firmware.dat"                                               /**< Name of new firmware file. */
#define IOT_DFU_DEFAULT_START_ADDRESS  DFU_BANK_1_REGION_START                                      /**< Default address where new firmwware is placed. */

/**@brief Possible states of bootloader settings operation. */
typedef enum {
    SETTINGS_STATE_NORMAL = 0,
    SETTINGS_STATE_RESET
} bl_settings_state_t;

/**@brief Structure holding the iot dfu module settigns. */
typedef struct {
    iot_dfu_callback_t  event_handler;                                                              /**< Application DFU Handler. */
    bl_settings_state_t bl_settings_state;                                                          /**< State of bootloader settings. */
} module_settings_t;

SDK_MUTEX_DEFINE(m_iot_dfu_mutex)                                                                   /**< Mutex variable. Currently unused, this declaration does not occupy any space in RAM. */
static iot_file_t         m_file_settings;                                                          /**< PStorage file instance used for bootloader settings. */
static iot_file_t         m_file_firmware;                                                          /**< PStorage file instance used for new firmware. */
static module_settings_t  m_settings;                                                               /**< IoT DFU module settings. */


/**@brief Function for handling bootloader file event.
 *
 * @param[in] p_file Reference to an IoT file instance.
 * @param[in] event  Event structure describing what has happened.
 * @param[in] result Result code (should be NRF_SUCCESS for all events except errors).
 * @param[in] p_data Pointer to memory buffer.
 * @param[in] size   Size of data stored in memory buffer.
 *
 * @retval  None.
 */
static void file_settings_handler(iot_file_t     * p_file,
                                  iot_file_evt_t   event,
                                  uint32_t         result,
                                  void           * p_data,
                                  uint32_t         size)
{
    if (result != NRF_SUCCESS)
    {
        m_settings.bl_settings_state = SETTINGS_STATE_NORMAL;
        m_settings.event_handler(result, IOT_DFU_ERROR);
        return;
    }

    switch (event)
    {
        case IOT_FILE_WRITE_COMPLETE:
        {
            if (m_settings.bl_settings_state == SETTINGS_STATE_RESET)
            {
                // Restart chip and jump to bootloader mode.
                NVIC_SystemReset();
            }

            break;
        }

        default:
            break;
    }
}


/**@brief Function for handling firmware file event.
 *
 * @param[in] p_file Reference to an IoT file instance.
 * @param[in] event  Event structure describing what has happened.
 * @param[in] result Result code (should be NRF_SUCCESS for all events except errors).
 * @param[in] p_data Pointer to memory buffer.
 * @param[in] size   Size of data stored in memory buffer.
 *
 * @retval  None.
 */
static void file_firmware_handler(iot_file_t     * p_file,
                                  iot_file_evt_t   event,
                                  uint32_t         result,
                                  void           * p_data,
                                  uint32_t         size)
{
    if (result != NRF_SUCCESS)
    {
        m_settings.event_handler(result, IOT_DFU_ERROR);
        return;
    }

    switch (event)
    {
        case IOT_FILE_WRITE_COMPLETE:
            // Inform application about write operation complete.
            m_settings.event_handler(result, IOT_DFU_WRITE_COMPLETE);
            break;

        default:
            break;
    }
}


/**@brief Function for setting the bootloader settings.
 *
 * @param[in] p_bootloader_settings Actual bootloader settings passed to Bootloader.
 *
 * @retval  NRF_SUCCESS else an error indicates reason of failure.
 */
static uint32_t settings_save(bootloader_settings_t * p_bootloader_settings)
{
    uint32_t err_code;

    err_code = iot_file_fopen(&m_file_settings, sizeof(bootloader_settings_t));

    if (err_code == NRF_SUCCESS)
    {
        err_code = iot_file_fwrite(&m_file_settings,
                                   p_bootloader_settings,
                                   sizeof(bootloader_settings_t));
    }

    return err_code;
}

/**@brief Validates if firmware parts are correctly set in firmware image.
 *
 * @param[in] p_firmware_desc Firmware description of the image.
 *
 * @note This function does not check CRC, but only correct placement of the firmware parts.
 *
 * @retval NRF_SUCCESS else an error indicates reason of failure.
 */
static uint32_t is_firmware_allowed(iot_dfu_firmware_desc_t * p_firmware_desc)
{
    if ((p_firmware_desc->application.size == 0 && p_firmware_desc->bootloader.size == 0) ||
        (p_firmware_desc->application.size != 0 && p_firmware_desc->bootloader.size != 0))
    {
        IOT_DFU_ERR("[IOT_DFU]: Incorrect combination of firmware.\r\n");

        return (NRF_ERROR_INVALID_PARAM | IOT_DFU_ERR_BASE);
    }
    else if (!IS_WORD_SIZED(p_firmware_desc->application.size) ||
             !IS_WORD_SIZED(p_firmware_desc->softdevice.size)  ||
             !IS_WORD_SIZED(p_firmware_desc->bootloader.size))
    {
        IOT_DFU_ERR("[IOT_DFU]: Invalid size of firmware, each part has to be a multiple of word size.\r\n");

        // Image_sizes are not a multiple of 4 (word size).
        return (NRF_ERROR_INVALID_PARAM | IOT_DFU_ERR_BASE);
    }
    else
    {
        // Parameters are correct.
        return NRF_SUCCESS;
    }
}

/**@brief Returns size of firmware parts.
 *
 * @param[in]  p_firmware_desc Firmware description of the image.
 * @param[in]  part            Part of software that start address has to be calculated.
 * @param[out] p_address       Calculated value of start address of the given part.
 *
 * @retval None.
 */
static void get_address(iot_dfu_firmware_desc_t * p_firmware_desc,
                        uint32_t                  part,
                        uint32_t                * p_address)
{
    switch (part)
    {
        case SOFTDEVICE_PART:
            *p_address = IOT_DFU_DEFAULT_START_ADDRESS;
            break;

        case APPLICATION_PART:
            *p_address = IOT_DFU_DEFAULT_START_ADDRESS + p_firmware_desc->softdevice.size;
            break;

        case BOOTLOADER_PART:
            *p_address = IOT_DFU_DEFAULT_START_ADDRESS + p_firmware_desc->softdevice.size
                                                       + p_firmware_desc->application.size;
             break;

        default:
            *p_address = 0;
            break;
    }
}


/**@brief Function for initializing IOT DFU module. */
uint32_t iot_dfu_init(iot_dfu_callback_t cb)
{
    NULL_PARAM_CHECK(cb);

    uint32_t err_code = NRF_SUCCESS;

    IOT_DFU_TRC("[IOT_DFU]: >> iot_dfu_init\r\n");

    SDK_MUTEX_INIT(m_iot_dfu_mutex);

    IOT_DFU_MUTEX_LOCK();

    if ((IOT_DFU_DEFAULT_START_ADDRESS % CODE_PAGE_SIZE) != 0)
    {
        IOT_DFU_ERR("[IOT_DFU]: Start address should be aligned to flash page size\r\n");

        err_code = (NRF_ERROR_INVALID_DATA | IOT_DFU_ERR_BASE);
    }
    else
    {
        m_settings.event_handler = cb;

        // Initialize file for bootloader settings.
        IOT_FILE_PSTORAGE_RAW_INIT(&m_file_settings,
                                   IOT_DFU_BOOT_SETTING_FILENAME,
                                   BOOTLOADER_SETTINGS_ADDRESS,
                                   CODE_PAGE_SIZE,
                                   file_settings_handler);
    }

    IOT_DFU_TRC("[IOT_DFU]: << iot_dfu_init\r\n");

    IOT_DFU_MUTEX_UNLOCK();

    return err_code;
}


/**@brief Function for creating a PStorage IoT File reference for flash operations and setting
 *        start address of PStorage File. */
uint32_t iot_dfu_file_create(iot_file_t ** pp_file)
{
    VERIFY_MODULE_IS_INITIALIZED();
    NULL_PARAM_CHECK(pp_file);

    uint32_t err_code = NRF_SUCCESS;

    IOT_DFU_TRC("[IOT_DFU]: >> iot_dfu_file_create\r\n");

    IOT_DFU_MUTEX_LOCK();

    // Initialize file for new firmware.
    IOT_FILE_PSTORAGE_RAW_INIT(&m_file_firmware,
                               IOT_DFU_FIRMWARE_FILENAME,
                               IOT_DFU_DEFAULT_START_ADDRESS,
                               (BOOTLOADER_REGION_START - IOT_DFU_DEFAULT_START_ADDRESS),
                               file_firmware_handler);

    // Pass file handler to application.
    *pp_file = &m_file_firmware;

    IOT_DFU_TRC("[IOT_DFU]: << iot_dfu_file_create\r\n");

    IOT_DFU_MUTEX_UNLOCK();

    return err_code;
}


/**@brief Function for validating the CRC of the received image. */
uint32_t iot_dfu_firmware_validate(iot_dfu_firmware_desc_t * p_firmware_desc)
{
    VERIFY_MODULE_IS_INITIALIZED();

    uint32_t err_code;
    uint32_t address;
    uint16_t crc;

    IOT_DFU_TRC("[IOT_DFU]: >> iot_dfu_firmware_validate\r\n");

    IOT_DFU_MUTEX_LOCK();

    // Validate firmware parts.
    err_code = is_firmware_allowed(p_firmware_desc);
    if (err_code == NRF_SUCCESS)
    {
        if ((p_firmware_desc->application.size != 0) && (p_firmware_desc->application.crc != 0))
        {
            // Get address of application.
            get_address(p_firmware_desc, APPLICATION_PART, &address);
            crc = crc16_compute((uint8_t *)address,
                                p_firmware_desc->application.size,
                                NULL);

            if (crc != p_firmware_desc->application.crc)
            {
                IOT_DFU_ERR("[IOT_DFU]: Application CRC mismatch.\r\n");
                err_code = NRF_ERROR_INVALID_DATA;
            }
        }

        if ((p_firmware_desc->softdevice.size != 0) && (p_firmware_desc->softdevice.crc != 0))
        {
            // Get address of softdevice.
            get_address(p_firmware_desc, SOFTDEVICE_PART, &address);
            crc = crc16_compute((uint8_t *)address,
                                p_firmware_desc->softdevice.size,
                                NULL);

            if (crc != p_firmware_desc->softdevice.crc)
            {
                IOT_DFU_ERR("[IOT_DFU]: Softdevice CRC mismatch.\r\n");
                err_code = NRF_ERROR_INVALID_DATA;
            }
        }

        if ((p_firmware_desc->bootloader.size != 0) && (p_firmware_desc->bootloader.crc != 0))
        {
            // Get address of bootloader.
            get_address(p_firmware_desc, BOOTLOADER_PART, &address);
            crc = crc16_compute((uint8_t *)address,
                                p_firmware_desc->bootloader.size,
                                NULL);

            if (crc != p_firmware_desc->bootloader.crc)
            {
                IOT_DFU_ERR("[IOT_DFU]: Bootloader CRC mismatch.\r\n");
                err_code = NRF_ERROR_INVALID_DATA;
            }
        }
    }

    IOT_DFU_TRC("[IOT_DFU]: << iot_dfu_firmware_validate\r\n");

    IOT_DFU_MUTEX_UNLOCK();

    return err_code;
}


/**@brief Function for rebooting device and trigger bootloader to swap the new firmware. */
uint32_t iot_dfu_firmware_apply(iot_dfu_firmware_desc_t * p_firmware_desc)
{
    VERIFY_MODULE_IS_INITIALIZED();
    NULL_PARAM_CHECK(p_firmware_desc);

    uint32_t              err_code;
    bootloader_settings_t bootloader_settings;

    IOT_DFU_TRC("[IOT_DFU]: >> iot_dfu_firmware_apply\r\n");

    IOT_DFU_MUTEX_LOCK();

    // Validate firmware parts.
    err_code = is_firmware_allowed(p_firmware_desc);
    if (err_code == NRF_SUCCESS)
    {
        // Save new configuration.
        memset(&bootloader_settings, 0, sizeof(bootloader_settings_t));

        CLEAR_BANK_MASK(bootloader_settings.bank_1);
        bootloader_settings.bank_0         = BANK_VALID_APP;
        bootloader_settings.sd_image_size  = p_firmware_desc->softdevice.size;
        bootloader_settings.bl_image_size  = p_firmware_desc->bootloader.size;
        bootloader_settings.app_image_size = p_firmware_desc->application.size;
        bootloader_settings.sd_image_start = IOT_DFU_DEFAULT_START_ADDRESS;

        if (p_firmware_desc->softdevice.size)
        {
            IOT_DFU_TRC("[IOT_DFU]: Update SoftDevice upgrade status.\r\n");

            bootloader_settings.bank_0 = BANK_INVALID_APP;
            ADD_BANK_MASK(bootloader_settings.bank_1, BANK_VALID_SD);
        }

        if (p_firmware_desc->bootloader.size)
        {
            IOT_DFU_TRC("[IOT_DFU]: Update BootLoader upgrade status.\r\n");
            ADD_BANK_MASK(bootloader_settings.bank_1, BANK_VALID_BOOT);
        }

        if (p_firmware_desc->application.size)
        {
            IOT_DFU_TRC("[IOT_DFU]: Update Application upgrade status.\r\n");

            bootloader_settings.bank_0      = BANK_INVALID_APP;
            bootloader_settings.bank_0_size = p_firmware_desc->application.size;

            ADD_BANK_MASK(bootloader_settings.bank_1, BANK_VALID_APP);
        }

        // Jump to reset state after write complete operation.
        m_settings.bl_settings_state = SETTINGS_STATE_RESET;

        err_code = settings_save(&bootloader_settings);
    }

    IOT_DFU_TRC("[IOT_DFU]: << iot_dfu_firmware_apply\r\n");

    IOT_DFU_MUTEX_UNLOCK();

    return err_code;
}
