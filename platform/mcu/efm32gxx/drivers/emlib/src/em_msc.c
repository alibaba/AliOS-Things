/***************************************************************************//**
 * @file
 * @brief Flash controller (MSC) Peripheral API
 * @author Energy Micro AS
 * @version 3.20.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_msc.h"
#if defined(MSC_COUNT) && (MSC_COUNT > 0)

#include "em_system.h"
#if defined( _MSC_TIMEBASE_MASK )
#include "em_cmu.h"
#endif
#include "em_assert.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined( MSC_WRITECTRL_WDOUBLE )
#define WORDS_PER_DATA_PHASE (FLASH_SIZE<(512*1024) ? 1 : 2)
#else
#define WORDS_PER_DATA_PHASE (1)
#endif


#ifdef __CC_ARM  /* MDK-ARM compiler */
msc_Return_TypeDef MscLoadData(uint32_t *data, int num);
msc_Return_TypeDef MscLoadAddress(uint32_t *address);
#endif /* __CC_ARM */

#ifdef __ICCARM__ /* IAR compiler */
__ramfunc msc_Return_TypeDef MscLoadData(uint32_t *data, int num);
__ramfunc msc_Return_TypeDef MscLoadAddress(uint32_t *address);
#endif /* __ICCARM__ */

#ifdef __GNUC__  /* GCC based compilers */
#ifdef __CROSSWORKS_ARM  /* Rowley Crossworks */
msc_Return_TypeDef MscLoadData(uint32_t *data, int num) __attribute__ ((section(".fast")));
msc_Return_TypeDef MscLoadAddress(uint32_t *address) __attribute__ ((section(".fast")));
#else /* Sourcery G++ */
msc_Return_TypeDef MscLoadData(uint32_t *data, int num) __attribute__ ((section(".ram")));
msc_Return_TypeDef MscLoadAddress(uint32_t *address) __attribute__ ((section(".ram")));
#endif /* __CROSSWORKS_ARM */
#endif /* __GNUC__ */

/** @endcond */

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup MSC
 * @brief Flash controller (MSC) Peripheral API
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enables the flash controller for writing.
 * @note
 *   IMPORTANT: This function must be called before flash operations when
 *   AUXHFRCO clock has been changed from default 14MHz band.
 ******************************************************************************/
void MSC_Init(void)
{
#if defined( _MSC_TIMEBASE_MASK )
  uint32_t freq, cycles;
#endif
  /* Unlock the MSC */
  MSC->LOCK = MSC_UNLOCK_CODE;
  /* Disable writing to the flash */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;

#if defined( _MSC_TIMEBASE_MASK )
  /* Configure MSC->TIMEBASE according to selected frequency */
  freq = CMU_ClockFreqGet(cmuClock_AUX);

  if (freq > 7000000)
  {
    /* Calculate number of clock cycles for 1us as base period */
    freq   = (freq * 11) / 10;
    cycles = (freq / 1000000) + 1;

    /* Configure clock cycles for flash timing */
    MSC->TIMEBASE = (MSC->TIMEBASE & ~(_MSC_TIMEBASE_BASE_MASK |
                                       _MSC_TIMEBASE_PERIOD_MASK)) |
                    MSC_TIMEBASE_PERIOD_1US |
                    (cycles << _MSC_TIMEBASE_BASE_SHIFT);
  }
  else
  {
    /* Calculate number of clock cycles for 5us as base period */
    freq   = (freq * 5 * 11) / 10;
    cycles = (freq / 1000000) + 1;

    /* Configure clock cycles for flash timing */
    MSC->TIMEBASE = (MSC->TIMEBASE & ~(_MSC_TIMEBASE_BASE_MASK |
                                       _MSC_TIMEBASE_PERIOD_MASK)) |
                    MSC_TIMEBASE_PERIOD_5US |
                    (cycles << _MSC_TIMEBASE_BASE_SHIFT);
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Disables the flash controller for writing.
 ******************************************************************************/
void MSC_Deinit(void)
{
  /* Disable writing to the flash */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  /* Lock the MSC */
  MSC->LOCK = 0;
}


/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Perform address phase of FLASH write cycle.
 * @details
 *   This function performs the address phase of a Flash write operation by
 *   writing the given flash address to the ADDRB register and issuing the
 *   LADDRIM command to load the address.
 * @note
 *   This function MUST be executed from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 * @param[in] address
 *   Address in flash memory. Must be aligned at a 4 byte boundary.
 * @return
 *   Returns the status of the address load operation, #msc_Return_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   mscReturnLocked - Operation tried to erase a locked area of the flash.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#if defined(__ICCARM__)
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif

msc_Return_TypeDef MscLoadAddress(uint32_t* address)
{
  uint32_t status;

  /* Load address */
  MSC->ADDRB    = (uint32_t) (address);
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  status = MSC->STATUS;
  if (status & (MSC_STATUS_INVADDR | MSC_STATUS_LOCKED))
  {
    /* Check for invalid address */
    if (status & MSC_STATUS_INVADDR)
      return mscReturnInvalidAddr;
    /* Check for write protected page */
    if (status & MSC_STATUS_LOCKED)
      return mscReturnLocked;
  }
  return mscReturnOk;
}

#if defined(__ICCARM__)
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */



/***************************************************************************//**
 * @brief
 *   Perform data phase of FLASH write cycle.
 * @details
 *   This function performs the data phase of a Flash write operation by loading
 *   the given number of 32-bit words to the WDATA register.
 * @note
 *   This function MUST be executed from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 * @param[in] data
 *   Pointer to the first data word to load.
 * @param[in] num
 *   Number of data words (32-bit) to load.
 * @return
 *   Returns the status of the data load operation, #msc_Return_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnTimeOut - Operation timed out waiting for flash operation
 *                      to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#if defined(__ICCARM__)
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif

msc_Return_TypeDef MscLoadData(uint32_t* data, int num)
{
  int      timeOut  = MSC_PROGRAM_TIMEOUT;
  int      i;

  /* Wait for the MSC to be ready for a new data word.
   * Due to the timing of this function, the MSC should
   * already by ready */
  timeOut = MSC_PROGRAM_TIMEOUT;
  while (((MSC->STATUS & MSC_STATUS_WDATAREADY) == 0) && (timeOut != 0))
  {
    timeOut--;
  }

  /* Check for timeout */
  if (timeOut == 0)
    return mscReturnTimeOut;

  /* Load 'num' 32-bit words into write data register. */
  for (i=0; i<num; i++, data++)
    MSC->WDATA = *data;

  /* Trigger write once */
  MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;

  /* Wait for the write to complete */
  timeOut = MSC_PROGRAM_TIMEOUT;
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }

  /* Check for timeout */
  if (timeOut == 0) return mscReturnTimeOut;

  return mscReturnOk;
}
#if defined(__ICCARM__)
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */

/** @endcond */


/***************************************************************************//**
 * @brief
 *   Erases a page in flash memory.
 * @note
 *   This function MUST be executed from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 * @param[in] startAddress
 *   Pointer to the flash page to erase. Must be aligned to beginning of page
 *   boundary.
 * @return
 *   Returns the status of erase operation, #msc_Return_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   mscReturnLocked - Operation tried to erase a locked area of the flash.
 *   mscReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#if defined(__ICCARM__)
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif
msc_Return_TypeDef MSC_ErasePage(uint32_t *startAddress)
{
  int      timeOut  = MSC_PROGRAM_TIMEOUT;

  /* Address must be aligned to pages */
  EFM_ASSERT((((uint32_t) startAddress) & (FLASH_PAGE_SIZE - 1)) == 0);

  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Load address */
  MSC->ADDRB    = (uint32_t) startAddress;
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  /* Check for invalid address */
  if (MSC->STATUS & MSC_STATUS_INVADDR)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnInvalidAddr;
  }

  /* Check for write protected page */
  if (MSC->STATUS & MSC_STATUS_LOCKED)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnLocked;
  }

  /* Send erase page command */
  MSC->WRITECMD = MSC_WRITECMD_ERASEPAGE;

  /* Wait for the erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }

  if (timeOut == 0)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnTimeOut;
  }

  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  return mscReturnOk;
}
#if defined(__ICCARM__)
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */



/***************************************************************************//**
 * @brief
 *   Writes a single word to flash memory. Data to write must be aligned to
 *   words and contain a number of bytes that is divisable by four.
 * @note
 *   The flash must be erased prior to writing a new word.
 *   This function must be run from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 *
 * @param[in] address
 *   Pointer to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] numBytes
 *   Number of bytes to write from flash. NB: Must be divisable by four.
 * @return
 *   Returns the status of the write operation, #msc_Return_TypeDef
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - Operation tried to erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#if defined(__ICCARM__)
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif

msc_Return_TypeDef MSC_WriteWord(uint32_t *address, void const *data, int numBytes)
{
  int wordCount;
  int numWords;
#if defined(_EFM32_TINY_FAMILY) || defined (_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY)
  int pageWords;
  uint32_t* pData;
#endif
  msc_Return_TypeDef retval = mscReturnOk;

  /* Check alignment (Must be aligned to words) */
  EFM_ASSERT(((uint32_t) address & 0x3) == 0);

  /* Check number of bytes. Must be divisable by four */
  EFM_ASSERT((numBytes & 0x3) == 0);

  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Convert bytes to words */
  numWords = numBytes >> 2;

