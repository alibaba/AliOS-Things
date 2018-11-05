/*******************************************************************************
  DMA System Service Library Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma_definitions_pic32c.h

  Summary:
    DMA System Service data type definitions header.

  Description:
    This file contains the data type definitions header.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _SYS_DMA_DEFINITIONS_PIC32C_H
#define _SYS_DMA_DEFINITIONS_PIC32C_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DMA System Service Channels

  Summary:
    This lists the set of channels available for data transfer using SYS_DMA.

  Description:
    This lists the set of channels available for data transfer using SYS_DMA.

  Remarks:
    None.
*/

typedef enum {

    DMA_CHANNEL_0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7,
    DMA_CHANNEL_8,
    DMA_CHANNEL_9,
    DMA_CHANNEL_10,
    DMA_CHANNEL_11,
    DMA_CHANNEL_12,
    DMA_CHANNEL_13,
    DMA_CHANNEL_14,
    DMA_CHANNEL_15,
    DMA_CHANNEL_16,
    DMA_CHANNEL_17,
    DMA_CHANNEL_18,
    DMA_CHANNEL_19,
    DMA_CHANNEL_20,
    DMA_CHANNEL_21,
    DMA_CHANNEL_22,
    DMA_CHANNEL_23,
    DMA_CHANNELS_NUMBER

} DMA_CHANNEL;


// *****************************************************************************
/* DMA System Service Hardware Trigger Type

  Summary:
    These two macros are used by DMA_TRIGGER_SOURCE type to distinguish
    whether it's a transmit type or receive type of trigger.

  Description:
    These two macros are used by DMA_TRIGGER_SOURCE type to distinguish
    whether it's a transmit type or receive type of trigger.

  Remarks:
    None.
*/

#define DMA_TRIGGER_TX     (1<<8)
#define DMA_TRIGGER_RX     (0<<8)


// *****************************************************************************
/* DMA System Service Hardware Trigger Source

  Summary:
    This lists the set of hardware requests available for the peripheral 
    synchronized transfer of SYS_DMA.

  Description:
    This lists the set of hardware requests available for the peripheral 
    synchronized transfer of SYS_DMA.
    Example: DMA_TRIGGER_<source> = PERID | DMA_TRIGGER_<type> .

  Remarks:
    None.
*/

