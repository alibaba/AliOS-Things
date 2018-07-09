/********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
********************************************************************************************************************/
/*******************************************************************************************************************
* File Name : r_flash_type2.c
* Description  : This module implements the Flash API control commands for the RX1xx
********************************************************************************************************************/
/*******************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.18.2014 1.10    Support for RX200, RX600
*           19.12.2014 1.20    Replaced some equate names for FLASH_TYPE_2.
*           20.06.2016 1.30    Fixed bug in blankcheck (mask) which caused
*                                the condition of block offset 0 or 2 to always be the result.
*           08.08.2016 2.00    Modified to operate without BSP. Removed warnings when
*                                FLASH_CFG_IGNORE_LOCK_BITS is 0 in config.h.
*           17.11.2016 2.10    Fixed bug where erasing code flash block 0 would return FLASH_ERR_ADDRESS.
*                              Fixed bug where erase ran at both app and interrupt level in BGO mode causing failure.
*                              Fixed bug where erase always erased first block more than once in BGO mode.
*                              Fixed bug where erasing more than 1 block in BGO mode did not work.
*                              Fixed bug where flash error interrupts remained disabled in BGO mode.
*                              Fixed bug where interrupts disabled in BGO mode when blank checking BLANK_CHECK_SMALLEST.
*                              Fixed bug where #ifs where not set properly for FLASH_CFG_CODE_FLASH_ENABLE set to 0.
*                              Added check to R_FLASH_Control() to detect missing argument.
*                              Added check that interrupt priority is 1-15 in BGO mode.
*                              Added check for DF erase on non-block boundary addresses.
*                              Added check for lockbit program/read on illegal DF addresses.
*                              Added check so Open() cannot be called while another operation is in progress in BGO mode.
*           02.08.2017 2.20    Removed #include r_mcu_config.h (now done via r_flash_rx_if.h"
********************************************************************************************************************/

/********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "r_flash_rx_if.h"
#if (FLASH_TYPE == FLASH_TYPE_2)
#include <machine.h>

/* Configuration for this package. */
#include "r_flash_rx_config.h"

/* Private header file for this package. */
#include "r_flash_type2_if.h"

/* include definition for NULL*/
#include <stddef.h>


/*********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Define read mode macro */
#define READ_MODE               0
/* Define ROM PE mode macro */
#define ROM_PE_MODE             1
/* Define data flash PE mode macro */
#define FLD_PE_MODE             2
/* The number of ICLK ticks needed for 35us delay are calculated below */
#define WAIT_TRESW              (35*(MCU_CFG_ICLK_HZ/1000000))
/* The number of ICLK ticks needed for 10us delay are calculated below */
#define WAIT_T10USEC            (10*(MCU_CFG_ICLK_HZ/1000000))
/* The number of loops to wait for FENTRYR timeout. */
#define FLASH_FENTRYR_TIMEOUT   (4)


/*  Bottom of User Flash Area */
#define ROM_PE_ADDR     ((0x100000000-MCU_ROM_SIZE_BYTES)&(0x00FFFFFF))

/* FCU-RAM address define */
/* FCU F/W Store Address */
#define FCU_PRG_TOP     0xFEFFE000
/* FCU RAM Address */
#define FCU_RAM_TOP     0x007F8000
/* FCU RAM Size */
#define FCU_RAM_SIZE    0x2000


/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

static int32_t g_flash_lock;

/* Signals whether FCU firmware has been transferred to the FCU RAM
   0 : No, 1 : Yes */
static uint8_t        g_fcu_transfer_complete = 0;
/* Valid values are 'READ_MODE','ROM_PE_MODE' or 'FLD_PE_MODE' */
static uint8_t        g_current_mode;
/* Stores whether the peripheral clock notification command has
   been executed */
static uint8_t        g_fcu_pclk_command = 0;

#if (FLASH_CFG_IGNORE_LOCK_BITS == 0)
/* Determines whether lock bit protection is used when programming/erasing */
static uint8_t g_lock_bit_protection = true;
#endif


#if ((FLASH_CFG_DATA_FLASH_BGO == 1) || (FLASH_CFG_CODE_FLASH_BGO == 1))

/* How many bytes are left to program */
static uint32_t  g_bgo_bytes;
/* Where we are programming or erasing */
static uint32_t  g_bgo_flash_addr;
/* Where the data is that we are programming */
static uint32_t  g_bgo_buffer_addr;
#endif

/* Flash intialisation function prototype */
static uint8_t  flash_init(void);
/* Enter PE mode function prototype */
static uint8_t  enter_pe_mode(uint32_t flash_addr);
/* Exit PE mode function prototype */
static void     exit_pe_mode(uint32_t flash_addr);
/* ROM write function prototype */
static flash_err_t  rom_write(uint32_t address, uint32_t data, uint32_t size);
/* Data flash write function prototype */
static flash_err_t data_flash_write (uint32_t address, uint32_t data, uint32_t size);

static bool flash_valid_addr_check (uint32_t block_start_address);

/* Data flash status clear function prototype */
static void     data_flash_status_clear(void);
/* Notify peripheral clock function prototype */
static uint8_t  notify_peripheral_clock(FCU_BYTE_PTR flash_addr);
/* FCU reset function prototype */
void     flash_reset(void);
/* Used to grab flash state */
static uint8_t  flash_grab_state(flash_states_t new_state);
/* Used to release flash state */
static void     flash_release_state2(void);
/* Used to issue an erase command to the FCU */
static flash_err_t  flash_erase_command(FCU_BYTE_PTR const erase_addr);

/* Used to get largest programming size that can be used. */
static uint32_t flash_get_program_size(uint32_t bytes, uint32_t flash_addr);

/* Enables or disables lock bit protection. */
#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_IGNORE_LOCK_BITS == 0))
static flash_err_t flash_lockbit_protection (uint32_t lock_bit);
#endif

/* Reads and returns the lock bit status for a ROM block */
flash_err_t flash_lockbit_read (flash_read_lockbit_config_t *lockbit_info);

flash_err_t flash_lockbit_program (flash_program_lockbit_config_t *lockbit_addr);

/* Allow read and program permissions to the Data Flash areas */
void flash_access_window_set(flash_access_window_config_t *pAccessInfo);




/******************************************************************************
* Function Name: flash_init
* Description  : Initializes the FCU peripheral block.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t flash_init (void)
{
#if defined(FCU_RAM_INIT_REQUIRED)
    /* Declare source and destination pointers */
    uint32_t * src, * dst;
    /* Declare iteration counter variable */
    uint16_t i;
#endif
    /* Timeout counter. */
    volatile int32_t  wait_cnt;

    /* Enable FCU error interrupts in FCU block */
    FLASH.FAEINT.BIT.ROMAEIE = 1;
    FLASH.FAEINT.BIT.CMDLKIE = 1;
    FLASH.FAEINT.BIT.DFLAEIE = 1;
    FLASH.FAEINT.BIT.DFLRPEIE = 1;
    FLASH.FAEINT.BIT.DFLWPEIE = 1;


#if ((FLASH_CFG_DATA_FLASH_BGO == 1) || (FLASH_CFG_CODE_FLASH_BGO == 1))
#if ((FLASH_CFG_FLASH_READY_IPL == 0) || (FLASH_CFG_FLASH_READY_IPL > 15))
#error "FLASH_CFG_FLASH_READY_IPL in r_flash_rx_config.h must be between 1 and 15 when BGO is enabled."
#endif
    IPR(FCU, FIFERR) = FLASH_CFG_FLASH_READY_IPL;
    IEN(FCU, FIFERR) = 1;
    /* Enable flash ready interrupt (FRDYI) */
    /* Make sure IPL is high enough that interrupt will trigger */
    IPR(FCU, FRDYI) = FLASH_CFG_FLASH_READY_IPL;
    IEN(FCU, FRDYI) = 1;
#else
    /* Disable flash interface error (FIFERR) */
    IPR(FCU, FIFERR) = 0;
    IEN(FCU, FIFERR) = 0;
    /* Disable flash ready interrupt (FRDYI) */
    IPR(FCU, FRDYI) = 0;
    IEN(FCU, FRDYI) = 0;
#endif

    /* Transfer Firmware to the FCU RAM. To use FCU commands, the FCU firmware
        must be stored in the FCU RAM. */

    /* Before writing data to the FCU RAM, clear FENTRYR to stop the FCU. */
    if(FLASH.FENTRYR.WORD != 0x0000)
    {
        /* Disable the FCU from accepting commands - Clear both the
           FENTRY0(ROM) and FENTRYD(Data Flash) bits to 0 */
        FLASH.FENTRYR.WORD = 0xAA00;

        /* Initialize timeout for FENTRYR being written. */
        wait_cnt = FLASH_FENTRYR_TIMEOUT;

        /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
           of the FENTRYR register is not retained and is read as 0x00. */
        while(0x0000 != FLASH.FENTRYR.WORD)
        {
            /* Wait until FENTRYR is 0 unless timeout occurs. */
            if (wait_cnt-- <= 0)
            {
                /* This should not happen. FENTRYR getting written to 0 should
                   only take 2-4 PCLK cycles. */
                return FLASH_ERR_FAILURE;
            }
        }
    }

#if defined(FCU_RAM_INIT_REQUIRED)
    /* Enable the FCU RAM */
    FLASH.FCURAME.WORD = 0xC401;

    /* Copies the FCU firmware to the FCU RAM.
       Source: H'FEFFE000 to H'FF00000 (FCU firmware area)
       Destination: H'007F8000 to H'007FA000 (FCU RAM area) */
    /* Set source pointer */
    src = (uint32_t *)FCU_PRG_TOP;

    /* Set destination pointer */
    dst = (uint32_t *)FCU_RAM_TOP;

    /* Iterate for loop to copy the FCU firmware */
    for( i=0; i<(FCU_RAM_SIZE/4); i++)
    {
        /* Copy data from the source to the destination pointer */
        *dst = *src;

        /* Increment the source and destination pointers */
        src++;
        dst++;
    }
#endif

    /* FCU firmware transfer complete, set the flag to 1 */
    g_fcu_transfer_complete = 1;

    /* Return no errors */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  flash_init
******************************************************************************/
/*********************************************************************************************************************
 External variables and functions
 *********************************************************************************************************************/
extern flash_states_t g_flash_state;

