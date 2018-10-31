/*******************************************************************************
  DMA System Service Library Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma_definitions.h

  Summary:
    DMA System Service data type definitions header.

  Description:
    This file contains data type definitions header.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_DMA_DEFINITIONS_H
#define _SYS_DMA_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"
#include "peripheral/dma/plib_dma.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

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
//DOM-IGNORE-BEGIN
#define DMA_CHANNEL_NONE                    ((DMA_CHANNEL)-1)
//DOM-IGNORE-END



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

//DOM-IGNORE-BEGIN
#define DMA_CHANNEL_ANY                    ((DMA_CHANNEL)-2)
//DOM-IGNORE-END

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
#define SYS_DMA_CHANNEL_COUNT     DMA_NUMBER_OF_CHANNELS

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

#define SYS_DMA_CHANNEL_HANDLE_INVALID ((SYS_DMA_CHANNEL_HANDLE)(-1))


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
    /* When the CPU enters idle mode, the DMA module continue operations */
    SYS_DMA_SIDL_DISABLE,

    /* When the CPU enters idle mode, the DMA module will discontinue operations */
    SYS_DMA_SIDL_ENABLE

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
    /* Chain to channel higher in natural priority */
    SYS_DMA_CHANNEL_CHAIN_PRIO_HIGH,

    /* Chain to channel lower in natural priority */
    SYS_DMA_CHANNEL_CHAIN_PRIO_LOW

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
    /* Disabling the feature to ignore a byte between a 2-byte match abort pattern. */
    SYS_DMA_CHANNEL_IGNORE_MATCH_DISABLE,

    /* Enable the feature to ignore a byte between a 2-byte match abort pattern. */
    SYS_DMA_CHANNEL_IGNORE_MATCH_ENABLE

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
    /* DMA reads the data from the source, passes it through the CRC module and
       writes it to the destination. the calculated CRC is left in the DCRCDATA
       register at the end of the block transfer. */
    SYS_DMA_CHANNEL_CRC_MODE_BACKGROUND,

    /* DMA only feeds source data to the CRC module. it does not write source data
       to the destination address. When a block transfer complete or pattern abort
       event occur, The DMA writes the CRC value to the destination address */
    SYS_DMA_CHANNEL_CRC_MODE_APPEND

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
    /* Source data is written to the destination unchanged regardless
       of the selected pre-CRC byte order. */
    SYS_DMA_CRC_WRITE_ORDER_MAINTAIN,

    /* Source data is written to the destination reordered according
       to the selected pre-CRC byte order. */
    SYS_DMA_CRC_WRITE_ORDER_CHANGE

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
    /* The normal DMA operation mode. The DMA channel will transfer data from
       a source to a destination without CPU intervention */
    SYS_DMA_CHANNEL_OP_MODE_BASIC       /*DOM-IGNORE-BEGIN*/ = (1<<0)  /*DOM-IGNORE-END*/,

    /* Pattern Match abort mode allows the user to end a transfer if data
       written during a transaction matches a specific pattern, as defined by
       the DCHxDAT register */
    SYS_DMA_CHANNEL_OP_MODE_MATCH_ABORT /*DOM-IGNORE-BEGIN*/ = (1<<1)  /*DOM-IGNORE-END*/,

    /* Channel chaining operating mode enhances the normal DMA channel
       operations. A channel(slave) can be chained to an adjacent channel(master).
       The slave channel will be enabled when a block transfer of the master
       channel completes. */
    /* only one of the below two chaining priorities must be provided.
       Chaining priority High. */
    SYS_DMA_CHANNEL_OP_MODE_CHAIN_HIGH /*DOM-IGNORE-BEGIN*/ = (1<<2)  /*DOM-IGNORE-END*/,
    /* Chaining priority Low */
    SYS_DMA_CHANNEL_OP_MODE_CHAIN_LOW  /*DOM-IGNORE-BEGIN*/ = (1<<3)  /*DOM-IGNORE-END*/,

    /* Auto enable operating mode allows a channel to be kept active, even if a
       block transfer completes or pattern match occurs. This prevents the user
       from having to re-enable the channel each time a block transfer completes. */
    SYS_DMA_CHANNEL_OP_MODE_AUTO        /*DOM-IGNORE-BEGIN*/ = (1<<4)  /*DOM-IGNORE-END*/,

    /* CRC operating mode allows computation of 16, 32-bit CRC. */
    SYS_DMA_CHANNEL_OP_MODE_CRC         /*DOM-IGNORE-BEGIN*/ = (1<<5)  /*DOM-IGNORE-END*/

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
typedef struct
{
    /* CRC type (PLIB-level).
       Type Selection identifies whether the CRC module will calculate an IP
       header checksum or an LFSR CRC */
    DMA_CRC_TYPE type;

    /* CRC mode, Background or Append */
    SYS_DMA_CHANNEL_CRC_MODE mode;

    /* Polynomial length, This value is redundant when the selected CRC type is
       IP header checksum. When the CRC type is LFSR, Identifies the length of
       the polynomial. */
    uint8_t polyLength;

    /* Bit order (PLIB-level).
       Identifies whether the CRC is computed LSb or MSb first */
    DMA_CRC_BIT_ORDER bitOrder;

    /* Byte order (PLIB-level).
       Identifies the byte selection order input pre-CRC Generator. */
    DMA_CRC_BYTE_ORDER byteOrder;

    /* Post CRC Write order */
    SYS_DMA_CRC_WRITE_ORDER writeOrder;

    /* CRC data feed: While enabling the CRC mode, this field gives
       the seed for the CRC computation. On block transfer complete or
       pattern match abort the field have the computed CRC. */
    uint32_t data;

    /* XOR bit mask, This value is redundant when the selected CRC type is
       IP header checksum. When the CRC type is LFSR, Identifies the XOR
       bit mask input to the shift register during CRC computation. */
    uint32_t xorBitMask;

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

    /* DMA address error. */
    SYS_DMA_ERROR_ADDRESS_ERROR /*DOM-IGNORE-BEGIN*/ = 1 << 1 /* DOM-IGNORE-END*/

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
    /* Enable/Disable stop in idle mode feature */
    SYS_DMA_SIDL                    sidl;

} SYS_DMA_INIT;


// *****************************************************************************
/* Data width options */
typedef enum {

    /* Data width configuration feature is not available */
    SYS_DMA_DATA_WIDTH_NONE

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

    /* Feature is not supported */
    ;
  
} SYS_DMA_DESCRIPTOR_CONTROL;


#endif // #ifndef _SYS_DMA_DEFINITIONS_H
/*******************************************************************************
 End of File
*/