typedef enum {
    
    DMA_TRIGGER_HSMCI_RX           = 0x00 | DMA_TRIGGER_RX,
    DMA_TRIGGER_HSMCI_TX           = 0x00 | DMA_TRIGGER_TX,
    DMA_TRIGGER_SPI_0_TX           = 0x01 | DMA_TRIGGER_TX,
    DMA_TRIGGER_SPI_0_RX           = 0x02 | DMA_TRIGGER_RX,
    DMA_TRIGGER_SPI_1_TX           = 0x03 | DMA_TRIGGER_TX,
    DMA_TRIGGER_SPI_1_RX           = 0x04 | DMA_TRIGGER_RX,
    DMA_TRIGGER_QSPI_TX            = 0x05 | DMA_TRIGGER_TX,
    DMA_TRIGGER_QSPI_RX            = 0x06 | DMA_TRIGGER_RX,
    DMA_TRIGGER_USART0_TX          = 0x07 | DMA_TRIGGER_TX,
    DMA_TRIGGER_USART0_RX          = 0x08 | DMA_TRIGGER_RX,
    DMA_TRIGGER_USART1_TX          = 0x09 | DMA_TRIGGER_TX,
    DMA_TRIGGER_USART1_RX          = 0x0A | DMA_TRIGGER_RX,
    DMA_TRIGGER_USART2_TX          = 0x0B | DMA_TRIGGER_TX,
    DMA_TRIGGER_USART2_RX          = 0x0C | DMA_TRIGGER_RX,
    DMA_TRIGGER_PWM0               = 0x0D | DMA_TRIGGER_TX,
    DMA_TRIGGER_TWIHS0_TX          = 0x0E | DMA_TRIGGER_TX,
    DMA_TRIGGER_TWIHS0_RX          = 0x0F | DMA_TRIGGER_RX,
    DMA_TRIGGER_TWIHS1_TX          = 0x10 | DMA_TRIGGER_TX,
    DMA_TRIGGER_TWIHS1_RX          = 0x11 | DMA_TRIGGER_RX,
    DMA_TRIGGER_TWIHS2_TX          = 0x12 | DMA_TRIGGER_TX,
    DMA_TRIGGER_TWIHS2_RX          = 0x13 | DMA_TRIGGER_RX,
    DMA_TRIGGER_UART0_TX           = 0x14 | DMA_TRIGGER_TX,
    DMA_TRIGGER_UART0_RX           = 0x15 | DMA_TRIGGER_RX,
    DMA_TRIGGER_UART1_TX           = 0x16 | DMA_TRIGGER_TX,
    DMA_TRIGGER_UART1_RX           = 0x17 | DMA_TRIGGER_RX,
    DMA_TRIGGER_UART2_TX           = 0x18 | DMA_TRIGGER_TX,
    DMA_TRIGGER_UART2_RX           = 0x19 | DMA_TRIGGER_RX,
    DMA_TRIGGER_UART3_TX           = 0x1A | DMA_TRIGGER_TX,
    DMA_TRIGGER_UART3_RX           = 0x1B | DMA_TRIGGER_RX,
    DMA_TRIGGER_UART4_TX           = 0x1C | DMA_TRIGGER_TX,
    DMA_TRIGGER_UART4_RX           = 0x1D | DMA_TRIGGER_RX,
    DMA_TRIGGER_DACC0              = 0x1E | DMA_TRIGGER_TX,
    DMA_TRIGGER_DACC1              = 0x1F | DMA_TRIGGER_TX,
    DMA_TRIGGER_SSC_TX             = 0x20 | DMA_TRIGGER_TX,
    DMA_TRIGGER_SSC_RX             = 0x21 | DMA_TRIGGER_RX,
    DMA_TRIGGER_PIOA               = 0x22 | DMA_TRIGGER_RX,
    DMA_TRIGGER_AFEC0              = 0x23 | DMA_TRIGGER_RX,
    DMA_TRIGGER_AFEC1              = 0x24 | DMA_TRIGGER_RX,
    DMA_TRIGGER_AES_TX             = 0x25 | DMA_TRIGGER_TX,
    DMA_TRIGGER_AES_RX             = 0x26 | DMA_TRIGGER_RX,
    DMA_TRIGGER_PWM1               = 0x27 | DMA_TRIGGER_TX,
    DMA_TRIGGER_TC0                = 0x28 | DMA_TRIGGER_RX,
    DMA_TRIGGER_TC3                = 0x29 | DMA_TRIGGER_RX,
    DMA_TRIGGER_TC6                = 0x2A | DMA_TRIGGER_RX,
    DMA_TRIGGER_TC9                = 0x2B | DMA_TRIGGER_RX,
    DMA_TRIGGER_I2SC0_TX_LEFT      = 0x2C | DMA_TRIGGER_TX,
    DMA_TRIGGER_I2SC0_RX_LEFT      = 0x2D | DMA_TRIGGER_RX,
    DMA_TRIGGER_I2SC1_TX_LEFT      = 0x2E | DMA_TRIGGER_TX,
    DMA_TRIGGER_I2SC1_RX_LEFT      = 0x2F | DMA_TRIGGER_RX,
    DMA_TRIGGER_I2SC0_TX_RIGHT     = 0x30 | DMA_TRIGGER_TX,
    DMA_TRIGGER_I2SC0_RX_RIGHT     = 0x31 | DMA_TRIGGER_RX,
    DMA_TRIGGER_I2SC1_TX_RIGHT     = 0x32 | DMA_TRIGGER_TX,
    DMA_TRIGGER_I2SC1_RX_RIGHT     = 0x33 | DMA_TRIGGER_RX

} DMA_TRIGGER_SOURCE;


// *****************************************************************************
/* DMA System Service Channel None

  Summary:
    DMA channel none

  Description:
    This constant identifies the specification of no choice from client
    for allocating a particular DMA channel.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.
*/

#define DMA_CHANNEL_NONE /*DOM-IGNORE-BEGIN*/((DMA_CHANNEL)-1)/*DOM-IGNORE-END*/


// *****************************************************************************
/* DMA System Service Channel Any

  Summary:
    Identifies the client specification to allocate any available DMA channel.

  Description:
    This constant identifies the specification by the client
    to allocate any available DMA channel.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.
*/

