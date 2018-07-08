/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
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
#include <stdbool.h>
#include "nrf_log.h"
#include "nrf_sdm.h"

/** @brief External definitions of symbols for the start of the application image.
 */
#if (__LINT__ == 1)
    // No implementation
#elif defined ( __CC_ARM )
    extern uint32_t* Image$$ER_IROM1$$Base   __attribute__((used));
#elif defined (__SES_ARM) && defined (__GNUC__)
    extern uint32_t * _vectors;
#elif defined ( __GNUC__ )
    extern uint32_t * __isr_vector;
#elif defined ( __ICCARM__ )
    extern void * __vector_table;
#else
    #error Not a valid compiler/linker for application image symbols.
#endif


/** @brief Macro for getting the start address of the application image.
 *
 * This macro is valid only when absolute placement is used for the application
 * image. The macro is not a compile time symbol. It cannot be used as a
 * constant expression, for example, inside a static assert or linker script
 * at-placement.
 */
#if (__LINT__ == 1)
    #define APP_START_ADDR        (0x1F000)
#elif APP_START_ADDR
    // Bootloader start address is defined at project level
#elif defined (__CC_ARM)
    #define APP_START_ADDR        (uint32_t)&Image$$ER_IROM1$$Base
#elif defined (__SES_ARM) && defined (__GNUC__)
    #define APP_START_ADDR        (uint32_t)&_vectors
#elif defined (__GNUC__)
    #define APP_START_ADDR        (uint32_t)&__isr_vector
#elif defined (__ICCARM__)
    #define APP_START_ADDR        (uint32_t)&__vector_table
#else
    #error Not a valid compiler/linker for APP_START_ADDR.
#endif


uint32_t nrf_dfu_svci_vector_table_set(void)
{
    uint32_t err_code;

    if (NRF_UICR->NRFFW[0] != 0xFFFFFFFF)
    {
        NRF_LOG_INFO("Setting vector table to bootloader: 0x%08x", NRF_UICR->NRFFW[0]);
        err_code = sd_softdevice_vector_table_base_set(NRF_UICR->NRFFW[0]);
        if (err_code != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("Failed running sd_softdevice_vector_table_base_set");
            return err_code;
        }

        return NRF_SUCCESS;
    }

    NRF_LOG_ERROR("No bootloader was found");
    return NRF_ERROR_NO_MEM;
}


uint32_t nrf_dfu_svci_vector_table_unset(void)
{
    uint32_t err_code;

    NRF_LOG_INFO("Setting vector table to main app: 0x%08x", APP_START_ADDR);
    err_code = sd_softdevice_vector_table_base_set(APP_START_ADDR);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed running sd_softdevice_vector_table_base_set");
        return err_code;
    }

    return NRF_SUCCESS;
}

