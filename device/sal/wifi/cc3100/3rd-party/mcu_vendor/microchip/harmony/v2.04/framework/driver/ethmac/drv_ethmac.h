/***********************************************************************
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_ethmac.h

  Summary:
    Ethernet MAC device driver interface file

  Description:
    Ethernet MAC Device Driver Interface
    
    The Ethernet MAC device driver provides a simple interface to manage
    the Ethernet peripheral. This file defines the interface definitions
    and prototypes for the Ethernet MAC driver.                         
  ***********************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_ETHMAC_H
#define _DRV_ETHMAC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/driver_common.h"

#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_ethernet.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Ethernet Driver Module Index Numbers

  Summary:
    Ethernet driver index definitions.

  Description:
    These constants provide Ethernet driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

    These values should be passed into the MAC initialization routines
    to identify the driver instance in use.
*/

#define DRV_ETHMAC_INDEX_0         0
#define DRV_ETHMAC_INDEX_1         1


// *****************************************************************************
/* Ethernet Driver Module Index Count

  Summary:
    Number of valid Ethernet driver indices.

  Description:
    This constant identifies number of valid Ethernet driver indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from part-specific header files defined as part of the
    peripheral libraries.
*/

#define DRV_ETHMAC_INDEX_COUNT  ETH_NUMBER_OF_MODULES


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ      DRV_ETHMAC_PIC32MACInitialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

  Summary:
    Initializes the PIC32 Ethernet MAC.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports the initialization of the PIC32 Ethernet MAC.  Used by tcpip_module_manager.

  Precondition:
    None

  Parameters:
    - index - index of the ETH MAC driver to be initialized
    - init  - Pointer to initialization data

  Returns:
    - a valid handle to a driver object, if successful.
    - SYS_MODULE_OBJ_INVALID if initialization failed.

  Example:
    <code>
    </code>

  Remarks:
    This function initializes the Ethernet controller, the MAC and the associated PHY.
    It should be called to be able to schedule any Ethernet transmit or receive operation.

*/
SYS_MODULE_OBJ DRV_ETHMAC_PIC32MACInitialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);


// *****************************************************************************
/* Function:
    void                DRV_ETHMAC_PIC32MACDeinitialize(SYS_MODULE_OBJ object);

  Summary:
    Deinitializes the PIC32 Ethernet MAC.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports teardown of the PIC32 Ethernet MAC (opposite of set up).  Used by tcpip_module_manager.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from DRV_ETHMAC_PIC32MACInitialize

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function deinitializes the Ethernet controller, the MAC and the associated PHY.
    It should be called to be release any resources allocated by the initialization
    and return the MAC and the PHY to the idle/power down state.

*/
void DRV_ETHMAC_PIC32MACDeinitialize(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void DRV_ETHMAC_PIC32MACReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);

  Summary:
    Reinitializes the PIC32 Ethernet MAC.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function supports re-initialization of the PIC32 Ethernet MAC (opposite of set up).

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize must have been called to set up the driver.

  Parameters:
    - object    - Driver object handle, returned from DRV_ETHMAC_PIC32MACInitialize
    - init      - Pointer to initialization data

  Returns:
     None.

  Example:
    <code>
    </code>

  Remarks:
    This function is not supported yet.

*/
void DRV_ETHMAC_PIC32MACReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);



/**************************************************************************
  Function:
       SYS_STATUS DRV_ETHMAC_PIC32MACStatus ( SYS_MODULE_OBJ object )
    
  Summary:
    Provides the current status of the MAC driver module.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the MAC driver
    module.

  Precondition:
    The DRV_ETHMAC_PIC32MACInitialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from DRV_ETHMAC_PIC32MACInitialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    </code>

  Remarks:
    None.
 **************************************************************************/

SYS_STATUS DRV_ETHMAC_PIC32MACStatus ( SYS_MODULE_OBJ object );

/***************************************************************************
  Function:
       void DRV_ETHMAC_PIC32MACTasks( SYS_MODULE_OBJ object )
    
  Summary:
    Maintains the EThernet MAC driver's state machine.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine

  Precondition:
    The DRV_ETHMAC_PIC32MACInitialize routine must have been called for the
    specified MAC driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                DRV_ETHMAC_PIC32MACInitialize)
  Returns:
    None

  Example:
    <code>
    </code>

  Remarks:
    None.                       

  ***************************************************************************/