/***********************************************************************************************************************
* Function Name: flash_api_open
* Description  : Function will initialize the flash for the RX600/RX200 family.
* Arguments    : void
* Return Value : FLASH_SUCCESS -
*                    API initialized successfully.
*                FLASH_ERR_BUSY
*                    API has already been initialized and another operation is ongoing.
*                FLASH_ERR_FAILURE
*                    Initialization failed.
*                    A RESET was performed on the Flash circuit to rectify any internal errors
***********************************************************************************************************************/
flash_err_t flash_api_open(void)
{

    /* Lock driver to initialize */
    if (flash_grab_state(FLASH_INITIALIZATION) != FLASH_SUCCESS )
    {
        return FLASH_ERR_BUSY;
    }

    /* If FCU firmware has already been transferred to FCU RAM,
       no need to do it again */
    if( !g_fcu_transfer_complete )
    {
        /* Initialise the FCU, and store operation resilt in result variable */
        /* Check if FCU initialisation was successful */
        if( flash_init() != FLASH_SUCCESS )
        {
            /* Unlock driver */
            flash_release_state2();

            /* FCU initialisiation failed - return operation failure */
            return FLASH_ERR_FAILURE;
        }
    }

    /* Unlock driver */
    flash_release_state2();

    return FLASH_SUCCESS;
}


/******************************************************************************
* Function Name: data_flash_status_clear
* Description  : Clear the status of the Data Flash operation.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void data_flash_status_clear (void)
{
    /* Declare temporary pointer */
    FCU_BYTE_PTR ptrb;

    /* Set pointer to Data Flash to issue a FCU command if needed */
    ptrb = (FCU_BYTE_PTR)(DF_ADDRESS);

    /* Check to see if an error has occurred with the FCU.  If set, then
       issue a status clear command to bring the FCU out of the
       command-locked state */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* FASTAT must be set to 0x10 before the status clear command
           can be successfully issued  */
        if(FLASH.FASTAT.BYTE != 0x10)
        {
            /* Set the FASTAT register to 0x10 so that a status clear
               command can be issued */
            FLASH.FASTAT.BYTE = 0x10;
        }
    }

    /* Issue a status clear command to the FCU */
    *ptrb = 0x50;
}
/******************************************************************************
End of function  data_flash_status_clear
******************************************************************************/

