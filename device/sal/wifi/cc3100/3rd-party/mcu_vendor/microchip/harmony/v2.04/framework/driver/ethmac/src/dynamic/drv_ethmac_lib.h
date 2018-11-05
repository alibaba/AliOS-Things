/*******************************************************************************
  Ethernet Driver Library Definition

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_etchmac_lib.h

  Summary:
    This file contains the Application Program Interface (API) definition  for
    the Ethernet Driver library.

  Description:
    This library provides a low-level abstraction of the Ethernet module
    on Microchip PIC32MX family microcontrollers with a convenient C language
    interface.  It can be used to simplify low-level access to the module
    without the necessity of interacting directly with the module's registers,
    thus hiding differences from one microcontroller variant to another.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2008-2009 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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


#ifndef _DRV_ETHMAC_LIB_H_
#define _DRV_ETHMAC_LIB_H_

#include <stdlib.h>
#include <sys/kmem.h>
#include <xc.h>

#include "system_config.h"
#include "system/debug/sys_debug.h"
#include "tcpip/tcpip_ethernet.h"
#include "peripheral/eth/plib_eth.h"
#include "driver/ethmac/src/dynamic/_eth_dcpt_lists.h"
#include "driver/ethmac/src/drv_ethmac_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************



/**************************************************************************
  Summary:
    Defines the possible results of Ethernet operations that can succeed or
    fail
  Description:
    Ethernet Operation Result Codes
    
    This enumeration defines the possible results of any of the Ethernet
    library operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.                                                                
**************************************************************************/

typedef enum
{
    /* Everything ok */
    DRV_ETHMAC_RES_OK                     /*DOM-IGNORE-BEGIN*/ =  0 /*DOM-IGNORE-END*/,

    /* Ethernet RX, TX, acknowledge packet codes */

    /* No such packet exist */
    DRV_ETHMAC_RES_NO_PACKET,

    /* Packet is queued (not transmitted or received and not processed) */
    DRV_ETHMAC_RES_PACKET_QUEUED,

    /* Not enough descriptors available */
    DRV_ETHMAC_RES_NO_DESCRIPTORS,
    
    /* Errors: Ethernet buffers, descriptors */

    /* Some memory allocation failed */
    DRV_ETHMAC_RES_OUT_OF_MEMORY          /*DOM-IGNORE-BEGIN*/ =  -1 /*DOM-IGNORE-END*/,

    /* We don't support user space buffers. */
    DRV_ETHMAC_RES_USPACE_ERR             /*DOM-IGNORE-BEGIN*/ =  -2 /*DOM-IGNORE-END*/,

    /* The size of the receive buffers too small */
    DRV_ETHMAC_RES_RX_SIZE_ERR            /*DOM-IGNORE-BEGIN*/ =  -3 /*DOM-IGNORE-END*/,

    /* A received packet spans more buffers/descriptors than supplied */
    DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR       /*DOM-IGNORE-BEGIN*/ =  -4 /*DOM-IGNORE-END*/,

} DRV_ETHMAC_RESULT; 






// Section: Descriptors and Buffers Functions
// *****************************************************************************
/* Ethernet Descriptor Allocation Function Pointer

  Function:
    void* <FunctionName> ( size_t nitems, size_t size, void* param )

  Summary:
    Pointer to a function that allocates the requested memory and returns a
    pointer to it.

  Description:
    Pointer to a function that allocates enough memory for "nitems" each of
    "size" bytes in length (total memory requested is nitems*size).
    The allocated space should be initialized to zero.

  Precondition:
    None

  Parameters:
    nitems      - number of items to be allocated
    size        - size of each item, in bytes
    param       - extra parameter that will be used by the function call

  Returns:
    On success  - A pointer to the requested space.
    On failure  = A null pointer.

  Remarks:
    This function will be invoked for allocation of descriptors only.
    Therefore, the size parameter will be the one returned by
    DRV_ETHMAC_LibDescriptorsGetSize(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
*/

typedef void* ( *DRV_ETHMAC_DCPT_AllocF )( size_t nitems, size_t size, void* param );


// *****************************************************************************
/* Ethernet Descriptor Free Function

  Function:
    void <FunctionName> ( void* ptr, void* param )

  Summary:
    Pointer to a function that deallocates the memory previously allocated by a
    DRV_ETHMAC_DCPT_AllocF function.

  Description:
    Pointer to a function that deallocates the memory previously allocated by a
    DRV_ETHMAC_DCPT_AllocF function.

  Precondition:
    The DRV_ETHMAC_DCPT_AllocF would have been previously called.

  Parameters:
    ptr         - Points to the space that was previously allocated with a
                  DRV_ETHMAC_DCPT_AllocF function.
    param       - extra parameter that will be used by the function call

  Returns:
    None.

  Remarks:
    None.
*/

typedef void ( *DRV_ETHMAC_DCPT_FreeF ) ( void* ptr, void* param );


// *****************************************************************************
/* Ethernet Descriptor Types

  Summary:
    Enumeration defining the Ethernet descriptor types.

  Description:
    This enumeration defines the Ethernet descriptor types.

  Remarks:
    Most descriptor operations support just one type, but multiple flags can be
    set.
*/

typedef enum
{
    // TX descriptor
    DRV_ETHMAC_DCPT_TYPE_RX    /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // RX descriptor
    DRV_ETHMAC_DCPT_TYPE_TX    /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // All (both) types.  Some descriptor operations support multiple types.
    DRV_ETHMAC_DCPT_TYPE_ALL   /*DOM-IGNORE-BEGIN*/ = (DRV_ETHMAC_DCPT_TYPE_RX|DRV_ETHMAC_DCPT_TYPE_TX) /*DOM-IGNORE-END*/

} DRV_ETHMAC_DCPT_TYPE;      // descriptor types