#define DMA_CHANNEL_ANY /*DOM-IGNORE-BEGIN*/((DMA_CHANNEL)-2)/*DOM-IGNORE-END*/

// *****************************************************************************
/* DMA System Service self trigger mode

  Summary:
    Identifies the client specification to use the memory to memory transfer.

  Description:
    This constant identifies the specification by the client
    to use memory to memory transfer.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.
*/

#define DMA_TRIGGER_SOURCE_NONE /*DOM-IGNORE-BEGIN*/((DMA_TRIGGER_SOURCE)-2)/*DOM-IGNORE-END*/

// *****************************************************************************
/* DMA System Service Channel Count

  Summary:
    Identifies the maximum number of DMA channel handles to be defined.

  Description:
    This constant identifies the maximum number of DMA channel handles
    that should be defined.

  Remarks:
    This value is derived from device-specific header files defined as
    part of the peripheral libraries.
*/
#define SYS_DMA_CHANNEL_COUNT /*DOM-IGNORE-BEGIN*/DMA_CHANNELS_NUMBER/*DOM-IGNORE-END*/

// *****************************************************************************
/* DMA System Service Channel Handle

  Summary:
    Assigns the handle for requested channel.

  Description:
    A channel handle value is returned by a call to the SYS_DMA_ChannelAllocate
    function. This handle is associated with the channel number that is used for
    data transfer and it allows the application to track the data transfer.

    The channel handle once assigned to a client expires when the
    client calls SYS_DMA_ChannelRelease.
    exits.

  Remarks:
    None.
*/

typedef uintptr_t SYS_DMA_CHANNEL_HANDLE;


// *****************************************************************************
/* DMA System Service Invalid Channel Handle

  Summary:
    Defines an invalid channel handle.

  Description:
    This is the definition of an invalid channel handle. An invalid buffer handle
    is returned by SYS_DMA_ChannelAllocate function if the channel number
    request was not successful.

  Remarks:
    None.
*/

#define SYS_DMA_CHANNEL_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((SYS_DMA_CHANNEL_HANDLE)(-1))/*DOM-IGNORE-END*/


// *****************************************************************************
/* DMA stop in idle mode

  Summary:
    Enable/Disable DMA operations in Idle mode.

  Description:
    This data type allows enabling/disabling of DMA operations in idle mode.

  Remarks:
    The field 'sidl' (in init data structure 'SYS_DMA_INIT') is enable/disable
    while initializing the DMA module by calling SYS_DMA_Initialize.

    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/
typedef enum
{
    /* Fature is not supported */
    SYS_DMA_SIDL_NONE

} SYS_DMA_SIDL;


// *****************************************************************************
/* DMA Channel chaining priority

  Summary:
    Identifies the priority of chaining channel.

  Description:
    This data type identifies the priority of chaining channel.

    Channel chaining priority identifies the channel that will be enabled
    on completion of block data transfer on the master channel.

  Remarks:
    None.
*/
typedef enum
{
    /* Feature is not supported */
    SYS_DMA_CHANNEL_CHAIN_PRIO_NONE

} SYS_DMA_CHANNEL_CHAIN_PRIO;


// *****************************************************************************
/* DMA Channel ignore pattern match

  Summary:
    Enables/Disables the feature to ignore a byte between a 2-byte match abort
    pattern.

  Description:
    This data type allows enabling/disabling the feature to ignore a byte
    between a 2-byte match abort pattern.

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/
typedef enum
{
    /* Channel chaining is not supported by the device */
    SYS_DMA_CHANNEL_IGNORE_MATCH_NONE
} SYS_DMA_CHANNEL_IGNORE_MATCH;


// *****************************************************************************
/* DMA channel CRC mode

  Summary:
    Identifies the operation mode of the CRC module.

  Description:
    This data type identifies CRC operating mode. The CRC computation can be
    in background mode or append mode.

  Remarks:
    None.
*/
typedef enum
{
    /* Feature is not supported. */
    SYS_DMA_CHANNEL_CRC_MODE_NONE

} SYS_DMA_CHANNEL_CRC_MODE;