/******************************************************************************
* Function Name: data_flash_write
* Description  : Write either bytes to Data Flash area.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : address -
*                    The address (in the Data Flash programming area)
*                    to write the data to
*                data -
*                    The address of the data to write
*                size -
*                    The size of the data to write. Must be set to
*                    either DF_PROGRAM_SIZE_LARGE or DF_PROGRAM_SIZE_SMALL.
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_ERR_FAILURE -
*                    Operation Failed
******************************************************************************/
static flash_err_t data_flash_write (uint32_t address, uint32_t data, uint32_t size)
{
    /* Declare wait counter variable */
    volatile int32_t wait_cnt;

    /* Define loop iteration count variable */
    volatile uint32_t n = 0;

    /* Check data size is valid */
#if defined(DF_PROGRAM_SIZE_LARGE)
    if((DF_PROGRAM_SIZE_LARGE == size) || (DF_PROGRAM_SIZE_SMALL == size))
#else
    if(DF_PROGRAM_SIZE_SMALL == size)
#endif
    {

        /* Perform bit shift since 2 bytes are written at a time */
        size = (uint8_t)(size >> 1);


        /* Send command to data flash area */
        *(FCU_BYTE_PTR)DF_ADDRESS = 0xE8;

        /* Specify data transfer size to data flash area */
        *(FCU_BYTE_PTR)DF_ADDRESS = size;

        /* Iterate through the number of data bytes */
        while(n++ < size)
        {
            /* Copy data from source address to destination area */
            *(FCU_WORD_PTR)address = *(uint16_t *)data;

            /* Increment data address by two bytes */
            data += 2;
        }

        /* Write the final FCU command for programming */
        *(FCU_BYTE_PTR)(DF_ADDRESS) = 0xD0;

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        /* Program is ongoing, return */
        return FLASH_SUCCESS;
#endif

        /* Set the wait counter with timeout value */
        wait_cnt = WAIT_MAX_DF_WRITE;

        /* Check if FCU has completed its last operation */
        while(FLASH.FSTATR0.BIT.FRDY == 0)
        {
            /* Decrement the wait counter */
            wait_cnt--;

            /* Check if the wait counter has reached zero */
            if(wait_cnt == 0)
            {
                /* Maximum time for writing a block has passed,
                   operation failed, reset FCU */
                flash_reset();

                /* Return FLASH_FAILURE, operation failure */
                return FLASH_ERR_FAILURE;
            }
        }

        /* Check for illegal command or programming errors */
        if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR == 1))
        {
            /* Return FLASH_FAILURE, operation failure */
            return FLASH_ERR_FAILURE;
        }
    }
    /* Data size is invalid */
    else
    {
        /*Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* Return FLASH_SUCCESS, operation success */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  data_flash_write
******************************************************************************/


#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section FRAM
#endif

/***********************************************************************************************************************
* Function Name: flash_api_blankcheck
* Description  : Performs a blank check on a specified data flash block
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
*                NOTE2: RX610 & RX62x support block and 8 byte checks
*                       RX630/631/63N support block and 2 byte checks
* Arguments    : address -
*                    The address to check if is blank.
*                    If the parameter 'size'=='BLANK_CHECK_8_BYTE',
*                    this should be set to an 8-byte address boundary.
*                    If the parameter 'size'=='BLANK_CHECK_ENTIRE_BLOCK',
*                    this should be set to an address in the data flash block.
*                    If the parameter 'size'=='BLANK_CHECK_2_BYTE',
*                    this should be set to a 2 byte address boundary.
*                size -
*                    This specifies if you are checking an 8-byte location,
*                    2-byte location, or an entire block. You must set this
*                    to either 'BLANK_CHECK_8_BYTE', 'BLANK_CHECK_2_BYTE',
*                    or 'BLANK_CHECK_ENTIRE_BLOCK'.
*
*                result -
*                 Returns the result of the blank check operation. This field is valid only in non-BGO mode
*                 operation
*
*  Return Value : FLASH_SUCCESS
*                 Operation was completed successfully. In non-BGO (blocking) mode result contains blank check status
*                FLASH_ERR_BYTES -
*                    Incorrect 'size' was submitted
*                FLASH_ERR_ADRRESS -
*                    Start address is an invalid Data Flash Address
*                FLASH_ERR_BUSY
*                    Flash peripheral is busy with another operation or not initialized
*                FLASH_ERR_FAILURE -
*                    Operation failed for some other reason.*
***********************************************************************************************************************/
flash_err_t flash_api_blankcheck(uint32_t address, uint32_t size, flash_res_t *result)
{

    /* Declare data flash pointer */
    FCU_BYTE_PTR ptrb;

    /* Declare result container variable */
    uint8_t local_result;

    /* Declare wait counter variable */
    volatile int32_t wait_cnt;

    /* Check to make sure address is valid. */
    if((address < FLASH_DF_BLOCK_0) || (address >= FLASH_DF_BLOCK_INVALID) )
    {
        /* Address is not a valid DF address */
        return FLASH_ERR_ADDRESS;
    }

    if (size == BLANK_CHECK_SMALLEST)
    {
        /* Make sure address is aligned correctly. */
        if( address & (DF_PROGRAM_SIZE_SMALL-1) )
        {
            /* Return address not on a programming boundary */
            return FLASH_ERR_ALIGNED;
        }
    }

    /* Check to make sure 'size' parameter is valid */
     if( (size != BLANK_CHECK_ENTIRE_BLOCK) && (size != BLANK_CHECK_SMALLEST) )
     {
         /* 'size' parameter is not valid. */
         return FLASH_ERR_BYTES;
     }

    /* Attempt to grab state */
    if( flash_grab_state(FLASH_BLANKCHECK) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_ERR_BUSY;
    }

    /* Set current FCU mode to Data Flash PE Mode */
    g_current_mode = FLD_PE_MODE;

    /* Enter Data Flash PE mode in the FCU */
    if( enter_pe_mode(address) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(address);

        /* Release state */
        flash_release_state2();

        /* Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* Set  bit FRDMD (bit 4) in FMODR to 1 */
    FLASH.FMODR.BIT.FRDMD = 1;

    /* Check if smallest check size has been passed */
    if(size == BLANK_CHECK_SMALLEST)
    {
        /* Set data flash pointer to beginning of the memory block */
        ptrb = (FCU_BYTE_PTR)(address & DF_MASK);

        /* Check if the next N bytes are blank (N = DF_PROGRAM_SIZE_SMALL)
           Bits BCADR to the address of the N-byte location to check.
           Set bit BCSIZE in EEPBCCNT to 0. */
        FLASH.DFLBCCNT.WORD = (uint16_t)(address & ~(DF_MASK));
    }
    /* Check entire data block */
    else
    {

        /* Any address in the erasure block */
        ptrb = (FCU_BYTE_PTR)address;

        /* Check if the entire block is blank
           Set bit BCSIZE in EEPBCCNT to 1. */
        FLASH.DFLBCCNT.BIT.BCSIZE = 1;
    }


    /* Send commands to FCU */
    *ptrb = 0x71;
    *ptrb = 0xD0;

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
    /* Only using BGO on block blank checks */
    if( size == BLANK_CHECK_ENTIRE_BLOCK )
    {
        /* Set global address in case we have error and need to reset FCU. */
        g_bgo_flash_addr = address;

        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_BLANK_CHECK;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {
            /* Maximum timeout duration for writing to ROM has elapsed -
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_ERR_TIMEOUT, operation failure */
            return FLASH_ERR_TIMEOUT;
        }
    }

    /* Reset the FRDMD bit back to 0 */
    FLASH.FMODR.BIT.FRDMD = 0x00;

    /* Check if the 'ILGERR' was set during the command */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)  /* Check 'ILGERR' bit */
    {
        /* Take the FCU out of PE mode */
        exit_pe_mode(address);

        /* Release state */
        flash_release_state2();

        /* Return FLASH_ERR_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* (Read the 'BCST' bit (bit 0) in the 'DFLBCSTAT' register
       0=blank, 1=not blank */
    local_result = (uint8_t)FLASH.DFLBCSTAT.BIT.BCST;

    /* Take the FCU out of PE mode */
    exit_pe_mode(address);

    /* Release state */
    flash_release_state2();

    /* Return (Not Blank/Blank), operation successful */
    if( local_result == 0 )
    {
        /* Block was blank */
        *result = FLASH_RES_BLANK;
    }
    else
    {
        /* Block was not blank */
        *result = FLASH_RES_NOT_BLANK;
    }
    return FLASH_SUCCESS;
}

/******************************************************************************
* Function Name: flash_access_window_set
* Description  : This function is used to allow read and program permissions
*                to the Data Flash areas.
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    :     pAccessInfo->read_en_mask -
*                    Bitmasked value. Bits 0-3 represents each Data
*                    Blocks 0-3 (respectively).
*                    '0'=no Read access.
*                    '1'=Allows Read by CPU
*
*                    pAccessInfo->write_en_mask -
*                    Bitmasked value. Bits 0-3 represents each Data
*                    Blocks 0-3 (respectively).
*                    '0'=no Erase/Write access.
*                    '1'=Allows Erase/Write by FCU
* Return Value : none
******************************************************************************/
void flash_access_window_set(flash_access_window_config_t *pAccessInfo)
{
    /* Used for making sure select bits are not set. */
    uint16_t temp_mask = 0x00FF;

#if   defined(MCU_RX610)

    /* Set Read access for the Data Flash blocks */
    FLASH.DFLRE.WORD = (uint16_t)(0x2D00 | (pAccessInfo->read_en_mask & 0x00FF));

    /* Set Erase/Program access for the Data Flash blocks */
    FLASH.DFLWE.WORD = (uint16_t)(0x1E00 | (pAccessInfo->write_en_mask & 0x00FF));

#else

    #if MCU_DATA_FLASH_SIZE_BYTES == 8192
    /* Mask off bits 4-7 to make sure they are 0. */
    temp_mask = 0x000F;
    #endif

    /* Set Read access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLRE0.WORD = (uint16_t)(0x2D00 | (pAccessInfo->read_en_mask & temp_mask));

    /* Set Erase/Program access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLWE0.WORD = (uint16_t)(0x1E00 |  (pAccessInfo->write_en_mask & temp_mask));

#if MCU_DATA_FLASH_SIZE_BYTES > 16384
    /* Set Read access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLRE1.WORD = (uint16_t)(0xD200 | ((pAccessInfo->read_en_mask >> 8) & 0x00FF));

    /* Set Erase/Program access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLWE1.WORD = (uint16_t)(0xE100 |  ((pAccessInfo->write_en_mask >> 8) & 0x00FF));
#endif

#endif
}



/******************************************************************************
* Function Name: flash_getStatus
* Description  : Returns the current state of the flash
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : none
* Return Value : FLASH_SUCCESS -flash_getStatus
*                    Flash is ready to use
*                FLASH_ERR_BUSY -
*                    Flash is busy with another operation
******************************************************************************/
flash_err_t flash_getStatus (void)
{
    /* Return flash status */
    if( g_flash_state == FLASH_READY )
    {
        return FLASH_SUCCESS;
    }
    else
    {
        return FLASH_ERR_BUSY;
    }
}
/******************************************************************************
End of function  R_FlashGetStatus
******************************************************************************/


/******************************************************************************
* Function Name: flash_grab_state
* Description  : Attempt to grab the flash state to perform an operation
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : new_state -
*                    Which state to attempt to transition to
* Return Value : FLASH_SUCCESS -
*                    State was grabbed
*                FLASH_BUSY -
*                    Flash is busy with another operation
******************************************************************************/
static uint8_t flash_grab_state (flash_states_t new_state)
{
    /* Check to see if lock was successfully taken */
    if (flash_softwareLock(&g_flash_lock) == true)
    {
        /* Lock taken, we can change state */
        g_flash_state = new_state;

        /* Return success */
        return FLASH_SUCCESS;
    }
    else
    {
        /* Another operation is on-going */
        return FLASH_ERR_BUSY;
    }
}
/******************************************************************************
End of function  flash_grab_state
******************************************************************************/


/******************************************************************************
* Function Name: flash_release_state2
* Description  : Release state so another flash operation can take place
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void flash_release_state2 (void)
{
    /* Set current FCU mode to READ */
    g_current_mode = READ_MODE;

    /* Done with programming */
    g_flash_state = FLASH_READY;

    /* Release hold on lock */
    flash_softwareUnlock(&g_flash_lock);
}
/******************************************************************************
End of function  flash_release_state2
******************************************************************************/

/******************************************************************************
* Function Name: flash_valid_addr_check
* Description  : Checks to see if the input address is actually a valid address.
*                This function checks against the actual memory size of the
*                MCU, not against the maximum. This means that if a memory
*                block is sent in that does not exist for this part, but does
*                for a larger memory variant of this MCU, this function will
*                return an error.
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : block_start_address - the address to validate
* Return Value : true -
*                    Address is valid.
*                false -
*                    Address is not valid.
******************************************************************************/
static bool flash_valid_addr_check (uint32_t block_start_address)

{
    if (((block_start_address >= FLASH_DF_BLOCK_0) && (block_start_address < FLASH_DF_BLOCK_INVALID )) ||
         ((block_start_address <= FLASH_CF_BLOCK_0) && (block_start_address > FLASH_CF_BLOCK_INVALID)))
        return(true);
    else
        return(false);
}

#if  (FLASH_CFG_IGNORE_LOCK_BITS == 0)
/******************************************************************************
* Function Name: flash_lockbit_protection
* Description  : Enables or disables lock bit protection.
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_CFG_CODE_FLASH_BGO is
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : lock_bit -
*                    Boolean, whether to enable or disable lock bit
*                    protection (true = enabled, false = disabled)
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_ERR_BUSY -
*                    Another flash operation is in progress
******************************************************************************/
static flash_err_t flash_lockbit_protection (uint32_t lock_bit)
{
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if (lock_bit > 1)
    {
        return FLASH_ERR_PARAM;
    }
#endif

    /* Attempt to grab state */;
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_ERR_BUSY;
    }

    /* Enable or disable? */
    g_lock_bit_protection = lock_bit;

    /* Release state */
    flash_release_state2();

    /* Return success */
    return FLASH_SUCCESS;
}
#endif
/******************************************************************************
End of function  flash_lockbit_protection
******************************************************************************/

#if (FLASH_CFG_DATA_FLASH_BGO == 1) || ((FLASH_CFG_CODE_FLASH_BGO == 1) && (FLASH_CFG_CODE_FLASH_ENABLE == 1))
/******************************************************************************
* Function Name: Excep_FCU_FIFERR
* Description  : ISR that is called when FCU is done with flash operation
*                and a failure occurred.
* Arguments    : none
* Return Value : none
******************************************************************************/
#pragma interrupt Excep_FCU_FIFERR(vect=VECT(FCU, FIFERR))
void Excep_FCU_FIFERR(void)
{

    /* Leave Program/Erase Mode and clear any error flags */
    exit_pe_mode(g_bgo_flash_addr);

    /* Release flash state */
    flash_release_state2();

    /* Operation failure, use callback function to alert user */
    FlashError();

    /* Exit ISR */
    return;
}


/******************************************************************************
* Function Name: Excep_FCU_FRDYI
* Description  : ISR that is called when FCU is done with flash operation
*                NOTE: This function MUST execute from RAM only when
*                      FLASH_CFG_CODE_FLASH_BGO is enabled.
* Arguments    : none
* Return Value : none
******************************************************************************/
#pragma interrupt Excep_FCU_FRDYI(vect=VECT(FCU, FRDYI))
void Excep_FCU_FRDYI(void)
{
    /* Local variables */
    uint32_t num_byte_to_write;
    uint8_t  ret;
#ifdef FLASH_CFG_FLASH_TO_FLASH
    uint32_t i;
#endif
    rom_block_info_t block_info;
    uint32_t block_number,bytes_erased;


    /* Check for any errors */
    if( (FLASH.FSTATR0.BIT.ILGLERR == 1) ||
        (FLASH.FSTATR0.BIT.ERSERR  == 1) ||
        (FLASH.FSTATR0.BIT.PRGERR == 1) )
    {
        /* Should never get here with FIFERR enabled */
        nop();
    }

    /* Check state and see if anything else needs to be done */
    if( g_flash_state == FLASH_ERASING )
    {
        /* Erase is done */
        /* If we are erasing data flash then we need to see if all requested
           blocks are erased. */
        if( FLD_PE_MODE == g_current_mode )
        {
            /* Check to see if there are more bytes to erase. */
            if( 0 < g_bgo_bytes )
            {
                /* Send FCU command to erase block. Cannot fail in BGO mode. */
                ret = flash_erase_command((FCU_BYTE_PTR)g_bgo_flash_addr);

                /* Advance pointer to next block */
                g_bgo_flash_addr += DF_ERASE_BLOCK_SIZE;

                /* Subtract off bytes erased */
                g_bgo_bytes -= DF_ERASE_BLOCK_SIZE;

                return;
            }
        }
        else // code flash
        {
            /* Check to see if there are more bytes to erase. */
            if( 0 < g_bgo_bytes )
            {
                /* Send FCU command to erase block. Cannot fail in BGO mode */
                flash_erase_command((FCU_BYTE_PTR)g_bgo_flash_addr);

                /* Advance pointer to next block */
                bytes_erased = flash_get_romBlock_size(g_bgo_flash_addr, &block_info);
                g_bgo_bytes -= bytes_erased;

                if (g_bgo_bytes > 0)
                {
                    block_number = block_info.block_number - 1;         // next block to erase
                    flash_get_romBlock_info (block_number, &block_info);

                    g_bgo_flash_addr = block_info.start_addr;
                }

                return;
            }
        }


        /* Leave Program/Erase Mode */
        exit_pe_mode(g_bgo_flash_addr);

        /* Release flash state */
        flash_release_state2();

        /* Flash operation finished callback function */
        FlashEraseDone();
    }
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
    else if( g_flash_state == FLASH_BLANKCHECK )
    {
        /* Blank check finished, check result */

        /* Reset the FRDMD bit back to 0 */
        FLASH.FMODR.BIT.FRDMD = 0x00;

        /* (Read the 'BCST' bit (bit 0) in the 'DFLBCSTAT' register
           0=blank, 1=not blank */
        ret = (uint8_t)FLASH.DFLBCSTAT.BIT.BCST;

        /* Take the FCU out of PE mode */
        exit_pe_mode(g_bgo_flash_addr);

        /* Release flash state */
        flash_release_state2();

        /* Return result, 0 means blank, 1 means not blank */
        if(ret == 0)
        {
            FlashBlankCheckDone(FLASH_RES_BLANK);
        }
        else
        {
            FlashBlankCheckDone(FLASH_RES_NOT_BLANK);
        }
    }
#endif
    else if( g_flash_state == FLASH_WRITING )
    {
        /* Iterate while there are still bytes remaining to write */
        if( g_bgo_bytes > 0 )
        {
            /* Get maximum programming size that can currently be used. */
            num_byte_to_write = flash_get_program_size(g_bgo_bytes, g_bgo_flash_addr);

#ifdef FLASH_CFG_FLASH_TO_FLASH
            /* Check to see if we need to buffer more data */
            if( g_flash_to_flash_op == 1 )
            {
                /* We must leave PE mode to transfer next buffer to RAM */
                exit_pe_mode(g_bgo_flash_addr);

                /* Copy over next bytes to write */
                for(i = 0; i < num_byte_to_write; i++)
                {
                    /* Copy over each byte */
                    g_temp_array[i] = *((uint8_t *)(g_bgo_buffer_addr+i));
                }

                /* Re-enter PE mode, check if operation is successful */
                if( enter_pe_mode(g_bgo_flash_addr) != FLASH_SUCCESS)
                {
                    /* Leave Program/Erase Mode and clear any error flags */
                    exit_pe_mode(g_bgo_flash_addr);

                    /* Release flash state */
                    flash_release_state2();

                    /* Operation failure, use callback function to alert user */
                    FlashError();

                    /* Exit ISR */
                    return;
                }
            }
#endif

            /* Are we writing DF or ROM? */
            if( g_current_mode == FLD_PE_MODE )
            {
                /* Writing DF */
                /* Call the Programming function again for next bytes */
#ifdef FLASH_CFG_FLASH_TO_FLASH
                if( g_flash_to_flash_op == 1 )
                {
                    ret = data_flash_write( g_bgo_flash_addr,
                                            (uint32_t)&g_temp_array[0],
                                            num_byte_to_write);
                }
                else
                {
                    ret = data_flash_write( g_bgo_flash_addr,
                                            g_bgo_buffer_addr,
                                            num_byte_to_write);
                }
#else
                ret = data_flash_write( g_bgo_flash_addr,
                                        g_bgo_buffer_addr,
                                        num_byte_to_write);
#endif
            }
            else
            {
                /* Writing ROM */
                /* Call the Programming function */
#ifdef FLASH_CFG_FLASH_TO_FLASH
                if( g_flash_to_flash_op == 1 )
                {
                    /* Use RAM array */
                    ret = rom_write(g_bgo_flash_addr,
                                    (uint32_t)&g_temp_array[0],
                                    num_byte_to_write);
                }
                else
                {
                    /* Go as usual */
                    ret = rom_write(g_bgo_flash_addr,
                                    g_bgo_buffer_addr,
                                    num_byte_to_write);
                }
#else
                ret = rom_write(g_bgo_flash_addr,
                                g_bgo_buffer_addr,
                                num_byte_to_write);
#endif
            }

            /* Check the result for errors */
            if( ret != FLASH_SUCCESS )
            {
                /* Error detected during programming, stop and return */
                /* Leave Program/Erase Mode and clear any error flags */
                exit_pe_mode(g_bgo_flash_addr);

                /* Release flash state */
                flash_release_state2();

                /* Operation failure, use callback function to alert user */
                FlashError();

                /* Exit ISR */
                return;
            }

            /* Increment the flash address and the buffer address by the size
               of the transfer thats just completed */
            g_bgo_flash_addr += num_byte_to_write;
            g_bgo_buffer_addr += num_byte_to_write;

            /* Decrement the number of bytes remaining by the size of the last
               flash write */
            g_bgo_bytes -= num_byte_to_write;

        }
        else
        {
            /* Leave Program/Erase Mode */
            exit_pe_mode(g_bgo_flash_addr);

            /* Release flash state */
            flash_release_state2();

            /* Flash operation finished callback function */
            FlashWriteDone();
        }

    }
    else
    {
        /* NOTE: Should never get here. However, sometimes after a FIFERR occurs
         * and errors are reset, an FRDYI interrupt occurs. The g_flash_state is
         * FLASH_READY which causes this else condition to occur. One logic analyzer
         * traced occurrence of this is doing a large write to data flash when
         * the area is not erased first.
         */
        nop();
    }
}
/******************************************************************************
End of function  flash_ready_isr
******************************************************************************/

#endif





#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
#pragma section FRAM
#endif

/******************************************************************************
* Function Name: rom_write
* Description  : Write bytes to ROM Area Flash.
*                NOTE: This function MUST execute from in RAM.
* Arguments    : address -
*                    ROM address of where to write to
*                data -
*                    Pointer to the data to write
*                size -
*                    The size of the data to write. Must be set to
*                    either a supported write size of the MCU.
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_ERR_FAILURE -
*                    Operation Failed
******************************************************************************/
static flash_err_t rom_write (uint32_t address, uint32_t data, uint32_t size)
{
/* If ROM programming is not enabled then do not compile this code as it is
   not needed. */
#ifdef FLASH_CFG_CODE_FLASH_ENABLE

    /* Declare iteration loop count variable */
    volatile uint32_t i;
    /* Declare wait counter variable */
    volatile int32_t  wait_cnt;

    /* Writes are done 16-bits (2 bytes) at a time, scale 'size' argument */
    size = size >> 1;

    /* Write the FCU Program command */
    *(FCU_BYTE_PTR)address = 0xE8;
    *(FCU_BYTE_PTR)address = size;

    /* Write 'size' bytes into flash, 16-bits at a time */
    for(i = 0; i < size; i++)
    {
        /* Copy data from source address to destination ROM */
        *(FCU_WORD_PTR)address = *(uint16_t *)data;

        /* Increment destination address by 2 bytes */
        data += 2;
    }

    /* Write the final FCU command for programming */
    *(FCU_BYTE_PTR)address = 0xD0;

    #if (FLASH_CFG_CODE_FLASH_BGO == 1)
    /* Return, rest of programming will be done in interrupt */
    return FLASH_SUCCESS;
    #endif

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_ROM_WRITE;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {
            /* Maximum timeout duration for writing to ROM has elapsed -
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_ERR_FAILURE;
        }
    }

    /* Check for illegal command or programming errors */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR  == 1))
    {
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

#endif /* FLASH_CFG_ENABLE_ROM_PROGRAMMING */

    /* If ROM programming is not enabled then this function will always just
       return FLASH_SUCCESS. This is okay because this function will never
       actually be called. The API write function will return an error when
       a ROM address is entered for programming. */

    /* Return FLASH_SUCCESS, operation successful */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  rom_write
******************************************************************************/

/******************************************************************************
* Function Name: enter_pe_mode
* Description  : Puts the FCU into program/erase mode.
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : flash_addr -
*                    The programming/erasure address
*                bytes -
*                    The number of bytes you are writing (if you are writing).
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t enter_pe_mode (uint32_t flash_addr)
{
    /* Used for timeout on FENTRYR write/read. */
    volatile int32_t wait_cnt;


    /* FENTRYR must be 0x0000 before bit FENTRY0 or FENTRYD can be set to 1 */
    FLASH.FENTRYR.WORD = 0xAA00;

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. */
            return FLASH_ERR_FAILURE;
        }
    }

    /* Check if FCU mode is set to ROM PE */
    if( g_current_mode == ROM_PE_MODE)
    {
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        /* Don't enable flash interrupts if programming lock bits */
        if( g_flash_state != FLASH_LOCK_BIT )
        {
            /* Re-enable the FRDYI interrupt */
            FLASH.FRDYIE.BIT.FRDYIE = 1;
        }
#else
        /* Disable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 0;
#endif

        /* Check which area of flash this address is in */
        if(flash_addr >= ROM_AREA_0)
        {
            /* Area 0 */
            /* Enter ROM PE mode for ROM Area 0 */
            FLASH.FENTRYR.WORD = 0xAA01;
        }
#if defined(ROM_AREA_1)
        else if((flash_addr < ROM_AREA_0) && (flash_addr >= ROM_AREA_1))
        {
            /* Area 1 */
            /* Enter ROM PE mode for ROM Area 1 */
            FLASH.FENTRYR.WORD = 0xAA02;
        }
#endif
#if defined(ROM_AREA_2)
        else if((flash_addr < ROM_AREA_1) && (flash_addr >= ROM_AREA_2))
        {
            /* Area 2 */
            /* Enter ROM PE mode for ROM Area 2 */
            FLASH.FENTRYR.WORD = 0xAA04;
        }
#endif
#if defined(ROM_AREA_3)
        else
        {
            /* Area 3 */
            /* Enter ROM PE mode for ROM Area 3 */
            FLASH.FENTRYR.WORD = 0xAA08;
        }
#endif

    }
    /* Check if FCU mode is set to data flash PE */
    else if( g_current_mode == FLD_PE_MODE )
    {
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        /* Re-enable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 1;
#else
        /* Disable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 0;
#endif

        /* Set FENTRYD bit(Bit 7) and FKEY (B8-15 = 0xAA) */
        FLASH.FENTRYR.WORD = 0xAA80;

        /*  First clear the FCU's status before doing Data Flash programming.
            This is to clear out any previous errors that may have occured.
            For example, if you attempt to read the Data Flash area
            before you make it readable using R_FlashDataAreaAccess(). */
        data_flash_status_clear();

    }
    /* Catch-all for invalid FCU mode */
    else
    {
        /* Invalid value of 'g_current_mode' */
        return FLASH_ERR_FAILURE;
    }

    /* Enable Write/Erase of ROM/Data Flash */
    FLASH.FWEPROR.BYTE = 0x01;

    /* Check for FCU error */
    if(     (FLASH.FSTATR0.BIT.ILGLERR == 1)
        ||  (FLASH.FSTATR0.BIT.ERSERR  == 1)
        ||  (FLASH.FSTATR0.BIT.PRGERR  == 1)
        ||  (FLASH.FSTATR1.BIT.FCUERR  == 1))
    {
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* Check to see if peripheral clock notification command is needed */
    if( g_fcu_pclk_command == 0 )
    {
        /* Disable FCU interrupts, so interrupt will not trigger after
           peripheral clock notification command */
        FLASH.FRDYIE.BIT.FRDYIE = 0;

        /* Inform FCU of flash clock speed, check if operation is succesful */
        if(notify_peripheral_clock((FCU_BYTE_PTR)flash_addr)!=0)
        {
            /* Return FLASH_FAILURE, operation failure */
            return FLASH_ERR_FAILURE;
        }

        /* Only re-enable flash ready interrupt if using BGO */
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        if( g_current_mode == FLD_PE_MODE )
        {
            /* Re-enable the FRDYI interrupt */
            FLASH.FRDYIE.BIT.FRDYIE = 1;
        }
#endif
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        if( g_current_mode == ROM_PE_MODE )
        {
            /* Don't enable flash interrupts if programming lock bits */
            if( g_flash_state != FLASH_LOCK_BIT )
            {
                /* Re-enable the FRDYI interrupt */
                FLASH.FRDYIE.BIT.FRDYIE = 1;
            }
        }
#endif

        /* No need to notify FCU of clock supplied to flash again */
        g_fcu_pclk_command = 1;
    }

    /* Return FLASH_SUCCESS, operation successful */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function enter_pe_mode
******************************************************************************/

/******************************************************************************
* Function Name: exit_pe_mode
* Description  : Takes the FCU out of program/erase mode.
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash
*                programming, this function can reside and execute in Flash.
* Arguments    : flash_addr -
*                    The programming/erasure address
* Return Value : none
******************************************************************************/
static void exit_pe_mode (uint32_t flash_addr)
{
    /* Declare wait timer count variable */
    volatile int32_t wait_cnt;

    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;

    /* Cast flash address so that it can be used as pointer and will be
       accessed correctly. */
    p_addr = (FCU_BYTE_PTR)flash_addr;

    /* Set wait timer count duration */
    wait_cnt = WAIT_MAX_ERASE;

    /* Iterate while loop whilst FCU operation is in progress */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait timer count variable */
        wait_cnt--;

        /* Check if wait timer count value has reached zero */
        if(wait_cnt == 0)
        {
            /* Timeout duration has elapsed, assuming operation failure and
               resetting the FCU */
            flash_reset();

            /* Break from the while loop prematurely */
            break;
        }
    }

    /* Check FSTATR0 and execute a status register clear command if needed */
    if(     (FLASH.FSTATR0.BIT.ILGLERR == 1)
        ||  (FLASH.FSTATR0.BIT.ERSERR  == 1)
        ||  (FLASH.FSTATR0.BIT.PRGERR  == 1))
    {

        /* Clear ILGLERR */
        if(FLASH.FSTATR0.BIT.ILGLERR == 1)
        {
            /* FASTAT must be set to 0x10 before the status clear command
               can be successfully issued  */
            if(FLASH.FASTAT.BYTE != 0x10)
            {
                /* Set the FASTAT register to 0x10 so that a status clear
                    command can be issued */
                FLASH.FASTAT.BYTE = 0x10;
            }
        }

        /* Send status clear command to FCU */
        *p_addr = 0x50;
    }

    /* Enter ROM Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. Could add a call to FlashError()
               here if user wants to process this. */
            break;
        }
    }

    /* Disable the FRDYI interrupt */
    FLASH.FRDYIE.BIT.FRDYIE = 0;

    /* Flash write/erase disabled */
    FLASH.FWEPROR.BYTE = 0x02;

}