// *****************************************************************************
/*  Buffer Flags

  Summary:
    Flags for buffer attributes.

  Description:
    Some buffers can have special attributes, as defined by this enumeration.
*/

typedef enum
{
    // The receive buffer is sticky, going to be used across multiple RX
    // operations once it's acknowledged
    DRV_ETHMAC_BUFF_FLAG_RX_STICKY /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // The receive descriptor associated to this buffer should not be
    // automatically acknowledged when DRV_ETHMAC_LibRxAcknowledgeBuffer/DRV_ETHMAC_LibRxAcknowledgePacket
    // is called.
    // User is responsible for specifically calling DRV_ETHMAC_LibAckRxDcpt(DRV_ETHMAC_INSTANCE_DCPT* pMacD) to acknowledge
    // the RX descriptor
    DRV_ETHMAC_BUFF_FLAG_RX_UNACK /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

} DRV_ETHMAC_BUFF_FLAGS;



// *****************************************************************************
/* Ethernet Close Flags

  Summary:
    Defines the possible disable codes of Ethernet controller "DRV_ETHMAC_LibClose" call.

  Description:
    This enumeration defines the close capabilities of the Ethernet module.
*/

typedef enum
{
    /* Wait for the current TX/RX operation to finish */
    DRV_ETHMAC_CLOSE_GRACEFUL  /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Default close options
    DRV_ETHMAC_CLOSE_DEFAULT /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

} DRV_ETHMAC_CLOSE_FLAGS; 




// *****************************************************************************
// *****************************************************************************
// Section: Ethernet Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

// Section:  Open and Configuration Functions

/*******************************************************************************
  Function:
      void DRV_ETHMAC_LibInit (DRV_ETHMAC_INSTANCE_DCPT* pMacD)

  Summary:
    Ethernet initialization routine
  Description:
    This function performs the initialization of the Ethernet Controller.

  Precondition:
    None.

  Parameters:
    index -  Identifier for the device instance.

  Returns:
    None
  Example:
    <code>
    DRV_ETHMAC_LibInit(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
    </code>
  Remarks:
    This function should be called before the DRV_ETHMAC_LibMACOpen()
    routine.

    Replaces:

    <b><c>void EthInit(void)</c></b>
  ************************************************************************/

void DRV_ETHMAC_LibInit (DRV_ETHMAC_INSTANCE_DCPT* pMacD);


/*******************************************************************************
  Function:
    void DRV_ETHMAC_LibMACOpen(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType )

  Summary:
    Initializes and configures the Ethernet MAC.

  Description:
    This function performs the open and configuration of the Ethernet MAC.  It needs
    to be called after the DRV_ETHMAC_LibInit(DRV_ETHMAC_INSTANCE_DCPT* pMacD) was called and the PHY initialized.


  Precondition:
    DRV_ETHMAC_LibInit must have been called.

  Parameters:
    pMacD       - driver instance.
    oFlags      - Open flags
    pauseType   - Pause type to be supported by the MAC


  Returns:
    Ethernet operation result code.

  Example:
    <code>
    // Enable loopback at the MAC level
    oFlags    = TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_HUGE_PKTS|TCPIP_ETH_OPEN_MAC_LOOPBACK;
    // Use both TX and RX pause type for Full Duplex
    pauseType = TCPIP_MAC_PAUSE_ALL;

    // Start the initialization sequence
    DRV_ETHMAC_LibInit(DRV_ETHMAC_INSTANCE_DCPT* pMacD);

    // Open and configure the MAC.
    //
    // Since we're not connected to an external PHY there's no negotiation
    // going on and we know all our opening flags
    DRV_ETHMAC_LibMACOpen(DRV_ETHMAC_INSTANCE_DCPT* pMacD, oFlags, pauseType);
    </code>

  Remarks:
    If no auto-negotiation is performed, the DRV_ETHMAC_LibMACOpen() could be called
    immediately after DRV_ETHMAC_LibInit(DRV_ETHMAC_INSTANCE_DCPT* pMacD) and PHY initialization.  Otherwise the open
    flags and the pause type should be the ones obtained as a result of the
    PHY negotiation procedure!
    <p>Replaces:<p><c><b>void EthMACOpen(TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType)</b></c>
 *****************************************************************************/

void DRV_ETHMAC_LibMACOpen (DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_ETH_OPEN_FLAGS oFlags, TCPIP_ETH_PAUSE_TYPE pauseType );


/*******************************************************************************
  Function:
    void DRV_ETHMAC_LibClose(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_CLOSE_FLAGS cFlags )

  Summary:
    Aborts any transactions and disables the Ethernet controller

  Description:
    This function performs an abort of any current Ethernet transactions, disables
    the Ethernet controller and interrupts.  It should be called after the PHY
    close/reset procedure.

  Precondition:
    None.

  Parameters:
    pMacD       - driver instance.
    cFlags - closing flags

  Returns:
    None.

  Example:
    <code>
    DRV_ETHMAC_LibClose(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_CLOSE_DEFAULT);
    </code>

  Remarks:
    It does not free any allocated memory.
    <p>Replaces:<p><c><b>void EthClose(DRV_ETHMAC_CLOSE_FLAGS cFlags)</b></c>
 *****************************************************************************/

void DRV_ETHMAC_LibClose (DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_CLOSE_FLAGS cFlags );