void DRV_ETHMAC_PIC32MACTasks( SYS_MODULE_OBJ object );

/**************************************************************************
  Function:
       DRV_HANDLE DRV_ETHMAC_PIC32MACOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
    
  Summary:
    Opens a client instance of the PIC32 MAC Driver.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function opens a client instance of the PIC32 MAC Driver.
    Used by tcpip_module_manager.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.

  Parameters:
    - drvIndex   - identifier for the driver instance to be opened.
    - intent     - Zero or more of the values from the enumeration
                   DRV_IO_INTENT ORed together to indicate the intended use
                   of the driver

  Return:
    - DRV_HANDLE - handle (pointer) to MAC client
    - 0 if call failed

  Example:
    <code>
    </code>

  Remarks:
    The intent parameter is not used in the current implementation and is maintained only for compatibility
    with the generic driver Open function signature.
  **************************************************************************/
DRV_HANDLE DRV_ETHMAC_PIC32MACOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);


/************************************************************************
  Function:
       void DRV_ETHMAC_PIC32MACClose( DRV_HANDLE hMac )
    
  Summary:
    Closes a client instance of the PIC32 MAC Driver.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function closes a client instance of the PIC32 MAC Driver.

  Precondition:
    DRV_ETHMAC_PIC32MACOpen() should have been called.

  Parameters:
    hMac -  valid MAC handle, obtained by a call to DRV_ETHMAC_PIC32MACOpen

  Return:
    None

  Example:
    <code>
    </code>
  Remarks:
    None
  ************************************************************************/

void DRV_ETHMAC_PIC32MACClose( DRV_HANDLE hMac );


// *****************************************************************************
/* Function:
    bool DRV_ETHMAC_PIC32MACLinkCheck( DRV_HANDLE hMac )

  Summary:
    Checks current link status.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks the link status of the associated network interface.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize must have been called to set up the driver.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    hMac - Ethernet MAC client handle

  Returns:
    - true  - If the link is up
    - false - If the link is not up

  Example:
<code>
</code>

  Remarks:
    The function will automatically perform a MAC reconfiguration if
    the link went up after being down and the PHY auto negotiation
    is enabled.

*/

bool DRV_ETHMAC_PIC32MACLinkCheck( DRV_HANDLE hMac );


// *****************************************************************************
/* Function:
    bool DRV_ETHMAC_PIC32MACPowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode )

  Summary:
    Selects the current power mode for the Ethernet MAC.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the power mode for the Ethernet MAC.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize must have been called to set up the driver.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - pwrMode   - required power mode

  Returns:
    - true if the call succeeded.
    - false if the call failed

  Example:
<code>
</code>

  Remarks:
    This function is not currently supported by the Ethernet MAC and
    will always return true.
*/

bool DRV_ETHMAC_PIC32MACPowerMode( DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode );

/*********************************************************************************************************************
  Function:
        TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)
    
  Summary:
    Sets the current MAC hash table receive filter.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the MAC hash table filtering to allow 
    packets sent to DestMACAddr to be received.
    It calculates a CRC-32 using polynomial 0x4C11DB7 over the 6 byte MAC address
    and then, using bits 28:23 of the CRC, will set the appropriate bits in
    the hash table filter registers ( ETHHT0-ETHHT1).

    The function will enable/disable the Hash Table receive filter if needed.
    
  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac        -  Ethernet MAC client handle
    - DestMACAddr - destination MAC address (6 bytes) to allow 
                    through the Hash Table Filter.
                    If DestMACAddr is set to 00-00-00-00-00-00,
                    then the hash table will be cleared of all entries
                    and the filter will be disabled.
  Return:
    - TCPIP_MAC_RES_OK if success
    - a TCPIP_MAC_RES error value if failed

  Example:
    <code>
    
    </code>
    
  Remarks:
    - Sets the appropriate bit in the ETHHT0/1 registers to allow packets
      sent to DestMACAddr to be received and enabled the Hash Table receive
      filter.
    - There is no way to individually remove destination MAC 
      addresses from the hash table since it is possible to have 
      a hash collision and therefore multiple MAC addresses 
      relying on the same hash table bit.
    - A workaround is to have the stack store each enabled MAC address
      and to perform the comparison at run time.
    - A call to DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet() using a 
      00-00-00-00-00-00 destination MAC address, which will clear 
      the entire hash table and disable the hash table filter.  
      This will allow the receive of all packets, regardless of their destination

  *********************************************************************************************************************/
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);