/******************************************************************************
* Function Name: flash_get_romBlock_size
* Description  : Return the size of the ROM block for this address, as well as
*                the starting and ending addresses for this block.
* Arguments    : romaddr -The address for which the rom block info is being requested
*                rom_block_info_t* block_info - ptr to a rom_block_info_t structure
*                                  which will hold the block info
* Return Value : 0   Invalid address supplied
*                    Block size otherwise
******************************************************************************/
uint32_t flash_get_romBlock_size (uint32_t romaddr, rom_block_info_t* pblock_info)
{
    uint16_t  index = 0;
    volatile rom_block_sizes_t block_info;
    volatile uint32_t total_section_size;
    volatile uint32_t this_section_blockSt = 0;

    if (pblock_info == NULL)
        return(0);

    block_info =  g_flash_RomBlockSizes[0];
    total_section_size  = block_info.block_size * block_info.num_blocks;

    pblock_info->end_addr = FLASH_CF_BLOCK_0 + block_info.block_size-1;
    pblock_info->start_addr = (pblock_info->end_addr - (total_section_size))+1;
    pblock_info->block_number = 0;

    for (index = 1; index < NUM_BLOCK_TABLE_ENTRIES; index++)
    {
        // Is this address in this block range?
        if ((romaddr >= pblock_info->start_addr)  && (romaddr <= pblock_info->end_addr))
            break;
        pblock_info->block_number += block_info.num_blocks;
        this_section_blockSt = block_info.num_blocks-1;
        block_info =  g_flash_RomBlockSizes[index];
        total_section_size  = block_info.block_size * block_info.num_blocks;
        pblock_info->end_addr = pblock_info->start_addr - 1;
        pblock_info->start_addr -= total_section_size;
    }

    pblock_info->block_number += ((pblock_info->end_addr - romaddr)/block_info.block_size);
    pblock_info->thisblock_stAddr = (pblock_info->end_addr - (pblock_info->block_number - this_section_blockSt)*block_info.block_size)+1;
    pblock_info->block_size = block_info.block_size;
    return(block_info.block_size);
}
/******************************************************************************
End of function  flash_get_romBlock_size
******************************************************************************/