// *****************************************************************************
// *****************************************************************************
// Section: Descriptors add and remove functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    int DRV_ETHMAC_LibDescriptorsPoolAdd (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType,
                                              DRV_ETHMAC_DCPT_AllocF fAlloc, void* fParam )

  Summary:
    Adds transmit & receive descriptors to the pool.

  Description:
    This function adds the specified number of transmit/receive descriptors to
    the pool of descriptors.  These descriptors are used whenever a new
    transmission or a reception is initiated.  The needed memory per descriptor
    is returned by DRV_ETHMAC_LibDescriptorsGetSize.   Descriptors can be freed with
    DRV_ETHMAC_LibDescriptorsPoolRemove.

  Precondition:
    DRV_ETHMAC_LibInit should have been called.

  Parameters:
    pMacD           - driver instance.
    nDescriptors    - Number of descriptors to be added

    dType           - TX/RX descriptors requested

    fAlloc          - Function to be called to allocate the needed memory

    fParam          - parameter to be used when fAlloc is called

  Returns:
    The number of descriptors successfully created.

  Example:
    <code>
    if( DRV_ETHMAC_LibDescriptorsPoolAdd(DRV_ETHMAC_INSTANCE_DCPT* pMacD, txDcptNo, DRV_ETHMAC_DCPT_TYPE_TX, fMemAlloc, &myInstanceData) != txDcptNo )
    {
        // Handle allocation error
    }
    </code>

  Remarks:
    The number of created descriptors is different from the number of
    requested ones only if the memory allocation failed.

    The first time the specific pool is initialized an extra descriptor is added to the pool
    so that the actual memory requested will be (nDescriptors+1)*EthDescriptorsGetSize().

    fAlloc should not be NULL.

    The complete clean-up is done by DRV_ETHMAC_LibDescriptorsPoolCleanUp();

    DRV_ETHMAC_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.
    <p>Replaces:<p><c><b>int EthDescriptorsPoolAdd ( int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_AllocF fAlloc, void* fParam )</b></c>
 *****************************************************************************/

int DRV_ETHMAC_LibDescriptorsPoolAdd (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_AllocF fAlloc, void* fParam );


/*******************************************************************************
  Function:
    int DRV_ETHMAC_LibDescriptorsPoolRemove (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType,
                               DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )

  Summary:
    Tries to remove the specified number of descriptors from the pool.

  Description:
    This function tries to remove the specified number of descriptors from the
    pool of transmit/receive descriptors.  These descriptors that are NOT used
    at the time of call can be freed (i.e. the TX descriptors not queued for a
    transmission or the not busy RX descriptors).

  Precondition:
    DRV_ETHMAC_LibInit should have been called.

  Parameters:
    pMacD           - driver instance.
    nDescriptors    - Number of descriptors to be removed
    dType           - TX/RX descriptors requested
    fFree           - Function to release the the memory
    fParam          - parameter to be used when fFree is called


  Returns:
    The number of descriptors removed.

  Example:
    <code>
    if ( DRV_ETHMAC_LibDescriptorsPoolRemove(pMacD, txDcptNo, DRV_ETHMAC_DCPT_TYPE_TX, ffree, 0) != txDcptNo)
    {
        // Handle de-allocation error.
    }
    </code>

  Remarks:
    If fFree is NULL then the descriptors are removed from the specified pool but no de-allocation
    function is called.
    The allocation/deallocation memory functions should be consistent.

    The complete clean-up is done by DRV_ETHMAC_LibDescriptorsPoolCleanUp.

    DRV_ETHMAC_DCPT_TYPE_ALL use is invalid. A proper descriptor type (RX or TX) has
    to be used.
    <p>Replaces:<p><c><b>int EthDescriptorsPoolRemove ( int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )</b></c>
 *****************************************************************************/

int DRV_ETHMAC_LibDescriptorsPoolRemove (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  int nDescriptors, DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam );


/*******************************************************************************
  Function:
    void DRV_ETHMAC_LibDescriptorsPoolCleanUp (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )

  Summary:
    Removes all the specified descriptors from the pool.

  Description:
    This function performs descriptors clean-up and removes all the specified
    descriptors from the pool of transmit/receive descriptors.

  Precondition:
    DRV_ETHMAC_LibClose should have been called.

  Parameters:
    pMacD        - driver instance.
    dType        - TX/RX/All descriptors requested
    fFree        - Function to release the the memory
    fParam       - parameter to be used when fFree is called

  Returns:
    None.

  Example:
    <code>
    DRV_ETHMAC_LibDescriptorsPoolCleanUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD, DRV_ETHMAC_DCPT_TYPE_ALL, fFree, &myInstanceData);
    </code>

  Remarks:
    If fFree is NULL then the descriptors are removed from the pools but no de-allocation
    function is called.
    The allocation/deallocation memory functions should be consistent.

    It does not free the memory allocated for the application supplied "sticky"
    buffers.  It is the calling application responsibility.

    DRV_ETHMAC_LibClose should have been called or no TX/RX activity should be enabled at
    the time this function is called.

    If DRV_ETHMAC_DCPT_TYPE_ALL is supplied the function will perform the clean-up of
    both types of descriptors.
    <p>Replaces:<p><c><b>void EthDescriptorsPoolCleanUp ( DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam )</b></c>
 *****************************************************************************/

void DRV_ETHMAC_LibDescriptorsPoolCleanUp (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_DCPT_TYPE dType, DRV_ETHMAC_DCPT_FreeF fFree, void* fParam );