// *****************************************************************************
/* DMA channel CRC write order

  Summary:
    Identifies altering/maintaining write order post CRC computation.

  Description:
    This data type identifies write byte order selection post CRC
    computation.

  Remarks:
    None.
*/
typedef enum
{
    /* Feature is not supported */
    SYS_DMA_CRC_WRITE_ORDER_NONE

} SYS_DMA_CRC_WRITE_ORDER;


// *****************************************************************************
/* DMA channel operation modes

  Summary:
    Identifies the available DMA operating modes.

  Description:
    This data type Identifies the available DMA operating modes.

    The supported DMA modes are not mutually exclusive, but can be simultaneously
    operational.

  Remarks:
    These supported DMA modes can be logically OR'd together. They are passed
    into the SYS_DMA_ChannelSetup function through the modeEnable parameter.
*/
typedef enum
{
    /* Basic DMA operation mode. 
       The DMA channel will transfer a block of data from source to 
       destination without CPU intervention.
       Use SYS_DMA_ChannelTransferAdd function to add a transfer. */
    SYS_DMA_CHANNEL_OP_MODE_BASIC,

    /* Auto enable operating mode allows a channel to be kept active, even if a
       block transfer completes. This prevents the user
       from having to re-enable the channel each time a block transfer completes. */
    SYS_DMA_CHANNEL_OP_MODE_AUTO,

    /* Linked list DMA operation mode. 
       The DMA channel will transfer multiple blocks of data as configured 
       by the descriptors of the linked list without CPU intervention. 
       Use SYS_DMA_ChannelLinkedListAdd function to add a linked list. */
    SYS_DMA_CHANNEL_OP_MODE_LINKED_LIST

} SYS_DMA_CHANNEL_OP_MODE;


// *****************************************************************************
/* DMA channel CRC mode

  Summary:
    Defines the attributes for CRC operation mode.

  Description:
    This data type defines the attributes for CRC operation mode.

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/
typedef enum
{
    SYS_DMA_CHANNEL_OPERATION_MODE_CRC_NONE

} SYS_DMA_CHANNEL_OPERATION_MODE_CRC;

// *****************************************************************************
/* DMA System service Events

   Summary:
    Enumeration of possible DMA System service events.

   Description:
    This data type provides an enumeration of all possible DMA System service events.

   Remarks:
    None.

*/
typedef enum
{
    /* Data was transferred successfully. */
    SYS_DMA_TRANSFER_EVENT_COMPLETE,

    /* Error while processing the request */
    SYS_DMA_TRANSFER_EVENT_ERROR,

    /* Data transfer was aborted. */
    SYS_DMA_TRANSFER_EVENT_ABORT

} SYS_DMA_TRANSFER_EVENT;

// *****************************************************************************
/* DMA System service Error

   Summary:
    Indicates the error information for the DMA System service.

   Description:
    This enumeration indicates the error information for the DMA System service.

   Remarks:
    None.

*/
typedef enum
{
    /* Data was transferred successfully. */
    SYS_DMA_ERROR_NONE /*DOM-IGNORE-BEGIN*/ = 1 << 0, /* DOM-IGNORE-END*/
    
    /* DMA Read/Write bus error. */
    SYS_DMA_ERROR_BUS_ERROR /*DOM-IGNORE-BEGIN*/ = 1 << 1, /* DOM-IGNORE-END*/
    
    /* DMA Read/Write bus error. */
    SYS_DMA_ERROR_REQUEST_OVERFLOW_ERROR /*DOM-IGNORE-BEGIN*/ = 1 << 2 /* DOM-IGNORE-END*/

} SYS_DMA_ERROR;

// *****************************************************************************
/* DMA Initialization data

  Summary:
    Defines the data required to initialize the DMA subsystem.

  Description:
    This data type defines the data required to initialize the DMA subsystem.

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/
typedef struct
{
    /* Number of channels selected */
    uint32_t       channelCount;

} SYS_DMA_INIT;


// *****************************************************************************
typedef enum {

    DMA_PATTERN_MATCH_LENGTH_NONE

} DMA_PATTERN_LENGTH;



// *****************************************************************************
/* Data width options */
typedef enum {

    SYS_DMA_DATA_WIDTH_BYTE = 1,
    SYS_DMA_DATA_WIDTH_HALF_WORD = 2,
    SYS_DMA_DATA_WIDTH_WORD = 4

} SYS_DMA_DATA_WIDTH;

