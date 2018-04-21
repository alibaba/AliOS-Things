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
#include <string.h>
#include "crc16.h"
#include "nrf_delay.h"
#include "pstorage.h"
#include "bootloader_api.h"
#include "dfu_boot.h"

#define IRQ_ENABLED             0x01                    /**< Field identifying if an interrupt is enabled. */
#define MAX_NUMBER_INTERRUPTS   32                      /**< Maximum number of interrupts available. */

#define NRF_ERROR_CHECK(err_code) \
    if (err_code != NRF_SUCCESS)  \
    {                             \
        return err_code;          \
    }

#if defined ( __CC_ARM )
uint8_t  m_boot_settings[CODE_PAGE_SIZE] __attribute__((at(BOOTLOADER_SETTINGS_ADDRESS))) __attribute__((used));                /**< This variable reserves a codepage for bootloader specific settings, to ensure the compiler doesn't locate any code or variables at his location. */
uint32_t m_uicr_bootloader_start_address __attribute__((at(NRF_UICR_BOOT_START_ADDRESS))) = BOOTLOADER_REGION_START;            /**< This variable ensures that the linker script will write the bootloader start address to the UICR register. This value will be written in the HEX file and thus written to UICR when the bootloader is flashed into the chip. */
#elif defined ( __GNUC__ )
__attribute__ ((section(".bootloader_settings_page"))) uint8_t m_boot_settings[CODE_PAGE_SIZE];                                       /**< This variable reserves a codepage for bootloader specific settings, to ensure the compiler doesn't locate any code or variables at his location. */
__attribute__ ((section(".uicr_bootloader_start_address"))) volatile uint32_t m_uicr_bootloader_start_address = BOOTLOADER_REGION_START; /**< This variable ensures that the linker script will write the bootloader start address to the UICR register. This value will be written in the HEX file and thus written to UICR when the bootloader is flashed into the chip. */
#elif defined ( __ICCARM__ )
__no_init uint8_t m_boot_settings[CODE_PAGE_SIZE] @ 0x0003FC00;                                                                 /**< This variable reserves a codepage for bootloader specific settings, to ensure the compiler doesn't locate any code or variables at his location. */
__root    const uint32_t m_uicr_bootloader_start_address @ 0x10001014 = BOOTLOADER_REGION_START;                                /**< This variable ensures that the linker script will write the bootloader start address to the UICR register. This value will be written in the HEX file and thus written to UICR when the bootloader is flashed into the chip. */
#endif

static pstorage_handle_t            m_bootsettings_handle;      /**< Pstorage handle to use for registration and identifying the bootloader module on subsequent calls to the pstorage module for load and store of bootloader setting in flash. */
static volatile bool                m_wait_for_settings_update; /**< Variable indicating status of PSTorage store operation. Used by settings_save function to act as "blocking" function. */

/**@brief   Function for handling callbacks from pstorage module.
 *
 * @details Handles pstorage results for clear and storage operation. For detailed description of
 *          the parameters provided with the callback, please refer to \ref pstorage_ntf_cb_t.
 */
static void pstorage_callback_handler(pstorage_handle_t * p_handle,
                                      uint8_t             op_code,
                                      uint32_t            result,
                                      uint8_t           * p_data,
                                      uint32_t            data_len)
{
    if (op_code == PSTORAGE_STORE_OP_CODE)
    {
        m_wait_for_settings_update = false;
    }
}

/**@brief Function for disabling all interrupts before jumping from bootloader to application. */
static void interrupts_disable(void)
{
    uint32_t interrupt_setting_mask;
    uint32_t irq; // We start from first interrupt, i.e. interrupt 0.

    // Fetch the current interrupt settings.
    interrupt_setting_mask = NVIC->ISER[0];

    for (irq = 0; irq < MAX_NUMBER_INTERRUPTS; irq++)
    {
        if (interrupt_setting_mask & (IRQ_ENABLED << irq))
        {
            // The interrupt was enabled, and hence disable it.
            NVIC_DisableIRQ((IRQn_Type)irq);
        }
    }
}

/*lint -save -e123 */
/**
 * @brief Function for aborting current application/bootloader jump to to other app/bootloader.
 *
 * @details This functions will use the address provide to swap the stack pointer and then load
 *          the address of the reset handler to be executed. It will check current system mode
 *          (thread/handler) and if in thread mode it will reset into other application.
 *          If in handler mode \ref isr_abort will be executed to ensure correct exit of handler
 *          mode and jump into reset handler of other application.
 *
 * @param[in]  start_addr  Start address of other application. This address must point to the
               initial stack pointer of the application.
 *
 * @note This function will never return but issue a reset into provided application.
 */
