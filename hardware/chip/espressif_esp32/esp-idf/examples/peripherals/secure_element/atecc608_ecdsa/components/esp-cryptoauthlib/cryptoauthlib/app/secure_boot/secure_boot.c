/**
 * \file
 *
 * \brief Provides required APIs to manage secure boot under various scenarios.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <string.h>
#include "secure_boot.h"
#include "io_protection_key.h"
#include "basic/atca_basic.h"

/*Initialization routines */
static ATCA_STATUS secure_boot_init(secure_boot_parameters* secure_boot_params);

/*Secure boot process routines */
static ATCA_STATUS secure_boot_calc_app_digest(secure_boot_parameters* secure_boot_params);

/** \brief Handles secure boot functionality through initialization, execution,
 *         and de-initialization.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS secure_boot_process(void)
{
    ATCA_STATUS status;
    secure_boot_parameters secure_boot_params;
    uint8_t secure_boot_mode;
    bool secure_boot_app_valid = false;

    /*Initialize secure boot */
    if ((status = secure_boot_init(&secure_boot_params)) != ATCA_SUCCESS)
    {
        return status;
    }

    do
    {
        /* Calculate application digest */
        if ((status = secure_boot_calc_app_digest(&secure_boot_params)) != ATCA_SUCCESS)
        {
            break;
        }

        #if (SECURE_BOOT_CONFIGURATION == SECURE_BOOT_CONFIG_FULL_BOTH)
        secure_boot_mode = SECUREBOOT_MODE_FULL;
        #else
        secure_boot_mode = SECUREBOOT_MODE_FULL_STORE;
        #endif

        #if SECURE_BOOT_UPGRADE_SUPPORT
        /*Check need for full copy */
        if (false == secure_boot_check_full_copy_completion())
        {
            secure_boot_mode = SECUREBOOT_MODE_FULL_COPY;
        }
        #endif  //#if SECURE_BOOT_UPGRADE_SUPPORT

        #if SECURE_BOOT_DIGEST_ENCRYPT_ENABLED
        /*Get Random number from Host*/
        if ((status = host_generate_random_number(secure_boot_params.randomnum)) != ATCA_SUCCESS)
        {
            return status;
        }

        /*Get IO Protection Key*/
        if ((status = io_protection_get_key(secure_boot_params.io_protection_key)) != ATCA_SUCCESS)
        {
            return status;
        }

        if ((status = atcab_secureboot_mac(secure_boot_mode,
                                           (const uint8_t*)&secure_boot_params.app_digest,
                                           (const uint8_t*)&secure_boot_params.memory_params.signature,
                                           (const uint8_t*)secure_boot_params.randomnum,
                                           (const uint8_t*)secure_boot_params.io_protection_key,
                                           &secure_boot_app_valid)) != ATCA_SUCCESS)
        {
            break;
        }
        #else
        if ((status = atcab_secureboot(secure_boot_mode,
                                       0,
                                       (const uint8_t*)&secure_boot_params.app_digest,
                                       (const uint8_t*)&secure_boot_params.memory_params.signature,
                                       NULL)) != ATCA_SUCCESS)
        {
            break;
        }
        secure_boot_app_valid = true;
        #endif

        /*Check whether the secure boot command executed successfully with the correct return mac  */
        if (!secure_boot_app_valid)
        {
            status = ATCA_CHECKMAC_VERIFY_FAILED;
            break;
        }

        /*Mark to indicate full copy is executed */
        if ((SECUREBOOT_MODE_FULL_COPY == secure_boot_mode) && ((status = secure_boot_mark_full_copy_completion()) != ATCA_SUCCESS))
        {
            break;
        }

    }
    while (0);

    /* De-initialize memory interface and release its resources*/
    secure_boot_deinit_memory(&secure_boot_params.memory_params);

    return status;
}

