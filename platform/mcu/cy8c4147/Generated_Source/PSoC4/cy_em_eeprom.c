/***************************************************************************//**
* \file cy_em_eeprom.c
* \version 2.0
*
* \brief
*  This file provides source code of the API for the Emulated EEPROM library.
*  The Emulated EEPROM API allows creating of an emulated EEPROM in flash that
*  has the ability to do wear leveling and restore corrupted data from a
*  redundant copy.
*
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "cytypes.h"
#include <string.h>

#if (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC6)
    #include "em_eeprom/cy_em_eeprom.h"
#else
    #include "cy_em_eeprom.h"
#endif /* (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC6) */


#if defined(__cplusplus)
extern "C" {
#endif


/***************************************
* Private Function Prototypes
***************************************/
static void FindLastWrittenRow(uint32 * lastWrRowPtr, cy_stc_eeprom_context_t * context);
static uint32 GetRowAddrBySeqNum(uint32 seqNum, cy_stc_eeprom_context_t * context);
static uint8 CalcChecksum(uint8 rowData[], uint32 len);
static void GetNextRowToWrite(uint32 seqNum,
                            uint32 * rowToWrPtr,
                            uint32 * rowToRdPtr,
                            cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t CheckRanges(cy_stc_eeprom_config_t* config);
static cy_en_em_eeprom_status_t WriteRow(uint32 rowAddr, uint32 *rowData, cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t EraseRow(uint32 rowAddr, uint32 ramBuffAddr, cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t CheckCrcAndCopy(uint32 startAddr,
                                                uint32 dstAddr,
                                                uint32 rowOffset,
                                                uint32 numBytes,
                                                cy_stc_eeprom_context_t * context);
static uint32 GetAddresses(uint32 *startAddr, uint32 *endAddr, uint32 *offset, uint32 rowNum, uint32 addr, uint32 len);
static cy_en_em_eeprom_status_t FillChecksum(cy_stc_eeprom_context_t * context);

/**
* \addtogroup group_em_eeprom_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Init
****************************************************************************//**
*
* Initializes the Emulated EEPROM library by filling the context structure. 
*
* \param config
* The pointer to a configuration structure. See \ref cy_stc_eeprom_config_t.
*
* \param context
* The pointer to the EEPROM context structure to be filled by the function.
* \ref cy_stc_eeprom_context_t.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
* \note
* The context structure should not be modified by the user after it is filled
* with this function. Modification of context structure may cause the 
* unexpected behavior of the Cy_Em_EEPROM API functions which rely on it.
*
* \note
* This function uses a buffer of the flash row size to perform read
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
* \sideeffect 
* If the "Redundant Copy" option is used, the function performs a number of 
* write operations to the EEPROM to initialize flash rows checksums. Therefore,
* Cy_Em_EEPROM_NumWrites(), when it is called right after Cy_Em_EEPROM_Init(), 
* will return a non-zero value that identifies the number of writes performed 
* by Cy_Em_EEPROM_Init().
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Init(cy_stc_eeprom_config_t* config, cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;

    if((NULL != context) && (NULL != config) && (NULL != ((uint32 *)config->userFlashStartAddr)) &&
        (config->wearLevelingFactor <= CY_EM_EEPROM_MAX_WEAR_LEVELING_FACTOR) && (config->eepromSize != 0u))
    {
        ret = CheckRanges(config);

        if(CY_EM_EEPROM_SUCCESS == ret)
        {
            /* Copy the user config structure fields into context */
            context->eepromSize = config->eepromSize;
            context->wearLevelingFactor = config->wearLevelingFactor;
            context->redundantCopy = config->redundantCopy;
            context->blockingWrite = config->blockingWrite;
            context->userFlashStartAddr = config->userFlashStartAddr;
            /* Store frequently used data for internal use */
            context->numberOfRows = CY_EM_EEPROM_GET_NUM_ROWS_IN_EEPROM(config->eepromSize);
            context->wlEndAddr = ((CY_EM_EEPROM_GET_EEPROM_SIZE(context->numberOfRows) * config->wearLevelingFactor) +
                    config->userFlashStartAddr);
            /* Find last written EEPROM row and store it for quick access */
            FindLastWrittenRow(&context->lastWrRowAddr, context);

            if((0u == CY_EM_EEPROM_GET_SEQ_NUM(context->lastWrRowAddr)) && (0u != context->redundantCopy))
            {
                /* Call the function only after device reprogramming in case
                * if redundant copy is enabled.
                */
                ret = FillChecksum(context);
                
                /* Update the last written EEPROM row for Cy_Em_EEPROM_NumWrites() */
                FindLastWrittenRow(&context->lastWrRowAddr, context);
            }
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Read
****************************************************************************//**
*
* This function takes the logical EEPROM address, converts it to the actual
* physical address where the data is stored and returns the data to the user.
*
* \param addr
* The logical start address in EEPROM to start reading data from.
*
* \param eepromData
* The pointer to a user array to write data to.
*
* \param size
* The amount of data to read.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
* \note
* This function uses a buffer of the flash row size to perform read
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
* \note
* In case if redundant copy option is enabled the function may perform writes
* to EEPROM. This is done in case if the data in the EEPPROM is corrupted and
* the data in redundant copy is valid based on CRC-8 data integrity check.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Read(uint32 addr, 
                                        void * eepromData, 
                                        uint32 size,
                                        cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;
    uint32 i;
    uint32 numBytesToRead;
    uint32 curEepromBaseAddr;
    uint32 curRowOffset;
    uint32 startRowAddr;
    uint32 actEepromRowNum;
    uint32 curRdEepromRowNum = 0u;
    uint32 dataStartEepromRowNum = 0u;
    uint32 eeData = (uint32) eepromData; /* To avoid the pointer arithmetic with void */

    /* Validate input parameters */
    if((0u != size) && ((addr + size) <= (context->eepromSize)) && (NULL != eepromData))
    {
        uint32 rdAddr = addr;
        uint32 rdSize = size;
        /* Get the sequence number of the last written row */
        uint32 seqNum = CY_EM_EEPROM_GET_SEQ_NUM(context->lastWrRowAddr);
        uint32 updateAddrFlag = 0u;

        /* Calculate the number of the row read operations. Currently this only concerns
        * the reads from the EEPROM data locations.
        */
        uint32 numRowReads = ((((rdAddr + rdSize) - 1u) / CY_EM_EEPROM_EEPROM_DATA_LEN) -
                              (rdAddr / CY_EM_EEPROM_EEPROM_DATA_LEN)) + 1u;

        /* Get the address of the first row of the currently active EEPROM sector. If
        * no wear leveling is used - the EEPROM has only one sector, so use the base
        * addr stored in "context->userFlashStartAddr".
        */
        curEepromBaseAddr = (((context->lastWrRowAddr - context->userFlashStartAddr) /
                              (CY_EM_EEPROM_FLASH_SIZEOF_ROW * context->numberOfRows)) *
                              (CY_EM_EEPROM_FLASH_SIZEOF_ROW * context->numberOfRows)) +
                               context->userFlashStartAddr;

        /* Find the number of the row that contains the start address of the data */
        for(i = 0u; i < context->numberOfRows; i++)
        {
            if(0u != CY_EM_EEPROM_IS_ADDR_IN_ROW_RANGE(rdAddr, i))
            {
                dataStartEepromRowNum = i;
                curRdEepromRowNum = dataStartEepromRowNum;
                break;
            }
        }

        /* Find the row number of the last written row */
        actEepromRowNum = (context->lastWrRowAddr - curEepromBaseAddr) / CY_EM_EEPROM_FLASH_SIZEOF_ROW;

        /* Check if wear leveling is used */
        if(context->wearLevelingFactor > 1u)
        {
            uint32 dataEndEepromRowNum = dataStartEepromRowNum + (numRowReads - 1u);

            /* Check if the future validation of the read address is required. */
            updateAddrFlag = (dataStartEepromRowNum > actEepromRowNum) ? 1u :
                              ((dataEndEepromRowNum > actEepromRowNum) ? 1u : 0u);
        }

        /* Copy data from the EEPROM data locations to the user buffer */
        for(i = 0u; i < numRowReads; i++)
        {
            startRowAddr = curEepromBaseAddr + (curRdEepromRowNum * CY_EM_EEPROM_FLASH_SIZEOF_ROW);
            curRowOffset = CY_EM_EEPROM_EEPROM_DATA_LEN + (rdAddr % CY_EM_EEPROM_EEPROM_DATA_LEN);

            /* Check if there are more reads pending and update the number of the
            * remaining bytes to read respectively.
            */
            if((i + 1u) < numRowReads)
            {
                numBytesToRead = CY_EM_EEPROM_EEPROM_DATA_LEN - (rdAddr % CY_EM_EEPROM_EEPROM_DATA_LEN);
            }
            else
            {
                numBytesToRead = rdSize;
            }

            /* Check if the read address needs to be updated to point to the correct
            * EEPROM sector.
            */
            if((0u != updateAddrFlag) && (curRdEepromRowNum > actEepromRowNum))
            {
                startRowAddr -= context->numberOfRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW;

                if(startRowAddr < context->userFlashStartAddr)
                {
                    startRowAddr = context->wlEndAddr -
                        ((context->numberOfRows - curRdEepromRowNum) * CY_EM_EEPROM_FLASH_SIZEOF_ROW);
                }
            }

            if(0u != context->redundantCopy)
            {
                /* Check a checksum of the EEPROM row and if it is bad, check a checksum in
                * the corresponding row in redundant copy, otherwise return failure.
                */
                ret = CheckCrcAndCopy(startRowAddr, eeData, curRowOffset, numBytesToRead, context);

                if(CY_EM_EEPROM_SUCCESS != ret)
                {
                    break;
                }
            }
            else
            {
                /* Copy the data to the user buffer */
                (void)memcpy((void *)(eeData),
                             (void *)(startRowAddr + curRowOffset),
                             numBytesToRead);

                /* Indicate success to be able to execute next code block */
                ret = CY_EM_EEPROM_SUCCESS;
            }

            /* Update variables anticipated in the read operation */
            rdAddr += numBytesToRead;
            rdSize -= numBytesToRead;
            eeData += numBytesToRead;
            curRdEepromRowNum++;
        }

        /* This code block will copy the latest data from the EEPROM headers into the
        * user buffer. The data previously copied into the user buffer may be updated
        * as the EEPROM headers contain more recent data. 
        * The code block is executed when two following conditions are true:
        *  1) The reads from "historic" data locations were successful;
        *  2) The user performed at least one write operation to Em_EEPROM (0u !=
        *     seqNum).        
        */
        if((CY_EM_EEPROM_SUCCESS == ret) && (0u != seqNum))
        {
            numRowReads = (context->numberOfRows <= seqNum) ? (context->numberOfRows) : (seqNum);
            numRowReads--;

            for(i = (seqNum - numRowReads); i <= seqNum; i++)
            {
                startRowAddr = GetRowAddrBySeqNum(i, context);

                if (0u != startRowAddr)
                {
                    /* The following variables are introduced to increase code readability. */
                    uint32 startAddr  = *(uint32 *)(startRowAddr + CY_EM_EEPROM_HEADER_ADDR_OFFSET);
                    uint32 endAddr    = startAddr + (*(uint32 *)(startRowAddr + CY_EM_EEPROM_HEADER_LEN_OFFSET));

                    /* Check if the current row EEPROM header contains the data requested for read */
                    if(0u != CY_EM_EEPROM_IS_ADDRESES_CROSSING(startAddr, endAddr, addr, addr + size))
                    {
                        uint32 srcOffset = (startAddr > addr) ? (0u) : (addr - startAddr);
                        uint32 dstOffset = (startAddr > addr) ? (startAddr - addr): (0u);
                        rdAddr = (startAddr > addr) ? (startAddr) : (addr);

                        srcOffset += CY_EM_EEPROM_HEADER_DATA_OFFSET;

                        /* Calculate the number of bytes to be read from the current row's EEPROM header */
                        numBytesToRead = ((endAddr < (addr + size)) ? endAddr : (addr + size)) - rdAddr;

                        /* Calculate the offset in the user buffer from which the data will be updated. */
                        eeData = ((uint32)eepromData) + dstOffset;

                        /* Check a checksum of the EEPROM row and if it is bad, check a checksum in the
                        * corresponding row in redundant copy, otherwise return failure. Copy the data
                        * from the recent EEPROM headers to the user buffer. This will overwrite the
                        * data copied form EEPROM data locations as the data in EEPROM headers is newer.
                        */
                        if(0u != context->redundantCopy)
                        {
                            ret = CheckCrcAndCopy(startRowAddr, eeData, srcOffset, numBytesToRead, context);

                            if(CY_EM_EEPROM_SUCCESS != ret)
                            {
                                break;
                            }
                        }
                        else
                        {
                            (void)memcpy((void *)(eeData), (void *)(startRowAddr + srcOffset), numBytesToRead);
                        }
                    }
                }
            }
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Write
****************************************************************************//**
*
* This function takes the logical EEPROM address and converts it to the actual
* physical address and writes data there. If wear leveling is implemented, the
* writing process will use the wear leveling techniques. This is a blocking
* function and it does not return until the write operation is completed. The
* user firmware should not enter Hibernate mode until write is completed. The
* write operation is allowed in Sleep and Deep-Sleep modes. During the flash
* operation, the device should not be reset, including the XRES pin, a software
* reset, and watchdog reset sources. Also, low-voltage detect circuits should
* be configured to generate an interrupt instead of a reset. Otherwise, portions
* of flash may undergo unexpected changes.
*
* \param addr
* The logical start address in EEPROM to start writing data from.
*
* \param eepromData
* Data to write to EEPROM.
*
* \param size
* The amount of data to write to EEPROM.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
* \note
* This function uses a buffer of the flash row size to perform write
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
* \sideeffect
* In case when blocking write option is used, if this function is called by
* the CM4 the user code on CM0P and the user code on CM4 are blocked until erase
* flash row operation is finished. If this function is called by the CM0P the
* user code on CM4 is not blocked and the user code on CM0P is blocked until
* erase flash row operation is finished. Plan your task allocation accordingly.
*
* \sideeffect
* In case if non-blocking write option is used and when user flash is used as
* an EEPROM storage care should be taken to prevent the read while write (RWW)
* exception. To prevent the RWW exception the user flash macro that includes
* the EEPROM storage should not be read while the EEPROM write is not completed.
* The read also means the user code execution from the respective flash macro.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Write(uint32 addr,
                                            void * eepromData,
                                            uint32 size,
                                            cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;
    uint32 i;
    uint32 wrCnt;
    uint32 actEmEepromRowNum;
    uint32 writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW / CY_EM_EEPROM_U32_DIV];
    uint32 startAddr = 0u;
    uint32 endAddr = 0u;
    uint32 tmpRowAddr;
    uint32 emEepromRowAddr = context->lastWrRowAddr;
    uint32 emEepromRowRdAddr;
    void * tmpData;
    uint32 eeData = (uint32) eepromData; /* To avoid the pointer arithmetic with void */

    /* Check if the EEPROM data does not exceed the EEPROM capacity */
    if((0u != size) && ((addr + size) <= (context->eepromSize)) && (NULL != eepromData))
    {
        uint32 numWrites = ((size - 1u) / CY_EM_EEPROM_HEADER_DATA_LEN) + 1u;
        uint32 eeHeaderDataOffset = 0u;

        for(wrCnt = 0u; wrCnt < numWrites; wrCnt++)
        {
            uint32 skipOperation = 0u;
            /* Get the sequence number of the last written row */
            uint32 seqNum = CY_EM_EEPROM_GET_SEQ_NUM(emEepromRowAddr);

            /* Get the address of the row to be written. The "emEepromRowAddr" may be
            * updated with the proper address (if wear leveling is used). The
            * "emEepromRowRdAddr" will point to the row address from which the historic
            * data will be read into the RAM buffer.
            */
            GetNextRowToWrite(seqNum, &emEepromRowAddr, &emEepromRowRdAddr, context);

            /* Clear the RAM buffer so to not put junk into flash */
            (void)memset(writeRamBuffer, 0, CY_EM_EEPROM_FLASH_SIZEOF_ROW);

            /* Fill the EM_EEPROM header info for the row in the RAM buffer */
            seqNum++;
            writeRamBuffer[CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32] = seqNum;
            writeRamBuffer[CY_EM_EEPROM_HEADER_ADDR_OFFSET_U32] = addr;
            tmpData = (void *) eeData;

            /* Check if this is the last row to write */
            if(wrCnt == (numWrites - 1u))
            {
                /* Fill in the remaining size value to the EEPROM header. */
                writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32] = size;
            }
            else
            {
                /* This is not the last row to write in the current EEPROM write operation.
                * Write the maximum possible data size to the EEPROM header. Update the
                * size, eeData and addr respectively.
                */
                writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32] = CY_EM_EEPROM_HEADER_DATA_LEN;
                size -= CY_EM_EEPROM_HEADER_DATA_LEN;
                addr += CY_EM_EEPROM_HEADER_DATA_LEN;
                eeData += CY_EM_EEPROM_HEADER_DATA_LEN;
            }

            /* Write the data to the EEPROM header */
            (void)memcpy((void *)&writeRamBuffer[CY_EM_EEPROM_HEADER_DATA_OFFSET_U32],
                         tmpData,
                         writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32]);

            if(emEepromRowRdAddr != 0UL)
            {
                /* Copy the EEPROM historic data for this row from flash to RAM */
                (void)memcpy((void *)&writeRamBuffer[CY_EM_EEPROM_EEPROM_DATA_OFFSET_U32],
                             (void *)(emEepromRowRdAddr + CY_EM_EEPROM_EEPROM_DATA_LEN),
                             CY_EM_EEPROM_EEPROM_DATA_LEN);
            }

            /* Check if there is data for this location in other EEPROM headers:
            * find out the row with the lowest possible sequence number which
            * may contain the data for the current row.
            */
            i = (seqNum > context->numberOfRows) ? ((seqNum - (context->numberOfRows)) + 1u) : 1u;

            for(; i <= seqNum; i++)
            {
                if(i == seqNum)
                {
                    /* The code reached the row that is about to be written. Analyze the recently
                    * created EEPROM header (stored in the RAM buffer currently): if it contains
                    * the data for EEPROM data locations in the row that is about to be written.
                    */
                    tmpRowAddr = (uint32) writeRamBuffer;
                }
                else
                {
                    /* Retrieve the address of the previously written row by its sequence number.
                    * The pointer will be used to get data from the respective EEPROM header.
                    */
                    tmpRowAddr = GetRowAddrBySeqNum(i, context);
                }

                actEmEepromRowNum = CY_EM_EEPROM_GET_ACT_ROW_NUM_FROM_ADDR(emEepromRowAddr,
                                                                           context->numberOfRows,
                                                                           context->userFlashStartAddr);
                if(0UL != tmpRowAddr)
                {
                    /* Calculate the required addressed for the later EEPROM historic data update */
                    skipOperation = GetAddresses(
                                              &startAddr,
                                              &endAddr,
                                              &eeHeaderDataOffset,
                                              actEmEepromRowNum,
                                              *(uint32 *)(tmpRowAddr + CY_EM_EEPROM_HEADER_ADDR_OFFSET),
                                              *(uint32 *)(tmpRowAddr + CY_EM_EEPROM_HEADER_LEN_OFFSET));
                }
                else
                {
                    /* Skip writes to the RAM buffer */
                    skipOperation++;
                }

                /* Write data to the RAM buffer */
                if(0u == skipOperation)
                {
                    uint32 dataAddr = ((uint32)((uint8 *)&writeRamBuffer)) + startAddr;

                    /* Update the address to point to the EEPROM header data and not to
                    * the start of the row.
                    */
                    tmpRowAddr = tmpRowAddr + CY_EM_EEPROM_HEADER_DATA_OFFSET + eeHeaderDataOffset;
                    (void)memcpy((void *)(dataAddr), (void *)(tmpRowAddr), endAddr - startAddr);
                }

                /* Calculate the checksum if redundant copy is enabled */
                if(0u != context->redundantCopy)
                {
                    writeRamBuffer[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32] = (uint32)
                        CalcChecksum((uint8 *) &writeRamBuffer[CY_EM_EEPROM_EEPROM_DATA_OFFSET_U32],
                                                  CY_EM_EEPROM_EEPROM_DATA_LEN);
                }
            }

            /* Write the data to the specified flash row */
            ret = WriteRow(emEepromRowAddr, writeRamBuffer, context);
            tmpRowAddr = emEepromRowAddr;

            /* Check if redundant copy is used */
            if((0u != context->redundantCopy) && (CY_EM_EEPROM_SUCCESS == ret))
            {
                /* Update the row address to point to the row in the redundant EEPROM's copy */
                tmpRowAddr = (emEepromRowAddr - context->userFlashStartAddr) + context->wlEndAddr;

                /* Write the data to the specified flash row */
                ret = WriteRow(tmpRowAddr, writeRamBuffer, context);
            }

            if(CY_EM_EEPROM_SUCCESS == ret)
            {
                /* Store last written row address only when EEPROM and redundant
                * copy writes were successful.
                */
                context->lastWrRowAddr = emEepromRowAddr;
            }
            else
            {
                break;
            }
        }
    }
    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Erase
****************************************************************************//**
*
* This function erases the entire contents of the EEPROM. Erased values are all
* zeros. This is a blocking function and it does not return until the write
* operation is completed. The user firmware should not enter Hibernate mode until
* erase is completed. The erase operation is allowed in Sleep and Deep-Sleep modes.
* During the flash operation, the device should not be reset, including the
* XRES pin, a software reset, and watchdog reset sources. Also, low-voltage
* detect circuits should be configured to generate an interrupt instead of a
* reset. Otherwise, portions of flash may undergo unexpected changes.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
* \note
* For all non PSoC 6 devices the erase operation is performed by clearing
* the EEPROM data using flash write. This affects the flash durability.
* So it is recommended to use this function in utmost case to prolongate
* flash life.
*
* \note
* This function uses a buffer of the flash row size to perform erase
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
* \sideeffect
* In case when blocking write option is used, if this function is called by
* the CM4 the user code on CM0P and the user code on CM4 are blocked until erase
* flash row operation is finished. If this function is called by the CM0P the
* user code on CM4 is not blocked and the user code on CM0P is blocked until
* erase flash row operation is finished. Plan your task allocation accordingly.
*
* \sideeffect
* In case if non-blocking write option is used and when user flash is used as
* an EEPROM storage care should be taken to prevent the read while write (RWW)
* exception. To prevent the RWW exception the user flash macro that includes
* the EEPROM storage should not be read while the EEPROM erase is not completed.
* The read also means the user code execution from the respective flash macro.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Erase(cy_stc_eeprom_context_t * context)
{
    uint32 i;
    uint32 seqNum;
    uint32 emEepromRowAddr = context->lastWrRowAddr;
    uint32 emEepromRowRdAddr;
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;
    uint32 writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW / CY_EM_EEPROM_U32_DIV] = {0u};
#if (CY_PSOC6)
    uint32 emEepromStoredRowAddr = context->lastWrRowAddr;
    uint32 storedSeqNum;
#endif /* (!CY_PSOC6) */

    /* Get the sequence number of the last written row */
    seqNum = CY_EM_EEPROM_GET_SEQ_NUM(emEepromRowAddr);

    /* If there were no writes to EEPROM - nothing to erase */
    if(0u != seqNum)
    {
        /* Calculate the number of row erase operations required */
        uint32 numWrites = context->numberOfRows * context->wearLevelingFactor;

    #if (CY_PSOC6)
        GetNextRowToWrite(seqNum, &emEepromStoredRowAddr, &emEepromRowRdAddr, context);
        storedSeqNum = seqNum + 1u;
    #endif /* (CY_PSOC6) */

        if(0u != context->redundantCopy)
        {
            writeRamBuffer[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32] = (uint32)
                        CalcChecksum((uint8 *) &writeRamBuffer[CY_EM_EEPROM_EEPROM_DATA_OFFSET_U32],
                                                  CY_EM_EEPROM_EEPROM_DATA_LEN);
        }

        for(i = 0u; i < numWrites; i++)
        {
        #if (CY_PSOC6)
            /* For PSoC 6 the erase operation moves backwards. From last written row
            * identified by "seqNum" down to "seqNum" - "numWrites". If "emEepromRowAddr"
            * is zero this means that the row identified by "seqNum" was previously 
            * erased.
            */
            if(0u != emEepromRowAddr)
            {
                ret = EraseRow(emEepromRowAddr, (uint32)writeRamBuffer, context);
            }

            seqNum--;

            if(0u == seqNum)
            {
                /* Exit the loop as there is no more row is EEPROM to be erased */
                break;
            }
            emEepromRowAddr = GetRowAddrBySeqNum(seqNum, context);
        #else
            seqNum = CY_EM_EEPROM_GET_SEQ_NUM(emEepromRowAddr);
            /* Get the address of the row to be erased. "emEepromRowAddr" may be updated
            * with the proper address (if wear leveling is used).
            */
            GetNextRowToWrite(seqNum, &emEepromRowAddr, &emEepromRowRdAddr, context);
            seqNum++;
            writeRamBuffer[0u] = seqNum;
            ret = EraseRow(emEepromRowAddr, (uint32)writeRamBuffer, context);
        #endif /* (CY_PSOC6) */
        }

    #if (CY_PSOC6)
        if(CY_EM_EEPROM_SUCCESS == ret)
        {
            writeRamBuffer[0u] = storedSeqNum;

            /* Write the previously stored sequence number to the flash row which would be
            * written next if the erase wouldn't happen. In this case the write to
            * redundant copy can be skipped as it does not add any value.
            */
            ret = WriteRow(emEepromStoredRowAddr, writeRamBuffer, context);

            if(CY_EM_EEPROM_SUCCESS == ret)
            {
                context->lastWrRowAddr = emEepromStoredRowAddr;
            }
        }
    #endif /* (CY_PSOC6) */

    }
    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_NumWrites
****************************************************************************//**
*
* Returns the number of the EEPROM writes completed so far.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* The number of writes performed to the EEPROM.
*
*******************************************************************************/
uint32 Cy_Em_EEPROM_NumWrites(cy_stc_eeprom_context_t * context)
{
    return(CY_EM_EEPROM_GET_SEQ_NUM(context->lastWrRowAddr));
}

/** \} */

/** \cond INTERNAL */


/*******************************************************************************
* Function Name: FindLastWrittenRow
****************************************************************************//**
*
* Performs a search of the last written row address of the EEPROM associated
* with the context structure. If there were no writes to the EEPROM the
* function returns the start address of the EEPROM. The row address is returned
* in the input parameter.
*
* \param lastWrRowPtr
* The pointer to a memory where the last written row will be returned.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
*******************************************************************************/
static void FindLastWrittenRow(uint32 * lastWrRowPtr, cy_stc_eeprom_context_t * context)
{
    uint32 seqNum = 0u;
    uint32 prevSeqNum = 0u;
    uint32 numRows;
    uint32 emEepromAddr = context->userFlashStartAddr;

    *lastWrRowPtr = emEepromAddr;

    for(numRows = 0u; numRows < (context->numberOfRows * context->wearLevelingFactor); numRows++)
    {
        seqNum = CY_EM_EEPROM_GET_SEQ_NUM(emEepromAddr);
        if((0u != seqNum) && (seqNum > prevSeqNum))
        {
            /* Some record in EEPROM was found. Store found sequence
            * number and row address.
            */
            prevSeqNum = seqNum;
            *lastWrRowPtr = emEepromAddr;
        }

        /* Switch to the next row */
        emEepromAddr = emEepromAddr + CY_EM_EEPROM_FLASH_SIZEOF_ROW;
    }
}


/*******************************************************************************
* Function Name: GetRowAddrBySeqNum
****************************************************************************//**
*
* Returns the address of the row in EEPROM using its sequence number.
*
* \param seqNum
* The sequence number of the row.
*
* \param context
* The pointer to the EEPROM context structure.
*
* \return
* The address of the row or zero if the row with the sequence number was not
* found.
*
*******************************************************************************/
static uint32 GetRowAddrBySeqNum(uint32 seqNum, cy_stc_eeprom_context_t * context)
{
    uint32 emEepromAddr = context->userFlashStartAddr;

    while(CY_EM_EEPROM_GET_SEQ_NUM(emEepromAddr) != seqNum)
    {
        /* Switch to the next row */
        emEepromAddr = emEepromAddr + CY_EM_EEPROM_FLASH_SIZEOF_ROW;

        if (CY_EM_EEPROM_ADDR_IN_RANGE !=
            CY_EM_EEPROM_IS_ADDR_EXCEED_RANGE(emEepromAddr, context->wlEndAddr))
        {
            emEepromAddr = 0u;
            /* Exit the loop as we reached the end of EEPROM */
            break;
        }
    }

    return (emEepromAddr);
}


/*******************************************************************************
* Function Name: GetNextRowToWrite
****************************************************************************//**
*
* Performs a range check of the row that should be written and updates the
* address to the row respectively. The similar actions are done for the read
* address.
*
* \param seqNum
* The sequence number of the last written row.
*
* \param rowToWrPtr
* The address of the last written row (input). The address of the row to be 
* written (output).
*
* \param rowToRdPtr
* The address of the row from which the data should be read into the RAM buffer
* in a later write operation. Out parameter.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
*******************************************************************************/
static void GetNextRowToWrite(uint32 seqNum,
                            uint32 * rowToWrPtr,
                            uint32 * rowToRdPtr,
                            cy_stc_eeprom_context_t * context)
{
    /* Switch to the next row to be written if the current sequence number is
    * not zero.
    */
    if(0u != seqNum)
    {
        *rowToWrPtr = (*rowToWrPtr + CY_EM_EEPROM_FLASH_SIZEOF_ROW);
    }

    /* If the resulting row address is out of EEPROM, then switch to the base
    * EEPROM address (Row#0).
    */
    if(CY_EM_EEPROM_ADDR_IN_RANGE !=
            CY_EM_EEPROM_IS_ADDR_EXCEED_RANGE(*rowToWrPtr, context->wlEndAddr))
    {
        *rowToWrPtr = context->userFlashStartAddr;
    }

    *rowToRdPtr = 0u;

    /* Check if the sequence number is larger than the number of rows in the EEPROM.
    * If not, do not update the row read address because there is no historic
    * data to be read.
    */
    if(context->numberOfRows <= seqNum)
    {
        /* Check if wear leveling is used in EEPROM */
        if(context->wearLevelingFactor > 1u)
        {
            /* The read row address should be taken from an EEPROM copy that became
            * inactive recently. This condition check handles that.
            */
            if((*rowToWrPtr - (context->numberOfRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW)) <
                context->userFlashStartAddr)
            {
                *rowToRdPtr = context->userFlashStartAddr +
                    (context->numberOfRows * (context->wearLevelingFactor - 1u) *
                        CY_EM_EEPROM_FLASH_SIZEOF_ROW) + (*rowToWrPtr - context->userFlashStartAddr);
            }
            else
            {
                *rowToRdPtr = *rowToWrPtr - (context->numberOfRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW);
            }
        }
        else
        {
            /* If no wear leveling, always read from the same flash row that
            * should be written.
            */
            *rowToRdPtr = *rowToWrPtr;
        }
    }
}


/*******************************************************************************
* Function Name: CalcChecksum
****************************************************************************//**
*
* Implements CRC-8 that is used in checksum calculation for the redundant copy
* algorithm.
*
* \param rowData
* The row data to be used to calculate the checksum.
*
* \param len
* The length of rowData.
*
* \return
* The calculated value of CRC-8.
*
*******************************************************************************/
static uint8 CalcChecksum(uint8 rowData[], uint32 len)
{
    uint8 crc = CY_EM_EEPROM_CRC8_SEED;
    uint8 i;
    uint16 cnt = 0u;

    while(cnt != len)
    {
        crc ^= rowData[cnt];
        for (i = 0u; i < CY_EM_EEPROM_CRC8_POLYNOM_LEN; i++)
        {
            crc = CY_EM_EEPROM_CALCULATE_CRC8(crc);
        }
        cnt++;
    }

    return (crc);
}


/*******************************************************************************
* Function Name: CheckRanges
****************************************************************************//**
*
* Checks if the EEPROM of the requested size can be placed in flash.
*
* \param config
* The pointer to a configuration structure. See \ref cy_stc_eeprom_config_t.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckRanges(cy_stc_eeprom_config_t* config)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_DATA;
    uint32 startAddr = config->userFlashStartAddr;
    uint32 endAddr = startAddr + CY_EM_EEPROM_GET_PHYSICAL_SIZE(config->eepromSize,
            config->wearLevelingFactor, config->redundantCopy);

    /* Range check if there is enough flash for EEPROM */
    if (CY_EM_EEPROM_IS_IN_FLASH_RANGE(startAddr, endAddr))
    {
        ret = CY_EM_EEPROM_SUCCESS;
    }
    return (ret);
}


/*******************************************************************************
* Function Name: WriteRow
****************************************************************************//**
*
* Writes one flash row starting from the specified row address.
*
* \param rowAdd
* The address of the flash row.
*
* \param rowData
* The pointer to the data to be written to the row.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t WriteRow(uint32 rowAddr,
                                        uint32 *rowData,
                                        cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;
#if (!CY_PSOC6)
    cystatus rc;
    uint32 rowId;
    #if ((CY_PSOC3) || (CY_PSOC5))
        uint32 arrayId;
    #endif /* (CY_PSOC3) */
    
    #if (CY_PSOC3)
        rowAddr &= CY_EM_EEPROM_CODE_ADDR_MASK;
        context = context;      /* To avoid compiler warning generation */
    #else
        (void)context;          /* To avoid compiler warning generation */
    #endif /* ((CY_PSOC3) */
    
    /* For non-PSoC 6 devices, the Array ID and Row ID needed to write the row */
    rowId = (rowAddr / CY_EM_EEPROM_FLASH_SIZEOF_ROW) % CY_EM_EEPROM_ROWS_IN_ARRAY;

    /* Write the flash row */
    #if (CY_PSOC4)
        rc = CySysFlashWriteRow(rowId, (uint8 *)rowData);
    #else

        #ifndef CY_EM_EEPROM_SKIP_TEMP_MEASUREMENT
            (void)CySetTemp();
        #endif /* (CY_EM_EEPROM_SKIP_TEMP_MEASUREMENT) */

        arrayId = rowAddr / CY_FLASH_SIZEOF_ARRAY;
        rc = CyWriteRowData((uint8)arrayId, (uint16)rowId, (uint8 *)rowData);
        
        #if (CY_PSOC5)
            CyFlushCache();
        #endif /* (CY_PSOC5) */
    #endif /* (CY_PSOC4) */

    if(CYRET_SUCCESS == rc)
    {
        ret = CY_EM_EEPROM_SUCCESS;
    }
#else /* PSoC 6 */
    if(0u != context->blockingWrite)
    {
        /* Do blocking write */
        if(CY_FLASH_DRV_SUCCESS == Cy_Flash_WriteRow(rowAddr, (const uint32 *)rowData))
        {
            ret = CY_EM_EEPROM_SUCCESS;
        }
    }
    else
    {
        /* Initiate write */
        if(CY_FLASH_DRV_OPERATION_STARTED == Cy_Flash_StartWrite(rowAddr, (const uint32 *)rowData))
        {
            uint32 countMs = CY_EM_EEPROM_MAX_WRITE_DURATION_MS;
            cy_en_flashdrv_status_t rc;

            do
            {
                CyDelay(1u);                         /* Wait 1ms */
                rc = Cy_Flash_IsWriteComplete();     /* Check if write completed */
                countMs--;
            }
            while ((rc == CY_FLASH_DRV_OPCODE_BUSY) && (0u != countMs));

            if(CY_FLASH_DRV_SUCCESS == rc)
            {
                ret = CY_EM_EEPROM_SUCCESS;
            }
        }
    }
#endif /* (CY_PSOC6) */

    return (ret);
}


/*******************************************************************************
* Function Name: EraseRow
****************************************************************************//**
*
* Erases one flash row starting from the specified row address. If the redundant
* copy option is enabled the corresponding row in the redundant copy will also
* be erased.
*
* \param rowAdd
* The address of the flash row.
*
* \param ramBuffAddr
* The address of the RAM buffer that contains zeroed data (used only for
* non-PSoC 6 devices).
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t EraseRow(uint32 rowAddr,
                                        uint32 ramBuffAddr,
                                        cy_stc_eeprom_context_t * context)
{
    uint32 emEepromRowAddr = rowAddr;
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;
#if (CY_PSOC6)
    uint32 i = 1u;

    (void)ramBuffAddr; /* To avoid compiler warning */

    if(0u != context->redundantCopy)
    {
        i++;
    }

    do
    {
        if(0u != context->blockingWrite)
        {
            /* Erase the flash row */
            if(CY_FLASH_DRV_SUCCESS == Cy_Flash_EraseRow(emEepromRowAddr))
            {
                ret = CY_EM_EEPROM_SUCCESS;
            }
        }
        else
        {
            /* Initiate erase */
            if(CY_FLASH_DRV_OPERATION_STARTED == Cy_Flash_StartErase(emEepromRowAddr))
            {
                uint32 countMs = CY_EM_EEPROM_MAX_WRITE_DURATION_MS;
                cy_en_flashdrv_status_t rc;

                do
                {
                    CyDelay(1u);                         /* Wait 1ms */
                    rc = Cy_Flash_IsWriteComplete();     /* Check if erase completed */
                    countMs--;
                }
                while ((rc == CY_FLASH_DRV_OPCODE_BUSY) && (0u != countMs));

                if(CY_FLASH_DRV_SUCCESS == rc)
                {
                    ret = CY_EM_EEPROM_SUCCESS;
                }
            }
        }

        if(CY_EM_EEPROM_SUCCESS == ret)
        {
            /* Update the address to point to the redundant copy row */
            emEepromRowAddr = (emEepromRowAddr - context->userFlashStartAddr) + context->wlEndAddr;
        }
        else
        {
            break;
        }
        i--;
    } while (0u != i);
#else
    /* Write the data to the specified flash row */
    ret = WriteRow(emEepromRowAddr, (uint32 *)ramBuffAddr, context);

    if((CY_EM_EEPROM_SUCCESS == ret) && (0u != context->redundantCopy))
    {
        /* Update the address to point to the redundant copy row */
        emEepromRowAddr = (emEepromRowAddr - context->userFlashStartAddr) + context->wlEndAddr;
        ret = WriteRow(emEepromRowAddr, (uint32 *)ramBuffAddr, context);
    }

    if(CY_EM_EEPROM_SUCCESS == ret)
    {
        context->lastWrRowAddr = rowAddr;
    }
#endif /* (CY_PSOC6) */

    return(ret);
}


/*******************************************************************************
* Function Name: CheckCrcAndCopy
****************************************************************************//**
*
* Checks the checksum of the specific row in EEPROM. If the CRC matches - copies
* the data to the "datAddr" from EEPROM. f the CRC does not match checks the
* CRC of the corresponding row in the EEPROM's redundant copy. If the CRC
* matches - copies the data to the "datAddr" from EEPROM redundant copy. If the
* CRC of the redundant copy does not match - returns bad checksum.
*
* \param startAddr
* The address that points to the start of the specified row.
*
* \param datAddr
* The start address of where the row data will be copied if the CRC check
* will succeed.
*
* \param rowOffset
* The offset in the row from which the data should be copied.
*
* \param numBytes
* The number of bytes to be copied.
*
* \param context
* The pointer to the EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
* \note
* This function uses a buffer of the flash row size to perform read
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckCrcAndCopy(uint32 startAddr,
                                                uint32 dstAddr,
                                                uint32 rowOffset,
                                                uint32 numBytes,
                                                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret;
    uint32 writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW / CY_EM_EEPROM_U32_DIV];

    /* Calculate the row address in the EEPROM's redundant copy */
    uint32 rcStartRowAddr = (startAddr - context->userFlashStartAddr) + context->wlEndAddr;

    /* Check the row data CRC in the EEPROM */
    if((*(uint32 *)(startAddr + CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET)) ==
        ((uint32) CalcChecksum((uint8 *)(startAddr + CY_EM_EEPROM_EEPROM_DATA_OFFSET),
            CY_EM_EEPROM_EEPROM_DATA_LEN)))
    {
        (void)memcpy((void *)(dstAddr), (void *)(startAddr + rowOffset), numBytes);

        ret = CY_EM_EEPROM_SUCCESS;
    }
    /* Check the row data CRC in the EEPROM's redundant copy */
    else if((*(uint32 *)(rcStartRowAddr + CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET)) ==
            ((uint32) CalcChecksum((uint8 *)(rcStartRowAddr + CY_EM_EEPROM_EEPROM_DATA_OFFSET),
                CY_EM_EEPROM_EEPROM_DATA_LEN)))
    {
        /* Copy the redundant copy row to RAM buffer to avoid read while write (RWW)
        * flash exception. The RWW occurs while trying to write and read the data from
        * same flash macro.
        */
        (void)memcpy((void *)(writeRamBuffer), (void *)(rcStartRowAddr), CY_EM_EEPROM_FLASH_SIZEOF_ROW);

        /* Restore bad row data from the RAM buffer */
        ret = WriteRow(startAddr, (uint32 *)writeRamBuffer, context);

        if(CY_EM_EEPROM_SUCCESS == ret)
        {
            (void)memcpy((void *)(dstAddr), (void *)(writeRamBuffer + rowOffset), numBytes);
        }
    }
    else
    {
        ret = CY_EM_EEPROM_BAD_CHECKSUM;
    }

    return(ret);
}