#if (defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY)) && (2==WORDS_PER_DATA_PHASE)

  /* For the Giant and Wonder families we want to use the double word write
   * feature in order to improve the speed. The devices with flash size
   * larger than 512KiBytes support double word write cycles. The address is
   * loaded by the software for the first word, and automatically incremented
   * by the hardware for the second word. However this will not work across a
   * page boundary, so we need to align the address of double word write
   * cycles to an even address.
   */

  if ((((uint32_t) address) % FLASH_PAGE_SIZE + numBytes)
      > FLASH_PAGE_SIZE)
  {
    if (((uint32_t) address) & 0x7)
    {
      MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
      retval = MscLoadAddress(address);
      if (mscReturnOk != retval) goto msc_write_word_exit;
      retval = MscLoadData((uint32_t *) data, 1);
      if (mscReturnOk != retval) goto msc_write_word_exit;
      data = (void *) ((uint32_t) data + sizeof(uint32_t));
      address++;
      numWords--;
    }
  }

  /* If there is an odd number of words remaining to be written,
   * we write the last word now because this has to be written
   * as a single word and will simplify the for() loop below writing
   * double words. */

  if (numWords & 0x1)
  {
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
    retval = MscLoadAddress(address + numWords - 1);
    if (mscReturnOk != retval) goto msc_write_word_exit;
    retval = MscLoadData(((uint32_t *) data) + numWords - 1, 1);
    if (mscReturnOk != retval) goto msc_write_word_exit;
    numWords--;
  }

  MSC->WRITECTRL |= MSC_WRITECTRL_WDOUBLE;

#endif /* (defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY)) && (2==WORDS_PER_DATA_PHASE) */

#if defined(_EFM32_TINY_FAMILY) || defined (_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY)

  pData = (uint32_t*) data;

  /* The following loop splits the data into chunks corresponding to flash pages.
     The address is loaded only once per page, because the hardware automatically
     increments the address internally for each data load inside a page. */
  for (wordCount = 0; wordCount < numWords; )
  {
    /* First we load address. The address is auto-incremented within a page.
       Therefore the address phase is only needed once for each page. */
    retval = MscLoadAddress(address + wordCount);
    if (mscReturnOk != retval) goto msc_write_word_exit;

    /* Compute the number of words to write to the current page. */
    pageWords =
      (FLASH_PAGE_SIZE - ((uint32_t) (address + wordCount)) % FLASH_PAGE_SIZE) /
      sizeof(uint32_t);
    if (pageWords > numWords-wordCount)
      pageWords = numWords-wordCount;
    wordCount += pageWords;

    /* Now program the data in this page. */
    for (; pageWords; pData+=WORDS_PER_DATA_PHASE, pageWords-=WORDS_PER_DATA_PHASE)
    {
      retval = MscLoadData(pData, WORDS_PER_DATA_PHASE);
      if (mscReturnOk != retval)
        goto msc_write_word_exit;
    }
  }

#else /* _EFM32_GECKO_FAMILY  */

  for (wordCount = 0; wordCount < numWords; wordCount++)
  {
    retval = MscLoadAddress(address + wordCount);
    if (mscReturnOk != retval)
      goto msc_write_word_exit;
    retval = MscLoadData(((uint32_t *) data) + wordCount, 1);
    if (mscReturnOk != retval)
      goto msc_write_word_exit;
  }

#endif

 msc_write_word_exit:

  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;

#if (defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY)) && (2==WORDS_PER_DATA_PHASE)
  /* Turn off double word write cycle support. */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
#endif

  return retval;
}

#if defined(__ICCARM__)
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */


#if defined( _MSC_MASSLOCK_MASK )
/***************************************************************************//**
 * @brief
 *   Erase entire flash in one operation
 * @note
 *   This command will erase the entire contents of the device.
 *   Use with care, both a debug session and all contents of the flash will be
 *   lost. The lock bit, MLW will prevent this operation from executing and
 *   might prevent successful mass erase.
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
msc_Return_TypeDef MSC_MassErase(void)
{
  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Unlock device mass erase */
  MSC->MASSLOCK = MSC_MASSLOCK_LOCKKEY_UNLOCK;

  /* Erase first 512K block */
  MSC->WRITECMD = MSC_WRITECMD_ERASEMAIN0;

  /* Waiting for erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY))
  {
  }

#if FLASH_SIZE >= (512 * 1024)
  /* Erase second 512K block */
  MSC->WRITECMD = MSC_WRITECMD_ERASEMAIN1;

  /* Waiting for erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY))
  {
  }
#endif

  /* Restore mass erase lock */
  MSC->MASSLOCK = MSC_MASSLOCK_LOCKKEY_LOCK;

  /* This will only successfully return if calling function is also in SRAM */
  return mscReturnOk;
}
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */
#endif

/** @} (end addtogroup MSC) */
/** @} (end addtogroup EM_Library) */
#endif /* defined(MSC_COUNT) && (MSC_COUNT > 0) */