/*******************************************************************************
  Function:
      void* DRV_ETHMAC_LibDescriptorGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *pDcpt )

  Summary:
    Returns the associated descriptor buffer.

  Description:
    This function returns the buffer currently associated with the specified descriptor.

  Precondition:

  Parameters:
    pMacD       - driver instance.
    pDcpt -  Pointer to a Ethernet Descriptor


  Returns:
    The address of the buffer associated with this descriptor or NULL if no such descriptor/buffer
    exists.

  Example:
    <code>

    DRV_ETHMAC_LibDescriptorsPoolRemove (pMacD,  1, DRV_ETHMAC_DCPT_TYPE_RX, myRxFreeFnc, 0 );

    void myRxFreeFnc( void* ptr, void* param)
    {
        void* pBuff = DRV_ETHMAC_LibDescriptorGetBuffer(pMacD, ptr);
        if(pBuff)
        {
            // De-allocate the associated buffer
        }
    }
    </code>
  Remarks:
    The function is a helper that returns the buffer associated with a descriptor.
    It can be used to perform the queued RX/TX buffers clean-up since this list is
    maintained by the Ethernet library anyway.

    The pointer for a descriptor is returned by one of the
    DRV_ETHMAC_LibDescriptorsPoolRemove() or DRV_ETHMAC_LibDescriptorsPoolCleanUp() calls as the
    first parameter of the fFree function.

    No protection against interrupts, multithreading, etc.
    <p>Replaces:<p><c><b>void* EthDescriptorGetBuffer ( void *pDcpt )</b></c>
 *****************************************************************************/

void*  DRV_ETHMAC_LibDescriptorGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *pDcpt );


// *****************************************************************************
// *****************************************************************************
// Section: buffers added for the RX process 
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxBuffersAppend (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *ppBuff[], int nBuffs, DRV_ETHMAC_BUFF_FLAGS rxFlags )

  Summary:
    Supplies buffers to the receiving process and enables the receiving part of
    the controller.

  Description:
    This function supplies buffers to the receiving process and enables the
    receiving part of the controller.  As soon as the controller starts
    receiving data packets these will be stored into memory at the addresses
    specified by these buffers.  A received packet can consist of multiple
    buffers, split across buffers with the SAME size, as specified in the
    DRV_ETHMAC_LibRxSetBufferSize.  Each buffer needs an associated receive
    descriptor. Therefore, the number of needed receive descriptors should be
    available for this function to succeed.  Once a receive operation is
    scheduled, DRV_ETHMAC_LibRxGetPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ) can be called to get the received packet.

  Precondition:
    DRV_ETHMAC_LibInit, DRV_ETHMAC_LibDescriptorsPoolAdd, DRV_ETHMAC_LibRxSetBufferSize should have been called.
    Each buffer supplied should be >= DRV_ETHMAC_LibRxSetBufferSize(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ).

  Parameters:
    pMacD       - driver instance.
    ppBuff      - Pointer to an array of buffers (could be NULL terminated) to
                  be appended to the receiving process

    nBuffs      - Number of buffers supplied (or 0 if ppBuff is NULL
                  terminated)

    rxFlags     - Flags applied to all RX buffers passed

  Returns:
    DRV_ETHMAC_RES_OK              - success

    DRV_ETHMAC_RES_NO_DESCRIPTORS  - no enough RX descriptors

    DRV_ETHMAC_RES_USPACE_ERR      - buffers in user space not supported


  Example:
    <code>
    ethRes = DRV_ETHMAC_LibRxBuffersAppend(DRV_ETHMAC_INSTANCE_DCPT* pMacD, rxBuffers, rxDcptNo, DRV_ETHMAC_BUFF_FLAG_RX_STICKY);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Handle error
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR -non ISR code or
    multiple ISR's!

    This function enables the Ethernet receiving.

    When a packet is split into multiple buffers, all buffers have the same
    size, set by the DRV_ETHMAC_LibRxSetBufferSize(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ).

    The append process continues until a NULL buffer pointer is retrieved or
    nBuffs buffers are appended.

    Only RX DRV_ETHMAC_BUFF_FLAGS are relevant for this function.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthRxBuffersAppend ( void *ppBuff[], int nBuffs, DRV_ETHMAC_BUFF_FLAGS rxFlags )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxBuffersAppend (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void *ppBuff[], int nBuffs, DRV_ETHMAC_BUFF_FLAGS rxFlags );


// Section:  TX/RX Functions
// *****************************************************************************
/* Ethernet Buffer Acknowledge Callback Function

  Function:
    void <Function Name> ( void *pPktBuff, int buffIx, void* param )

  Summary:
    Pointer to an function called to acknowledge a buffer/packet.

  Description:
    Pointer to a function called to acknowledge the completion of a buffer/packet.

  Precondition:
    A TX or RX packet must have been queued.

  Parameters:
    pPktBuff    - Pointer to the currently acknowledged transmitted or received
                  buffer.

    buffIx      - The 0-based buffer index for a packet that spans multiple
                  buffers.

    param       - extra parameter that will be used by the function call


  Returns:
    None.

  Remarks:
    This function is meant to be used for both RX and TX packets acknowledge.
    It allows the caller to pass an extra parameter that will be used in the
    function call.
*/