/*******************************************************************************
* Function Name: GetAddresses
****************************************************************************//**
*
* Calculates the start and end address of the row's EEPROM data to be updated.
* The start and end are not absolute addresses but a relative addresses in a
* flash row.
*
* \param startAddr
* The pointer the address where the EEPROM data start address will be returned.
*
* \param endAddr
* The pointer the address where the EEPROM data end address will be returned.
*
* \param offset
* The pointer the address where the calculated offset of the EEPROM header data
* will be returned.
*
* \param rowNum
* The row number that is about to be written.
*
* \param addr
* The address of the EEPROM header data in the currently analyzed row that may
* concern to the row about to be written.
*
* \param len
* The length of the EEPROM header data in the currently analyzed row that may
* concern to the row about to be written.
*
* \return
* Zero indicates that the currently analyzed row has the data to be written to
* the active EEPROM row data locations. Non zero value indicates that there is
* no data to be written
*
*******************************************************************************/
static uint32 GetAddresses(uint32 *startAddr, 
                        uint32 *endAddr, 
                        uint32 *offset,
                        uint32 rowNum,
                        uint32 addr,
                        uint32 len)
{
    uint32 skip = 0u;

    *offset =0u;

    if(0u != CY_EM_EEPROM_IS_ADDR_IN_ROW_RANGE(addr, rowNum))
    {
        *startAddr = CY_EM_EEPROM_EEPROM_DATA_LEN + (addr % CY_EM_EEPROM_EEPROM_DATA_LEN);

        if(0u != CY_EM_EEPROM_IS_ADDR_IN_ROW_RANGE(addr + len, rowNum))
        {
            *endAddr = *startAddr + len;
        }
        else
        {
            *endAddr = CY_EM_EEPROM_FLASH_SIZEOF_ROW;
        }
    }
    else
    {

        if(0u != CY_EM_EEPROM_IS_ADDR_IN_ROW_RANGE(addr + len, rowNum))
        {
            *startAddr = CY_EM_EEPROM_EEPROM_DATA_LEN;
            *endAddr = (*startAddr + len) - (*startAddr - (addr % CY_EM_EEPROM_EEPROM_DATA_LEN));
            *offset = len - (*endAddr - *startAddr);
        }
        else
        {
            skip++;
        }
    }

    return (skip);
}


