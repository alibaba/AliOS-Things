/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_FLASH Flash
 *
 * @{
 * @brief Unified flash API for MCU internal flash memory access.
 * @details Usually the internal flash memory gets initialized by the BSP (Board support
 * package). In contrast to other MCU driver API there is no need to provide
 * initialize and deinitialize and control functions for this component.
 *
 * A typical use of this API would start with inclusion of this header file.
 * After that every function can be used without any initialization sequence.
 *
 * @note It is possible to disable the entire flash feature in the HAL-BSP
 * configuration file. I.e. the Flash code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_FLASH</b> is set to 1 in the configuration file.
 *
 * @file
 */

#ifndef BCDS_MCU_FLASH_H
#define BCDS_MCU_FLASH_H

#include "BCDS_HAL.h"
#include "BCDS_Essentials.h"


/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_FLASH

/* Need to include the driver specific header file which should be available
 * with that name for each MCU specific driver. This must be done to get
 * the correct flash page size as a  constant expression else it would not
 * be possible to create e.g. static buffers of the correct size.
 * The defines checked here are usually coming from the MCU specific makefile.
 */

/**
 * \brief Defines the size of a single flash page in bytes
 * \details This value depends on the flash part that is used by the MCU.
 *
 * \note For EFM32 MCUs the flash page size is defined in the particular
 * device include file of the EMLib and taken from there. The size differs
 * depending on the MCU used e.g. FLASH_PAGE_SIZE is 4KB for EFM32GG and
 * 2KB for EFM32LG.
 *
 * @note For STM32 MCUs the flash page size is defined in the particular device
 * include file of STM32Cube and taken from there. Sizes may differ depending on
 * the used MCU e.g. FLASH_PAGE_SIZE is 2 KB for STM32l4XX devices.
 */
#if BCDS_TARGET_PLATFORM == efm32
#include "em_msc.h"
#define MCU_FLASH_PAGE_SIZE     (FLASH_PAGE_SIZE)
#elif BCDS_TARGET_PLATFORM == stm32
#include "stm32l4xx_hal.h"
#define MCU_FLASH_PAGE_SIZE     (FLASH_PAGE_SIZE)
#else
#define MCU_FLASH_PAGE_SIZE     (0)
#error MCU Flash: You have to define the correct device type!
#endif

#include "BCDS_Retcode.h"
/**
 * @brief enumeration of special flash interface error return codes
 *//**< Flash write/erase successful. */
enum RETCODE_HAL_UART_E
{
    RETCODE_HAL_FLASH_INVALID_ADDRESS = RETCODE_FIRST_CUSTOM_CODE, /**< Invalid address. Write to an address that is not flash. */
    RETCODE_HAL_FLASH_ADDRESS_LOCKED,                              /**< Flash address is locked. */
    RETCODE_HAL_FLASH_TIMEOUT,                                     /**< Timeout while writing to flash. */
};

/**
 * @brief Gets the page size of the internal flash device
 * @details Use the function to get the page size of the flash device.
 * This function may be used if a function pointer is needed that is
 * able to return the page size.
 * @return The page size of the flash device in bytes.
 */
uint32_t MCU_Flash_GetPageSize(void);

/**
 * @brief Erases a single page of internal flash
 * @details Use the function to erase a single page of internal flash. Erasing
 * a page is necessary before new values can be written to the flash page.
 *
 * Usage:
 * @code
 * #include "BCDS_MCU_Flash.h"
 * const uint32 MyFlashPageStartAddress = 0x0FE000000
 * if (RETCODE_SUCCESS == MCU_Flash_ErasePage((uint32_t*)MyFlashPageStartAddress))
 * {
 *      // Start writing to flash
 * }
 * @endcode
 * @param pageAddress is a pointer on the start address of the flash page to be
 * erased. Please notice that the address needs to be aligned to flash page
 * size of the particular flash device used. E.g. if the flash page size is
 * 4KB the address needs to be aligned to 4KB boundaries else the call to this
 * function will fail.
 * @retval RETCODE_SUCCESS upon if erasing of the flash page was successful.
 * @retval RETCODE_FAILURE if any error has occurred. Possible reasons may
 * be:
 *  - The flash page is locked
 *  - The address is not aligned to flash page size boundaries
 *  - The passed address is not valid flash address
 *  - The command to erase the page has timed out
 *  @note As this function only erases a single page it is more efficient to
 *  use MCU_Flash_Erase() if an application has to erase multiple pages of flash
 *  memory.
 *  @see MCU_Flash_Erase
 */
Retcode_T MCU_Flash_ErasePage(uint32_t * pageAddress);

/**
 * \brief Erases multiple pages of internal flash
 * @details Use the function to erase multiple pages of internal flash.
 *
 * Usage:
 * @code
 * #include "BCDS_MCU_Flash.h"
 * const uint32_t MyFlashPageStartAddress = 0x000AF000
 * const uint32_t MyNumPages = 3
 * // Erase 3 pages starting at MyFlashPageStartAddress
 * if (RETCODE_SUCCESS == MCU_Flash_Erase((uint32_t*)MyFlashPageStartAddress,MyNumPages)))
 * {
 *      // Start writing to flash
 * }
 * @endcode
 *
 * @param startAddress is a pointer on the start address of the first flash page
 * to be erased. Please notice that the address needs to be aligned to the flash
 * page size of the particular flash device used. E.g. if the flash page size is
 * 4KB then the address needs to be aligned at 4KB boundaries else the call to
 * this function will fail.
 * @param numPages is the number of pages that should be erased starting from
 * the given start address.
 * @retval RETCODE_SUCCESS upon if erasing of the flash pages was successful.
 * @retval RETCODE_FAILURE if any error has occurred. Possible reasons may
 * be:
 *  - The flash page is locked
 *  - The startAddress is not aligned to flash page size boundaries
 *  - The passed startAddress is not a valid flash address
 *  - A command to erase a page has timed out
 *  @see MCU_Flash_ErasePage
 */
Retcode_T MCU_Flash_Erase(uint32_t* startAddress, uint32_t numPages);

/**
 *  @brief Writes multiple byte values beginning at the specified target address.
 *  @details Use the function to write byte values into flash beginning at the
 *  specified target address.
 *
 *  Usage:
 *  @code
 *  #include "BCDS_MCU_Flash.h"
 *  const uint32_t MyFlashPageStartAddress = 0x000AF000
 *  const uint8_t MyValuesToWrite[4] = { 0xFE, 0xED, 0xFA, 0xCE };
 *  // Write 4 Byte values to flash
 *  if (RETCODE_SUCCESS == MCU_Flash_Write((uint8_t*)(MyFlashPageStartAddress),
 *                                  MyValuesToWrite,4))
 *  {
 *       // Do something else
 *  }
 *  @endcode
 *
 *  @param [ in ] targetAddress is the target address at which writing the
 *  byte values should start. The function DOES NOT check whether the
 *  destination address is erased before it writes the values.
 *  @param [ in ] sourceAddress is a pointer on the data to be written.
 *  @param [ in ] numBytes is the number of bytes to write into flash.
 *
 *  @retval RETCODE_SUCCESS if data was successfully written
 *  @retval RETCODE_FAILURE if writing fails. Possible reasons to fail are:
 *      - The flash is locked.
 *      - The provided targetAddress is not a valid flash address.
 *      - The operation timed out.
 *  @note Please note that the flash region to write must be erased before
 *  this function is executed.
 */
Retcode_T MCU_Flash_Write( const uint8_t* targetAddress, const uint8_t* sourceAddress,
                     uint32_t numBytes);
//Retcode_T MCU_Flash_Write( uint8_t* targetAddress, uint8_t* sourceAddress,
//                     uint32_t numBytes);

/**
 *  @brief Reads multiple byte values beginning at the specified target address.
 *  @details Use the function to read multiple byte values from flash
 *  beginning at the specified address.
 *
 *  Usage:
 *  @code
 *  #include "BCDS_MCU_Flash.h"
 *  const uint32_t MyFlashPageStartAddress = 0x000AF000
 *  const uint8_t MyReadBuffer[20];
 *  // Read 20 byte values from flash
 *  if (RETCODE_SUCCESS == MCU_Flash_Read((uint8_t*)(MyFlashPageStartAddress),
 *                                          MyReadBuffer,20))
 *  {
 *       // Do something else
 *  }
 *  @endcode
 *
 *  @param [ in ] readFrom is the target address from which reading the
 *  byte values should start.
 *
 *  @param [ in ] writeTo is a pointer to which the read data will be copied
 *  @param [ in ] numBytes is the number of byte values to read from flash.
 *
 *  @retval RETCODE_SUCCESS if data was successfully read.
 *  @retval RETCODE_FAILURE if reading fails. Possible reasons to fail are:
 *      - The flash is locked.
 *      - The provided targetAddress is not a valid flash address.
 *      - The operation timed out.
 */
Retcode_T MCU_Flash_Read( uint8_t* readFrom, uint8_t* writeTo, uint32_t numBytes);

#endif /* BCDS_FEATURE_FLASH */
#endif /* BCDS_MCU_Flash_H */
/**  @} */
