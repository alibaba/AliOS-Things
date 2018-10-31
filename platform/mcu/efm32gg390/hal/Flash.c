/*****************************************************************************/
/**
 *  Copyright (C) Robert Bosch. All Rights Reserved. Confidential.
 *
 *  Distribution only to people who need to know this information in
 *  order to do their job.(Need-to-know principle).
 *  Distribution to persons outside the company, only if these persons
 *  signed a non-disclosure agreement.
 *  Electronic transmission, e.g. via electronic mail, must be made in
 *  encrypted form.
 *
 *  @file
 *
 *  <Brief description about the functionality of the module>
 *
 * ****************************************************************************/

#include "BCDS_Essentials.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_FLASH

#if BCDS_FEATURE_FLASH

/* system header files */
#include "em_int.h"
#include "em_msc.h"
#include "em_dma.h"
#include "em_version.h"
/* additional interface header files */
#include "BCDS_Retcode.h"
/* own header files */
#include "BCDS_MCU_Flash.h"

/* Check the version of the emlib to see if the MSC_WriteWord bug is fixed */
#if (_EMLIB_VERSION_MAJOR > 3)
/** With EMLIB >= 4.x.x. it is OK to write multiple words from a buffer to flash */
#define USE_SINGLE_WORD_WRITES  0
#else
/** With EMLIB <= 3.x.x. only single word writes should be used because of
 * a bug in the implementation of MSC_WriteWord */
#define USE_SINGLE_WORD_WRITES  1
#endif

/* local functions */
static Retcode_T MscErrorMapping(msc_Return_TypeDef returnStatus)
{
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    if (mscReturnOk == returnStatus)
    {
        returnValue = RETCODE_OK;
    }
    else if (mscReturnInvalidAddr == returnStatus)
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_FLASH_INVALID_ADDRESS);
    }
    else if (mscReturnLocked == returnStatus)
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_FLASH_ADDRESS_LOCKED);
    }
    else if (mscReturnTimeOut == returnStatus)
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_FLASH_TIMEOUT);
    }
    else
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    return (returnValue);
}

/* functions */
uint32_t MCU_Flash_GetPageSize()
{
    return FLASH_PAGE_SIZE;
}

/*lint -ecall(534, INT_Enable)  INT_Enable return checks will cause more trouble than having a real benefit here */
/*lint -ecall(534, INT_Disable) INT_Disable return checks will cause more trouble than having a real benefit here */

Retcode_T MCU_Flash_ErasePage(uint32_t * pageAddress)
{
    uint32_t address = (uint32_t) pageAddress;
    Retcode_T retval = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_HAL_FLASH_INVALID_ADDRESS);
    /* Check if address is aligned to flash page size. */
    if (!(address & (FLASH_PAGE_SIZE - 1)))
    {
        msc_Return_TypeDef flashReturnCode;
        MSC_Init();
        INT_Disable();
        flashReturnCode = MSC_ErasePage(pageAddress);
        INT_Enable();
        MSC_Deinit();
        retval = MscErrorMapping(flashReturnCode);

    }
    return retval;
}

Retcode_T MCU_Flash_Erase(uint32_t* startAddress, uint32_t numPages)
{
    Retcode_T ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    uint32_t i = 0;
    uint32_t PageSize = 0;
    PageSize = MCU_Flash_GetPageSize();
    for (i = 0; i < numPages; i++)
    {
        ret = MCU_Flash_ErasePage(startAddress);

        if (RETCODE_OK == ret)
        {
            startAddress = (uint32_t *) ((uint8_t *) startAddress + PageSize);
        }
        else
        {
            return ret;
        }

    }
    return ret;
}