/*******************************************************************************
* Function Name: FillChecksum
****************************************************************************//**
*
* Performs calculation of the checksum on each row in the Em_EEPROM and fills
* the Em_EEPROM headers checksum field with the calculated checksums.
*
* \param context
* The pointer to the EEPROM context structure.
*
* \return
* error / status code. See \ref cy_en_em_eeprom_status_t.
*
* \theory 
* In case if redundant copy option is used the Em_EEPROM would return bad 
* checksum while trying to read the EEPROM rows which were not yet written by 
* the user. E.g. any read after device reprogramming without previous Write() 
* operation to the EEPROM would fail. This would happen because the Em_EEPROM 
* headers checksum field values (which is zero at the moment) would not be 
* equal to the actual data checksum. This function allows to avoid read failure
* after device reprogramming. 
*
* \note
* This function uses a buffer of the flash row size to perform read
* operation. For the size of the row refer to the specific PSoC device
* datasheet.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t FillChecksum(cy_stc_eeprom_context_t * context)
{
    uint32 i;
    uint32 rdAddr;
    uint32 writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW / CY_EM_EEPROM_U32_DIV];
    uint32 wrAddr = context->lastWrRowAddr;
    uint32 tmpRowAddr;
    /* Get the sequence number (number of writes) */
    uint32 seqNum = CY_EM_EEPROM_GET_SEQ_NUM(wrAddr);
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;

    for(i = 0u; i < (context->numberOfRows * context->wearLevelingFactor); i++)
    {
        /* Copy the EEPROM row from Flash to RAM */
        (void)memcpy((void *)&writeRamBuffer[0u], (void *)(wrAddr), CY_EM_EEPROM_FLASH_SIZEOF_ROW);

        /* Increment the sequence number */
        seqNum++;
        writeRamBuffer[CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32] = seqNum;

        /* Calculate and fill the checksum to the Em_EEPROM header */
        writeRamBuffer[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32] = (uint32)
                    CalcChecksum((uint8 *) &writeRamBuffer[CY_EM_EEPROM_EEPROM_DATA_OFFSET_U32],
                                              CY_EM_EEPROM_EEPROM_DATA_LEN);

        /* Write the data to the specified flash row */
        ret = WriteRow(wrAddr, writeRamBuffer, context);

        /* Update the row address to point to the relevant row in the redundant 
        * EEPROM's copy.
        */
        tmpRowAddr = (wrAddr - context->userFlashStartAddr) + context->wlEndAddr;

        /* Write the data to the specified flash row */
        ret = WriteRow(tmpRowAddr, writeRamBuffer, context);

        /* Get the address of the next row to be written. 
        * "rdAddr" is not used in this function but provided to prevent NULL 
        * pointer exception in GetNextRowToWrite().
        */
        GetNextRowToWrite(seqNum, &wrAddr, &rdAddr, context);
    }
    
    return(ret);
}

/** \endcond */

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