#if defined ( __CC_ARM )
__asm static void util_reset(uint32_t start_addr)
{
    LDR   R5, [R0]              ; Get App initial MSP for bootloader.
    MSR   MSP, R5               ; Set the main stack pointer to the applications MSP.
    LDR   R0, [R0, #0x04]       ; Load Reset handler into R0. This will be first argument to branch instruction (BX).

    MOVS  R4, #0xFF             ; Load ones to R4.
    SXTB  R4, R4                ; Sign extend R4 to obtain 0xFFFFFFFF instead of 0xFF.
    MRS   R5, IPSR              ; Load IPSR to R5 to check for handler or thread mode.
    CMP   R5, #0x00             ; Compare, if 0 then we are in thread mode and can continue to reset handler of bootloader.
    BNE   isr_abort             ; If not zero we need to exit current ISR and jump to reset handler of bootloader.

    MOV   LR, R4                ; Clear the link register and set to ones to ensure no return, R4 = 0xFFFFFFFF.
    BX    R0                    ; Branch to reset handler of bootloader.

isr_abort
                                ; R4 contains ones from line above. Will be popped as R12 when exiting ISR (Cleaning up the registers).
    MOV   R5, R4                ; Fill with ones before jumping to reset handling. We be popped as LR when exiting ISR. Ensures no return to application.
    MOV   R6, R0                ; Move address of reset handler to R6. Will be popped as PC when exiting ISR. Ensures the reset handler will be executed when exist ISR.
    MOVS  r7, #0x21             ; Move MSB reset value of xPSR to R7. Will be popped as xPSR when exiting ISR. xPSR is 0x21000000 thus MSB is 0x21.
    REV   r7, r7                ; Reverse byte order to put 0x21 as MSB.
    PUSH  {r4-r7}               ; Push everything to new stack to allow interrupt handler to fetch it on exiting the ISR.

    MOVS  R4, #0x00             ; Fill with zeros before jumping to reset handling. We be popped as R0 when exiting ISR (Cleaning up of the registers).
    MOVS  R5, #0x00             ; Fill with zeros before jumping to reset handling. We be popped as R1 when exiting ISR (Cleaning up of the registers).
    MOVS  R6, #0x00             ; Fill with zeros before jumping to reset handling. We be popped as R2 when exiting ISR (Cleaning up of the registers).
    MOVS  R7, #0x00             ; Fill with zeros before jumping to reset handling. We be popped as R3 when exiting ISR (Cleaning up of the registers).
    PUSH  {r4-r7}               ; Push zeros (R4-R7) to stack to prepare for exiting the interrupt routine.

    MOVS  R0, #0xF9             ; Move the execution return command into register, 0xFFFFFFF9.
    SXTB  R0, R0                ; Sign extend R0 to obtain 0xFFFFFFF9 instead of 0xF9.
    BX    R0                    ; No return - Handler mode will be exited. Stack will be popped and execution will continue in reset handler initializing other application.
    ALIGN
}
#elif defined ( __GNUC__ )
static inline void util_reset(uint32_t start_addr)
{
    __asm volatile(
        "ldr   r0, [%0]\t\n"            // Get App initial MSP for bootloader.
        "msr   msp, r0\t\n"             // Set the main stack pointer to the applications MSP.
        "ldr   r0, [%0, #0x04]\t\n"     // Load Reset handler into R0.

        "movs  r4, #0xFF\t\n"           // Move ones to R4.
        "sxtb  r4, r4\t\n"              // Sign extend R4 to obtain 0xFFFFFFFF instead of 0xFF.

        "mrs   r5, IPSR\t\n"            // Load IPSR to R5 to check for handler or thread mode.
        "cmp   r5, #0x00\t\n"           // Compare, if 0 then we are in thread mode and can continue to reset handler of bootloader.
        "bne   isr_abort\t\n"           // If not zero we need to exit current ISR and jump to reset handler of bootloader.

        "mov   lr, r4\t\n"              // Clear the link register and set to ones to ensure no return.
        "bx    r0\t\n"                  // Branch to reset handler of bootloader.

        "isr_abort:  \t\n"

        "mov   r5, r4\t\n"              // Fill with ones before jumping to reset handling. Will be popped as LR when exiting ISR. Ensures no return to application.
        "mov   r6, r0\t\n"              // Move address of reset handler to R6. Will be popped as PC when exiting ISR. Ensures the reset handler will be executed when exist ISR.
        "movs  r7, #0x21\t\n"           // Move MSB reset value of xPSR to R7. Will be popped as xPSR when exiting ISR. xPSR is 0x21000000 thus MSB is 0x21.
        "rev   r7, r7\t\n"              // Reverse byte order to put 0x21 as MSB.
        "push  {r4-r7}\t\n"             // Push everything to new stack to allow interrupt handler to fetch it on exiting the ISR.

        "movs  r4, #0x00\t\n"           // Fill with zeros before jumping to reset handling. We be popped as R0 when exiting ISR (Cleaning up of the registers).
        "movs  r5, #0x00\t\n"           // Fill with zeros before jumping to reset handling. We be popped as R1 when exiting ISR (Cleaning up of the registers).
        "movs  r6, #0x00\t\n"           // Fill with zeros before jumping to reset handling. We be popped as R2 when exiting ISR (Cleaning up of the registers).
        "movs  r7, #0x00\t\n"           // Fill with zeros before jumping to reset handling. We be popped as R3 when exiting ISR (Cleaning up of the registers).
        "push  {r4-r7}\t\n"             // Push zeros (R4-R7) to stack to prepare for exiting the interrupt routine.

        "movs  r0, #0xF9\t\n"           // Move the execution return command into register, 0xFFFFFFF9.
        "sxtb  r0, r0\t\n"              // Sign extend R0 to obtain 0xFFFFFFF9 instead of 0xF9.
        "bx    r0\t\n"                  // No return - Handler mode will be exited. Stack will be popped and execution will continue in reset handler initializing other application.
        ".align\t\n"
        :: "r" (start_addr)             // Argument list for the gcc assembly. start_addr is %0.
        :  "r0", "r4", "r5", "r6", "r7" // List of register maintained manually.
    );
}
#elif defined ( __ICCARM__ )
static inline void util_reset(uint32_t start_addr)
{
    asm("ldr   r5, [%0]\n"                    // Get App initial MSP for bootloader.
        "msr   msp, r5\n"                     // Set the main stack pointer to the applications MSP.
        "ldr   r0, [%0, #0x04]\n"             // Load Reset handler into R0.

        "movs  r4, #0x00\n"                   // Load zero into R4.
        "mvns  r4, r4\n"                      // Invert R4 to ensure it contain ones.

        "mrs   r5, IPSR\n"                    // Load IPSR to R5 to check for handler or thread mode
        "cmp   r5, #0x00\n"                   // Compare, if 0 then we are in thread mode and can continue to reset handler of bootloader.
        "bne   isr_abort\n"                   // If not zero we need to exit current ISR and jump to reset handler of bootloader.

        "mov   lr, r4\n"                      // Clear the link register and set to ones to ensure no return.
        "bx    r0\n"                          // Branch to reset handler of bootloader.

        "isr_abort: \n"
                                              // R4 contains ones from line above. We be popped as R12 when exiting ISR (Cleaning up the registers).
        "mov   r5, r4\n"                      // Fill with ones before jumping to reset handling. Will be popped as LR when exiting ISR. Ensures no return to application.
        "mov   r6, r0\n"                      // Move address of reset handler to R6. Will be popped as PC when exiting ISR. Ensures the reset handler will be executed when exist ISR.
        "movs  r7, #0x21\n"                   // Move MSB reset value of xPSR to R7. Will be popped as xPSR when exiting ISR. xPSR is 0x21000000 thus MSB is 0x21.
        "rev   r7, r7\n"                      // Reverse byte order to put 0x21 as MSB.
        "push  {r4-r7}\n"                     // Push everything to new stack to allow interrupt handler to fetch it on exiting the ISR.

        "movs  r4, #0x00\n"                   // Fill with zeros before jumping to reset handling. We be popped as R0 when exiting ISR (Cleaning up of the registers).
        "movs  r5, #0x00\n"                   // Fill with zeros before jumping to reset handling. We be popped as R1 when exiting ISR (Cleaning up of the registers).
        "movs  r6, #0x00\n"                   // Fill with zeros before jumping to reset handling. We be popped as R2 when exiting ISR (Cleaning up of the registers).
        "movs  r7, #0x00\n"                   // Fill with zeros before jumping to reset handling. We be popped as R3 when exiting ISR (Cleaning up of the registers).
        "push  {r4-r7}\n"                     // Push zeros (R4-R7) to stack to prepare for exiting the interrupt routine.

        "movs  r0, #0x06\n"                   // Load 0x06 into R6 to prepare for exec return command.
        "mvns  r0, r0\n"                      // Invert 0x06 to obtain EXEC_RETURN, 0xFFFFFFF9.
        "bx    r0\n"                          // No return - Handler mode will be exited. Stack will be popped and execution will continue in reset handler initializing other application.
        :: "r" (start_addr)                   // Argument list for the IAR assembly. start_addr is %0.
        :  "r0", "r4", "r5", "r6", "r7");     // List of register maintained manually.
}
#else
#error Compiler not supported.
#endif
/*lint -restore */

/**@brief Function for getting the bootloader settings. Part of DFU_BOOT API (used by memory management layer like dual bank). */
void bootloader_settings_load(bootloader_settings_t * p_settings)
{
    if (p_settings == NULL)
    {
        return;
    }

    memcpy(p_settings, m_boot_settings, sizeof(bootloader_settings_t));
}

/**@brief Function for setting the bootloader settings. Part of DFU_BOOT API (used by memory management layer like dual bank). */
void bootloader_settings_save(bootloader_settings_t * p_settings)
{
    uint32_t err_code = pstorage_clear(&m_bootsettings_handle, sizeof(bootloader_settings_t));
    if (err_code != NRF_SUCCESS)
    {
        return;
    }

    m_wait_for_settings_update = true;
    err_code = pstorage_store(&m_bootsettings_handle,
                              (uint8_t *)p_settings,
                              sizeof(bootloader_settings_t),
                              0);
    if (err_code != NRF_SUCCESS)
    {
        return;
    }

    while (m_wait_for_settings_update)
    {
        ;
    }
}

/**@brief Function for initializing the Bootloader. */
uint32_t bootloader_init(void)
{
    uint32_t                err_code;
    pstorage_module_param_t storage_params =
    {
        .cb = pstorage_callback_handler
    };

    err_code = pstorage_init();
    NRF_ERROR_CHECK(err_code);

    m_bootsettings_handle.block_id = BOOTLOADER_SETTINGS_ADDRESS;
    err_code = pstorage_register(&storage_params, &m_bootsettings_handle);

    return err_code;
}

/**@brief Function for validating application region in flash. */
bool bootloader_app_is_valid(uint32_t app_addr)
{
    static bootloader_settings_t bootloader_settings;

    // There is no application in CODE region 1.
    if (*((uint32_t *)app_addr) == EMPTY_FLASH_MASK)
    {
        return false;
    }

    bootloader_settings_load(&bootloader_settings);

    // In case user doesn't configure BOOTLAODER_SETTINGS.
    if (*(uint32_t *)(&bootloader_settings) == EMPTY_FLASH_MASK)
    {
        return true;
    }

    // The application in CODE region 1 is flagged as valid during update.
    if (!(bootloader_settings.bank_0 & BANK_VALID_APP))
    {
        return false;
    }

    return true;
}

/**@brief Function for exiting bootloader and booting into application. */
uint32_t bootloader_app_start(uint32_t app_addr)
{
    // If the applications CRC has been checked and passed, the magic number will be written and we
    // can start the application safely.
    uint32_t err_code = sd_softdevice_disable();
    NRF_ERROR_CHECK(err_code);

    interrupts_disable();

    err_code = sd_softdevice_vector_table_base_set(CODE_REGION_1_START);
    NRF_ERROR_CHECK(err_code);

    util_reset(CODE_REGION_1_START);

    // Previous call should change register values and jump into application, so it is forbidden to
    // reach this line.
    return NRF_ERROR_FORBIDDEN;
}

/**@brief Function getting state of DFU update in progress. */
bool bootloader_dfu_update_in_progress(void)
{
    static bootloader_settings_t bootloader_settings;

    bootloader_settings_load(&bootloader_settings);

    if ((bootloader_settings.bank_1 & BANK_VALID_MASK) &&
       !(bootloader_settings.bank_1 & ~BANK_VALID_MASK))
    {
        return true;
    }

    return false;
}

/**@brief Function for continuing the Device Firmware Update. */
uint32_t bootloader_dfu_update_continue(void)
{
    uint32_t err_code;

    // Ensure that flash operations are not executed within the first 100 ms seconds to allow
    // a debugger to be attached.
    nrf_delay_ms(100);

    err_code = dfu_sd_image_validate();
    if (err_code != NRF_SUCCESS)
    {
        err_code = dfu_sd_image_swap();
        NRF_ERROR_CHECK(err_code);
    }

    err_code = dfu_app_image_validate();
    if (err_code != NRF_SUCCESS)
    {
        err_code = dfu_app_image_swap();
        NRF_ERROR_CHECK(err_code);
    }

    err_code = dfu_bl_image_validate();
    if (err_code != NRF_SUCCESS)
    {
        err_code = dfu_bl_image_swap();
        NRF_ERROR_CHECK(err_code);
    }

    return err_code;
}