/******************************************************************************************************
  Function:
        TCPIP_MAC_RES DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
    
  Summary:
    MAC driver transmit function.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This is the MAC transmit function.
    Using this function a packet is submitted to the MAC driver for transmission.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - ptrPacket - pointer to a TCPIP_MAC_PACKET that's completely formatted
                  and ready to be transmitted over the network

  Return:
    - TCPIP_MAC_RES_OK if success
    - a TCPIP_MAC_RES error value if failed

  Example:
    <code>
    
    </code>
                                                                                                       
  Remarks:
    - The MAC driver supports internal queuing.
      A packet is rejected only if it's not properly formatted.
      Otherwise it will be scheduled for transmission and queued internally if needed.
    
    - Once the packet is scheduled for transmission the MAC driver will set
      the TCPIP_MAC_PKT_FLAG_QUEUED flag so that the stack is aware that this
      packet is under processing and cannot be modified.
    
    - Once the packet is transmitted, the TCPIP_MAC_PKT_FLAG_QUEUED will be
      cleared, the proper packet acknowledgment result (ackRes) will be
      set and the packet acknowledgment function (ackFunc) will be called.

  ******************************************************************************************************/
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);


// *****************************************************************************
/* Function:
    TCPIP_MAC_PACKET* DRV_ETHMAC_PIC32MACPacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat);

  Summary:
    This is the MAC receive function.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will return a packet if such a pending packet exists.
    
    Additional information about the packet is available by providing the pRes and
    ppPktStat fields.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - Ethernet MAC client handle
    - pRes        - optional pointer to an address that will receive an additional
                    result associated with the operation.
                    Can be 0 if not needed.
    - ppPktStat   - optional pointer to an address that will receive the received
                    packet status.
                    Note that this pointer cannot be used once the packet
                    acknowledgment function was called.
                    Can be 0 if not needed.


  Returns:
    - a valid pointer to an available RX packet
    - 0 if no packet pending/available

  Example:
<code>
</code>

  Remarks:
    - Once a pending packet is available in the MAC driver internal RX queues
      this function will dequeue the packet and hand it over to the
      MAC driver's client - i.e., the stack - for further processing.

    - The flags for a RX packet are updated by the MAC driver:
        - TCPIP_MAC_PKT_FLAG_RX will be set
        - TCPIP_MAC_PKT_FLAG_UNICAST is set if that packet is a unicast packet
        - TCPIP_MAC_PKT_FLAG_BCAST is set if that packet is a broadcast packet
        - TCPIP_MAC_PKT_FLAG_MCAST is set if that packet is a multicast packet
        - TCPIP_MAC_PKT_FLAG_QUEUED is set
        - TCPIP_MAC_PKT_FLAG_SPLIT is set if the packet has multiple data segments

    - The MAC driver dequeues and return to the caller just one single packet.
      That is the packets are not chained.

    - The packet buffers are allocated by the Ethernet MAC driver itself,
      Once the higher level layers in the stack are done with processing the RX packet,
      they have to call the corresponding packet acknowledgment function
      that tells the MAC driver that it can resume control of that packet.

    - Once the stack modules are done processing the RX packets and the acknowledge function is called
      the MAC driver will reuse the RX packets.

    - The MAC driver may use the DRV_ETHMAC_PIC32MACProcess() for obtaining new RX packets if needed.

*/
TCPIP_MAC_PACKET* DRV_ETHMAC_PIC32MACPacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat);


/*****************************************************************************
  Function:
        TCPIP_MAC_RES DRV_ETHMAC_PIC32MACProcess(DRV_HANDLE hMac);
    
  Summary:
    MAC periodic processing function.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that allows for internal processing by the MAC
    driver. It is meant for processing that cannot be done from within ISR.
        
    Normally this function will be called in response to an TX and/or RX
    event signaled by the driver. This is specified by the MAC driver at
    initialization time using TCPIP_MAC_MODULE_CTRL.
    
  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    hMac -  Ethernet MAC client handle

  Return:
    - TCPIP_MAC_RES_OK if all processing went on OK
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Example:
    <code>
    
    </code>

  Remarks:
    - The MAC driver may use the DRV_ETHMAC_PIC32MACProcess() for:   
      * Processing its pending TX queues
      * RX buffers replenishing functionality.
        If the number of packets in the RX queue falls below a
        specified limit, the MAC driver may use this function to
        allocate some extra RX packets.
        Similarly, if there are too many allocated RX packets,
        the MAC driver can free some of them.

  *****************************************************************************/
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACProcess(DRV_HANDLE hMac);


// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES       DRV_ETHMAC_PIC32MACStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);

  Summary:
    Gets the current MAC statistics.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic counters
    maintained by the MAC driver.


  Precondition:
   DRV_ETHMAC_PIC32MACInitialize() should have been called.
   DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - pRxStatistics - pointer to a TCPIP_MAC_RX_STATISTICS that will receive the current 
                      RX statistics counters
                      Can be NULL if not needed

    - pTxStatistics - pointer to a TCPIP_MAC_TX_STATISTICS that will receive the current 
                      TX statistics counters
                      Can be NULL if not needed

  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES       DRV_ETHMAC_PIC32MACStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
    
// *****************************************************************************
/*  MAC Parameter Get function
     TCPIP_MAC_RES     DRV_ETHMAC_PIC32MACParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

  Summary:
    MAC parameter get function.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that returns the run time parameters of the MAC driver.

  Precondition:
   DRV_ETHMAC_PIC32MACInitialize() should have been called.
   DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac        - handle identifying the MAC driver client
    - pMacParams  - address to store the MAC parameters

  Returns:
    - TCPIP_MAC_RES_OK if pMacParams updated properly
    - a TCPIP_MAC_RES error code if processing failed for some reason

  Remarks:
    None.

*/
TCPIP_MAC_RES     DRV_ETHMAC_PIC32MACParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);

// *****************************************************************************
/*  Function:
     TCPIP_MAC_RES       DRV_ETHMAC_PIC32MACRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);

  Summary:
    Gets the current MAC hardware statistics registers.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current value of the statistic registers
    of the associated MAC controller.


  Precondition:
   DRV_ETHMAC_PIC32MACInitialize() should have been called.
   DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - pRegEntries   - pointer to an array of TCPIP_MAC_STATISTICS_REG_ENTRY that will receive the current 
                      hardware statistics registers
                      Can be NULL if not needed

    - nEntries      - number of TCPIP_MAC_STATISTICS_REG_ENTRY in the array.

    - pHwEntries    - address to store the number of hardware supported registers by the associated MAC
                      Can be NULL if not needed
                
  Returns:
    - TCPIP_MAC_RES_OK if all processing went on OK.
    - TCPIP_MAC_RES_OP_ERR error code if function not supported by the driver.

  Example:
    <code>
    </code>

  Remarks:
    - The reported values are info only and change dynamically.

*/
TCPIP_MAC_RES       DRV_ETHMAC_PIC32MACRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
    
// *****************************************************************************
/*  Function:
     size_t      DRV_ETHMAC_PIC32MACConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

  Summary:
    Gets the current MAC driver configuration.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will get the current MAC driver configuration and store it into
    a supplied buffer.

  Precondition:
   DRV_ETHMAC_PIC32MACInitialize() should have been called.
   DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac          - handle identifying the MAC driver client

    - configBuff    - pointer to a buffer to store the configuration
                      Can be NULL if not needed

    - buffSize      - size of the supplied buffer

    - pConfigSize   - address to store the number of bytes needed for the storage of the MAC configuration.
                      Can be NULL if not needed
                
  Returns:
    - number of bytes copied into the supplied storage buffer

  Example:
    <code>
    </code>

  Remarks:
    - None

*/
size_t      DRV_ETHMAC_PIC32MACConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);
    