/******************************************************************************
* Function Name: flash_get_romBlock_info
* Description  : Return the details for the supplied Rom block number
*                the starting and ending addresses for this block.
* Arguments    : block_number - A valid ROM block number
*                rom_block_info_t* block_info - ptr to a rom_block_info_t structure
*                                  which will hold the block info.
* Return Value : False   Invalid block supplied
*                True -  supplied block_info structure holds the block info
******************************************************************************/
bool flash_get_romBlock_info (uint32_t block_number, rom_block_info_t* pblock_info)
{
    uint16_t  this_block = 0;
    rom_block_sizes_t bl_info =  g_flash_RomBlockSizes[0];
    volatile uint32_t total_section_size  = bl_info.block_size * bl_info.num_blocks;
    volatile uint32_t this_section_blockSt = 0;
    volatile uint32_t section_counter = 0;
    volatile uint32_t blksize_counter = 0;


    if (block_number > ROM_NUM_BLOCKS)
        return(false);

    pblock_info->end_addr = FLASH_CF_BLOCK_0 + bl_info.block_size-1;
    pblock_info->start_addr = FLASH_CF_BLOCK_0;
    pblock_info->block_size = bl_info.block_size;

    for (this_block = 0; this_block < ROM_NUM_BLOCKS; this_block++)
    {
        if (this_block == block_number)
            break;

        if (blksize_counter++ == bl_info.num_blocks-1)
        {
            bl_info =  g_flash_RomBlockSizes[++section_counter];
            blksize_counter = 0;
        }
        pblock_info->end_addr = pblock_info->start_addr-1;
        pblock_info->start_addr -= bl_info.block_size;
        pblock_info->block_size = bl_info.block_size;

    }

    return(true);
}
/******************************************************************************
End of function  flash_get_romBlock_info
******************************************************************************/