// *****************************************************************************
/* DMA descriptor control

  Summary:
    Defines the descriptor control for linked list operation.

  Description:
    This data type defines the descriptor control for linked list operation.

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

typedef union {
    
    struct 
    {
        /* Size of block for the current descriptor. 
           This variable is not needed for the first descriptor control 
           parameter of SYS_DMA_ChannelLinkedListSetup because block length
           for first descriptor is configured by the first descriptor itself. */
        uint32_t blockDataLength:24;
        
        /* Next descriptor fetch enable. 
           Zero in this field indicates the end of linked list. */
        uint32_t nextDescriptorFetchEnable:1;
        
        /* Enable/Disable source parameters update when the descriptor 
           is retrieved*/
        uint32_t nextDescriptorSrcUpdate:1;
        
        /* Enable/Disable destination parameters update when the descriptor 
           is retrieved*/
        uint32_t nextDescriptorDestUpdate:1;
        
        /* Next descriptor view type.
           Views can be changed when switching descriptors. */
        uint32_t nextDescriptorView:2;
        
        /* Reserved */
        uint32_t :3;
    } mbr_ubc_type;

    uint32_t mbr_ubc;
  
} SYS_DMA_DESCRIPTOR_CONTROL;

// *****************************************************************************
/* DMA descriptor views

  Summary:
    Defines the different descriptor views available for master transfer.

  Description:
    This data type defines the different descriptor views available.

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

/* View 0 */
typedef struct {
	
    /* Next Descriptor Address number. */
	uint32_t mbr_nda;
    
	/* Microblock Control Member. */
	SYS_DMA_DESCRIPTOR_CONTROL mbr_ubc;
	
    /* Destination Address Member. */
	uint32_t mbr_da;
    
} SYS_DMA_DESCRIPTOR_VIEW_0;

/* View 1 */
typedef struct {
	
    /* Next Descriptor Address number. */
	uint32_t mbr_nda;
	
    /* Microblock Control Member. */
	SYS_DMA_DESCRIPTOR_CONTROL mbr_ubc;
	
    /* Source Address Member. */
	uint32_t mbr_sa;
	
    /* Destination Address Member. */
	uint32_t mbr_da;
    
} SYS_DMA_DESCRIPTOR_VIEW_1;

/* View 2 */
typedef struct {
	
    /* Next Descriptor Address number. */
	uint32_t mbr_nda;
	
    /* Microblock Control Member. */
	SYS_DMA_DESCRIPTOR_CONTROL mbr_ubc;
	
    /* Source Address Member. */
	uint32_t mbr_sa;
	
    /* Destination Address Member. */
	uint32_t mbr_da;
	
    /* Configuration Register. */
    /* TODO: Redefine type to XDMAC_CC white updating to N type */
	uint32_t mbr_cfg;
    
} SYS_DMA_DESCRIPTOR_VIEW_2;

/* View 3 */
typedef struct {
	
    /* Next Descriptor Address number. */
	uint32_t mbr_nda;
	
    /* Microblock Control Member. */
	SYS_DMA_DESCRIPTOR_CONTROL mbr_ubc;
	
    /* Source Address Member. */
	uint32_t mbr_sa;
	
    /* Destination Address Member. */
	uint32_t mbr_da;
	
    /* Configuration Register. */
    /* TODO: Redefine type to XDMAC_CC while updating to N type */
	uint32_t mbr_cfg;
	
    /* Block Control Member. */
    /* TODO: Redefine type to XDMAC_BC while updating to N type */
	uint32_t mbr_bc;
	
    /* Data Stride Member. */
    /* TODO: Redefine type to XDMAC_DS while updating to N type */
	uint32_t mbr_ds;
	
    /* Source Microblock Stride Member. */
    /* TODO: Redefine type to XDMAC_CSUS while updating to N type */
	uint32_t mbr_sus;
	
    /* Destination Microblock Stride Member. */
    /* TODO: Redefine type to XDMAC_CDUS while updating to N type */
	uint32_t mbr_dus;
    
} SYS_DMA_DESCRIPTOR_VIEW_3;


#endif // #ifndef _SYS_DMA_DEFINITIONS_PIC32C_H
/*******************************************************************************
 End of File
*/