typedef void ( *DRV_ETHMAC_BUFF_AckF ) ( void *pPktBuff, int buffIx, void* param );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, unsigned short int nBytes )

  Summary:
    Schedules the supplied buffer for transmission

  Description:
    This function schedules the supplied buffer for transmission.  To transmit
    this buffer an associated transmit descriptor is needed.  Therefore, a
    transmit descriptor should be available for this function to succeed.  Once
    a transmission is scheduled and done, DRV_ETHMAC_LibTxGetBufferStatus can be called to
    check the transmission result.

  Precondition:
    DRV_ETHMAC_LibMACOpen, DRV_ETHMAC_LibDescriptorsPoolAdd should have been called.
    Valid buffer pointer required.
    Buffers specified should be > 0 and < 2048 bytes in size.

  Parameters:
    pMacD       - driver instance.
    pBuff       - Adress of buffer to be sent

    nBytes      - Size of the buffer in bytes

  Returns:
    DRV_ETHMAC_RES_OK              - success

    DRV_ETHMAC_RES_NO_DESCRIPTORS  - no descriptors available

    DRV_ETHMAC_RES_USPACE_ERR      - user space buffers not supported

  Example:
    <code>
    DRV_ETHMAC_LibTxSendBuffer(pMacD, txBuffer, pktSize);
    </code>

  Remarks:
    Not multithreaded safe.  Don't call from from both ISR and non ISR code
    or multiple ISR's!

    This function enables the Ethernet transmission.

    This function is for transmission of packets completely assembled in one
    buffer.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxSendBuffer ( const void *pBuff, unsigned short int nBytes )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, unsigned short int nBytes );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendPacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const DRV_ETHMAC_PKT_DCPT *pPkt )

  Summary:
    Schedules the supplied packet for transmission.

  Description:
    This function schedules the supplied packet for transmission.  Each packet
    can consist of multiple buffers, each having a different size, as specified
    in the packet descriptor.  Each buffer will need an associated transmit
    descriptor. Therefore, the number of needed transmit descriptors should be
    available for this function to succeed.  Once a transmission is scheduled
    and done, DRV_ETHMAC_LibTxGetPacketStatus can be called to check the transmission
    result.

  Precondition:
    DRV_ETHMAC_LibMACOpen, DRV_ETHMAC_LibDescriptorsPoolAdd should have been called.
    Valid packet descriptor pointer required.
    Buffers specified should be >0 and < 2048 bytes in size.

  Parameters:
    pMacD       - driver instance.
    pPkt        - Packet descriptor consisting of a list of buffer descriptors,
                  terminated by a NULL pointer.

  Returns:
    DRV_ETHMAC_RES_OK              - success

    DRV_ETHMAC_RES_NO_DESCRIPTORS  - no descriptors available

    DRV_ETHMAC_RES_USPACE_ERR      - user space buffers not supported

  Example:
    <code>
    ethRes = DRV_ETHMAC_LibTxSendPacket(pMacD, &txDcpt);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Transmission failed
    }
    </code>

  Remarks:
    Not multithreaded safe. Don't call from from both ISR and non-ISR code or
    multiple ISR's!

    This function enables the Ethernet transmission.

    The packet construction from buffers stops when a descriptor with NULL buffer
    or 0 size is retrieved or the list ends.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxSendPacket ( const DRV_ETHMAC_PKT_DCPT *pPkt )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxSendPacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const DRV_ETHMAC_PKT_DCPT *pPkt );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxGetBufferStatus (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, const DRV_ETHMAC_PKT_STAT_TX **pTxStat )

  Summary:
    Provides the status of a transmitted buffer/packet.

  Description:
    This function provides the status of a transmitted buffer/packet.  The
    transmission of this packet has to have been completed otherwise the call
    will fail.

  Precondition:
    DRV_ETHMAC_LibTxSendBuffer/DRV_ETHMAC_LibTxSendPacket should have been called and the pBuff pointer must be valid.

  Parameters:
    pMacD       - driver instance.
    pBuff       - Buffer part of the packet whose status is being queried.

    pTxStat     - Address to store the pointer to the packet status or NULL if
                  status not needed

  Returns:
    DRV_ETHMAC_RES_OK              - the packet was transmitted OK,

    DRV_ETHMAC_RES_PACKET_QUEUED   - the packet is not done yet

    DRV_ETHMAC_RES_NO_PACKET       - no such packet exists

  Example:
    <code>
    ethRes = DRV_ETHMAC_LibTxGetBufferStatus(pMacD, pTxBuff, NULL);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Handle packet not transmitted
    }
    </code>

  Remarks:
    The returned pointer to the packet status is invalid after the
    DRV_ETHMAC_LibTxAcknowledgeBuffer/DRV_ETHMAC_LibTxAcknowledgePacket is called.

    pBuff must be the pointer to the first buffer in the packet, if the packet spans
    multiple buffers.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxGetBufferStatus ( const void *pBuff, const DRV_ETHMAC_PKT_STAT_TX **pTxStat )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxGetBufferStatus (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, const DRV_ETHMAC_PKT_STAT_TX **pTxStat );




/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxGetPacketStatus(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, const DRV_ETHMAC_PKT_STAT_TX** pTxStat)

  Summary:
    Provides the status of a transmitted packet.

  Description:
    This function provides the status of a transmitted packet.  The
    transmission of this packet has to have been completed otherwise the call
    will fail.

  Precondition:
    DRV_ETHMAC_LibTxSendPacket should have been called and the pPkt pointer must be valid.

  Parameters:
    pMacD       - driver instance.
    pPkt        - Packet whose status is being queried.

    pTxStat     - Address to store the pointer to the packet status or NULL if
                  status not needed

  Returns:
    DRV_ETHMAC_RES_OK              - the packet was transmitted OK,

    DRV_ETHMAC_RES_PACKET_QUEUED   - the packet is not done yet

    DRV_ETHMAC_RES_NO_PACKET       - no such packet exists

  Example:
    <code>
    ethRes = DRV_ETHMAC_LibTxGetPacketStatus(pMacD, pTxPacket, NULL);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Handle packet not transmitted
    }
    </code>

  Remarks:
    The returned pointer to the packet status is invalid after the
    DRV_ETHMAC_LibTxAcknowledgeBuffer/DRV_ETHMAC_LibTxAcknowledgePacket is called.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxGetPacketStatus(const DRV_ETHMAC_PKT_DCPT* pPkt, const DRV_ETHMAC_PKT_STAT_TX** pTxStat)</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxGetPacketStatus(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, const DRV_ETHMAC_PKT_STAT_TX** pTxStat);