/***********************************************************************************************************************
* Function Name: flash_api_erase
* Description  : This function executes specific functionality to erase the specified Code or Data Flash blocks.
* Arguments    : uint32_t block_start_address -
*                 Start address of the first block. Actual address or entry from "flash_block_address_t" enum can be used
*                uint32_t num_blocks -
*                 For FCU based MCU's the number of blocks is always 1.
*                 Only a single block may be erased at a time.
* Return Value : FLASH_SUCCESS -
*                    Erase completed successfully; successfully initialized in case of BGO mode.
*                FLASH_ERR_ADRRESS -
*                    Start address is an invalid Code/Data Flash block start address
*                FLASH_ERR_BOUNDARY - The block_start_address and accompanying num_blocks cross one of the
*                                     defined ROM AREA boundaries.
*                FLASH_ERR_BUSY -
*                    Flash peripheral is busy with another operation
*                FLASH_ERR_FAILURE
*                    Flash Write operation failed for some other reason. This can be a result of trying to erase an area
*                    that has been protected via access control.
***********************************************************************************************************************/
flash_err_t flash_api_erase(flash_block_address_t block_start_address, uint32_t num_blocks)
{
    flash_err_t  result = FLASH_SUCCESS;
    uint32_t p_addr = (uint32_t )block_start_address;        /* Declare address pointer */
    rom_block_info_t block_info;
    uint32_t blockcnt = 0;
    uint32_t block_number;
    uint32_t initial_block_number;
    uint32_t entry_address;
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    uint32_t block_size;
#endif

    volatile int32_t bytes_to_erase = (int32_t)(FLASH_DF_BLOCK_1 - FLASH_DF_BLOCK_0)  * num_blocks;   // for DF this is correct

    /* Take off upper byte since for programming/erase addresses for ROM are
          the same as read addresses except upper byte is masked off to 0's.
          Data Flash addresses are not affected. */
    block_start_address &= 0x00FFFFFF;
    entry_address = block_start_address;

    if (num_blocks == 0)
    {
        return(FLASH_ERR_BLOCKS);
    }

    if (flash_valid_addr_check(block_start_address) == true)
    {

        /* Do we want to erase a Data Flash block or ROM block? */
        if ((block_start_address >= FLASH_DF_BLOCK_0) && (block_start_address < FLASH_DF_BLOCK_INVALID ))
        {
            /* Set current FCU mode to data flash PE */
            g_current_mode = FLD_PE_MODE;


            /* Check to make sure erase will not go over the end of DF. */
             if ((block_start_address + bytes_to_erase) > (DF_ADDRESS + MCU_DATA_FLASH_SIZE_BYTES))
             {
                 return FLASH_ERR_OVERFLOW;
             }

             /* Verify address is on block boundary */
             if ((block_start_address & (FLASH_DF_BLOCK_SIZE-1)) != 0)
             {
                 return FLASH_ERR_ADDRESS;
             }
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
             /* Set global variables so that erase can continue in ISR. Set these first so that if the Erase generates an error the
              * error ISR will have the correct address to use in clearing the error flag (the one provided to enter_pe_mode() */
             g_bgo_flash_addr = p_addr + DF_ERASE_BLOCK_SIZE;
             g_bgo_bytes = bytes_to_erase - DF_ERASE_BLOCK_SIZE;
#endif
        }
        else
        {
            /* Set current FCU mode to ROM PE */
            g_current_mode = ROM_PE_MODE;

            // Calculate the total # of bytes to erase,  blocks may differ in sizes.
            bytes_to_erase = flash_get_romBlock_size(block_start_address, &block_info);
            initial_block_number = block_number = block_info.block_number;


            for (blockcnt = 0; blockcnt < num_blocks-1; blockcnt++)
             {
                block_number--;
                // We now have the block number for this address
                if (flash_get_romBlock_info (block_number, &block_info) == false)
                {
                    return(FLASH_ERR_ADDRESS);
                }
                bytes_to_erase += block_info.block_size;
             }

            /* Check to make sure erase will not go over the end of CF. */
             if ((block_start_address + bytes_to_erase) > (ROM_PE_ADDR + MCU_ROM_SIZE_BYTES))
             {
                 return FLASH_ERR_OVERFLOW;
             }

             /* Check for attempts to program over flash area boundaries. These
                boundaries are defined in r_flash_rx*MCU Group*.h as 'ROM_AREA_#'.
                Use ">" rather than ">=" because "bytes" does not include "flash_addr". */
             if (((block_start_address < (ROM_AREA_0)) && ((block_start_address + bytes_to_erase) > ROM_AREA_0))
#if defined(ROM_AREA_1)
                || ((block_start_address < (ROM_AREA_1))  && ((block_start_address + bytes_to_erase) > ROM_AREA_1))
#endif
#if defined(ROM_AREA_2)
                || ((block_start_address < (ROM_AREA_2))  && ((block_start_address + bytes_to_erase) > ROM_AREA_2))
#endif
#if defined(ROM_AREA_3)
                || ((block_start_address < (ROM_AREA_3))  && ((block_start_address + bytes_to_erase) > ROM_AREA_3))
#endif
                 )
             {
                 /* Return cannot write across a flash area boundary */
                 return FLASH_ERR_BOUNDARY;
             }
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        /* Set global variables so that erase can continue in ISR. Set these first so that if the Erase generates an error the
         * error ISR will have the correct address to use in clearing the error flag (the one provided to enter_pe_mode() */
        block_size = flash_get_romBlock_size(block_start_address, &block_info);
        g_bgo_flash_addr = p_addr + block_size;
        g_bgo_bytes = bytes_to_erase - block_size;

#endif
        }

        /* Attempt to grab state */
        if( flash_grab_state(FLASH_ERASING) != FLASH_SUCCESS )
        {
            /* Another operation is already in progress */
            return FLASH_ERR_BUSY;
        }

        /* Enter ROM PE mode, check if operation successful */
        if( enter_pe_mode(block_start_address) != FLASH_SUCCESS)
        {
            /* Make sure part is in ROM read mode. */
            exit_pe_mode(block_start_address);

            /* Release state */
            flash_release_state2();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_ERR_FAILURE;
        }

#if (FLASH_CFG_IGNORE_LOCK_BITS == 1)
        /* Cancel the ROM Protect feature
           NOTE: If you are using block locking feature to protect your data,
           then comment out 'FLASH_CFG_IGNORE_LOCK_BITS' in r_flash_rx_config.h */
        FLASH.FPROTR.WORD = 0x5501;
#else
        /* Only disable lock bit protection if user has specified to
           do so earlier */
        if( g_lock_bit_protection == false )
        {
            /* Cancel the ROM Protect feature */
            FLASH.FPROTR.WORD = 0x5501;
        }
#endif

        block_number = initial_block_number;
        while(0 < bytes_to_erase)
        {
             /* Send FCU command to erase block */
             result = flash_erase_command((FCU_BYTE_PTR)block_start_address);
             if (result != FLASH_SUCCESS)
                 break;

             if( g_current_mode == ROM_PE_MODE )
             {
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                 break;
#endif

                // Code Flash Erase
                 /* Advance pointer to next block */
                 block_number--;

                 // Decrease by the amount we just wrote
                 bytes_to_erase -= (int32_t)block_info.block_size;

                 if (bytes_to_erase == 0)
                 {
                     break;
                 }

                if (flash_get_romBlock_info (block_number, &block_info) == false)
                 {
                     return(FLASH_ERR_ADDRESS);
                 }
                 block_start_address = (flash_block_address_t)block_info.start_addr;
             }
             else
             {
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
                 break;
#endif
                 // Data Flash Erase
                 /* Advance pointer to next block */
                  block_start_address += DF_ERASE_BLOCK_SIZE;

                  /* Subtract off bytes erased */
                  bytes_to_erase -= DF_ERASE_BLOCK_SIZE;
             }

         }


#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        if( g_current_mode == ROM_PE_MODE )
        {

            /* Return, check result later in ISR */
            return FLASH_SUCCESS;
        }
#endif

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        if( g_current_mode == FLD_PE_MODE )
        {
            /* Return, check result later in ISR */
            return FLASH_SUCCESS;
        }

#endif
    }
    else
    {
            return FLASH_ERR_ADDRESS;
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode(entry_address);

    /* Release state */
    flash_release_state2();

    /* Return erase result */
    return result;
}

/******************************************************************************
* Function Name: flash_erase_command
* Description  : Issues the FCU command to erase a flash block
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : erase_addr -
*                    An address in the block to be erased
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static flash_err_t flash_erase_command (FCU_BYTE_PTR const erase_addr)
{
    /* Declare timer wait count variable */
    volatile int32_t wait_cnt;
    /* Declare erase operation result container variable */
    flash_err_t result = FLASH_SUCCESS;

    /* Send the FCU Command */
    *erase_addr = 0x20;
    *erase_addr = 0xD0;

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    if( g_current_mode == ROM_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
    if( g_current_mode == FLD_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif

    /* Set the wait counter timeout duration */
    wait_cnt = WAIT_MAX_ERASE;

    /* Wait while FCU operation is in progress */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0){

            /* Maximum timeout duration for erasing a ROM block has
               elapsed, assuming operation failure - reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_ERR_FAILURE;
        }
    }

    /* Check if erase operation was successful by checking
       bit 'ERSERR' (bit5) and 'ILGLERR' (bit 6) of register 'FSTATR0' */
    /* Check FCU error */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.ERSERR  == 1))
    {
        result = FLASH_ERR_FAILURE;
    }

    /* Return erase result */
    return result;
}




