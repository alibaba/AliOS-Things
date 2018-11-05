/*******************************************************************************
  DMA System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma_local_pic32c.h

  Summary:
    DMA System Service Local Data Structures

  Description:
    System Service Local Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _SYS_DMA_LOCAL_PIC32C_H
#define _SYS_DMA_LOCAL_PIC32C_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/int/sys_int.h"
#include "system/dma/sys_dma.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

/* Minimum and maximum of two values */
#define MIN_SIZE(X, Y) (((X) <= (Y)) ? (X) : (Y))
#define MAX_SIZE(X, Y) (((X) >= (Y)) ? (X) : (Y))

/* PERID will be in the lower 2 bytes of DMA_TRIGGER_SOURCE enum value */
#define SYS_DMA_PERID_MASK    0x00FF

/* Micro-block bit-width */
#define SYS_DMA_UBLEN_BIT_WIDTH     24

// *****************************************************************************
typedef struct
{
    /* DMA channel associated with this channel object */
    DMA_CHANNEL channelID;
    
    /* DMA channel trigger source */
    DMA_TRIGGER_SOURCE channelTrigger;
    
    /* Indicates whether the channel object is in use or not */
    bool inUse;

    /* Inidcates the error information for
       the last DMA operation on this channel */
    SYS_DMA_ERROR errorInfo;
    
    /* Call back function for this DMA channel */
    SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER  pEventCallBack;

    /* Client data(Event Context) that will be returned at callback */
    uintptr_t hClientArg;

} SYS_DMA_CHANNEL_OBJECT;


// *****************************************************************************
// *****************************************************************************
// Section: Local functions.
// *****************************************************************************
// *****************************************************************************

void _SYS_DMA_PeripheralSyncChannelSetup( SYS_DMA_CHANNEL_OBJECT *chanObj );
void _SYS_DMA_PeripheralSyncTransferSetup( SYS_DMA_CHANNEL_OBJECT *chanObj, size_t srcSize, size_t destSize );
void _SYS_DMA_ChannelTasks( DMA_CHANNEL channelID );
void _SYS_DMA_ChannelEnable( DMA_CHANNEL channelID );


#endif
/*******************************************************************************
 End of File
*/