// DOM-IGNORE-BEGIN
extern __inline__ DRV_ETHMAC_RESULT __attribute__((always_inline)) DRV_ETHMAC_LibTxGetPacketStatus(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, const DRV_ETHMAC_PKT_STAT_TX** pTxStat)
{
    return  DRV_ETHMAC_LibTxGetBufferStatus(pMacD, pPkt?pPkt->pBuff:0, pTxStat);
}
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: packet acknowledge functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxAcknowledgeBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )

  Summary:
    Acknowledges a transmitted buffer/packet

  Description:
    This function acknowledges a transmitted buffer/packet.  The transmission
    of this packet has to have been completed otherwise the call will fail.
    When pBuff==NULL, all currently transmitted packets will be acknowledged.
    The ackFnc, if !NULL, will be called for each buffer within the packet in
    turn.

  Precondition:
    DRV_ETHMAC_LibTxSendBuffer/DRV_ETHMAC_LibTxSendPacket should have been called.

  Parameters:
    pMacD       - driver instance.
    pBuff           - Buffer of the packet to be acknowledged or NULL.

    ackFnc          - Function to be called for the acknowledged buffers or
                      NULL

    fParam          - Parameter to be used in the ackFnc callback

  Returns:
    DRV_ETHMAC_RES_OK              - Success

    DRV_ETHMAC_RES_PACKET_QUEUED   - The packet is scheduled for transmission/
                              There are packets scheduled to be transmitted

    DRV_ETHMAC_RES_NO_PACKET       - No such packet exists in the transmit queue/
                              No packets scheduled for transmit

  Example:
    <code>
    ethRes=EthTxAcknowledgeBuffer(pTxBuff, myAckFnc, &myInstanceData);

    void myAckFnc(void* pBuff, int buffIx, void* param)
    {
        myInstanceType* pType=(myInstanceType*)param;
        // Handle each buffer acknowledgement
    }
    </code>

  Remarks:
    Any transmitted packet has to be acknowledged, otherwise the Ethernet library will
    run out of transmission descriptors.

    pBuff must be the pointer to the first buffer in the packet, if the packet
    spans multiple buffers.

    ackFnc is just a helper that allows the application to acknowledge the
    transmitted buffers without the need to maintain a list of buffers
    scheduled for transmission (this list is maintained by the Ethernet library
    anyway).

    Useful especially when the transmission result is not really needed and
    the function is called with pBuff=0.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxAcknowledgeBuffer ( const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxAcknowledgeBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxAcknowledgePacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )

  Summary:
    Acknowledges a transmitted packet

  Description:
    This function acknowledges a transmitted packet.  The transmission
    of this packet has to have been completed otherwise the call will fail.
    When pPkt==NULL, all currently transmitted packets will be acknowledged.
    The ackFnc, if !NULL, will be called for each buffer within the packet in
    turn.

  Precondition:
    DRV_ETHMAC_LibTxSendPacket should have been called.

  Parameters:
    pMacD       - driver instance.
    pPkt            - Packet to be acknowledged or NULL.

    ackFnc          - Function to be called to acknowledge each buffer
                      within the packet or NULL

    fParam          - Parameter to be used in the ackFnc callback


  Returns:
    DRV_ETHMAC_RES_OK              - Success

    DRV_ETHMAC_RES_PACKET_QUEUED   - The packet is queued for transmission/
                              There are packets scheduled to be transmitted

    DRV_ETHMAC_RES_NO_PACKET       - No such packet exist sin th etransmit queue/
                              No packets scheduled for transmit

  Example:
    <code>
    ethRes=DRV_ETHMAC_LibTxAcknowledgePacket(pMacD, pTxPkt, 0, 0);
    </code>

  Remarks:
    Any transmitted packet has to be acknowledged, otherwise the Ethernet library will
    run out of transmission descriptors.

    ackFnc is just a helper that allows the application to acknowledge the
    transmitted buffers without the need to maintain a list of buffers
    scheduled for transmission (this list is maintained by the Ethernet library
    anyway).

    Useful especially when the transmission result is not really needed and
    the function is called with pPkt=0.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthTxAcknowledgePacket(const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxAcknowledgePacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam );

// DOM-IGNORE-BEGIN
extern __inline__ DRV_ETHMAC_RESULT __attribute__((always_inline)) DRV_ETHMAC_LibTxAcknowledgePacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )
{
    return  DRV_ETHMAC_LibTxAcknowledgeBuffer(pMacD, pPkt?pPkt->pBuff:0, ackFnc, fParam);
}
// DOM-IGNORE-END


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxAcknowledgeBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )

  Summary:
    Acknowledges a received buffer/packet.

  Description:
    This function acknowledges a received buffer/packet.  The supplied packet
    has to have been previously received otherwise the call will fail.
    When pBuff==NULL, all currently received packets will be acknowledged.
    The ackFnc, if !NULL, will be called for each buffer within the packet in turn.

  Precondition:
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend, DRV_ETHMAC_LibRxGetPacket should have been
    called.

  Parameters:
    pMacD       - driver instance.
    pBuff       - Buffer/packet to be acknowledged or NULL

    ackFnc      - Function to be called for the acknowledged buffers or NULL

    fParam      - Parameter to be used in the ackFnc callback

  Returns:
    DRV_ETHMAC_RES_OK              - Success

    DRV_ETHMAC_RES_PACKET_QUEUED   - The packet is in the receiving queue/
                              There are packets in the receiving queue

    DRV_ETHMAC_RES_NO_PACKET       - No such packet in the receiving queue/
                              No packets available in the receiving queue

  Example:
    <code>
    ethRes = DRV_ETHMAC_LibRxAcknowledgeBuffer(pMacD, pRxBuff, myAckFnc, &myInstanceData);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Handle buffer not acknowledged
    }

    void myAckFnc(void* pBuff, int buffIx, void* param)
    {
        myInstanceType* pType=(myInstanceType*)param;
        // Handle each buffer acknowledgement
    }
    </code>

  Remarks:
    Any received packet has to be acknowledged, otherwise the Ethernet library will run
    out of descriptors.

    pBuff must be the pointer to the first buffer in the packet, if the packet
    spans multiple buffers.

    ackFnc is just a helper that allows the application to call an acknowledge
    function for each received buffer in turn.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthRxAcknowledgeBuffer ( const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxAcknowledgeBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const void *pBuff, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxAcknowledgePacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )

  Summary:
    Acknowledges a received packet.

  Description:
    This function acknowledges a received packet.  The supplied packet
    has to have been previously received otherwise the call will fail.
    When pPkt==NULL, all currently received packets will be acknowledged.
    The ackFnc, if !NULL, will be called for each buffer within the packet in turn.

  Precondition:
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend, DRV_ETHMAC_LibRxGetPacket should have been
    called.

  Parameters:
    pMacD       - driver instance.
    pPkt        - Packet to be acknowledged or NULL

    ackFnc      - Function to be called for the each buffer within the packet or NULL

    fParam      - Parameter to be used in the ackFnc callback

  Returns:
    DRV_ETHMAC_RES_OK              - Success

    DRV_ETHMAC_RES_PACKET_QUEUED   - The packet is in the receiving queue/
                              There are packets in the receiving queue

    DRV_ETHMAC_RES_NO_PACKET       - No such packet in the receiving queue/
                              No packets available in the receiving queue

  Example:
    <code>
    ethRes = DRV_ETHMAC_LibRxAcknowledgePacket(pMacD, pRxPkt, NULL, 0);
    if ( ethRes != DRV_ETHMAC_RES_OK )
    {
        // Handle packet not acknowledged
    }
    </code>

  Remarks:
    Any received packet has to be acknowledged, otherwise the Ethernet library will run
    out of descriptors.

    ackFnc is just a helper that allows the application to call an acknowledge
    function for each received buffer in turn.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthRxAcknowledgePacket ( const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxAcknowledgePacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam );

// DOM-IGNORE-BEGIN
extern __inline__ DRV_ETHMAC_RESULT __attribute__((always_inline)) DRV_ETHMAC_LibRxAcknowledgePacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, const DRV_ETHMAC_PKT_DCPT* pPkt, DRV_ETHMAC_BUFF_AckF ackFnc, void* fParam )
{
    return DRV_ETHMAC_LibRxAcknowledgeBuffer(pMacD, pPkt?pPkt->pBuff:0, ackFnc, fParam);
}
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Packet receive functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void **ppBuff, const DRV_ETHMAC_PKT_STAT_RX **pRxStat )

  Summary:
     Returns a pointer to the data and the status of the next available
     received packet.

  Description:
    This function returns a pointer to the data and the status of the next
    available received packet.  If there's no available fully received packet
    the call will fail (DRV_ETHMAC_RES_PACKET_QUEUED/DRV_ETHMAC_RES_NO_PACKET).  If the
    received packet spans multiple buffers/descriptors (i.e. when the size of
    the buffer, DRV_ETHMAC_LibRxSetBufferSize(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ) is less than the packet size)
    DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR will be returned.  However, if the ppBuff is NULL,
    then the caller can use the returned pRxStat to check the total size of the
    packet. Then using the size of the set buffers, the number of buffers used
    in the packet will result.

  Precondition:
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMacD       - driver instance.
    ppBuff          - Address of a pointer to packet buffer data or NULL

    pRxStat         - Address to store the pointer to the packet status or
                      NULL

  Returns:
    DRV_ETHMAC_RES_OK                  - For success

    DRV_ETHMAC_RES_PACKET_QUEUED       - The packet is in the reception queus/
                                  There are packets queued for receive but not
                                  completed yet

    DRV_ETHMAC_RES_NO_PACKET           - No such packet is in the receiving queue/
                                  No packets available in the receiving queue

    DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR    - Packet spans accross multiple buffers

  Example:
    <code>
    // Wait to receive a packet
    do
    {
        ethRes = DRV_ETHMAC_LibRxGetBuffer(pMacD, &pBuff, &pRxPktStat);
    } while ( ethRes == DRV_ETHMAC_RES_PACKET_QUEUED );
    </code>

  Remarks:
    The returned status pointer is invalid once the DRV_ETHMAC_LibRxAcknowledgeBuffer/DRV_ETHMAC_LibRxAcknowledgePacket
    is called.

    Also the associated packet buffer if it was appended with a
    DRV_ETHMAC_BUFF_FLAG_RX_STICKY flag.

    The returned buffer, if existent, will be marked as reported only when the
    result returned is DRV_ETHMAC_RES_OK.

    In this case a new call to the function will not return the same buffer.

    If the call failed for some reason (result!=DRV_ETHMAC_RES_OK) the returned
    buffer is not marked as reported.

    A new call to this function will return the same buffer.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthRxGetBuffer ( void **ppBuff, const DRV_ETHMAC_PKT_STAT_RX **pRxStat )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetBuffer (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  void **ppBuff, const DRV_ETHMAC_PKT_STAT_RX **pRxStat );