Retcode_T MCU_Flash_Write(const uint8_t* targetAddress, const uint8_t* sourceAddress, uint32_t numBytes)
{
    Retcode_T retval = RETCODE_OK;
    if (sourceAddress && numBytes)
    {
        /* Return value from MSC_XXXX calls */
        msc_Return_TypeDef mscRet = mscReturnOk;
        /* Pointer to traverse data to write */
        uint8_t* pSource = (uint8_t*) sourceAddress;
        /* stores length of padding needed */
        /* Get padding if address is not aligned.*/
        /* Hint targetAddress & 3 equals targetAddress % 4 */
        uint32_t volatile BytesToPad = (uint32_t) targetAddress & 0x03;
        /* used to create a mini stream to store 4 byte sequence to be flashed */
        uint8_t uBytes[4];

        MSC_Init();
        if (BytesToPad)
        {
            /* pad in front */
            /* align address to 4 byte boundaries */
            targetAddress -= BytesToPad;
            /* Get original word from flash */
            /*lint -e419 -e662 -e661 */
            memcpy(uBytes, targetAddress, sizeof(uint32_t));
            /* Get bytes to be changed from input stream */
            for (uint32_t i = BytesToPad; (i < sizeof(uint32_t)) && numBytes; i++)
            {
                uBytes[i] = *pSource;
                pSource++;
                numBytes--;
            }
            /*lint +e419 +e661 +e662 */
            INT_Disable();
            mscRet = MSC_WriteWord((uint32_t*) targetAddress, (void*) uBytes, sizeof(uint32_t));
            INT_Enable();
            retval = MscErrorMapping(mscRet);
            if (RETCODE_OK != retval)
            {
                MSC_Deinit();
                return (retval);
            }
            else
            {
                targetAddress += sizeof(uint32_t);
            }
        }
        /* Address should now be 4 byte aligned, so write all aligned word
         * values if there are any */
        if ((numBytes >= sizeof(uint32_t)) && (RETCODE_OK == retval))
        {
            /*lint -e506 */
            if (USE_SINGLE_WORD_WRITES)
            /*lint +e506 */
            {
                /* By some reason it seems like the MSC_WriteWord function from
                 * EMlib 3.20.7 is not working correctly when it should write
                 * more than 4 bytes at once, so this implmentation uses
                 * single word writes .
                 */
                while ((numBytes >= sizeof(uint32_t)) && (RETCODE_OK == retval))
                {
                    INT_Disable();
                    mscRet = MSC_WriteWord((uint32_t*) targetAddress, (void*) pSource, sizeof(uint32_t));
                    INT_Enable();
                    retval = MscErrorMapping(mscRet);
                    if (RETCODE_OK != retval)
                    {
                        MSC_Deinit();
                        return (retval);
                    }
                    else
                    {
                        numBytes -= sizeof(uint32_t);
                        targetAddress += sizeof(uint32_t);
                        pSource += sizeof(uint32_t);
                    }
                }
            }
            else
            {
                // EMLIB >= 4.x.x
                INT_Disable();
                /*Hint:  numByte & ~3 gets the highest multiple of 4 from  number of bytes */
                mscRet = MSC_WriteWord((uint32_t*) targetAddress, (void*) pSource, (numBytes & ~0x03));
                INT_Enable();
                retval = MscErrorMapping(mscRet);
                if (RETCODE_OK != retval)
                {
                    MSC_Deinit();
                    return (retval);
                }
                else
                {
                    targetAddress += (numBytes & ~0x03);
                    pSource += (numBytes & ~0x03);
                    numBytes -= (numBytes & ~0x03);
                }
            }
        }
        /* if there a bytes left, pad them, target address should be aligned already */
        if ((numBytes > 0) && (RETCODE_OK == retval))
        {
            /* pad at back */
            /* Get original word from flash */
            /*lint -e419 */
            memcpy(uBytes, targetAddress, sizeof(uint32_t));
            for (uint32_t i = 0; i < numBytes; i++)
            {
                uBytes[i] = *pSource;
                pSource++;
            }
            /*lint +e419 */
            INT_Disable();
            mscRet = MSC_WriteWord((uint32_t*) targetAddress, (void*) uBytes, sizeof(uint32_t));
            INT_Enable();
            retval = MscErrorMapping(mscRet);
            if (RETCODE_OK != retval)
            {
                MSC_Deinit();
                return (retval);
            }
        }
        MSC_Deinit();
    }
    return retval;
}

/*lint +ecall(534, INT_Enable) turned on again*/
/*lint +ecall(534, INT_Disable) turned on again*/

Retcode_T MCU_Flash_Read(uint8_t* readFrom, uint8_t* writeTo, uint32_t numBytes)
{
    uint8_t *readAddress;

    if ((readFrom == NULL) || (writeTo == NULL))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }

    readAddress = readFrom;

    for (uint32_t index = 0; index < numBytes; index++)
    {
        writeTo[index] = readAddress[index];
    }

    return RETCODE_OK;
}

#endif