/****************************************************************************************
* Function Name: flash_api_write()
* Description  : This function executes RX specific functionality to write into flash
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : buffer_addr -
*                    Address location of data buffer to write into flash.
*
*                flash_addr -
*                    Flash address location to write to. This address
*                    must be on a program boundary (e.g. RX62N has
*                    256-byte ROM writes and 8-byte DF writes).
*                bytes -
*                    The number of bytes to write. You must always pass a
*                    multiple of the programming size (e.g. RX62N has
*                    256-byte ROM writes and 8-byte DF writes).
* Return Value : FLASH_SUCCESS -
*                    Operation Successful.
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_ERR_ALIGNED -
*                    Flash address was not on correct boundary
*                FLASH_ERR_BYTES -
*                    Number of bytes did not match programming size of ROM or DF
*                FLASH_ERR_ADRRESS -
*                    Invalid address
*                FLASH_ERR_BOUNDARY -
*                    (ROM) Cannot write across flash areas.
*                FLASH_ERR_BUSY -
*                    Flash is busy with another operation
*
******************************************************************************/
flash_err_t flash_api_write(uint32_t buffer_addr, uint32_t flash_addr, uint32_t bytes)
{
    flash_err_t result = FLASH_SUCCESS;
    uint32_t entry_address;

    /* Declare result container and number of bytes to write variables */
    uint32_t num_byte_to_write;
#ifdef FLASH_CFG_FLASH_TO_FLASH
    /* Local variable when using FLASH_CFG_FLASH_TO_FLASH */
    uint16_t i;
#endif

    /* Take off upper byte since for programming/erase addresses for ROM are
        the same as read addresses except upper byte is masked off to 0's.
        Data Flash addresses are not affected. */
    flash_addr &= 0x00FFFFFF;
    entry_address = flash_addr;

    /* Make sure 'bytes' is not 0. */
    if (bytes == 0)
    {
        return FLASH_ERR_BYTES;
    }


    /* ROM area or Data Flash area - Check for DF first */
    if ((flash_addr >= FLASH_DF_BLOCK_0) && (flash_addr < FLASH_DF_BLOCK_INVALID ))
   {
        /* Check if the number of bytes were passed is a multiple of the
           programming size for the data flash */
        if( bytes & (DF_PROGRAM_SIZE_SMALL-1) )
        {
            /* Return number of bytes not a multiple of the programming size */
            return FLASH_ERR_BYTES;
        }
        /* Check for an address on a programming boundary. */
        if( flash_addr & (DF_PROGRAM_SIZE_SMALL-1) )
        {
            /* Return address not on a programming boundary */
            return FLASH_ERR_ALIGNED;
        }

        /* Check to make sure erase will not go over the end of DF. */
        if ((flash_addr + bytes) > (DF_ADDRESS + MCU_DATA_FLASH_SIZE_BYTES))
        {
            return FLASH_ERR_OVERFLOW;
        }

        /* Set current FCU mode to data flash PE */
        g_current_mode = FLD_PE_MODE;
    }
    /* Check for ROM area */
    else if(    (flash_addr >= ROM_PE_ADDR)
            &&  (flash_addr < ROM_PE_ADDR + MCU_ROM_SIZE_BYTES) )
    {
        /* First make sure that ROM operations are enabled. */
#if (FLASH_CFG_CODE_FLASH_ENABLE == 0)
        /* ROM operations are not enabled! Enable them in r_flash_rx_config.h */
        return FLASH_ERR_FAILURE;
#endif

        /* Check if the number of bytes were passed is a multiple of the
           programming size for ROM */
        if( bytes & (ROM_PROGRAM_SIZE-1) )
        {
            /* Return number of bytes not a multiple of the programming size */
            return FLASH_ERR_BYTES;
        }

        /* Check for an address on a programming boundary. */
        if( flash_addr & (ROM_PROGRAM_SIZE-1))
        {
            /* Return address not on a ROM programming byte boundary */
            return FLASH_ERR_ALIGNED;
        }

        /* Check for attempts to program over flash area boundaries. These
           boundaries are defined in r_flash_rx*MCU Group*.h as 'ROM_AREA_#'.
           Use ">" rather than ">=" because "bytes" does not include "flash_addr". */
        if (((flash_addr < (ROM_AREA_0)) && ((flash_addr + bytes) > ROM_AREA_0))
#if defined(ROM_AREA_1)
           || ((flash_addr < (ROM_AREA_1))  && ((flash_addr + bytes) > ROM_AREA_1))
#endif
#if defined(ROM_AREA_2)
           || ((flash_addr < (ROM_AREA_2))  && ((flash_addr + bytes) > ROM_AREA_2))
#endif
#if defined(ROM_AREA_3)
           || ((flash_addr < (ROM_AREA_3))  && ((flash_addr + bytes) > ROM_AREA_3))
#endif
            )
        {
            /* Return cannot write across a flash area boundary */
            return FLASH_ERR_BOUNDARY;
        }

        /* Make sure write is not going over end of flash. */
        if ((flash_addr + bytes) > (ROM_PE_ADDR + MCU_ROM_SIZE_BYTES))
        {
            return FLASH_ERR_OVERFLOW;
        }

        /* Set FCU to ROM PE mode */
        g_current_mode = ROM_PE_MODE;
    }
    else
    {
        /* Return invalid flash address */
        return FLASH_ERR_ADDRESS;
    }

    /* Attempt to grab state */
    if( flash_grab_state(FLASH_WRITING) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_ERR_BUSY;
    }

#ifdef FLASH_CFG_FLASH_TO_FLASH
    /* Are we doing a ROM to ROM or DF to DF transfer? */
    if( (buffer_addr >= ROM_PE_ADDR) ||
        ((buffer_addr >= DF_ADDRESS) && (buffer_addr < (DF_ADDRESS + MCU_DATA_FLASH_SIZE_BYTES))))
    {
        /* Buffering is needed. */
        /* Get maximum programming size that can currently be used. */
        num_byte_to_write = flash_get_program_size(bytes, flash_addr);

        /* Copy over first buffer */
        for(i = 0; i < num_byte_to_write; i++)
        {
            /* Copy over each byte */
            g_temp_array[i] = *((uint8_t *)(buffer_addr+i));
        }

        /* Check size */
        if( bytes == num_byte_to_write )
        {
            /* Since we were able to put the entire buffer in RAM we can
               just change the buffer pointer and go on as usual. */
            g_flash_to_flash_op = 0;

            /* Set new buffer address */
            buffer_addr = (uint32_t)&g_temp_array[0];
        }
        else
        {
            /* Set FLASH_CFG_FLASH_TO_FLASH flag */
            g_flash_to_flash_op = 1;
        }
    }
    else
    {
        /* No need to buffer */
        g_flash_to_flash_op = 0;
    }

#endif /* FLASH_CFG_FLASH_TO_FLASH */

    /* Enter PE mode, check if operation is successful */
    if( enter_pe_mode(flash_addr) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(flash_addr);

        /* Release state */
        flash_release_state2();

        /* Return operation failure */
        return FLASH_ERR_FAILURE;
    }

#if (FLASH_CFG_IGNORE_LOCK_BITS == 1)
    /* Cancel the ROM Protect feature
       NOTE: If you are using block locking feature to protect your data,
       then set  'FLASH_CFG_IGNORE_LOCK_BITS' to 0 in r_flash_rx_config.h */
    FLASH.FPROTR.WORD = 0x5501;
#else
    /* Only disable lock bit protection if user has specified to
       do so earlier */
    if( g_lock_bit_protection == false )
    {
        /* Cancel the ROM Protect feature */
        FLASH.FPROTR.WORD = 0x5501;
    }
#endif

    /* Iterate while there are still bytes remaining to write */
    while( bytes )
    {
        /* Get maximum programming size that can currently be used. */
        num_byte_to_write = flash_get_program_size((uint32_t)bytes, flash_addr);

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        if( g_current_mode == FLD_PE_MODE )
        {
            /* At this point we are going to set some global variables which
               will continue on with write after these bytes are written.
               I set these values before the actual first write because I was
               getting an error when the flash ready interrupt was being called
               before these values were set. */
            g_bgo_flash_addr = flash_addr + num_byte_to_write;
            g_bgo_buffer_addr = buffer_addr + num_byte_to_write;
            g_bgo_bytes = bytes - num_byte_to_write;
        }
#endif

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        if( g_current_mode == ROM_PE_MODE )
        {
            /* At this point we are going to set some global variables which
               will continue on with write after these bytes are written.
               I set these values before the actual first write because I was
               getting an error when the flash ready interrupt was being called
               before these values were set. */
            g_bgo_flash_addr = flash_addr + num_byte_to_write;
            g_bgo_buffer_addr = buffer_addr + num_byte_to_write;
            g_bgo_bytes = bytes - num_byte_to_write;
        }
#endif

        /* Call the Programming function, store the operation status in the
           container variable, result */
#if (FLASH_CFG_FLASH_TO_FLASH == 1)
        if( g_flash_to_flash_op == 1 )
        {
            if( g_current_mode == FLD_PE_MODE )
            {
                result = data_flash_write( flash_addr, (uint32_t)&g_temp_array[0],
                                           num_byte_to_write);
            }
            else
            {
                result = rom_write( flash_addr, (uint32_t)&g_temp_array[0],
                                    num_byte_to_write);
            }
        }
        else
        {
            if( g_current_mode == FLD_PE_MODE )
            {
                result = data_flash_write( flash_addr, buffer_addr,
                                           num_byte_to_write);
            }
            else
            {
                result = rom_write( flash_addr, buffer_addr,
                                    num_byte_to_write);
            }
        }
#else
        if( g_current_mode == FLD_PE_MODE )
        {
            result = data_flash_write(flash_addr, buffer_addr, num_byte_to_write);
        }
        else
        {
            result = rom_write(flash_addr, buffer_addr, num_byte_to_write);
        }
#endif

        /* Check the container variable result for errors */
        if( result != FLASH_SUCCESS )
        {
            /* Data flash write error detected, break from flash write
               while loop prematurely */
            break;
        }

#if (FLASH_CFG_DATA_FLASH_BGO == 1)
        if( g_current_mode == FLD_PE_MODE )
        {
            /* Return FLASH_SUCCESS, rest of programming will be done
               in interrupt */
            return FLASH_SUCCESS;
        }
#endif
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        if( g_current_mode == ROM_PE_MODE )
        {
            /* Return FLASH_SUCCESS, rest of programming will be done
               in interrupt */
            return FLASH_SUCCESS;
        }
#endif

#if ((FLASH_CFG_CODE_FLASH_BGO == 0)) || ((FLASH_CFG_DATA_FLASH_BGO == 0))

        /* Increment the flash address and the buffer address by the size
           of the transfer thats just completed */
        flash_addr += num_byte_to_write;
        buffer_addr += num_byte_to_write;

        /* Decrement the number of bytes remaining by the size of the last
           flash write */
        bytes -= num_byte_to_write;

    #ifdef FLASH_CFG_FLASH_TO_FLASH
        /* Check to see if we need to buffer more data */
        if( (bytes > 0) &&
            (g_flash_to_flash_op == 1) )
        {
            /* We must leave PE mode to transfer next buffer to RAM */
            exit_pe_mode(flash_addr);

            /* Get maximum programming size that can currently be used. */
            num_byte_to_write = flash_get_program_size(bytes, flash_addr);

            /* Copy over up to next bytes */
            for(i = 0; i < num_byte_to_write; i++)
            {
                /* Copy over each byte */
                g_temp_array[i] = *((uint8_t *)(buffer_addr+i));
            }

            /* Re-enter PE mode, check if operation is successful */
            if( enter_pe_mode(flash_addr) != FLASH_SUCCESS)
            {
                /* Make sure part is in ROM read mode. */
                exit_pe_mode(flash_addr);

                /* Release state */
                flash_release_state2();

                /* Return operation failure */
                return FLASH_ERR_FAILURE;
            }
        }
    #endif /* FLASH_CFG_FLASH_TO_FLASH */
#endif
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode(entry_address);

    /* Release state */
    flash_release_state2();

    /* Return flash programming result */
    return result;

}





/*****************************************************************************
* Function Name: flash_api_control()
* Description  : This function supports additional configuration operations for
*                FCU enabled MCu's.
*                The supported commands are listed in the flash_cmd_t enum.
* Arguments    : flash_cmd_t cmd -
*                 command to be executed.
*                void *pcfg -
*                 Pointer to configuration. This argument can be NULL for
*                 commands that do not require a configuration.
*
*                Command                                |   Argument
*                FLASH_CMD_RESET------------------------| NULL
*                FLASH_CMD_STATUS_GET-------------------| NULL
* RX100 Series only:
*
*                FLASH_CMD_ACCESSWINDOW_SET----------------| void (*flash_access_window_config_t)(void *)
*                FLASH_CMD_LOCKBIT_READ--------------------| void (*flash_lockbit_config_t)(void *)
*                FLASH_CMD_LOCKBIT_PROTECTION--------------| void (*flash_lockbit_enable_t)(void *)
*                FLASH_CMD_LOCKBIT_PROGRAM-----------------| void (*flash_program_lockbit_config_t)(void *)
*
* Return Value : FLASH_SUCCESS -
*                    Operation completed successfully.
*                FLASH_ERR_FAILURE -
*                    Operation not available for this MCU
*                FLASH_ERR_ADRRESS -
*                    Address is an invalid Code/Data Flash block start address
*                FLASH_ERR_NULL_PTR -
*                    Pointer was NULL for a command that expects a configuration structure
*                FLASH_ERR_BUSY -
*                    Flash peripheral is busy with another operation or not initialized
*
******************************************************************************/
flash_err_t flash_api_control(flash_cmd_t cmd,  void  *pcfg)
{
    flash_err_t err = FLASH_SUCCESS;
    flash_access_window_config_t   *pAccessInfo = pcfg;
#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_IGNORE_LOCK_BITS == 0))
    flash_read_lockbit_config_t    *pLockBitConfig = pcfg;
    flash_lockbit_enable_t         *pLockBitProtect = pcfg;
    flash_program_lockbit_config_t *pLockBitProgram = pcfg;
#endif

    (void)&pAccessInfo;      /* Prevent compiler warning for not using 'pAccessInfo' AND not generate any code  */

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((cmd != FLASH_CMD_RESET) && (cmd != FLASH_CMD_STATUS_GET))
    {
        if ((pcfg == NULL) || (pcfg == FIT_NO_PTR))
        {
            return FLASH_ERR_PARAM;
        }
    }
#endif

    /*If the command is to reset the Flash, then no attempt is made to grab the lock
     * before executing the reset since the assumption is that the RESET command
     * is used to terminate any existing operation. */
    if (cmd == FLASH_CMD_RESET)
    {
        flash_release_state2();
        flash_reset();           // reset the flash circuit
        return err;
    }

    /*Check if API is busy*/
    if(g_flash_state != FLASH_READY)
    {
       // In blocking mode we will return busy if the flash state is busy no matter what
        return FLASH_ERR_BUSY;  /* API not initialized or busy with another operation*/
    }


    switch (cmd)
    {

        case FLASH_CMD_ACCESSWINDOW_SET:       // DF only
            flash_access_window_set(pAccessInfo);
        break;

        case FLASH_CMD_STATUS_GET:
            err = flash_getStatus();
        break;

#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_IGNORE_LOCK_BITS == 0))
        case FLASH_CMD_LOCKBIT_PROTECTION:      // CF only
            err = flash_lockbit_protection(pLockBitProtect->enable);
        break;

        case FLASH_CMD_LOCKBIT_READ:            // CF only
            err = flash_lockbit_read(pLockBitConfig);
        break;

        case FLASH_CMD_LOCKBIT_PROGRAM:         // CF only
            err = flash_lockbit_program(pLockBitProgram);
        break;
#endif

        default:
            err = FLASH_ERR_PARAM;
    }
    flash_release_state2();
    return err;
}



/******************************************************************************
* Function Name: flash_reset
* Description  : Reset the FCU.
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash
*                programming, this function can reside and execute in Flash.
* Arguments    : none
* Return Value : none
******************************************************************************/
void flash_reset (void)
{
    /* Declare wait counter variable */
    volatile int32_t    wait_cnt;

    /* Reset the FCU */
    FLASH.FRESETR.WORD = 0xCC01;

    /* Give FCU time to reset */
    wait_cnt = WAIT_TRESW;

    /* Wait until the timer reaches zero */
    while(wait_cnt != 0)
    {
        /* Decrement the timer count each iteration */
        wait_cnt--;
    }

    /* Release state */
    flash_release_state2();

    /* FCU is not reset anymore */
    FLASH.FRESETR.WORD = 0xCC00;

    /* Enter ROM Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. Could add a call to FlashError()
               here if user wants to process this. */
            break;
        }
    }

    /* Flash write/erase disabled */
    FLASH.FWEPROR.BYTE = 0x02;
}


