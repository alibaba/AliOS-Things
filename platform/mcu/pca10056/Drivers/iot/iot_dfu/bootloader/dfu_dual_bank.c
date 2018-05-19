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
#include "nrf_mbr.h"
#include "dfu_types.h"
#include "dfu_boot.h"
#include "nrf_delay.h"

/**@brief Function for comparing data inside persistent memory using MBR calls.
 *
 * @param[in]  p_addr1     Address to the first region in flash to be compared.
 * @param[in]  p_addr2     Address to the second region in flash to be compared.
 * @param[in]  len         Length of data to be compared.
 *
 * @retval     NRF_SUCCESS If memory regions have the same data, else if memory regions have
 *             different data.
 *
 */
static uint32_t compare(uint32_t * p_addr1, uint32_t * p_addr2, uint32_t len)
{
    sd_mbr_command_t sd_mbr_cmd;

    sd_mbr_cmd.command             = SD_MBR_COMMAND_COMPARE;
    sd_mbr_cmd.params.compare.ptr1 = p_addr1;
    sd_mbr_cmd.params.compare.ptr2 = p_addr2;
    sd_mbr_cmd.params.compare.len  = len / sizeof(uint32_t);

    return sd_mbr_command(&sd_mbr_cmd);
}

/**@brief Function for copying data inside persistent memory using MBR calls.
 *
 * @param[in]  p_src       Address of memory to be copied.
 * @param[in]  p_dst       Address of memory to be changed.
 * @param[in]  len         Length of data to be copied.
 *
 * @retval     NRF_SUCCESS If memory copy was successful, else error indicating reason of failure.
 *
 */
static uint32_t copy(uint32_t * p_src, uint32_t * p_dst, uint32_t len)
{
    sd_mbr_command_t sd_mbr_cmd;

    sd_mbr_cmd.command            = SD_MBR_COMMAND_COPY_SD;
    sd_mbr_cmd.params.copy_sd.src = p_src;
    sd_mbr_cmd.params.copy_sd.dst = p_dst;
    sd_mbr_cmd.params.copy_sd.len = len / sizeof(uint32_t);

    return sd_mbr_command(&sd_mbr_cmd);
}

/**@brief Function for getting address of stored application inside SWAP area.
 *
 * @param[in]  part         Integer representing firmware part.
 *
 * @retval     NRF_SUCCESS If passed integer has not assigned firmware part, address otherwise.
 *
 */
static uint32_t get_address(uint32_t part)
{
    bootloader_settings_t bootloader_settings;

    bootloader_settings_load(&bootloader_settings);

    switch (part)
    {
        case SOFTDEVICE_PART:
            return bootloader_settings.sd_image_start;

        case APPLICATION_PART:
            return bootloader_settings.sd_image_start + bootloader_settings.sd_image_size;

        case BOOTLOADER_PART:
            return bootloader_settings.sd_image_start + bootloader_settings.sd_image_size
                                                      + bootloader_settings.app_image_size;

        default:
            return NRF_SUCCESS;

    }
}

/**@brief Function for swapping existing SoftDevice with newly received. */
uint32_t dfu_sd_image_swap(void)
{
    uint32_t              err_code = NRF_SUCCESS;
    bootloader_settings_t bootloader_settings;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_SD))
    {
        bootloader_settings.bank_1 &= ~(BANK_VALID_SD);
        bootloader_settings_save(&bootloader_settings);

        err_code = copy((uint32_t *)get_address(SOFTDEVICE_PART),
                        (uint32_t *)SOFTDEVICE_REGION_START,
                        bootloader_settings.sd_image_size);

        nrf_delay_ms(100);

        if (err_code == NRF_SUCCESS)
        {
            NVIC_SystemReset();
        }
    }

    return err_code;
}

/**@brief Function for swapping existing application with newly received. */
uint32_t dfu_app_image_swap(void)
{
    bootloader_settings_t bootloader_settings;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_APP))
    {
        bootloader_settings.bank_1 &= ~(BANK_VALID_APP);
        bootloader_settings.bank_0 = BANK_VALID_APP;
        bootloader_settings_save(&bootloader_settings);

        return copy((uint32_t *)get_address(APPLICATION_PART),
                    (uint32_t *)(SD_SIZE_GET(MBR_SIZE)),
                    bootloader_settings.app_image_size);
    }

    return NRF_SUCCESS;
}

/**@brief Function for swapping existing bootloader with newly received. */
uint32_t dfu_bl_image_swap(void)
{
    bootloader_settings_t bootloader_settings;
    sd_mbr_command_t      sd_mbr_cmd;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_BOOT))
    {
        sd_mbr_cmd.command               = SD_MBR_COMMAND_COPY_BL;
        sd_mbr_cmd.params.copy_bl.bl_src = (uint32_t *)get_address(BOOTLOADER_PART);
        sd_mbr_cmd.params.copy_bl.bl_len = bootloader_settings.bl_image_size / sizeof(uint32_t);

        bootloader_settings.bank_1 &= ~(BANK_VALID_BOOT);
        bootloader_settings_save(&bootloader_settings);

        return sd_mbr_command(&sd_mbr_cmd);
    }

    return NRF_SUCCESS;
}


/**@brief Function for validating that new application has been correctly installed. */
uint32_t dfu_app_image_validate(void)
{
    bootloader_settings_t bootloader_settings;
    uint32_t              err_code = NRF_SUCCESS;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_APP))
    {
        err_code = compare((uint32_t *)(SD_SIZE_GET(MBR_SIZE)),
                           (uint32_t *)get_address(APPLICATION_PART),
                           bootloader_settings.app_image_size);
    }

    return err_code;
}

/**@brief Function for validating that new SoftDevicehas been correctly installed. */
uint32_t dfu_sd_image_validate(void)
{
    bootloader_settings_t bootloader_settings;
    uint32_t              err_code = NRF_SUCCESS;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_SD))
    {
        err_code = compare((uint32_t *)SOFTDEVICE_REGION_START,
                           (uint32_t *)get_address(SOFTDEVICE_PART),
                           bootloader_settings.sd_image_size);
    }

    return err_code;
}

/**@brief Function for validating that new bootloader has been correctly installed. */
uint32_t dfu_bl_image_validate(void)
{
    bootloader_settings_t bootloader_settings;
    uint32_t              err_code = NRF_SUCCESS;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) & (BANK_VALID_BOOT))
    {
        err_code = compare((uint32_t *)BOOTLOADER_REGION_START,
                           (uint32_t *)get_address(BOOTLOADER_PART),
                           bootloader_settings.bl_image_size);

        if (err_code == NRF_SUCCESS)
        {
            bootloader_settings.bank_1 &= ~(BANK_VALID_BOOT);
            bootloader_settings_save(&bootloader_settings);
        }
    }

    return err_code;
}