/***********************************************************************************************************
  Function:
        bool DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
    
  Summary:
    Enables/disables the MAC events.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This is a function that enables or disables the events to be reported
    to the Ethernet MAC client (TCP/IP stack).

     All events that are to be enabled will be added to the notification process.
     All events that are to be disabled will be removed from the notification process.
     The stack has to catch the events that are notified and process them.
     After that the stack should call DRV_ETHMAC_PIC32MACEventAcknowledge()
     so that the events can be re-enable

     The stack should process at least the following transfer events:
        - TCPIP_MAC_EV_RX_PKTPEND
        - TCPIP_MAC_EV_RX_DONE
        - TCPIP_MAC_EV_TX_DONE

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - macEvMask - events the user of the stack wants to add/delete for
                  notification
    - enable    - if true, the events will be enabled, else disabled

  Return:
    always true, operation succeeded.

  Example:
    <code>
        DRV_ETHMAC_PIC32MACEventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    - The event notification system enables the user of the TCP/IP stack to
      call into the stack for processing only when there are relevant events
      rather than being forced to periodically call from within a loop.
    
    - If the notification events are nil, the interrupt processing will be
      disabled. Otherwise, the event notification will be enabled and the
      interrupts relating to the requested events will be enabled.
    
    - Note that once an event has been caught by the stack ISR (and reported
      if a notification handler is in place) it will be disabled until the
      DRV_ETHMAC_PIC32MACEventAcknowledge() is called.                                                        
  ***********************************************************************************************************/
bool DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);


/****************************************************************************************************
  Function:
   bool    DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);
    
  Summary:
    Acknowledges and re-enables processed events.
	<p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function acknowledges and re-enables processed events. Multiple
    events can be ORed together as they are processed together. The events
    acknowledged by this function should be the events that have been
    retrieved from the stack by calling DRV_ETHMAC_PIC32MACEventPendingGet()
    or have been passed to the stack by the driver using the registered notification handler
    and have been processed and have to be re-enabled.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac      - Ethernet MAC client handle
    - tcpAckEv  - the events that the user processed and need to be re-enabled

  Return:
    - true if events acknowledged
    - false if no events to be acknowledged

  Example:
    <code>
        DRV_ETHMAC_PIC32MACEventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:
    - All events should be acknowledged, in order to be re-enabled.
    
    - Some events are fatal errors and should not be acknowledged
      ( TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
      Driver/stack re-initialization is needed under such circumstances.
    
    - Some events are just system/application behavior and they are intended
      only as simple info (TCPIP_MAC_EV_RX_OVFLOW, TCPIP_MAC_EV_RX_BUFNA,
      TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).
    
    - The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part
      of the normal flow control operation (if auto flow control was
      enabled). They should be enabled alternatively, if needed.
    
    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.                                                          
  ****************************************************************************************************/
bool    DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv);



/******************************************************************************
  Function:
      TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac)
    
  Summary:
    Returns the currently pending events.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the currently pending Ethernet MAC events.
    Multiple events will be ORed together as they accumulate.
    The stack should perform processing whenever a transmission related
    event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an
    user. They will have to be eventually acknowledged if re-enabling is
    needed.

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    DRV_ETHMAC_PIC32MACOpen() should have been called to obtain a valid handle.

  Parameters:
    - hMac -  parameter identifying the intended MAC client

  Return:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_ETHMAC_PIC32MACEventPendingGet( hMac);
    </code>

  Remarks:
    - This is the preferred method to get the current pending MAC events. The
      stack maintains a proper image of the events from their occurrence to
      their acknowledgment.
    
    - Even with a notification handler in place it's better to use this
      function to get the current pending events rather than using the
      events passed by the notification handler which could be stale.
    
    - The events are persistent. They shouldn't be re-enabled unless they
      have been processed and the condition that generated them was removed.
      Re-enabling them immediately without proper processing will have
      dramatic effects on system performance.
    
    - The returned value is just a momentary value. The pending events can
      change any time.                                                           
  ******************************************************************************/
TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac);


/******************************************************************************
  Function:
      void DRV_ETHMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex )
    
  Summary:
    Ethernet MAC driver interrupt function.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This is the Ethernet MAC driver interrupt service routine.
    It processes the Ethernet related interrupts and notifies the 
    events to the driver user (the TCP/IP stack). 

  Precondition:
    DRV_ETHMAC_PIC32MACInitialize() should have been called.
    The TCP/IP stack event notification should be enabled.

  Parameters:
    - macIndex -  parameter identifying the intended MAC client

  Return:
    None.

  Remarks:
    None.
  ******************************************************************************/
void DRV_ETHMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex );


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _DRV_ETHMAC_H

/*******************************************************************************
 End of File
*/