#if (FLASH_CFG_IGNORE_LOCK_BITS == 0)

/******************************************************************************
* Function Name: flash_lockbit_program
* Description  : Programs the lock bit for a specified ROM block. If
*                the lock bit for a block is set and lock bit protection is
*                enabled then that block cannot be programmed/erased.
*                NOTE: This function MUST execute from RAM
* Arguments    :flash_program_lockbit_config_t *lockbit_addr -
*                    Which ROM erasure block to set the lock bit for
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_ERR_BUSY -
*                    Another flash operation is in progress
******************************************************************************/
flash_err_t flash_lockbit_program (flash_program_lockbit_config_t *lockbit_addr)
{
    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;
    /* Declare operation result container variable */
    flash_err_t result = FLASH_SUCCESS;
    /* Declare timer wait count variable */
    volatile int32_t wait_cnt;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if (lockbit_addr->block_addr <= FLASH_CF_BLOCK_INVALID)
    {
        return FLASH_ERR_PARAM;
    }
#endif

    /* Attempt to grab state */
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_ERR_BUSY;
    }

    /* Flash Command Address */
    p_addr = (FCU_BYTE_PTR)lockbit_addr->block_addr;

    /* Set FCU to ROM PE mode */
    g_current_mode = ROM_PE_MODE;

    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode((uint32_t)p_addr) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode((uint32_t)p_addr);

        /* Release state */
        flash_release_state2();

        /* Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* Send the FCU Command */
    *p_addr = 0x77;
    *p_addr = 0xD0;

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_ROM_WRITE;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {
            /* Maximum timeout duration for writing to ROM has elapsed -
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            result = FLASH_ERR_FAILURE;
        }
    }

    /* Check for illegal command or programming errors */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR  == 1))
    {
        /* Return FLASH_FAILURE, operation failure */
        result = FLASH_ERR_FAILURE;
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode((uint32_t)p_addr);

    /* Release state */
    flash_release_state2();

    /* Return result of lock bit programming */
    return result;
}
/******************************************************************************
End of function  R_FlashProgramLockBit
******************************************************************************/




/******************************************************************************
* Function Name: flash_lockbit_read
* Description  : Reads and returns the lock bit status for a ROM block
*                NOTE: This function MUST execute from RAM
* Arguments    : lockbit_info -
*                    Which ROM erasure block to read the lock bit of, and a place to store the result
*
* Return Value : FLASH_SUCCESS - operation successful, lockbit_info->result contains either
*                                FLASH_LOCK_BIT_SET or FLASH_LOCK_BIT_NOT_SET
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_ERR_BUSY -
*                    Another flash operation is in progress
******************************************************************************/
flash_err_t flash_lockbit_read (flash_read_lockbit_config_t *lockbit_info)
{
    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;
    /* Declare operation result container variable */
    flash_err_t result = FLASH_SUCCESS;
    /* Holds outcome of lock-bit read */
    uint8_t lock_bit;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if (lockbit_info->block_addr <= FLASH_CF_BLOCK_INVALID)
    {
        return FLASH_ERR_PARAM;
    }
#endif

    /* Attempt to grab state */;
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_ERR_BUSY;
    }

    /* Flash Command Address */
    p_addr = (FCU_BYTE_PTR)lockbit_info->block_addr;

    /* Set FCU to ROM PE mode */
    g_current_mode = ROM_PE_MODE;

    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode((uint32_t)p_addr) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode((uint32_t)p_addr);

        /* Release state */
        flash_release_state2();

        /* Return FLASH_FAILURE, operation failure */
        return FLASH_ERR_FAILURE;
    }

    /* Switch to ROM Lock-Bit Read Mode */
    *p_addr = 0x71;

    /* Check for illegal command */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* Return FLASH_FAILURE, operation failure */
        result = FLASH_ERR_FAILURE;
    }
    else
    {
        /* Read lock bit value */
        lock_bit = *p_addr;

        /* Set return value */
        if(lock_bit != 0x00)
        {
            /* Lock bit is not set */
            lockbit_info->result = FLASH_LOCK_BIT_NOT_SET;
        }
        else
        {
            /* Lock bit is set */
            lockbit_info->result = FLASH_LOCK_BIT_SET;
        }
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode((uint32_t)p_addr);

    /* Release state */
    flash_release_state2();

    /* Return result of lock bit programming */
    return result;
}
/******************************************************************************
End of function  R_FlashReadLockBit
******************************************************************************/
#endif





/******************************************************************************
* Function Name: flash_get_program_size
* Description  : Gets the number of bytes that can be written at this time.
*                Since the DF and ROM have different available programming
*                sizes, we want to use the largest option to save time.
* Arguments    : bytes -
*                    How many bytes are left to write total.
*                flash_addr -
*                    Current address we are writing to.
* Return Value : Number of bytes to write at this time.
******************************************************************************/
static uint32_t flash_get_program_size (uint32_t bytes, uint32_t flash_addr)
{
    uint32_t num_byte_to_write = 0;

    if( g_current_mode == FLD_PE_MODE )
    {
        /* This is a DF write. */
#if defined(DF_PROGRAM_SIZE_LARGE)
        if( (bytes >= DF_PROGRAM_SIZE_LARGE) &&
            ((flash_addr & (DF_PROGRAM_SIZE_LARGE-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = DF_PROGRAM_SIZE_LARGE;
        }
        else
        {
            /* Cannot use large program size, use small */
            num_byte_to_write = DF_PROGRAM_SIZE_SMALL;
        }
#else
        /* Set the next data transfer size to be DF_PROGRAM_SIZE_SMALL*/
        num_byte_to_write = DF_PROGRAM_SIZE_SMALL;
#endif
    }
    else
    {
        /* This is a ROM write. */
#if defined(ROM_PROGRAM_SIZE_LARGE)
        if( (bytes >= ROM_PROGRAM_SIZE_LARGE) &&
            ((flash_addr & (ROM_PROGRAM_SIZE_LARGE-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = ROM_PROGRAM_SIZE_LARGE;
        }
    #if defined(ROM_PROGRAM_SIZE_MEDIUM)
        else if( (bytes >= ROM_PROGRAM_SIZE_MEDIUM) &&
                 ((flash_addr & (ROM_PROGRAM_SIZE_MEDIUM-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = ROM_PROGRAM_SIZE_MEDIUM;
        }
    #endif
        else
        {
            /* Cannot use large program size, use small */
            num_byte_to_write = ROM_PROGRAM_SIZE_SMALL;
        }
#else
        /* Set the next data transfer size to be smallest supported write. */
        num_byte_to_write = ROM_PROGRAM_SIZE;
#endif
    }

    return num_byte_to_write;
}



/******************************************************************************
* Function Name: notify_peripheral_clock
* Description  : Notifies FCU or clock supplied to flash unit
*                NOTE: This function MUST execute from in RAM for 'ROM Area'
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : flash_addr -
*                    Flash address you will be erasing or writing to
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t notify_peripheral_clock (FCU_BYTE_PTR flash_addr)
{
    /* Declare wait counter variable */
    volatile int32_t wait_cnt;

    /* Notify Peripheral Clock(PCK) */
    /* Set frequency of PCK in MHz */
    FLASH.PCKAR.WORD = (MCU_CFG_FCLK_HZ/1000000);

    /* Execute Peripheral Clock Notification Commands */
    *flash_addr = 0xE9;
    *flash_addr = 0x03;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *flash_addr = 0xD0;

    /* Set timeout wait duration */
    wait_cnt = WAIT_MAX_NOTIFY_FCU_CLOCK;

    /* Check FRDY */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait counter */
        wait_cnt--;

        /* Check if wait counter has reached zero */
        if(wait_cnt == 0)
        {
            /* Timeout duration elapsed, assuming operation failure - Reset
               FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure  */
            return FLASH_ERR_FAILURE;
        }
    }

    /* Check ILGLERR */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* Return FLASH_FAILURE, operation failure*/
        return FLASH_ERR_FAILURE;
    }

    /* Return FLASH_SUCCESS, operation success */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function notify_peripheral_clock
******************************************************************************/





#endif

#pragma section // end FLASH_SECTION_ROM