/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetPacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_PKT_DCPT *pPkt, int *pnBuffs,
                             const DRV_ETHMAC_PKT_STAT_RX **pRxStat )

  Summary:
    Provides the layout and the status of the next available received packet.

  Description:
    This function returns the layout and the status of the next available
    received packet.  If there's no available fully received packet the call
    will fail (DRV_ETHMAC_RES_PACKET_QUEUED/DRV_ETHMAC_RES_NO_PACKET).  The supplied pPkt has
    to be large enough to accommodate the packet layout in the case of packets
    split across multiple buffers/descriptors (i.e. when the size of the buffer,
    DRV_ETHMAC_LibRxSetBufferSize(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ) is less than the packet size) otherwise
    DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR will be returned.  However, if the pPkt is NULL,
    then the caller can use the returned pRxStat to check the total size of the
    packet. Then using the size of the set buffers, the number of buffers in
    pPkt will result.  pnBuffs can be used for the same purpose. If pnBuffs is
    supplied, the numbers of buffers in the packet will be returned.

  Precondition:
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMacD       - driver instance.
    pPkt            - Pointer to a packet descriptor: a list of buffer
                      descriptors large enough to receive the packet layout or
                      NULL.

    pnBuffs         - Pointer to store the number of buffers in the packet or
                      NULL.

    pRxStat         - Address to store the pointer to the packet status or
                      NULL.

  Returns:
    DRV_ETHMAC_RES_OK                  - For success.

    DRV_ETHMAC_RES_PACKET_QUEUED       - The packet is queued for reception/
                                  There are packets queued for receive but not
                                  completed yet.

    DRV_ETHMAC_RES_NO_PACKET           - No such packet in the receiving queue/
                                  No packets available in the receiving queue.

    DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR    - Packet descriptor not large enough to store
                                  the whole packet.

  Example:
    <code>
    // Wait to receive a packet
    do
    {
        ethRes = DRV_ETHMAC_LibRxGetPacket(pMacD, &rxPktDcpt, &buffsPerRxPkt, &pRxPktStat);

    } while ( ethRes== DRV_ETHMAC_RES_PACKET_QUEUED );
    </code>

  Remarks:
    The packet list, when supplied, is terminated with a NULL buffer

    The returned status pointer is invalid after the DRV_ETHMAC_LibRxAcknowledgePacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, ) is
    called.  Also any buffers that were appended with a DRV_ETHMAC_BUFF_FLAG_RX_STICKY
    flag associated to the packet buffers.

    The returned packet, if existent, will be marked as reported only when the
    result returned is DRV_ETHMAC_RES_OK.  In this case a new call to the function
    will not return the same packet.

    If the call failed for some reason (result!=DRV_ETHMAC_RES_OK) the returned
    packet is not marked as reported.

    A new call to this function will return the same packet.
    <p>Replaces:<p><c><b>DRV_ETHMAC_RESULT EthRxGetPacket ( DRV_ETHMAC_PKT_DCPT *pPkt, int *pnBuffs, const DRV_ETHMAC_PKT_STAT_RX **pRxStat )</b></c>
 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxGetPacket (DRV_ETHMAC_INSTANCE_DCPT* pMacD,  DRV_ETHMAC_PKT_DCPT *pPkt, int *pnBuffs, const DRV_ETHMAC_PKT_STAT_RX **pRxStat );


// *****************************************************************************
// *****************************************************************************
// Section: Statistics and debug functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)

  Summary:
    Returns the number of pending RX buffers/descriptors in the driver's queues.

  Description:
    This function returns the number of pending RX buffers/descriptors in the driver's queues.
    If each incoming packet fits within a RX buffer (the RX buffer is large enough)
    than this corresponds to the number of pending RX packets.
    Otherwise the number of packets is less than the pending buffers.

  Precondition:
    DRV_ETHMAC_LibInit must have been called.
    DRV_ETHMAC_LibDescriptorsPoolAdd should have been called.
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMacD       - driver instance.
    pnBuffs     - pointer to an address to store the number of buffers

  Returns:
    DRV_ETHMAC_RES_OK  - the number of buffers was returned OK


  Example:
    <code>
    </code>

  Remarks:
    Info only, the number changes dynamically.

 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs);

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxScheduledBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)

  Summary:
    Returns the number of scheduled RX buffers/descriptors in the driver's queues.

  Description:
    This function returns the number of scheduled RX buffers/descriptors in the driver's queues.

  Precondition:
    DRV_ETHMAC_LibInit must have been called.
    DRV_ETHMAC_LibDescriptorsPoolAdd should have been called.
    DRV_ETHMAC_LibRxSetBufferSize, DRV_ETHMAC_LibRxBuffersAppend should have been called.

  Parameters:
    pMacD       - driver instance.
    pnBuffs     - pointer to an address to store the number of buffers

  Returns:
    DRV_ETHMAC_RES_OK  - the number of buffers was returned OK


  Example:
    <code>
    </code>

  Remarks:
    Info only, the number changes dynamically.

 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibRxScheduledBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs);

/*******************************************************************************
  Function:
    DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs)

  Summary:
    Returns the number of pending TX buffers/descriptors in the driver's queues.

  Description:
    This function returns the number of pending TX buffers/descriptors in the driver's queues.
    If each outgoing packet consists of only one TX RX buffer
    than this corresponds to the number of pending TX packets.
    Otherwise the number of TX packets is less than the pending buffers.

  Precondition:
    DRV_ETHMAC_LibInit must have been called.
    DRV_ETHMAC_LibDescriptorsPoolAdd should have been called.

  Parameters:
    pMacD       - driver instance.
    pnBuffs     - pointer to an address to store the number of buffers

  Returns:
    DRV_ETHMAC_RES_OK  - the number of buffers was returned OK

  Example:
    <code>
    </code>

  Remarks:
    Info only, the number changes dynamically.

 *****************************************************************************/

DRV_ETHMAC_RESULT DRV_ETHMAC_LibTxPendingBuffersGet(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int* pnBuffs);



#endif  // _DRV_ETHMAC_LIB_H_