/** \brief Verifies configuration as well as memory initialization.
 *  \param[inout] secure_boot_params  Secure boot parameters
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS secure_boot_init(secure_boot_parameters* secure_boot_params)
{
    ATCA_STATUS status;
    uint16_t secure_boot_config;
    uint8_t buf[2];

    /*Reset all parameters before starting the process */
    memset((void*)secure_boot_params, 0, sizeof(secure_boot_parameters));

    /*Read Configuration data & verify device configuration settings*/
    if ((status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, SECUREBOOTCONFIG_OFFSET, buf, 2)) != ATCA_SUCCESS)
    {
        return status;
    }
    secure_boot_config = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);

    /*Return error status if configuration and device settings doesn't match */
    if ((secure_boot_config & SECUREBOOTCONFIG_MODE_MASK) != SECURE_BOOT_CONFIGURATION)
    {
        return ATCA_GEN_FAIL;
    }

    /*Initialize memory interface and get its parameters*/
    if ((status = secure_boot_init_memory(&secure_boot_params->memory_params)) != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}
/** \brief Calculates the application digest using software SHA256
 *         implementation.
 *  \param[inout] secure_boot_params  Secure boot parameters
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS secure_boot_calc_app_digest(secure_boot_parameters* secure_boot_params)
{
    ATCA_STATUS status;
    uint8_t sha_data[ATCA_SHA256_BLOCK_SIZE];
    uint32_t total_data_count = 0;
    uint32_t current_data_count;

    /*Initialize SHA engine*/
    if ((status = atcac_sw_sha2_256_init(&secure_boot_params->s_sha_context)) != ATCA_SUCCESS)
    {
        return status;
    }

    /*Loop through SHA calculation for given memory*/
    while (total_data_count < secure_boot_params->memory_params.memory_size)
    {
        /*Default set to data size*/
        current_data_count = sizeof(sha_data);

        /*Check if data exceeds available memory*/
        if ((total_data_count + current_data_count) > secure_boot_params->memory_params.memory_size)
        {
            /*Restrict to upper limit of memory*/
            current_data_count = secure_boot_params->memory_params.memory_size - total_data_count;
        }

        /*Read data from memory*/
        if ((status = secure_boot_read_memory(sha_data, &current_data_count)) != ATCA_SUCCESS)
        {
            return status;
        }
        total_data_count += current_data_count;

        /*Calculate SHA for the current set of data*/
        if ((status = atcac_sw_sha2_256_update(&secure_boot_params->s_sha_context, sha_data, current_data_count)) != ATCA_SUCCESS)
        {
            return status;
        }
    }

    /*Initiate final step and get SHA output*/
    if ((status = atcac_sw_sha2_256_finish(&secure_boot_params->s_sha_context, secure_boot_params->app_digest)) != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}
/** \brief Binds host MCU and Secure element with IO protection key.
 *  \param[in]  slot    The slot number of IO protection Key.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS bind_host_and_secure_element_with_io_protection(uint16_t slot)
{
    bool is_locked;
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t io_prot_key[ATCA_KEY_SIZE];

    /*IO protection key is not on host... Get bind with device */
    do
    {
        /* First check Lock status on device */
        if ((status = atcab_is_slot_locked(slot, &is_locked)) != ATCA_SUCCESS)
        {
            break;
        }

        if (is_locked)
        {
            /*IO Protection Key shouldn't have already locked */
            status = ATCA_GEN_FAIL;
            break;
        }

        /*Get random number from device */
        if ((status = atcab_random(io_prot_key)) != ATCA_SUCCESS)
        {
            break;
        }

        /*Load the random number as IO Protection key on the device */
        if ((status = atcab_write_zone(ATCA_ZONE_DATA, slot, 0, 0, io_prot_key, ATCA_KEY_SIZE)) != ATCA_SUCCESS)
        {
            break;
        }

        /*Store the same as IO Protection Key on host*/
        if ((status = io_protection_set_key(io_prot_key)) != ATCA_SUCCESS)
        {
            break;
        }

        /*Lock IO protection key slot */
        if ((status = atcab_lock_data_slot(slot)) != ATCA_SUCCESS)
        {
            break;
        }
    }
    while (0);

    return status;
}