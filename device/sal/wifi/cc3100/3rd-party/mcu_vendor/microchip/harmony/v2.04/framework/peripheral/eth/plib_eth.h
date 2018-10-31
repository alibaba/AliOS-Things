/*******************************************************************************
  Ethernet Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_eth.h

  Summary:
    Defines the Ethernet Peripheral Library Interface.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Ethernet
    Peripheral Library for Microchip microcontrollers.  The definitions in this 
    file are for the Ethernet module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _PLIB_ETH_H
#define _PLIB_ETH_H

#include <stdio.h>
#include <xc.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/* RMII Speed Selection

  Summary:
    Lists the possible speed selection for the Reduced Media Independent 
    Interface (RMII).

  Description:
    This enumeration lists the possible speed selection for RMII.
    The body contains only two states: RMII_10Mbps or RMII_100Mbps.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/
typedef enum {

    ETH_RMII_10Mbps  /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,
    ETH_RMII_100Mps  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/

} ETH_RMII_SPEED;


//*****************************************************************************
/* MII Clock Selection

  Summary:
    Lists the possible speed selection for the Reduced Media Independent 
    Interface (RMII).

  Description:
    This enumeration lists the possible speed selection for RMII.
    The body contains only two states: RMII_10Mbps or RMII_100Mbps.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/
typedef enum {

    ETH_MIIM_SYSCLK_DIV_BY_4   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_RSVD   /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_6   /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_8   /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_10  /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_14  /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_20  /*DOM-IGNORE-BEGIN*/ = 0x06 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_28  /*DOM-IGNORE-BEGIN*/ = 0x07 /*DOM-IGNORE-END*/ ,
    ETH_MIIM_SYSCLK_DIV_BY_40  /*DOM-IGNORE-BEGIN*/ = 0x08 /*DOM-IGNORE-END*/

} ETH_MIIM_CLK;


//*****************************************************************************
/* Pattern Match Modes

  Summary:
    Lists the possible pattern match values.

  Description:
    This enumeration lists the pattern match mode values.

  Remarks:
    In all pattern match enabled cases, it is the pattern match inversion XOR 
    pattern match checksum AND <enum option>.

    Some states were omitted from the enumeration since they were duplicates.

    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/

typedef enum _ETH_PATTERN_MATCH_MODE_
{

    ETH_PATTERN_MATCH_DISABLED            /*DOM-IGNORE-BEGIN*/=0x00/*DOM-IGNORE-END*/,//Pattern Match is disabled; pattern match is always unsuccessful
    ETH_PATTERN_MATCH_MODE_CHECKSUM_MATCH /*DOM-IGNORE-BEGIN*/=0x01/*DOM-IGNORE-END*/,//Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches)
    ETH_PATTERN_MATCH_MODE_STATION_ADDR   /*DOM-IGNORE-BEGIN*/=0x02/*DOM-IGNORE-END*/,//Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches) AND (Destination Address = Station Address)
    ETH_PATTERN_MATCH_MODE_UNICAST_ADDR   /*DOM-IGNORE-BEGIN*/=0x04/*DOM-IGNORE-END*/,//Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches) AND (Destination Address = Unicast Address)
    ETH_PATTERN_MATCH_MODE_BROADCAST_ADDR /*DOM-IGNORE-BEGIN*/=0x06/*DOM-IGNORE-END*/,//Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches) AND (Destination Address = Broadcast Address)
    ETH_PATTERN_MATCH_MODE_HASH_MATCH     /*DOM-IGNORE-BEGIN*/=0x08/*DOM-IGNORE-END*/,//Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches) AND (Hash Table Filter match)
    ETH_PATTERN_MATCH_MODE_MAGIC_PACKET   /*DOM-IGNORE-BEGIN*/=0x09/*DOM-IGNORE-END*/ //Pattern match if (NOTPM = 1 XOR Pattern Match Checksum matches) AND (Packet = Magic Packet)

} ETH_PATTERN_MATCH_MODE;


//*****************************************************************************
/* ETH Module Receive Filter Mask

  Summary:
    Lists the possible values of the receive filter.

  Description:
    This enumeration lists the possible values of the receive filter.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/

typedef enum {

    ETH_HASH_FILTER              /*DOM-IGNORE-BEGIN*/ = 1<<15  /*DOM-IGNORE-END*/ , //Enable: Hash Table Filtering.
    ETH_MAGIC_PACKET_FILTER      /*DOM-IGNORE-BEGIN*/ = 1<<14  /*DOM-IGNORE-END*/ , //Enable: Magic Packet Filtering.
    ETH_PATTERN_MATCH_INVERSION  /*DOM-IGNORE-BEGIN*/ = 1<<12  /*DOM-IGNORE-END*/ , //Enable: Pattern Match Checksum must not match to be accepted.
    ETH_CRC_ERROR_FILTER         /*DOM-IGNORE-BEGIN*/ = 1<<7   /*DOM-IGNORE-END*/ , //Enable: Received packet CRC must be invalid to be accepted.
    ETH_CRC_OK_FILTER            /*DOM-IGNORE-BEGIN*/ = 1<<6   /*DOM-IGNORE-END*/ , //Enable: Received packet CRC must be valid to be accepted.
    ETH_RUNT_ERROR_FILTER        /*DOM-IGNORE-BEGIN*/ = 1<<5   /*DOM-IGNORE-END*/ , //Enable: Received packet must be runt packet to be accepted.
    ETH_RUNT_ENABLE_FILTER       /*DOM-IGNORE-BEGIN*/ = 1<<4   /*DOM-IGNORE-END*/ , //Enable: Received packet must not be a runt packet to be accepted.
    ETH_UNICAST_FILTER           /*DOM-IGNORE-BEGIN*/ = 1<<3   /*DOM-IGNORE-END*/ , //Enable: Unicast Packets whose destination address matches the station address are accepted.
    ETH_NOT_ME_UNICAST_FILTER    /*DOM-IGNORE-BEGIN*/ = 1<<2   /*DOM-IGNORE-END*/ , //Enable: Unicast Packets whose destination address do NOT match the station address are accepted.
    ETH_MULTICAST_FILTER         /*DOM-IGNORE-BEGIN*/ = 1<<1   /*DOM-IGNORE-END*/ , //Enable: Multicast Address Packets are accepted.
    ETH_BROADCAST_FILTER         /*DOM-IGNORE-BEGIN*/ = 1<<0   /*DOM-IGNORE-END*/ , //Enable: Broadcast Address Packets are accepted.
    ETH_ENABLE_ALL_FILTER        /*DOM-IGNORE-BEGIN*/ = 0xD0FF /*DOM-IGNORE-END*/   //Enable: All of the above packets are accepted.

} ETH_RECEIVE_FILTER;


//*****************************************************************************
/* AutoDetectPad Selection

  Summary:
    Lists the possible automatic padding configurations.

  Description:
    This enumeration lists the possible automatic padding configurations.

  Remarks:
    This enumeration is processor specific and is defined in the processor-
    specific header files (see processor.h).
*/
typedef enum {

    ETH_AUTOPAD_DISABLED_CHECK_CRC   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,
    ETH_AUTOPAD_TO_60BYTES_ADD_CRC   /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
    ETH_AUTOPAD_TO_64BYTES_ADD_CRC   /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
    ETH_AUTOPAD_BY_PKT_TYPE_ADD_CRC  /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/

} ETH_AUTOPAD_OPTION;


//*****************************************************************************
/* ETH Module Interrupt Mask

  Summary:
    Lists the possible values of ETH_INTERRUPT_SOURCES.

  Description:
    This enumeration lists the possible values of ETH_INTERRUPT_SOURCES.

  Remarks:
    See also PLIB_ETH_EVENTS in plib_eth_lib.h.
*/

typedef enum {

    ETH_TRANSMIT_BVCI_BUS_ERROR_INTERRUPT       /*DOM-IGNORE-BEGIN*/ = 1<<14 /*DOM-IGNORE-END*/ ,
    ETH_RECEIVE_BVCI_BUS_ERROR_INTERRUPT        /*DOM-IGNORE-BEGIN*/ = 1<<13 /*DOM-IGNORE-END*/ ,
    ETH_EMPTY_WATERMARK_INTERRUPT               /*DOM-IGNORE-BEGIN*/ = 1<<9  /*DOM-IGNORE-END*/ ,
    ETH_FULL_WATERMARK_INTERRUPT                /*DOM-IGNORE-BEGIN*/ = 1<<8  /*DOM-IGNORE-END*/ ,
    ETH_RECEIVE_DONE_INTERRUPT                  /*DOM-IGNORE-BEGIN*/ = 1<<7  /*DOM-IGNORE-END*/ ,
    ETH_PACKET_PENDING_INTERRUPT                /*DOM-IGNORE-BEGIN*/ = 1<<6  /*DOM-IGNORE-END*/ ,
    ETH_RECEIVE_ACTIVITY_INTERRUPT              /*DOM-IGNORE-BEGIN*/ = 1<<5  /*DOM-IGNORE-END*/ ,
    ETH_TRANSMIT_DONE_INTERRUPT                 /*DOM-IGNORE-BEGIN*/ = 1<<3  /*DOM-IGNORE-END*/ ,
    ETH_TRANSMIT_ABORT_INTERRUPT                /*DOM-IGNORE-BEGIN*/ = 1<<2  /*DOM-IGNORE-END*/ ,
    ETH_RECEIVE_BUFFER_NOT_AVAILABLE_INTERRUPT  /*DOM-IGNORE-BEGIN*/ = 1<<1  /*DOM-IGNORE-END*/ ,
    ETH_RECEIVE_FIFO_OVERFLOW_ERROR_INTERRUPT   /*DOM-IGNORE-BEGIN*/ = 1<<0  /*DOM-IGNORE-END*/ ,
    ETH_ALL_INTERRUPT_SOURCES                   /*DOM-IGNORE-BEGIN*/ = 0x63EF/*DOM-IGNORE-END*/

} ETH_INTERRUPT_SOURCES;


// *****************************************************************************
/* Ethernet Receive Filter Flags

  Summary:
    Defines the receive filter flags

  Description:
    This enumeration defines the receive filter flags.

  Remarks:
    Multiple values can be OR-ed together.

    The values in this enumeration are displayed in the order of priority
    that the receive filter state machine works, with the highest priority first.
    Once a filter accepts or rejects a packet, further filters are not tried.
    If a packet isn't rejected/accepted after all filters are tried, it will be rejected by
    default!
*/

typedef enum
{
#if defined(_ETHRXFC_CRCERREN_MASK)
    // Frames with wrong CRC are accepted
    ETH_FILT_CRC_ERR_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_CRCERREN_MASK /*DOM-IGNORE-END*/,

    // Runt frames accepted
    ETH_FILT_RUNT_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_RUNTERREN_MASK /*DOM-IGNORE-END*/,

    // Frames with wrong CRC are rejected
    ETH_FILT_CRC_ERR_REJECT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_CRCOKEN_MASK /*DOM-IGNORE-END*/,

    // Runt frames rejected
    ETH_FILT_RUNT_REJECT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_RUNTEN_MASK /*DOM-IGNORE-END*/,

    // Me unicast accepted
    ETH_FILT_ME_UCAST_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_UCEN_MASK /*DOM-IGNORE-END*/,

    // Not me unicast accepted
    ETH_FILT_NOTME_UCAST_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_NOTMEEN_MASK /*DOM-IGNORE-END*/,

    // Multicast accepted
    ETH_FILT_MCAST_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_MCEN_MASK /*DOM-IGNORE-END*/,

    // Broadcast accepted
    ETH_FILT_BCAST_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_BCEN_MASK /*DOM-IGNORE-END*/,

    // Hash table matches destination address accepted
    ETH_FILT_HTBL_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_HTEN_MASK /*DOM-IGNORE-END*/,

    // Magic packet accepted
    ETH_FILT_MAGICP_ACCEPT
        /*DOM-IGNORE-BEGIN*/ = _ETHRXFC_MPEN_MASK /*DOM-IGNORE-END*/,


    // All Filters
    ETH_FILT_ALL_FILTERS
     /*DOM-IGNORE-BEGIN*/    = ETH_FILT_CRC_ERR_ACCEPT  | ETH_FILT_RUNT_ACCEPT        |
                               ETH_FILT_CRC_ERR_REJECT  | ETH_FILT_RUNT_REJECT        |
                               ETH_FILT_ME_UCAST_ACCEPT | ETH_FILT_NOTME_UCAST_ACCEPT |
                               ETH_FILT_MCAST_ACCEPT    | ETH_FILT_BCAST_ACCEPT       |
                               ETH_FILT_HTBL_ACCEPT     | ETH_FILT_MAGICP_ACCEPT  /*DOM-IGNORE-END*/
#else
    /*DOM-IGNORE-BEGIN*/        
    // Reserved for processors that do not have the internal Ethernet module
    ETH_RX_FILTERS_RESERVED,
    /*DOM-IGNORE-END*/
#endif
} ETH_RX_FILTERS;


// *****************************************************************************
/* Ethernet Pattern Match Modes

  Summary:
    Defines pattern match modes

  Description:
    This enumeration defines the Ethernet pattern match modes.

  Remarks:
    These are mutually exclusive modes, not flags.  However,
    ETH_FILT_PMATCH_INVERT act as a flag and can be applied to any value.
*/

typedef enum
{
    // Simple Pattern Match accepted
    ETH_FILT_PMATCH_ACCEPT /*DOM-IGNORE-BEGIN*/ = 1/*DOM-IGNORE-END*/,

    // Pattern Match AND destination==me
    ETH_FILT_PMATCH_ME_UCAST_ACCEPT,

    // Pattern Match AND destination!=me
    ETH_FILT_PMATCH_NOTME_UCAST_ACCEPT,

    // Pattern Match AND destination!=unicast
    ETH_FILT_PMATCH_MCAST_ACCEPT,

    // Pattern Match AND destination==unicast
    ETH_FILT_PMATCH_NOT_MCAST_ACCEPT,

    // Pattern Match AND destination==broadcast
    ETH_FILT_PMATCH_BCAST_ACCEPT,

    // Pattern Match AND destination!=broadcast
    ETH_FILT_PMATCH_NOT_BCAST_ACCEPT,

    // Pattern Match AND hash table filter match (regardless of the
    // ETH_FILT_HTBL_ACCEPT setting)
    ETH_FILT_PMATCH_HTBL_ACCEPT,

    // Pattern Match AND packet ==magic packet
    ETH_FILT_PMATCH_MAGICP_ACCEPT,

    // If set, the pattern must NOT match for a successful Pattern Match to occur!
    ETH_FILT_PMATCH_INVERT /*DOM-IGNORE-BEGIN*/ = 0x80000000 /*DOM-IGNORE-END*/

} ETH_PMATCH_MODE;


// *****************************************************************************
/* Ethernet Event Flags

  Summary:
    Ethernet event flags

  Description:
    This enumeration defines flags for the possible Ethernet events that can
    cause interrupts.
*/

typedef enum
{
#if defined(_ETHRXFC_CRCERREN_MASK)
    // RX FIFO overflow
    ETH_EV_RXOVFLOW
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_RXOVFLW_MASK /*DOM-IGNORE-END*/,

    // RX buffer not available (descriptor overrun)
    ETH_EV_RXBUFNA
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_RXBUFNA_MASK /*DOM-IGNORE-END*/,

    // TX abort condition
    ETH_EV_TXABORT
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_TXABORT_MASK /*DOM-IGNORE-END*/,

    // TX done
    ETH_EV_TXDONE
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_TXDONE_MASK  /*DOM-IGNORE-END*/,

    // RX activity
    ETH_EV_RXACT
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_RXACT_MASK   /*DOM-IGNORE-END*/,

    // RX packet pending
    ETH_EV_PKTPEND
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_PKTPEND_MASK /*DOM-IGNORE-END*/,

    // RX packet successfully received
    ETH_EV_RXDONE
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_RXDONE_MASK  /*DOM-IGNORE-END*/,

    // Full watermark reached
    ETH_EV_FWMARK
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_FWMARK_MASK  /*DOM-IGNORE-END*/,

    // Empty watermark reached
    ETH_EV_EWMARK
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_EWMARK_MASK  /*DOM-IGNORE-END*/,

    // RX bus error
    ETH_EV_RXBUSERR
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_RXBUSE_MASK  /*DOM-IGNORE-END*/,

    // TX bus error
    ETH_EV_TXBUSERR
        /*DOM-IGNORE-BEGIN*/ = _ETHIRQ_TXBUSE_MASK  /*DOM-IGNORE-END*/,

    // All events
    ETH_EV_ALL
    /*DOM-IGNORE-BEGIN*/= (ETH_EV_RXOVFLOW | ETH_EV_RXBUFNA | ETH_EV_TXABORT |
                           ETH_EV_TXDONE   | ETH_EV_RXACT   | ETH_EV_PKTPEND |
                           ETH_EV_RXDONE   | ETH_EV_FWMARK  | ETH_EV_EWMARK  |
                           ETH_EV_RXBUSERR | ETH_EV_TXBUSERR ) /*DOM-IGNORE-END*/
#else
    /*DOM-IGNORE-BEGIN*/        
    // Reserved for processors that do not have the internal Ethernet module
    PLIB_ETH_EVENTS_RESERVED,
    /*DOM-IGNORE-END*/
#endif
} PLIB_ETH_EVENTS;


// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************
/* See Bottom of file for implementation header include files.
*/
// DOM-IGNORE-BEGIN
#include "peripheral/eth/processor/eth_processor.h"
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_ETH_PauseTimerSet(ETH_MODULE_ID index, uint16_t PauseTimerValue)

  Summary:
    Sets the Pause Timer value used for flow control.

  Description:
    This function sets the Pause Timer value used for flow control.

  Precondition:
    Write to the Pause Timer register before enabling the receiver. Call this 
	function before calling PLIB_ETH_ReceiveEnable.

  Parameters:
    index - Identifier for the device instance
    PauseTimerValue - used for Flow Control

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PauseTimerSet(MY_ETH_INSTANCE, PauseTimerValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the 
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PauseTimerSet(ETH_MODULE_ID index, uint16_t PauseTimerValue);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_PauseTimerGet(ETH_MODULE_ID index)

  Summary:
    Gets the Pause Timer value used for flow control.

  Description:
    This function gets the Pause Timer value used for flow control.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - PauseTimerValue - Pause Timer Value

  Example:
    <code>
    PauseTimerValue = PLIB_ETH_PauseTimerGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_PauseTimerGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_Enable(ETH_MODULE_ID index)

  Summary:
    Enables the Ethernet module.

  Description:
    This function enables the Ethernet module.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_Enable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_Enable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_Disable(ETH_MODULE_ID index)

  Summary:
    Disables the Ethernet module.

  Description:
    This function disables the Ethernet module.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_Disable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_Disable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_IsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module enable status.

  Description:
    This function returns the Ethernet module enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module is enabled
    - false - Ethernet module is disabled

  Example:
    <code>
    stat = PLIB_ETH_IsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_IsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_StopInIdleEnable(ETH_MODULE_ID index)

  Summary:
    Ethernet module operation will stop when the device enters Idle mode.

  Description:
    This function directs the Ethernet module to stop operation when the device 
    enters Idle mode.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_StopInIdleEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_StopInIdleEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_StopInIdleDisable(ETH_MODULE_ID index)

  Summary:
    Ethernet module operation will continue when the device enters Idle mode.

  Description:
    This function directs the Ethernet module to continue operation when the
    device enters Idle mode.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_StopInIdleDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_StopInIdleDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_StopInIdleIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module Stop in Idle mode enable status.

  Description:
    This function returns the Ethernet module Stop in Idle mode enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module transfers stop during Idle mode
    - false - Ethernet module transfers continue to run during Idle mode

  Example:
    <code>
    stat = PLIB_ETH_StopInIdleIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_StopInIdleIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxRTSEnable(ETH_MODULE_ID index)

  Summary:
    Enables the Ethernet transmit request to send.

  Description:
    This function enables the Ethernet request to send. Transmit logic is activated
    and any packets defined in the Ethernet descriptor table are transmitted.

  Precondition:
    The TX descriptor list and TX DMA must be initialized using 
    PLIB_ETH_TransmitPacketDescStartAddrSet.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxRTSEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This status is cleared by hardware when the transmission is complete.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxRTSEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxRTSDisable(ETH_MODULE_ID index)

  Summary:
    Aborts an Ethernet module transmission.

  Description:
    This function aborts an Ethernet module transmission and disables the 
    transmitter after the current packet has completed.

  Precondition
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxRTSDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    When disabled by software, transmission stops after the current packet
    has been completed.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxRTSDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TxRTSIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module transmit request to send status.

  Description:
    This function returns the Ethernet module transmit request to send status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module transmit is active
    - false - Ethernet module transmission has stopped or has completed

  Example:
    <code>
    stat = PLIB_ETH_TransmitRTSIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TxRTSIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxEnable(ETH_MODULE_ID index)

  Summary:
    Enables the Ethernet receive logic.

  Description:
    This function enables the Ethernet receive logic. Packets are received and
    stored in the receive buffer as controlled by the filter configuration.

  Precondition:
    All receive registers must be configured before calling this function.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxDisable(ETH_MODULE_ID index)

  Summary:
    Disables the Ethernet module receive logic.

  Description:
    This function disables the Ethernet receive logic.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Disabling Ethernet receive is not recommended for making changes to any
    receive-related registers. After the receiver has been enabled,
    the Ethernet module must be reinitialized to implement changes.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_RxIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module receive enable status.

  Description:
    This function returns the Ethernet module receive enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module receive is enabled
    - false - Ethernet module receive is disabled

  Example:
    <code>
    stat = PLIB_ETH_ReceiveIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_RxIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_AutoFlowControlEnable(ETH_MODULE_ID index)

  Summary:
    Enables the Ethernet Automatic Flow Control logic.

  Description:
    This function enables Ethernet Automatic Flow Control logic.
	This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_AutoFlowControlEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The full and empty watermarks are used to automatically enable and disable
    flow control, respectively. When the number of received buffers rises to
    the full watermark, flow control is automatically enabled.
    When the receive buffer count falls to the empty watermark, flow control is
    automatically disabled.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_AutoFlowControlEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_AutoFlowControlDisable(ETH_MODULE_ID index)

  Summary:
    Disables the Ethernet module Automatic Flow Control logic.

  Description:
    This function disables the Ethernet Automatic Flow Control logic.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_AutoFlowControlDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_AutoFlowControlDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_AutoFlowControlIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module Automatic Flow Control status.

  Description:
    This function gets the Ethernet Automatic Flow Control enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module Automatic Flow Control is enabled
    - false - Ethernet module Automatic Flow Control is disabled

  Example:
    <code>
    stat = PLIB_ETH_AutoFlowControlIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_AutoFlowControlIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ManualFlowControlEnable(ETH_MODULE_ID index)

  Summary:
    Enables the Ethernet Manual Flow Control logic.

  Description:
    This function enables the Ethernet Manual Flow Control logic. Enabling Manual 
    Flow Control will send a Pause frame using the Pause Timer value. While enabled, 
    a Pause frame is repeated every 128 x (Pause timer value)/2 transmit clock cycles.

    Affects both transmit and receive operations.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ManualFlowControlEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ManualFlowControlEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ManualFlowControlDisable(ETH_MODULE_ID index)

  Summary:
    Disable Ethernet module Manual Flow Control logic.

  Description:
    This function disables the Ethernet Manual Flow Control logic and automatically 
    sends a Pause frame with a 0x0000 Pause Timer value. This function affects
    both transmit and receive operations.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ManualFlowControlDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Disabling Manual Flow Control will automatically send a Pause frame with
    a 0x0000 Pause Timer value to disable flow control.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ManualFlowControlDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_ManualFlowControlIsEnabled(ETH_MODULE_ID index)

 Summary:
    Gets the Ethernet module Manual Flow Control enable status.

  Description:
    This function returns the Ethernet module Manual Flow Control enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet module Manual Flow Control is enabled
    - false - Ethernet module Manual Flow Control is disabled

  Example:
    <code>
    stat = PLIB_ETH_ManualFlowControlIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ManualFlowControlIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxBufferCountDecrement(ETH_MODULE_ID index)

  Summary:
    Causes the Receive Descriptor Buffer Counter to decrement by 1.

  Description:
    This function causes the Receive Descriptor Buffer Counter to decrement by 1.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxBufferCountDecrement(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Hardware increments the receive buffer counter and software decrements it.
    If the receive buffer counter is incremented by the receive logic
    at the same time, the receive buffer counter will appear unchanged.

    Always reads as '0', so there is no get value routine.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxBufferCountDecrement(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ReceiveBufferSizeSet(ETH_MODULE_ID index, uint8_t ReceiveBufferSize)

  Summary:
    Sets the Ethernet module receive buffer size.

  Description:
    This function sets the Ethernet receive buffer size. The buffer size
    is set from 16 bytes up to 2032 bytes in increments of 16 bytes each.

  Precondition:
    None.

  Parameters:
    index             - Identifier for the device instance
    ReceiveBufferSize - In units of 16 bytes, must be a value of 0x01 to 0x7F, 
                        0x00 is invalid
  
  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveBufferSizeSet(MY_ETH_INSTANCE, ReceiveBufferSize/16 );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReceiveBufferSizeSet(ETH_MODULE_ID index, uint8_t ReceiveBufferSize);

//******************************************************************************
/* Function:
    int PLIB_ETH_RxSetBufferSize(ETH_MODULE_ID index, int rxBuffSize)

  Summary:
    Sets the Ethernet module receive buffer size.

  Description:
   This function sets the required buffer size for the receive operation.
    In this implementation, all receive descriptors use the same buffer 
	size (unlike the transmission flow where each descriptor can have 
	a different buffer size)

  Precondition:
    0 < rxBuffSize <= 2032

  Parameters:
    index             - Identifier for the device instance
    ReceiveBufferSize - In units of 16 bytes, must be a value of 0x01 to 0x7F, 
                        0x00 is invalid
  
  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveBufferSizeSet(MY_ETH_INSTANCE, ReceiveBufferSize/16 );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
	This function should be part of the initialization process and should
	NOT be called when the rx process is active! The receive buffer size 
	is always TRUNCATED to a multiple of 16 bytes.
*/
int PLIB_ETH_RxSetBufferSize(ETH_MODULE_ID index,int rxBuffSize);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_ReceiveBufferSizeGet(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module receive buffer size.

  Description:
    This function gets the Ethernet receive buffer size. The buffer size
    is set from 16 bytes up to 2032 bytes in increments of 16 bytes each.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    ReceiveBufferSize - receive buffer size divided by 16

  Example:
    <code>
    size = PLIB_ETH_ReceiveBufferSizeGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_ReceiveBufferSizeGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t *txPacketDescStartAddr)

  Summary:
    Sets the Ethernet module transmit packet descriptor start address.

  Description:
    This function sets the Ethernet transmit packet descriptor start address.

  Precondition:
    No transmit, receive, or DMA operations should be in progress when this
    function is called. Call this function before enabling transmit and
    receive.

  Parameters:
    index                 - Identifier for the device instance
    txPacketDescStartAddr - This address must be 4-byte aligned. (The least 
                            significant 2 bits must be '00'.)

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxPacketDescAddrSet(MY_ETH_INSTANCE, txPacketDescStartAddr)
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t *txPacketDescStartAddr);


//******************************************************************************
/* Function:
    uint8_t *PLIB_ETH_TxPacketDescAddrGet(ETH_MODULE_ID index)

  Summary:
    Gets the address of the next descriptor to be transmitted.

  Description:
    This function gets the address of the next transmit descriptor.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    Transmit Packet Descriptor Start Address.

  Example:
    <code>
    txPacketDescAddr = PLIB_ETH_TxPacketDescAddrGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t *PLIB_ETH_TxPacketDescAddrGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t *rxPacketDescStartAddr)

   Summary:
    Sets the Ethernet module receive packet descriptor start address.

  Description:
    This function sets the Ethernet receive packet descriptor start address.

  Precondition:
    No transmit, receive, or DMA operations should be in progress when this
    function is called. Call this function before enabling transmit and
    receive.

  Parameters:
    index                 - Identifier for the device instance
    rxPacketDescStartAddr - This address must be 4-byte aligned. (The least 
                            significant 2 bits must be '00'.)

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxPacketDescAddrSet(MY_ETH_INSTANCE, rxPacketDescStartAddr)
    </code>

  Remarks:
   This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t *rxPacketDescStartAddr);



//******************************************************************************
/* Function:
    uint8_t *PLIB_ETH_RxPacketDescAddrGet(ETH_MODULE_ID index)

  Summary:
    Gets the address of the next receive descriptor.

  Description:
    This function gets the address of the next receive descriptor to be used.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - ReceivePacketDescriptorAddress - receive packet descriptor address

  Example:
    <code>
    RxPacketDescAddr = PLIB_ETH_RxPacketDescAddrGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t * PLIB_ETH_RxPacketDescAddrGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_HashTableSet(ETH_MODULE_ID index, uint64_t hashTableValue)

  Summary:
    Sets the Ethernet module Hash table with the new value.

  Description:
    This function sets the Hash table with the new value.

  Precondition:
    Call PLIB_ETH_HashTableFilterDisable to disable the Hash table filter
    before changing the hash table, or set the Hash Table prior to calling 
    PLIB_ETH_ReceiveEnable.

  Parameters:
    index - Identifier for the device instance
    hashTableValue - hash table value (64-bits)

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_HashTableSet(MY_ETH_INSTANCE, hashTableValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_HashTableSet(ETH_MODULE_ID index, uint64_t hashTableValue);



//******************************************************************************
/* Function:
    void  PLIB_ETH_RxFiltersHTSet ( ETH_MODULE_ID index, uint64_t htable )

  Summary:
    Sets the hash table for the hash table RX filter.

  Description:
     This function sets the hash table for the hash table RX filter.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled and the
    hash filter is active.

  Parameters:
    index       - Identifier for the device instance.
    htable      - The hash table itself

  Returns:
    None.

  Example:
     <code>
    </code>

  Remarks:
    Properly set the hash table with this function before enabling the Hash
    Table filter.
*/
void  PLIB_ETH_RxFiltersHTSet ( ETH_MODULE_ID index, uint64_t htable );

//******************************************************************************
/* Function:
    uint64_t PLIB_ETH_HashTableGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the Hash table.

  Description:
    This function gets the value of the Hash table.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - hashTable - Hash table value (64-bits)

  Example:
    <code>
    value = PLIB_ETH_HashTableGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint32_t PLIB_ETH_HashTableGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PatternMatchSet(ETH_MODULE_ID index, uint64_t patternMatchMaskValue)

  Summary:
    Sets the Ethernet module pattern match mask register with the new value.

  Description:
    This function sets the pattern match mask register with the new value.

  Precondition:
    Call PLIB_ETH_PatternMatchModeSet(index,ETH_PATTERN_MATCH_DISABLED) to
    disable PatternMatch before changing the pattern match mask,
    or set the value before calling PLIB_ETH_ReceiveEnable.

  Parameters:
    index - Identifier for the device instance
    patternMatchMaskValue - Pattern Match Mask Values (64-bits)

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PatternMatchSet(MY_ETH_INSTANCE, patternMatchMaskValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PatternMatchSet(ETH_MODULE_ID index, uint64_t patternMatchMaskValue);


//******************************************************************************
/* Function:
    uint64_t PLIB_ETH_PatternMatchGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the selected pattern match mask register.

  Description:
    This function gets the selected value of the patter match mask register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - patternMatchMaskValue - Pattern Match Mask Values

  Example:
    <code>
    value = PLIB_ETH_PatternMatchGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint64_t PLIB_ETH_PatternMatchGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PatternMatchChecksumSet(ETH_MODULE_ID index, uint16_t PatternMatchChecksumValue)

  Summary:
    Sets the Ethernet module pattern match checksum register with the new value.

  Description:
    This function sets the pattern match checksum register with the new value.

  Precondition:
    Call PLIB_ETH_PatternMatchModeSet(ETH_PATTERN_MATCH_DISABLED) to
    disable PatternMatch before changing the pattern match mask,
    or set the value before calling PLIB_ETH_ReceiveEnable.

  Parameters:
    index - Identifier for the device instance to be configured
    PatternMatchChecksumValue - Pattern match checksum value

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PatternMatchChecksumSet(MY_ETH_INSTANCE, PatternMatchChecksumValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PatternMatchChecksumSet(ETH_MODULE_ID index, uint16_t PatternMatchChecksumValue);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_PatternMatchChecksumGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the pattern match checksum register.

  Description:
    This function gets the value of the patter match checksum register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    The pattern match checksum.

  Example:
    <code>
    value = PLIB_ETH_PatternMatchChecksumGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_PatternMatchChecksumGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PatternMatchOffsetSet(ETH_MODULE_ID index, uint16_t PatternMatchOffsetValue)

  Summary:
    Sets the Ethernet module patter match offset register with the new value.

  Description:
    This function sets the pattern match offset register with the new value.

  Precondition:
    Call PLIB_ETH_PatternMatchModeSet(ETH_PATTERN_MATCH_DISABLED) to
    disable PatternMatch before changing the pattern match mask,
    or set the value before calling PLIB_ETH_ReceiveEnable.

  Parameters:
    index - Identifier for the device instance
    PatternMatchOffsetValue - Pattern match offset value

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PatternMatchOffsetSet(MY_ETH_INSTANCE, PatternMatchOffsetValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PatternMatchOffsetSet(ETH_MODULE_ID index, uint16_t PatternMatchOffsetValue);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_PatternMatchOffsetGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the patter match offset register.

  Description:
    This function gets the value of the patter match offset register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - PatternMatchOffsetValue - Pattern match offset value

  Example:
    <code>
    value = PLIB_ETH_PatternMatchOffsetGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/
uint16_t PLIB_ETH_PatternMatchOffsetGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PatternMatchModeSet(ETH_MODULE_ID index, ETH_PATTERN_MATCH_MODE modeSel)

  Summary:
    Sets the Ethernet module pattern match mode.

  Description:
    This function sets the pattern match mode.

  Precondition:
    Set the value before calling PLIB_ETH_ReceiveEnable().

  Parameters:
    index - Identifier for the device instance
    modeSel - The method of pattern matching from ETH_PATTERN_MATCH_DISABLED

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PatternMatchModeSet(MY_ETH_INSTANCE, ETH_PATTERN_MATCH_DISABLED);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PatternMatchModeSet(ETH_MODULE_ID index, ETH_PATTERN_MATCH_MODE modeSel);

//******************************************************************************
/* Function:
    void PLIB_ETH_RxFiltersPMClr ( ETH_MODULE_ID index )

  Summary:
    Disables the Pattern Match receive filter.

  Description:
    This function disables the Pattern Match receive filter.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled.

  Parameters:
     index       - Identifier for the device instance.

  Returns:
     None.

  Example:
    <code>
    PLIB_ETH_RxFiltersPMClr(index);
    </code>

  Remarks:
 */
void PLIB_ETH_RxFiltersPMClr ( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    ETH_PATTERN_MATCH_MODE PLIB_ETH_PatternMatchModeGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the selected pattern match mask register.

  Description:
    This function gets the selected value of the patter match mask register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - modeSel - The method of pattern matching from ETH_PATTERN_MATCH_DISABLED

  Example:
    <code>
    value = PLIB_ETH_PatternMatchModeGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

ETH_PATTERN_MATCH_MODE PLIB_ETH_PatternMatchModeGet(ETH_MODULE_ID index);



//******************************************************************************
/* Function:
    void PLIB_ETH_ReceiveFilterEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)

  Summary:
    Enables the specified receive filter.

  Description:
    This function enables the specified receive filter.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance
    filter - The selection of receive filters to be enabled from the enumerated 
             selection ETH_RECEIVE_FILTER

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveFilterEnable(index,
                                 ETH_CRC_OK_FILTER      |
                                 ETH_RUNT_ENABLE_FILTER |
                                 ETH_UNICAST_FILTER     );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReceiveFilterEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxFiltersSet (ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters )

  Summary:
    Sets the acceptance/rejection filters for the Ethernet receive.

  Description:
    This function sets the acceptance/rejection filters for the Ethernet receive
    mechanism.  Multiple filters can be OR-ed together.  All filter specified
    in rxFilters will be set.

  Precondition:
   This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    index       - Identifier for the device instance.
    rxFilters   - RX filters that are to be set

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFiltersSet(index,
                    ETH_FILT_CRC_ERR_REJECT | ETH_FILT_RUNT_REJECT  |
                    ETH_FILT_ME_UCAST_ACCEPT| ETH_FILT_MCAST_ACCEPT |
                    ETH_FILT_BCAST_ACCEPT   | ETH_FILT_NOTME_UCAST_ACCEPT);
    </code>

  Remarks:
    All filters except Pattern Match can be set. Use PLIB_ETH_RxFiltersPMSet().
    Before enabling the Hash Table filter, the hash table values should be set
    using PLIB_ETH_RxFiltersHTSet().
    See the definition of the ETH_RX_FILTERS for the priority of the RX filters.
*/
void PLIB_ETH_RxFiltersSet (ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters );

//******************************************************************************
/* Function:
    void PLIB_ETH_ReceiveFilterDisable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)

  Summary:
    Disables the specified receive filter.

  Description:
    This function disables the specified receive filter.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance
    filter - The selection of receive filters to be disabled from the enumerated 
             selection ETH_RECEIVE_FILTER

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveFilterDisable(index,
                                  ETH_CRC_OK_FILTER      |
                                  ETH_RUNT_ENABLE_FILTER |
                                  ETH_UNICAST_FILTER     );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReceiveFilterDisable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxFiltersClr (ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters )

  Summary:
     Clears the acceptance/rejection filters for the Ethernet receive.

  Description:
    This function clears the acceptance/rejection filters for the Ethernet receive
    mechanism.  Multiple filters can be OR-ed together.  All filters specified
    in rxFilters will be cleared.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    index       - Identifier for the device instance.
    rxFilters   - RX filters that are to be cleared

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFiltersClr(index,ETH_FILT_ALL_FILTERS);
    </code>

  Remarks:
    All filters except Pattern Match can be cleared. Use PLIB_ETH_RxFiltersPMClr().
*/
void PLIB_ETH_RxFiltersClr (ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters );

//******************************************************************************
/* Function:
    bool PLIB_ETH_ReceiveFilterIsEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)

  Summary:
    Disables the specified receive filter.

  Description:
    This function disables the specified receive filter.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance
    filter - The selection of receive filters to be disabled from the enumerated 
             selection ETH_RECEIVE_FILTER

  Returns:
    - true  - If at least one of the specified filters is enabled
    - false - If no specified filter is enabled

  Example:
    <code>
    if (PLIB_ETH_ReceiveFilterIsEnable(index, ETH_UNICAST_FILTER))
    {
        PLIB_ETH_ReceiveFilterDisable(MY_MODULE_ID, ETH_UNICAST_FILTER);
    }
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ReceiveFilterIsEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter);

//******************************************************************************
/* Function:
    void PLIB_ETH_RxFiltersWrite ( ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters )

  Summary:
    Updates the acceptance/rejection filters for the Ethernet receive.

  Description:
    This function updates the acceptance/rejection filters for the Ethernet receive
    mechanism to the required value.


  Precondition:
    This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    index       - Identifier for the device instance.
    rxFilters       - RX filters that are to be written

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFiltersWrite(index, ETH_FILT_ALL_FILTERS);
    </code>
	
  Remarks:
    All filters except Pattern Match can be updated.

    The Pattern Match filter will be disabled by this call.  Use
    PLIB_ETH_RxFiltersPMSet();

    Before enabling the Hash Table filter, the hash table values should be set
    using PLIB_ETH_RxFiltersHTSet().

    See the definition of the ETH_RX_FILTERS for the priority of the
    RX filters.
*/
void PLIB_ETH_RxFiltersWrite ( ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters );


//*******************************************************************************
/*  Function:
    void PLIB_ETH_RxFiltersPMSet ( ETH_MODULE_ID index,
                                   ETH_PMATCH_MODE mode,
                                   uint64_t matchMask,
                                   uint32_t matchOffs,
                                   uint32_t matchChecksum )

  Summary:
    Enables the Pattern Match filter with the specified settings.

  Description:
    This function enables the Pattern Match filter with the specified settings
    for the offset and window mask. The calculated checksum of the 64 bytes
    window starting at matchOffs and using the matchMask bytes in the window,
    is compared against matchChecksum.  If ETH_FILT_PMATCH_INVERT is used in
    the mode, then the calculated checksum must not match the passed
    matchChecksum in order for the Pattern Match to succeed.

  Precondition:
    This function should not be called when Ethernet RX operation is enabled.

  Parameters:
    index       - Identifier for the device instance.
    mode            - The required Pattern Match Mode

    matchMask       - Mask in the 64 byte window

    matchOffs       - The offset applied to the incoming data (0 to 8128)

    matchChecksum   - The 16 bit checksum used for comparison

  Returns:

  Example:
    <code>
    PLIB_ETH_RxFiltersPMSet(index, ETH_FILT_PMATCH_ACCEPT, MY_PATTERN, 0, MY_CHECKSUM);
    </code>

  Remarks:
 */

void PLIB_ETH_RxFiltersPMSet ( ETH_MODULE_ID index,
                              ETH_PMATCH_MODE mode, uint64_t matchMask,
                              uint32_t matchOffs, uint32_t matchChecksum );


//******************************************************************************
/* Function:
    void PLIB_ETH_RxFullWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue)

  Summary:
    Sets the Ethernet module to receive a full watermark.

  Description:
    This function sets the Ethernet to receive a full watermark with a new value.

    The software controlled RX Buffer Full Watermark (Wmark) Pointer is compared
    against the receive buffer count to determine the full watermark
    condition for the full watermark interrupt and for enabling flow control
    if Auto Flow Control is enabled.

    The Full Wmark value should always be greater than the Empty Wmark value.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    watermarkValue - Full watermark value

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFullWmarkSet(MY_ETH_INSTANCE, watermarkValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxFullWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_RxFullWmarkGet(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module to receive a full watermark.

  Description:
    This function gets the Ethernet to receive a full watermark.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - receiveFullWmark - Full watermark value

  Example:
    <code>
    watermarkValue = PLIB_ETH_RxFullWmarkGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_RxFullWmarkGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxEmptyWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue)

  Summary:
    Sets the Ethernet module receive empty water mark.

  Description:
    This function sets the Ethernet receive empty water mark with a new value.
    The software controlled Receive Buffer Empty Wmark is compared against
    the receive buffer count to determine the empty watermark condition
    for the empty watermark interrupt and for disabling flow control
    if Auto Flow Control is enabled.

    The Full Wmark value should always be greater than the Empty Wmark value.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    watermarkValue - Empty watermark value

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxEmptyWmarkSet(MY_ETH_INSTANCE, watermarkValue);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxEmptyWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue);



//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_RxEmptyWmarkGet(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module receive empty watermark.

  Description:
    This function gets the Ethernet receive empty watermark.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - receiveEmptyWmark - Empty watermark value

  Example:
    <code>
    watermarkValue = PLIB_ETH_RxEmptyWmarkGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_RxEmptyWmarkGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_InterruptSourceEnable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)

  Summary:
    Sets the Ethernet module Interrupt Enable register in a group, using a mask.

  Description:
    This function enables the Ethernet module interrupts by using a mask to enable
    one or multiple interrupt enables. Logically 'OR' elements of ETH_INTERRUPT_SOURCES 
    together.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    intmask - Members of ETH_INTERRUPT_SOURCES logically ORed together

  Returns:
    None.

  Example:
    <code>
    // To enable some interrupts with one command, execute the following:
    PLIB_ETH_InterruptSourceEnable(MY_ETH_INSTANCE,
                                        ETH_TRANSMIT_DONE_INTERRUPT                |
                                        ETH_TRANSMIT_ABORT_INTERRUPT               |
                                        ETH_RECEIVE_BUFFER_NOT_AVAILABLE_INTERRUPT |
                                        ETH_RECEIVE_FIFO_OVERFLOW_ERROR_INTERRUPT);

    // Or to enable one interrupt with one command, execute the following:
    PLIB_ETH_InterruptSourceEnable(MY_ETH_INSTANCE, ETH_TRANSMIT_DONE_INTERRUPT);

    // Or to enable all interrupts with one command, execute the following:
    PLIB_ETH_InterruptSourceEnable(MY_ETH_INSTANCE, ETH_ALL_ETH_INTERRUPTS);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_InterruptSourceEnable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);


//******************************************************************************
/* Function:
    void PLIB_ETH_EventsEnableSet ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents )

  Summary:
    Enables the events that will generate interrupts for the Ethernet controller.

  Description:
    The function enables the events that will generate interrupts for the Ethernet
    controller.  Multiple events can be OR-ed together.  Any event that is set
    in the eEvents will be enabled , the other events won't be touched.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance.
    eEvents  - Events with the significance described in the PLIB_ETH_EVENTS definition.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_EventsEnableSet(index,ETH_EV_ALL);
    </code>

  Remarks:
 */

void PLIB_ETH_EventsEnableSet ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents );


//******************************************************************************
/* Function:
    void PLIB_ETH_InterruptSourceDisable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)

  Summary:
    Clears the Ethernet module Interrupt Enable register as a group, using a mask.

  Description:
    This function disables the Ethernet module interrupts by using a mask to select
    one or multiple interrupts to disable. Logically 'OR' elements of 
    ETH_INTERRUPT_SOURCES together.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    intmask - Members of ETH_INTERRUPT_SOURCES logically ORed together

  Returns:
    None.

  Example:
    <code>
    // To disable some interrupts with one command, execute the following:
    PLIB_ETH_InterruptSourceDisble(MY_ETH_INSTANCE,
                                        ETH_TRANSMIT_DONE_INTERRUPT                |
                                        ETH_TRANSMIT_ABORT_INTERRUPT               |
                                        ETH_RECEIVE_BUFFER_NOT_AVAILABLE_INTERRUPT |
                                        ETH_RECEIVE_FIFO_OVERFLOW_ERROR_INTERRUPT );

    // Or to disable one interrupt with one command, execute the following:
    PLIB_ETH_InterruptSourceDisble(MY_ETH_INSTANCE, ETH_TRANSMIT_DONE_INTERRUPT);

    // Or to disable all interrupts with one command, execute the following:
    PLIB_ETH_InterruptSourceDisble(MY_ETH_INSTANCE, ETH_ALL_ETH_INTERRUPTS);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_InterruptSourceDisable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);

//*******************************************************************************
/*  Function:
    void PLIB_ETH_EventsEnableClr ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents )

  Summary:
    Disables the events that will generate interrupts.

  Description:
    The function disables the events that will generate interrupts for the Ethernet
    controller.  Multiple events can be OR-ed together.  Any event that is set
    in the eEvents will be disabled , the other events won't be touched.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance.
    eEvents  - Events with the significance described in the PLIB_ETH_EVENTS definition.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_EventsEnableClr(index,ETH_EV_ALL);
    </code>

  Remarks:
*/

void PLIB_ETH_EventsEnableClr ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents );



//*******************************************************************************
/*  Function:
    void PLIB_ETH_EventsEnableWrite ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents )

  Summary:
    Enables the events that will generate interrupts for the Ethernet controller.

  Description:
    This function enables the events that will generate interrupts for the Ethernet
    controller.  The enabled events are forced to the eEvents value.

  Precondition:
    None.

  Parameters:
    index   - Identifier for the device instance.
    eEvents - Events with the significance described in the PLIB_ETH_EVENTS definition.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_EventsEnableWrite( index,
                          ETH_EV_RXOVFLOW | ETH_EV_RXBUFNA | ETH_EV_TXABORT  |
                          ETH_EV_RXACT    | ETH_EV_PKTPEND | ETH_EV_RXBUSERR |
                          ETH_EV_TXBUSERR );
    </code>

  Remarks:
 */

void PLIB_ETH_EventsEnableWrite ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents );

//******************************************************************************
/* Function:
    bool PLIB_ETH_InterruptSourceIsEnabled(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)

  Summary:
    Gets the Ethernet module Interrupt Enable register singularly or as a group.

  Description:
     This function returns a true if any of the specified interrupt sources are 
     enabled.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    mask  - The interrupt mask(s) to be checked

  Returns:
    - true  - If any of the specified sources are enabled
    - false - If none of the specified sources are enabled

  Example:
    <code>
    if ( PLIB_ETH_InterruptSourceIsEnabled(index, ETH_TRANSMIT_DONE_INTERRUPT))
    {
        //If the interrupt is enabled, disable it...
        PLIB_ETH_InterruptSourceDisable(index, ETH_TRANSMIT_DONE_INTERRUPT);
    }
   </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_InterruptSourceIsEnabled(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);


//******************************************************************************
/* Function:
    ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptSourcesGet(ETH_MODULE_ID index)

  Summary:
    Returns the entire interrupt enable register.

  Description:
    This function returns the entire interrupt enable register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    Bit map of interrupt sources, all ORed together, as defined by
    ETH_INTERRUPT_SOURCES.

  Example:
    <code>
   </code>

  Remarks:
    None.
*/

ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptSourcesGet(ETH_MODULE_ID index);


//*******************************************************************************
/*  Function:
    PLIB_ETH_EVENTS PLIB_ETH_EventsEnableGet ( ETH_MODULE_ID index )

  Summary:
    Returns the enabled events for the Ethernet controller.

  Description:
    This function returns the enabled events for the Ethernet controller.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    index   - Identifier for the device instance.
    eEvents - events with the significance described in the PLIB_ETH_EVENTS
    definition.

  Example:
    <code>
    ethEnabledEvents = PLIB_ETH_EventsEnableGet(index);
    </code>

  Remarks:
*/

PLIB_ETH_EVENTS PLIB_ETH_EventsEnableGet ( ETH_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_ETH_InterruptSet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)

  Summary:
    Sets the Ethernet module interrupt source status as a group, using a mask.

  Description:
    This function sets the Ethernet module interrupt source status using a mask.
    Logically 'OR' the masks together. Any masks not OR'd with the others will 
    be disabled.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    intmask - Members of ETH_INTERRUPT_SOURCES logically ORed together

  Returns:
    None.

  Example:
    <code>
    // To set several interrupt flags with one command, execute the following:
    PLIB_ETH_InterruptSet(MY_ETH_INSTANCE,
                                        ETH_EMPTY_WATERMARK_INTERRUPT      |
                                        ETH_FULL_WATERMARK_INTERRUPT       |
                                        ETH_RX_FIFO_OVERFLOW_ERROR_INTERRUPT );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_InterruptSet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);


//******************************************************************************
/* Function:
    void PLIB_ETH_InterruptClear(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)

  Summary:
    Clears the Ethernet module interrupt source status as a group, using a mask.

  Description:
    This function clears the Ethernet module interrupt source status using a mask.
    Logically 'OR' the masks together. Any masks not ORed with the others will 
    be disabled.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    intmask - Members of ETH_INTERRUPT_SOURCES logically ORed together

  Returns:
    None.

  Example:
    <code>
    // To clear several interrupts with one command, execute the following:
    PLIB_ETH_InterruptClear(MY_ETH_INSTANCE,
                                        ETH_EMPTY_WATERMARK_INTERRUPT      |
                                        ETH_FULL_WATERMARK_INTERRUPT       |
                                        ETH_RX_FIFO_OVERFLOW_ERROR_INTERRUPT );
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_InterruptClear(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);

//*******************************************************************************
/*  Function:
    void PLIB_ETH_EventsClr ( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents )

  Summary:
    Clears the selected events for the Ethernet controller.

  Description:
    The function clears the selected events for the Ethernet controller.  Multiple
    events can be OR-ed together. Any event that is set in the eEvents will be
    cleared, the other events won't be touched.

  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance.
    eEvents  - Events with the significance described in the PLIB_ETH_EVENTS definition.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_EventsClr(index,ETH_EV_TXDONE);
    </code>

  Remarks:
    The ETH_EV_FWMARK cannot be cleared directly. It is cleared indirectly by
    PLIB_ETH_RxAcknowledgePacket/PLIB_ETH_RxAcknowledgeBuffer.

    The ETH_EV_EWMARK cannot be cleared directly. It is cleared by hardware when
    receiving a new packet.

    The ETH_EV_PKTPEND cannot be cleared directly. It is cleared indirectly by
    PLIB_ETH_RxAcknowledgePacket/PLIB_ETH_RxAcknowledgeBuffer.

*/

void PLIB_ETH_EventsClr( ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents );


//******************************************************************************
/* Function:
bool PLIB_ETH_InterruptStatusGet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);

  Summary:
    Gets the Ethernet module Interrupt Flag register as a group, using a mask.

  Description:
    This function gets the Ethernet module Interrupt Flag register using a mask.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    intmask - Members of ETH_INTERRUPT_SOURCES logically ORed together

  Returns:
    - true  - If any of the specified source statuses are enabled
    - false - If none of the specified source statuses are enabled

  Example:
    <code>
    if (PLIB_ETH_InterruptStatusGet(index, ETH_TRANSMIT_DONE_INTERRUPT))
    {
        PLIB_ETH_InterruptClear(index, ETH_TRANSMIT_DONE_INTERRUPT);
    }
   </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_InterruptStatusGet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask);


//******************************************************************************
/* Function:
    ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptsGet(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module Interrupt Flag register as a group.

  Description:
    Gets the Ethernet module Interrupt Flag register as a group.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    Interrupt bit map, as defined in ETH_INTERRUPT_SOURCES, showing which 
    interrupts have fired.

  Example:
    <code>
   </code>

  Remarks:
    none.
*/

ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptsGet(ETH_MODULE_ID index);


/*******************************************************************************
  Function:
    PLIB_ETH_EVENTS PLIB_ETH_EventsGet ( ETH_MODULE_ID index )

  Summary:
    Returns the active events for the Ethernet controller.

  Description:
    This function returns the active events for the Ethernet controller.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    Events with the significance described in the PLIB_ETH_EVENTS definition.

  Example:
    <code>
    ethEvents = PLIB_ETH_EventsGet(index);
    </code>

  Remarks:
 *****************************************************************************/

PLIB_ETH_EVENTS PLIB_ETH_EventsGet ( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_RxPacketCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the value of the receive packet buffer count.

  Description:
    This function gets the value of the receive packet buffer count.
    When a packet has been successfully received, this value is incremented
    by hardware. Software decrements the counter after a packet has been read.
    Call  PLIB_ETH_ReceiveBufferCountDecrement(ETH_MODULE_ID index) to decrement.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - RxPacketCount - Number of received packets in memory

  Example:
    <code>
    count = PLIB_ETH_RxPacketCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Receive Packet Buffer Counter cannot be decremented below 0x00
    Cleared when the Ethernet module is reset.

    The Receive Packet Buffer Count is not set to '0' when the Ethernet module
    is turned OFF. This allows software to continue to utilize and decrement
    the count.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_RxPacketCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_EthernetIsBusy(ETH_MODULE_ID index)

  Summary:
    Gets the status value of the Ethernet logic busy.

  Description:
    This function sets the value of the Ethernet logic busy. A request indicates 
    that the module has just been turned ON or is completing a transaction after 
    being turned OFF.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Ethernet logic has been turned ON or is completing a transaction
    - false - Ethernet logic is idle

  Example:
    <code>
    stat = PLIB_ETH_EthernetIsBusy(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_EthernetIsBusy(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TransmitIsBusy(ETH_MODULE_ID index)

  Summary:
    Gets the status value of the Ethernet transmit logic busy status

  Description:
    This function gets the value of the Ethernet transmit logic busy status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Transmit logic is sending data
    - false - Transmit logic is idle

  Example:
    <code>
    stat = PLIB_ETH_TransmitIsBusy(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TransmitIsBusy(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_ReceiveIsBusy(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet receive logic busy status.

  Description:
    This function gets the Ethernet receive logic busy status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Receive logic is receiving data
    - false - Receive logic is idle

  Example:
    <code>
    stat = PLIB_ETH_ReceiveIsBusy(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ReceiveIsBusy(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxOverflowCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the value of the Ethernet receive overflow count to zero.

  Description:
    This function sets the value of the Ethernet receive overflow count to zero.
    Increment counter for frames accepted by the RX filter and subsequently
    dropped due to internal receive error. This event also sets the receive 
    overflow interrupt flag.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxOverflowCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Setting or clearing any bits in this register
    should be done for debug/test purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxOverflowCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_RxOverflowCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of the dropped Ethernet receive frames.

  Description:
    This function gets the count of the dropped Ethernet receive frames.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - count - uint16_t receiver overflow counts

  Example:
    <code>
    count = PLIB_ETH_RxOverflowCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_RxOverflowCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FramesTxdOkCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the count of Ethernet Control frames transmitted to zero.

  Description:
    This function sets the count of Ethernet Control frames transmitted to zero.
	This function performs atomic register access. 

	Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - Count of control frames transmitted correctly

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FramesTxdOkCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Setting or clearing any bits in this register should be done for 
    debug/test purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FramesTxdOkCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_FramesTxdOkCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of Ethernet Control Frames transmitted successfully.

  Description:
    This function gets the count of Ethernet Control Frames transmitted successfully.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - count - count of control frames transmitted correctly

  Example:
    <code>
    count = PLIB_ETH_FramesTxdOkCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_FramesTxdOkCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_SingleCollisionCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the value of the Ethernet single collision frame count to zero.

  Description:
    This function sets the value of the Ethernet single collision frame count to zero.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - Count of single collision frames

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_SingleCollisionCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_SingleCollisionCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_SingleCollisionCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of the frames transmitted successfully on a second attempt.

  Description:
    This function gets the count of the frames transmitted successfully on
    a second attempt.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - count - Count of frames transmitted successfully on second attempt

  Example:
    <code>
    count = PLIB_ETH_SingleCollisionCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_SingleCollisionCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MultipleCollisionCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the value of the Ethernet multiple collision frame count to zero.
	
  Description:
    This function sets the value of the Ethernet multiple collision frame count to zero.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - Count of multiple collision frames

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MultipleCollisionCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Setting or clearing any bits in this register should be done for 
    debug/test purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MultipleCollisionCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_MultipleCollisionCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of the frames transmitted successfully after there was more 
    than one collision.

  Description:
    This function gets the count of the frames transmitted successfully after 
    there was more than one collision.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - count - Count of multiple collision frames

  Example:
    <code>
    count = PLIB_ETH_MultipleCollisionCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_MultipleCollisionCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FramesRxdOkCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the value of the Ethernet received frames 'OK' count to zero.

  Description:
    This function sets the value of the Ethernet frames 'OK' count to zero.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - Count of frames received correctly

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FramesRxdOkCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Changing the value of this register should be done for debug/test 
    purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FramesRxdOkCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_FramesRxdOkCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of the frames frames received successfully.

  Description:
    This function gets the count of the frames received successfully.
    Increment count for frames received successfully by the RX Filter.
    This count will not be incremented if there is a Frame Check Sequence (FCS)
    or Alignment error.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - value - Count of frames received correctly

  Example:
    <code>
    count = PLIB_ETH_FramesRxdOkCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_FramesRxdOkCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FCSErrorCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the value of the Ethernet frame check sequence error to zero.

  Description:
    This function sets the value of the Ethernet frame check sequence error to zero.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - Count of FCS Errors

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FCSErrorCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Setting or clearing any bits in this register should be done for 
    debug/test purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FCSErrorCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_FCSErrorCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of the frame check sequence error.

  Description:
    This function gets the count of the frame check sequence error.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - value - Count of FCS Errors

  Example:
    <code>
    count = PLIB_ETH_FCSErrorCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_FCSErrorCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_AlignErrorCountClear(ETH_MODULE_ID index)

  Summary:
    Sets the count of Ethernet alignment errors to zero.

  Description:
    This function sets the count of Ethernet alignment errors to zero.
	This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    value - count of alignment errors

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_AlignErrorCountClear(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Setting or clearing any bits in this register should be done for 
    debug/test purposes only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_AlignErrorCountClear(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_AlignErrorCountGet(ETH_MODULE_ID index)

  Summary:
    Gets the count of Ethernet alignment errors.

  Description:
    This function gets the count of Ethernet alignment errors.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - value - Count of alignment errors

  Example:
    <code>
    count = PLIB_ETH_AlignErrorCountGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_AlignErrorCountGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC MIIM Soft reset.

  Description:
    This function enables the EMAC MIIM soft reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC Soft reset.

  Description:
    This function disables the EMAC MIIM Soft reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets EMAC MIIM Soft Reset enable status.

  Description:
    This function gets EMAC MIIM Soft reset enable status. By default this bit is 
    set to '1'.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true   - MAC MII is in reset
    - false  - MAC MII is not in reset

  Example:
    <code>
    stat = PLIB_ETH_MIIResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_SimResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC simulation reset.

  Description:
    This function enables the EMAC simulation reset and resets the random number
    generator within the transmit (TX) function.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_SimResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_SimResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_SimResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC simulation reset.

  Description:
    This function disables the EMAC simulation reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_SimResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_SimResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_SimResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC simulation reset status.

  Description:
    This function returns the EMAC simulation reset status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true   - Simulation Reset is enabled
    - false  - Simulation Reset is disabled

  Example:
    <code>
    stat = PLIB_ETH_SimResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_SimResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MCSRxResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the reset EMAC Control Sub-layer/Receive domain logic.

  Description:
    This function enables the reset EMAC Control Sub-layer/Receive domain logic.
    The MCS/Receive domain logic is held in reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MCSRxResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MCSRxResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MCSRxResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the reset EMAC Control Sub-layer/Receive domain logic.

  Description:
    This function disables the reset EMAC Control Sub-layer/Receive domain logic.
    The MCS/Receive domain logic is released from reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MCSRxResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MCSRxResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MCSRxResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Control Sub-layer/Receive domain logic reset status.

  Description:
    This function gets the EMAC Control Sub-layer/Receive domain logic reset status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true   -  EMAC Control Sub-layer/Receive domain logic is in reset
    - false  -  EMAC Control Sub-layer/Receive domain logic is not in reset

  Example:
    <code>
    stat = PLIB_ETH_MCSRxResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MCSRxResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxFuncResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC reset receive function logic.

  Description:
    This function enables the EMAC reset receive function logic. The receive 
    function logic is held in reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFuncResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxFuncResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxFuncResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC reset receive function logic.

  Description:
    This function disables the EMAC reset receive function logic. The reset receive 
    function logic is released from reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxFuncResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxFuncResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_RxFuncResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC reset receive function status.

  Description:
    This function gets the EMAC reset receive function status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  -  EMAC Receive function logic is held in reset
    - false -  EMAC Receive function logic is not in reset

  Example:
    <code>
    stat = PLIB_ETH_RxFuncResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_RxFuncResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MCSTxResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the reset of EMAC Control Sub-layer/Transmit domain logic.

  Description:
    This function resets the EMAC Control Sub-layer/Transmit domain logic.
    The MCS/Transmit domain logic is held in reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MCSTxResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MCSTxResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MCSTxResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the reset EMAC Control Sub-layer/Transmit domain logic.

  Description:
    This function disables the reset EMAC Control Sub-layer/Transmit domain logic.
    The MCS/Transmit domain logic is released from reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MCSTxResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MCSTxResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MCSTxResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Control Sub-layer/Transmit domain logic reset status.

  Description:
    This function gets the EMAC Control Sub-layer/Transmit domain logic reset status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true   -  EMAC Control Sub-layer/Transmit domain logic is held in reset
    - false  -  EMAC Control Sub-layer/Transmit domain logic is not in reset

  Example:
    <code>
    stat = PLIB_ETH_MCSTxResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MCSTxResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxFuncResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC transmit function reset.

  Description:
    This function enables the EMAC transmit function reset. The transmit function 
    is held in reset.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxFuncResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxFuncResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxFuncResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC Transmit function reset.

  Description:
    This function disables the EMAC Transmit function reset. The EMAC transmit 
    function is released from reset for normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxFuncResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxFuncResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TxFuncResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Transmit function reset status.

  Description:
    This function gets the EMAC Transmit function reset status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - EMAC transmit function logic is held in reset
    - false - EMAC transmit function logic is not in reset

  Example:
    <code>
    PLIB_ETH_TxFuncResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TxFuncResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_LoopbackEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC loopback logic.

  Description:
    This function enables the EMAC loopback logic. The EMAC transmit interface is 
    looped back to the EMAC receive interface.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_LoopbackEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_LoopbackEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_LoopbackDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC loopback logic.

  Description:
    This function disables the EMAC loopback. EMAC resumes normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_LoopbackDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_LoopbackDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_LoopbackIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Loopback interface enable status.

  Description:
    This function gets the EMAC Loopback interface enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - EMAC transmit interface is looped back to the EMAC receive interface
    - false - EMAC normal operation

  Example:
    <code>
    stat = PLIB_ETH_LoopbackIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_LoopbackIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxPauseEnable(ETH_MODULE_ID index)

  Summary:
    Enables the transmission Pause frames.

  Description:
    This function enables the transmission Pause frames. The Pause frames are 
    allowed for transmission.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxPauseEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxPauseEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TxPauseDisable(ETH_MODULE_ID index)

  Summary:
    Disables the transmission of Pause frames.

  Description:
    This function disables the transmit Pause frames. The Pause frames are blocked 
    from being transmitted.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TxPauseDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TxPauseDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TxPauseIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet module enable status.

  Description:
    This function gets the Ethernet module enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Pause Flow Control frames are allowed to be transmitted
    - false - Pause Flow Control frames are blocked

  Example:
    <code>
    status = PLIB_ETH_TxPauseIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TxPauseIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxPauseEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC receive flow control.

  Description:
    This function enables the EMAC receive flow control. The EMAC will act upon 
    the received Pause frames.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxPauseEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxPauseEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RxPauseDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC receive flow control.

  Description:
    This function disables the EMAC receive flow control. The received Pause Flow 
    control frames are ignored.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RxPauseDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RxPauseDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_RxPauseIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC receive flow pause enable status.

  Description:
    This function gets the EMAC receive flow pause enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC acts upon received Pause Flow Control frames
    - false - Received Pause Flow Control frames are ignored

  Example:
    <code>
    status = PLIB_ETH_RxPauseIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_RxPauseIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PassAllEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC PassAll.

  Description:
    This function enables the EMAC PassAll, which enables both the normal and
    the control frames to be passed.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PassAllEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PassAllEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PassAllDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC PassAll.

  Description:
    This function disables the EMAC PassAll. Control frames are ignored.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PassAllDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PassAllDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_PassAllIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC PassAll enable status.

  Description:
    This function gets the EMAC PassAll enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC will accept all frames regardless of type
              (normal vs. Control)
    - false - The received Control frames are ignored

  Example:
    <code>
    stat = PLIB_ETH_PassAllIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_PassAllIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ReceiveEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC to receive the frames.

  Description:
    This function enables the EMAC to receive the frames.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReceiveEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ReceiveDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC from receiving frames.

  Description:
    This function disables the EMAC from receiving frames.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReceiveDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReceiveDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_ReceiveIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the Ethernet EMAC receive enable status.

  Description:
    This function gets the Ethernet EMAC receive enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Enable the EMAC to receive frames
    - false - Disable the EMAC to receive frames

  Example:
    <code>
    stat = PLIB_ETH_ReceiveIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ReceiveIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ExcessDeferEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC excess defer.

  Description:
    This function enables EMAC excess defer. The EMAC will defer to the carrier 
    indefinitely.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ExcessDeferEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ExcessDeferEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ExcessDeferDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC excess defer.

  Description:
    This function disables EMAC excess defer. The EMAC will abort when the 
    excessive deferral limit is reached.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ExcessDeferDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ExcessDeferDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_ExcessDeferIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC excess defer enable status.

  Description:
    This function gets the EMAC excess defer enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC will defer to carrier indefinitely as per the Standard
    - false - The EMAC will abort when the excessive deferral limit is reached

  Example:
    <code>
    stat = PLIB_ETH_ExcessDeferIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ExcessDeferIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_BackPresNoBackoffEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC back pressure/no back-off.

  Description:
    This function enables EMAC backpressure/no back-off. The EMAC will not back-off 
    when there is backpressure and collisions occur.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_BackPresNoBackoffEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The EMAC after incidentally causing a collision during backpressure will
    immediately retransmit without back-off reducing the chance of further
    collisions and ensuring transmit packets get sent.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_BackPresNoBackoffEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_BackPresNoBackoffDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC backpressure/no back-off.

  Description:
    This function disables EMAC backpressure/no back-off. The EMAC will back-off 
    when there is backpressure and collisions occur.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_BackPresNoBackoffDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_BackPresNoBackoffDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_BackPresNoBackoffIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC backpressure/no back-off enable status.

  Description:
    This function gets the EMAC backpressure/no back-off enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC after incidentally causing a collision during
              backpressure will immediately retransmit without back-off
              reducing the chance of further collisions and ensuring
              transmit packets get sent
    - false - The EMAC will not remove the back-off

  Example:
    <code>
    stat = PLIB_ETH_BackPresNoBackoffIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_BackPresNoBackoffIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_NoBackoffEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC no back-off.

  Description:
    This function enables EMAC no back-off. The EMAC will not back-off when a 
    collision occurs.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_NoBackoffEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    Following a collision, the EMAC will immediately retransmit rather than
    using the Binary Exponential Back-off algorithm as specified in the Standard.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_NoBackoffEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_NoBackoffDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC no back-offs.

  Description:
    This function disables EMAC no back-off. The EMAC will back-off when a 
    collision occurs.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_NoBackoffDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_NoBackoffDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_NoBackoffIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC no back-off enable status.

  Description:
    This function gets the EMAC no back-off enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Following a collision, the EMAC will immediately retransmit
    - false - Following a collision, the EMAC will use the Binary Exponential 
              Back-off algorithm

  Example:
    <code>
    stat = PLIB_ETH_NoBackoffIsEnabled(MY_ETsH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_NoBackoffIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_LongPreambleEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC long preamble enforcement.

  Description:
    This function enables EMAC long preamble enforcement. The EMAC only allows 
    receive packets which contain preamble fields less than 12 bytes in length.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_LongPreambleEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_LongPreambleEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_LongPreambleDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC long preamble enforcement.

  Description:
    This function disables EMAC long preamble enforcement. The EMAC allows any 
    length preamble.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_LongPreambleDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_LongPreambleDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_LongPreambleIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC long preamble enforcement enable status.

  Description:
    This function gets the EMAC long preamble enforcement enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC only allows receive packets, which contain preamble fields 
              less than 12 bytes in length
    - false - The EMAC allows any length preamble

  Example:
    <code>
    stat = PLIB_ETH_LongPreambleIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_LongPreambleIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PurePreambleEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC pure preamble enforcement.

  Description:
    This function enables EMAC pure preamble enforcement. The EMAC will verify 
    the contents of the preamble and discard packets with errors in the preamble.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PurePreambleEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The EMAC will verify the content of the preamble to ensure it contains 0x55 
    and is error-free. A packet with errors in its preamble is discarded.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PurePreambleEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PurePreambleDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC pure preamble enforcement.

  Description:
    This function disables the EMAC pure preamble enforcement. The EMAC does not 
    perform any preamble checking.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PurePreambleDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PurePreambleDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_PurePreambleIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets EMAC pure preamble enforcement enable status.

  Description:
    This function gets the EMAC pure preamble enforcement enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC will verify the content of the preamble to ensure it
              contains 0x55 and is error-free. A packet with errors in its 
              preamble is discarded.
    - false - The EMAC does not perform any preamble checking

  Example:
    <code>
    stat = PLIB_ETH_PurePreambleIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_PurePreambleIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_AutoDetectPadClear(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option );

  Summary:
    Clears the EMAC Auto-Pad option.

  Description:
    This function sets the EMAC Auto-Pad option. If any auto-pad option other
    than ETH_AUTOPAD_DISABLED_CHECK_CRC is selected, the CRC must be enabled by 
    PLIB_ETH_CRCEnable.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_AutoDetectPadClear(MY_ETH_INSTANCE, PAD_OPTION);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.

*/

void PLIB_ETH_AutoDetectPadClear(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option );


//******************************************************************************
/* Function:
    void PLIB_ETH_AutoDetectPadSet(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option );

  Summary:
    Sets the EMAC Auto-Pad option.

  Description:
    This function sets the EMAC Auto-Pad option. If any auto-pad option other than 
    ETH_AUTOPAD_DISABLED_CHECK_CRC is selected, the CRC must be enabled by 
    PLIB_ETH_CRCEnable.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_AutoDetectPadSet(MY_ETH_INSTANCE, PAD_OPTION);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.

*/

void PLIB_ETH_AutoDetectPadSet(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option );

/*DOM-IGNORE-BEGIN*/
//PLIB_ETH_AutoDetectPadSet() will set  bits AUTOPAD, VLANPAD, and PADENABLE.
// Software must also enable CRC.
//  See the FRM PAD Operation Table.
/*DOM-IGNORE-END*/


//******************************************************************************
/* Function:
    ETH_AUTOPAD_OPTION PLIB_ETH_AutoDetectPadGet(ETH_MODULE_ID index);

  Summary:
    Gets the current EMAC Auto-Pad setting.

  Description:
    This function gets the current EMAC Auto-Pad setting.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    ETH_AUTOPAD_OPTION.

  Example:
    <code>
    autoPadOption = PLIB_ETH_AutoDetectPadGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

ETH_AUTOPAD_OPTION PLIB_ETH_AutoDetectPadGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_CRCEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC CRC.

  Description:
    This function enables EMAC CRC. The EMAC will append CRC whether padding is 
    required or not. This must be enabled if auto-padding is enabled.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_CRCEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_CRCEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_CRCDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC CRC.

  Description:
    This function disables EMAC CRC.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_CRCDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The frames presented to the EMAC have a valid CRC

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_CRCDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_CRCIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC CRC enable status.

  Description:
    This function returns the EMAC CRC enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC will append a CRC to every frame whether or not padding
              was required. Must be set if auto-padding is enabled.
    - false - The frames presented to the EMAC have a valid CRC

  Example:
    <code>
    stat = PLIB_ETH_CRCIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_CRCIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_DelayedCRCEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC delayed CRC.

  Description:
    This function enables EMAC delayed CRC. A proprietary packet header exists. Four
    bytes of the packet header are ignored by the CRC function.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_DelayedCRCEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_DelayedCRCEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_DelayedCRCDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC delayed CRC.

  Description:
    This function disables EMAC delayed CRC. No proprietary header exists.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_DelayedCRCDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_DelayedCRCDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_DelayedCRCIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC delayed CRC enable status.

  Description:
    This function returns the EMAC delayed CRC enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - A proprietary header exists. Four bytes of packet header are ignored
              by the CRC function.
    - false - No proprietary header

  Example:
    <code>
    stat = PLIB_ETH_DelayedCRCIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_DelayedCRCIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_HugeFrameEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC to receive huge frames.

  Description:
    This function enables the EMAC receiving the frames of any length.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_HugeFrameEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_HugeFrameEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_HugeFrameDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC from receiving huge frames.

  Description:
    This function disables the EMAC from receiving huge frames.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_HugeFrameDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_HugeFrameDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_HugeFrameIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC huge frame enable status.

  Description:
    This function returns the EMAC huge frame enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Frames of any length are transmitted and received
    - false - Huge frames are not allowed for receive or transmit

  Example:
    <code>
    stat = PLIB_ETH_HugeFrameIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_HugeFrameIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FrameLengthCheckEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC frame length check.

  Description:
    This function enables the EMAC frame length check. Both transmit and receive 
    frame lengths are compared to the Length/Type field. If the Length/Type field 
    represents a length, the check is performed. Mismatches are reported on the
    transmit/receive statistics vector.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FrameLengthCheckEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FrameLengthCheckEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FrameLengthCheckDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC frame length check.

  Description:
    This function disables the EMAC frame length check.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FrameLengthCheckDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FrameLengthCheckDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_FrameLengthCheckIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC frame length check status.

  Description:
    This function returns the EMAC frame length check status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Both transmit and receive frame lengths are compared to
              the Length/Type field. If the Length/Type field represents
              a length then the check is performed. Mismatches are reported
              on the transmit/receive statistics vector.
    - false - Length/Type field check is not performed

  Example:
    <code>
    stat = PLIB_ETH_FrameLengthCheckIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_FrameLengthCheckIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FullDuplexEnable(ETH_MODULE_ID index)

  Summary:
    Enables the EMAC full duplex operation.

  Description:
    This function enables the EMAC full duplex operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FullDuplexEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FullDuplexEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_FullDuplexDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC full duplex operation.

  Description:
    This function disables the EMAC full-duplex operation, which results in the
    EMAC operating in half-duplex.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_FullDuplexDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_FullDuplexDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_FullDuplexIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC full duplex enable status.

  Description:
    This function returns the EMAC full duplex enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC operates in Full-Duplex mode
    - false - The EMAC operates in Half-Duplex mode

  Example:
    <code>
    stat = PLIB_ETH_FullDuplexIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_FullDuplexIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_BackToBackIPGSet(ETH_MODULE_ID index, uint8_t backToBackIPGValue)

  Summary:
    Sets the EMAC back-to-back interpacket gap.

  Description:
    This function sets the EMAC back-to-back interpacket gap.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    backToBackIPGValue - Back-to-back interpacket gap

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_BackToBackIPGSet(MY_ETH_INSTANCE, backToBackIPGValue);
    </code>

  Remarks:
    In Full-Duplex mode, the register value should be the desired period
    in nibble times minus 3. In Full-Duplex, the recommended setting is 0x15.

    In Half-Duplex mode, the register value should be the desired period
    in nibble times minus 6. n Half-Duplex, the recommended setting is 0x12.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_BackToBackIPGSet(ETH_MODULE_ID index, uint8_t backToBackIPGValue);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_BackToBackIPGGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC back-to-back interpacket gap.

  Description:
    This function gets the EMAC back-to-back interpacket gap.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - backToBackIPGValue - Back-to-back interpacket gap (7 bits)

  Example:
    <code>
    value = PLIB_ETH_BackToBackIPGGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_BackToBackIPGGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_NonBackToBackIPG1Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue)

  Summary:
    Sets the EMAC non-back-to-back interpacket gap register 1.

  Description:
    This function sets the EMAC non-back-to-back interpacket gap register 1.
    A value of 0x0C is recommended.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    nonBackToBackIPGValue - Non-back-to-back interpacket gap

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_NonBackToBackIPG1Set(MY_ETH_INSTANCE, nonBackToBackIPGValue);
    </code>

  Remarks:
    This is a programmable field representing the optional carrierSense window
    referenced in the IEEE 802.3 Specification.
    If a carrier is detected during the timing of IPGR1, the MAC defers to the carrier.
    If, however, the carrier comes after IPGR1, the MAC continues timing IPGR2 and
    transmits, knowingly causing a collision, thereby ensuring fair access to the medium.
    Its range of values is 0x0 to IPGR2. Its recommended value is 0xC (12d).

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_NonBackToBackIPG1Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_NonBackToBackIPG1Get(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC non-back-to-back interpacket gap register 1.

  Description:
    This function gets the EMAC non-back-to-back interpacket gap register 1.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - nonBackToBackIPGValue - Non-back-to-back interpacket gap (7 bits)

  Example:
    <code>
    value = PLIB_ETH_NonBackToBackIPG1Get(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_NonBackToBackIPG1Get(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_NonBackToBackIPG2Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue)

  Summary:
    Sets the EMAC non-back-to-back interpacket gap register 2.

  Description:
    This function sets the EMAC non-back-to-back interpacket gap register 2.
    A value of 0x12 is recommended.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    nonBackToBackIPGValue - Non-back-to-back interpacket gap

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_NonBackToBackIPG2Set(MY_ETH_INSTANCE, nonBackToBackIPGValue);
    </code>

  Remarks:
    This is a programmable field representing the non-back-to-back
    Inter-Packet-Gap. Its recommended value is 0x12 (18d), which represents
    the minimum IPG of 0.96 us (in 100 Mbps) or 9.6 us (in 10 Mbps).

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_NonBackToBackIPG2Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_NonBackToBackIPG2Get(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC non-back-to-back interpacket gap register 2.

  Description:
    This function gets the EMAC non-back-to-back interpacket gap register 2.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - nonBackToBackIPGValue - Non-back-to-back interpacket gap (7 bits)

  Example:
    <code>
    value = PLIB_ETH_NonBackToBackIPG2Get(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_NonBackToBackIPG2Get(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_CollisionWindowSet(ETH_MODULE_ID index, uint8_t collisionWindowValue)

  Summary:
    Sets the EMAC collision window.

  Description:
    This function sets the EMAC collision window.
	This function performs atomic register access. 

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    collisionWindowValue - Collision window

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_CollisionWindowSet(MY_ETH_INSTANCE, collisionWindowValue);
    </code>

  Remarks:
    This is a programmable field representing the slot time or collision window
    during which collisions occur in properly configured networks. Since the
    collision window starts at the beginning of transmission, the preamble
    and SFD is included. Its default of 0x37 (55d) corresponds to the count of
    frame bytes at the end of the window.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_CollisionWindowSet(ETH_MODULE_ID index, uint8_t collisionWindowValue);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_CollisionWindowGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC collision window.

  Description:
    This function gets the EMAC collision window.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - collisionWindowValue - A uint8_t (6-bit) value.

  Example:
    <code>
    value = PLIB_ETH_CollisionWindowGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_CollisionWindowGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ReTxMaxSet(ETH_MODULE_ID index, uint8_t retransmitMax)

  Summary:
    Sets the EMAC maximum retransmissions.

  Description:
    This function sets the EMAC maximum retransmissions.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    retransmitMax - Maximum number of retransmissions

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ReTxMaxSet(retransmitMax);
    </code>

  Remarks:
    This is a programmable field specifying the number of retransmission
    attempts following a collision before aborting the packet due to excessive
    collisions. The IEEE 802.3 Specification standard specifies the maximum
    number of attempts (attemptLimit) to be 0xF (15d). Its default is 000.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ReTxMaxSet(ETH_MODULE_ID index, uint16_t retransmitMax);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_ReTxMaxGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC maximum retransmissions.

  Description:
    This function gets the EMAC maximum retransmissions.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - retransmitMax - Maximum number of retransmissions

  Example:
    <code>
    value = PLIB_ETH_ReTxMaxGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The maximum number of attempts is limited to 0x0F.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_ReTxMaxGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MaxFrameLengthSet(ETH_MODULE_ID index, uint16_t MaxFrameLength)

  Summary:
    Sets the EMAC maximum frame length.

  Description:
    This function sets the EMAC maximum frame length.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    MaxFrameLength - Maximum frame length

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MaxFrameLengthSet(MY_ETH_INSTANCE, MaxFrameLength);
    </code>

  Remarks:
    This field resets to 0x05EE, which represents a maximum receive
    frame of 1518 bytes.
    An untagged maximum size Ethernet frame is 1518 bytes.
    A tagged frame adds four octets for a total of 1522 bytes.
    If a shorter/longer maximum length restriction is desired, program
    this 16-bit field.

    If a proprietary header is allowed, this field should be adjusted 
    accordingly. For example, if 4-byte headers are prepended to frames, 
    MACMAXF could be set to 1527 bytes. This would allow the maximum VLAN 
    tagged frame plus the 4-byte header.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MaxFrameLengthSet(ETH_MODULE_ID index, uint16_t MaxFrameLength);

//******************************************************************************
/* Function:
    void PLIB_ETH_MACSetMaxFrame(ETH_MODULE_ID index,uint16_t maxFrmSz)

  Summary:
    Sets the MAC maximum frame size.

  Description:
    This function sets the EMAC maximum frame length.

  Precondition:
    Should be called after PLIB_ETH_Init().

  Parameters:
    index       - Identifier for the device instance.
    maxFrmSz    - Maximum frame for the MAC to transmit or receive.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MACSetMaxFrame(index,0x800);
    </code>

  Remarks:
    The default MAC frame size (0x600) is set by the PLIB_ETH_Init() call.
*/

void PLIB_ETH_MACSetMaxFrame(ETH_MODULE_ID index,uint16_t maxFrmSz);

//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_MaxFrameLengthGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC maximum frame length.

  Description:
    This function gets the EMAC maximum frame length.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - MaxFrameLength - Maximum frame length

  Example:
    <code>
    value = PLIB_ETH_MaxFrameLengthGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_MaxFrameLengthGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RMIIResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC Reset RMII.

  Description:
    This function enables EMAC Reset RMII.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RMIIResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RMIIResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RMIIResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC Reset RMII.

  Description:
    This function disables EMAC Reset RMII for normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RMIIResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RMIIResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_RMIIResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Reset RMII enable status.

  Description:
    This function gets the EMAC Reset RMII enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Reset the EMAC RMII module
    - false - Normal operation

  Example:
    <code>
    stat = PLIB_ETH_RMIIResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_RMIIResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RMIISpeedSet(ETH_MODULE_ID index, ETH_RMII_SPEED RMIISpeed)

  Summary:
    Sets EMAC RMII Speed.

  Description:
    This function sets EMAC RMII speed. RMII speed can be either RMII_100Mbps or 
    RMII_10Mbps.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance
    RMIISpeed - RMII_100Mbps or RMII_10Mbps of type ETH_RMII_SPEED

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RMIISpeedSet(MY_ETH_INSTANCE, ETH_RMII_SPEED RMIISpeed);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RMIISpeedSet(ETH_MODULE_ID index, ETH_RMII_SPEED RMIISpeed);


//******************************************************************************
/* Function:
    ETH_RMII_SPEED PLIB_ETH_RMIISpeedGet(ETH_MODULE_ID index)

  Summary:
    Gets the current EMAC RMII speed.

  Description:
    This function gets the current EMAC RMII speed.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - RMII_100Mbps  - RMII running at 100 Mbps
    - RMII_10Mbps   - RMII running at 10 Mbps

  Example:
    <code>
    PLIB_ETH_RMIISpeedGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

ETH_RMII_SPEED PLIB_ETH_RMIISpeedGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TestBackPressEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC Test backpressure.

  Description:
    This function enables EMAC Test backpressure. The EMAC will assert backpressure 
    on the link. Backpressure causes preamble to be transmitted, raising 
    carrier sense. A transmit packet from the system will be sent during backpressure.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TestBackPressEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is intended for testing only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TestBackPressEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TestBackPressDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC Test backpressure.

  Description:
    This function disables EMAC Test backpressure. The EMAC will resume normal 
    operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TestBackPressDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TestBackPressDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TestBackPressIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC test backpressure enable status.

  Description:
    This function gets the EMAC test backpressure enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC will assert backpressure on the link. Backpressure causes 
              the preamble to be transmitted, raising the carrier sense. A transmit 
              packet from the system will be sent during backpressure.
    - false - Normal operation

  Example:
    <code>
    PLIB_ETH_TestBackPressIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TestBackPressIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TestPauseEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC test pause.

  Description:
    This function enables EMAC test pause. The EMAC Control sub-layer will inhibit 
    transmissions, just as if a Pause Receive Control frame with a non-zero pause 
    time parameter was received.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TestPauseEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is intended for testing only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TestPauseEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_TestPauseDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC test pause.

  Description:
    This function disables EMAC Test Pause. The EMAC will resume normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_TestPauseDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is intended for testing only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_TestPauseDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_TestPauseIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC test pause enable status.

  Description:
    This function returns the EMAC test pause enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC Control sub-layer will inhibit transmissions, just as if a 
              Pause Receive Control frame with a non-zero pause time parameter was 
              received
    - false - Normal operation

  Example:
    <code>
    stat = PLIB_ETH_TestPauseIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is intended for testing only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_TestPauseIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ShortcutQuantaEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC shortcut pause quanta.

  Description:
    This function enables EMAC shortcut pause quanta. The EMAC reduces the effective 
    pause quanta from 64 byte-times to 1 byte-time.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ShortcutQuantaEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is intended for testing only.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ShortcutQuantaEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_ShortcutQuantaDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC shortcut pause quanta.

  Description:
    This function disables EMAC shortcut pause quanta. The EMAC will resume normal 
    operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_ShortcutQuantaDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_ShortcutQuantaDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_ShortcutQuantaIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets EMAC shortcut pause quanta enable status.

  Description:
    This function returns EMAC shortcut pause quanta enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The EMAC reduces the effective Pause Quanta from 64 byte-times
              to 1 byte-time
    - false - Normal operation

  Example:
    <code>
    stat = PLIB_ETH_ShortcutQuantaIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_ShortcutQuantaIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMResetEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC Reset Media Independent Interface (MII) Management.

  Description:
    This function enables EMAC Reset MII Management and holds the MII Management 
    module in reset while enabled.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMResetEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    MII Management held in Reset after This function is called. Disable ResetMIIanagement 
    to return to normal operation.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMResetEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMResetDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC Reset MII Management.

  Description:
    This function disables EMAC Reset MII Management.
    EMAC will resume normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMResetDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMResetDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMResetIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC Reset MII Management enable status.

  Description:
    This function gets the EMAC Reset MII Management enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - Reset the MII Management module
    - false - Normal operation

  Example:
    <code>
    stat = PLIB_ETH_MIIMResetIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMResetIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMClockSet(ETH_MODULE_ID index, ETH_MIIM_CLK MIIMClock)

  Summary:
    Sets the EMAC MIM clock selection.

  Description:
    This function sets the EMAC MIIM clock selection.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    MIIMClock - of type ETH_MIIM_CLK - the system clock divisor for MII

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMClockSet(MY_ETH_INSTANCE, MIIMClock);
    </code>

  Remarks:
    This field is used by the clock divide logic in creating the MII Management 
    Clock (MDC), which the IEEE 802.3 Specification defines to be no faster than 
    2.5 MHz. Some PHYs support clock rates up to 12.5 MHz.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMClockSet(ETH_MODULE_ID index, ETH_MIIM_CLK MIIMClock);


//******************************************************************************
/* Function:
    ETH_MII_CLK PLIB_ETH_MIIMClockGet(ETH_MODULE_ID index)

  Summary:
    Gets the current EMAC MIIM clock selection.

  Description:
    This function returns the current EMAC MIIM clock selection.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - MIIMClock - Of type PLIB_ETH_MIIM_CLK

  Example:
    <code>
    clkstat = PLIB_ETH_MIIMClockGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

ETH_MIIM_CLK PLIB_ETH_MIIMClockGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMNoPreEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC MIIM No Preamble (suppresses preamble).

  Description:
    This function enables EMAC MIIM No Preamble (suppresses preamble). The MII 
    Management will perform read/write cycles without the 32-bit preamble field.
    Some PHYs support suppressed preamble.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMNoPreEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMNoPreEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMNoPreDisable(ETH_MODULE_ID index)

  Summary:
    Disables EMAC No Preamble (allows preamble).

  Description:
    This function disables EMAC No preamble (allows preamble). Normal read/write 
    cycles are performed.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMNoPreDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMNoPreDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMNoPreIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC MIIM No Preamble enable status.

  Description:
    This function gets the EMAC MIIM No Preamble enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management will perform read/write cycles without the 32-bit 
              preamble field. Some PHYs support suppressed preamble.
    - false - Normal read/write cycles are performed

  Example:
    <code>
    stat = PLIB_ETH_MIIMNoPreIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMNoPreIsEnabled(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMScanIncrEnable(ETH_MODULE_ID index)

  Summary:
    Enables EMAC MIIM Scan Increment.

  Description:
    This function enables EMAC MIIM Scan Increment. The MII Management module will 
    perform read cycles across a range of PHYs. The read cycles will start from 
    address 1 through the value set in the PHY address register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMScanIncrEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    The read cycles will start at PHY address 1 and continue through the value
    set for as the PHY address register.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMScanIncrEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMScanIncrDisable(ETH_MODULE_ID index)

  Summary:
    Disables the EMAC MIIM Scan Increment.

  Description:
    This function disables the EMAC MIIM Scan Increment. Allows continuous reads 
    of the same PHY.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMScanIncrDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMScanIncrDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMScanIncrIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC MIIM scan increment enable status.

  Description:
    This function gets the EMAC MIIM scan increment enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management module will perform read cycles across a range of 
              PHYs. The read cycles will start from address 1 through the value set 
              in the PHY address register.
    - false - Continuous reads of the same PHY

  Example:
    <code>
    ScanIncrement = PLIB_ETH_MIIMScanIncrIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMScanIncrIsEnabled(ETH_MODULE_ID index);



//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMScanModeEnable(ETH_MODULE_ID index)

  Summary:
    Enables MIIM scan mode.

  Description:
    This function enables MIIM scan mode. The MII Management module will perform 
    read cycles continuously. (Useful for monitoring the Link Fail.)

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMScanModeEnable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMScanModeEnable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMScanModeDisable(ETH_MODULE_ID index)
  Summary:
    Disables MIIM scan mode.

  Description:
    This function disables MIIM scan mode. Scan is disabled for Normal operation.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMScanModeDisable(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMScanModeDisable(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMScanModeIsEnabled(ETH_MODULE_ID index)

  Summary:
    Gets the MII management scan enable status.

  Description:
    This function returns the MII management scan enable status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management module will perform read cycles continuously
              (for example, useful for monitoring the Link Fail)
    - false - Normal operation

  Example:
    <code>
    stat = PLIB_ETH_MIIMScanModeIsEnabled(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMScanModeIsEnabled(ETH_MODULE_ID index);



//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMReadStart(ETH_MODULE_ID index)

  Summary:
    Initiates an MII management read command.

  Description:
    This function initiates an MII read command. The MII Management module will 
    perform a single read cycle. To get data, use PLIB_ETH_MIIMReadDataGet.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    while (PLIB_ETH_MIIMIsBusy(MY_ETH_INSTANCE))
    {
        //Wait until MII Link is not busy
    }
    PLIB_ETH_MIIReadStart(MY_ETH_INSTANCE);
    while (PLIB_ETH_MIIMIsBusy(MY_ETH_INSTANCE))
    {
        //Wait until read is complete
    }
    data = PLIB_ETH_MIIMReadDataGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMReadStart(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMWriteStart(ETH_MODULE_ID index)

  Summary:
    Initiates an MII management write command.

  Description:
    This function initiates an MII management read command. The MII Management module 
    will perform a write cycle.

  Precondition:
    The PHY address and MII register address must be configured before a write
    using PLIB_ETH_MIIPHYAddressSet(MY_ETH_INSTANCE, phyAddr)   and
    PLIB_ETH_MIIRegisterAddressSet(MY_ETH_INSTANCE, regAddr)

    Data to be written must be first loaded into the MII write register using
    PLIB_ETH_MIIMWriteDataSet(MY_ETH_INSTANCE, writeData)

  Parameters:
    index - Identifier for the device instance

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMWriteDataSet(MY_ETH_INSTANCE, dataToWrite);
    PLIB_ETH_MIIMWriteStart(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMWriteStart(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_PHYAddressSet(ETH_MODULE_ID index, uint8_t phyAddr)

  Summary:
    Sets the EMAC MIIM PHY address.

  Description:
    This function sets the EMAC MIIM PHY address. This field represents the 5-bit 
    PHY Address field of Management cycles. Up to 31 PHYs can be addressed (0 is reserved).

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    phyAddr - A 5-bit address of the PHY

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_PHYAddressSet(MY_ETH_INSTANCE, PhyAddr);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_PHYAddressSet(ETH_MODULE_ID index, uint8_t phyAddr);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_PHYAddressGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC MIIM management PHY address.

  Description:
    This function gets the EMAC MIIM management PHY address.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - phyAddr - A 5-bit address of the PHY

  Example:
    <code>
    phyAddr = PLIB_ETH_PHYAddressGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_PHYAddressGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_RegisterAddressSet(ETH_MODULE_ID index, uint8_t regAddr)

  Summary:
    Sets EMAC MIIM register address.

  Description:
    This function sets the EMAC MIIM register address. Up to 32 registers may 
    be accessed.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    regAddr - The (5-bit) address of the MII Registers.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_RegisterAddressSet(MY_ETH_INSTANCE, regAddr);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_RegisterAddressSet(ETH_MODULE_ID index, uint8_t regAddr);


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_RegisterAddressGet(ETH_MODULE_ID index)

  Summary:
    Gets the EMAC MIIM management register address.

  Description:
    This function gets the EMAC MIIM management register address.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - regAddr - The (5-bit) address of the MII Registers

  Example:
    <code>
    regAddr = PLIB_ETH_RegisterAddressGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_RegisterAddressGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_MIIMWriteDataSet(ETH_MODULE_ID index, uint16_t writeData)

  Summary:
    Sets the EMAC MIIM write data before initiating an MII write cycle.

  Description:
    This function sets the EMAC MIIM write data before initiating write cycle.

  Precondition:
    Prior to a call to this routine, the PHY and Register addresses
    should be set using PLIB_ETH_MIIPHYAddressSet and PLIB_ETH_MIIRegisterAddressSet.

  Parameters:
    index - Identifier for the device instance
    writeData - MII write data

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_MIIMWriteDataSet(MY_ETH_INSTANCE, WriteData);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_MIIMWriteDataSet(ETH_MODULE_ID index, uint16_t writeData);


//******************************************************************************
/* Function:
    uint16_t PLIB_ETH_MIIMReadDataGet(ETH_MODULE_ID index)

  Summary:
    Gets EMAC MIIM management read data after a MII read cycle has completed.

  Description:
    This function gets EMAC MIIM management read data after a MII read cycle
    has completed.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - readData - MII read data

  Example:
    <code>
    readData = PLIB_ETH_MIIMReadDataGet(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint16_t PLIB_ETH_MIIMReadDataGet(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_LinkHasFailed(ETH_MODULE_ID index)

  Summary:
    Gets the MII management link fail status.

  Description:
    This function returns the MII management link fail status. This value reflects 
    the last read from the PHY status register.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management module link fail has occurred
    - false - The MII Management module link fail has not occurred

  Example:
    <code>
    stat = PLIB_ETH_LinkHasFailed(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_LinkHasFailed(ETH_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_ETH_ClearDataNotValid(ETH_MODULE_ID index)

  Summary:
    Clears the MII management Received data not valid flag.

  Description:
    This function Clears the MII management Received data not valid flag. 
	
  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - None

  Example:
    
  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/
void PLIB_ETH_ClearDataNotValid( ETH_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_ETH_DataNotValid(ETH_MODULE_ID index)

  Summary:
    Gets the MII management read data not valid status.

  Description:
    This function gets the MII management read data not valid status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management read cycle has not completed and the read data
              is not yet valid
    - false - The MII Management read cycle is complete and the read data is valid

  Example:
    <code>
    stat = PLIB_ETH_DataNotValid(MY_ETH_INSTANCE);
    </code>
    Also see the example for function: PLIB_ETH_MIIReadStart().

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_DataNotValid(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMIsScanning(ETH_MODULE_ID index)

  Summary:
    Gets the MII Management scanning status.

  Description:
    This function gets the MII Management scanning status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management module scan operation (continuous MII Management 
              Read cycles) is in progress
    - false - The MII Management scan operation is not in progress

  Example:
    <code>
    stat = PLIB_ETH_MIIMIsScanning(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMIsScanning(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_ETH_MIIMIsBusy(ETH_MODULE_ID index)

  Summary:
    Gets the MII management busy status.

  Description:
    This function returns the MII management busy status.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance

  Returns:
    - true  - The MII Management module is currently performing an MII Management 
              read or write cycle
    - false - The MII Management is free

  Example:
    <code>
    stat = PLIB_ETH_MIIMIsBusy(MY_ETH_INSTANCE);
    </code>

  Remarks:
    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

bool PLIB_ETH_MIIMIsBusy(ETH_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_ETH_StationAddressSet(ETH_MODULE_ID index, uint8_t which, uint8_t stationAddress)

  Summary:
    Sets the selected EMAC Station Address.

  Description:
    This function sets the selected EMAC Station Address.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    which - Select station address to change. Valid values are 1,2,3,4,5,6.
    stationAddress - Station Address.

  Returns:
    None.

  Example:
    <code>
    PLIB_ETH_StationAddressSet(MY_ETH_INSTANCE, which, stationAddress);
    </code>

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

void PLIB_ETH_StationAddressSet(ETH_MODULE_ID index, uint8_t which, uint8_t stationAddress);

//******************************************************************************
/* Function:
     void PLIB_ETH_MACSetAddress (ETH_MODULE_ID index, uint8_t *bAddress )

  Summary:
    Sets the MAC address.

  Description:
    This function sets the selected MAC Station Address.

  Precondition:
    None.

  Parameters:
    index       - Identifier for the device instance.
    bAddress    - standard MAC address, 6 bytes, Network order!

  Returns:
    None.

  Example:
    <code>
    uint8_t hostAddress[6]= { 0x00, 0x04, 0xa3, 0x00, 0x00, 0x02};

    PLIB_ETH_MACSetAddress(index,hostAddress);
    </code>

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.
*/

void PLIB_ETH_MACSetAddress (ETH_MODULE_ID index, uint8_t *bAddress );


//******************************************************************************
/* Function:
    uint8_t PLIB_ETH_StationAddressGet(ETH_MODULE_ID index, uint8_t which)

  Summary:
    Gets the selected EMAC station address.

  Description:
    This function gets the selected EMAC station address.

  Precondition:
    None.

  Parameters:
    index - Identifier for the device instance
    which - Select station address to change. Valid values are 1,2,3,4,5,6.

  Returns:
    - stationAddress1 - Station address

  Example:
    <code>
    stationAddr1 = PLIB_ETH_StationAddress1Get(MY_ETH_INSTANCE, which);
    </code>

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/

uint8_t PLIB_ETH_StationAddressGet(ETH_MODULE_ID index, uint8_t which);

//******************************************************************************
/* Function:
    void PLIB_ETH_MACGetAddress (ETH_MODULE_ID index, uint8_t *bAddress )

  Summary:
    Returns the current MAC address.

  Description:
    This function returns the current MAC address.

  Precondition:
    None.

  Parameters:
    index       - Identifier for the device instance.
    bAddress    - Address to store a standard MAC address, 6 bytes, Network order!

  Returns:
    None.

  Example:
     <code>
    uint8_t currentMacAddress;

    PLIB_ETH_MACGetAddress(index,&currentMacAddress);
    </code>

  Remarks:
    On a reset, this register is loaded with the factory preprogrammed
    station address.

    This functionality is not available on all devices. Please refer to the
    specific device data sheet to determine availability.
*/
void PLIB_ETH_MACGetAddress (ETH_MODULE_ID index, uint8_t *bAddress );


// *****************************************************************************
// *****************************************************************************
// Section: ETH Peripheral Library Exists API Routines
// *****************************************************************************
// *****************************************************************************
/* The functions below indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_ETH_ExistsPauseTimer( ETH_MODULE_ID index )

  Summary:
    Identifies whether the PauseTimer feature exists on the Ethernet module.

  Description:
    This function identifies whether the PauseTimer feature is available on 
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_PauseTimerSet
    - PLIB_ETH_PauseTimerGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PauseTimer feature is supported on the device
    - false  - The PauseTimer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsPauseTimer( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsEnable( ETH_MODULE_ID index )

  Summary:
    Identifies whether the Enable feature exists on the Ethernet module.

  Description:
    This function identifies whether the Enable feature is available on
    the Ethernet module. When this function returns true, these functions are
    supported on the device:
    - PLIB_ETH_Enable
    - PLIB_ETH_Disable
    - PLIB_ETH_IsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Enable feature is supported on the device
    - false  - The Enable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsEnable( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsStopInIdle( ETH_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the Ethernet module.

  Description:
    This function identifies whether the StopInIdle feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_StopInIdleEnable
    - PLIB_ETH_StopInIdleDisable
    - PLIB_ETH_StopInIdleIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The StopInIdle feature is supported on the device
    - false  - The StopInIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsStopInIdle( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsTransmitRTS( ETH_MODULE_ID index )

  Summary:
    Identifies whether the TransmitRTS feature exists on the Ethernet module.

  Description:
    This function identifies whether the TransmitRTS feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_TxRTSEnable
    - PLIB_ETH_TxRTSDisable
    - PLIB_ETH_TxRTSIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitRTS feature is supported on the device
    - false  - The TransmitRTS feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsTransmitRTS( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRxEnable( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveEnable feature exists on the Ethernet module.

  Description:
    This function identifies whether the ReceiveEnable feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RxEnable
    - PLIB_ETH_RxDisable
    - PLIB_ETH_RxIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveEnable feature is supported on the device
    - false  - The ReceiveEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsRxEnable( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsAutoFlowControl( ETH_MODULE_ID index )

  Summary:
    Identifies whether the AutoFlowControl feature exists on the Ethernet module.

  Description:
    This function identifies whether the AutoFlowControl feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_AutoFlowControlEnable
    - PLIB_ETH_AutoFlowControlDisable
    - PLIB_ETH_AutoFlowControlIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AutoFlowControl feature is supported on the device
    - false  - The AutoFlowControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsAutoFlowControl( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsManualFlowControl( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ManualFlowControl feature exists on the Ethernet module.

  Description:
    This function identifies whether the ManualFlowControl feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_ManualFlowControlEnable
    - PLIB_ETH_ManualFlowControlDisable
    - PLIB_ETH_ManualFlowControlIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ManualFlowControl feature is supported on the device
    - false  - The ManualFlowControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsManualFlowControl( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRxBufferCountDecrement( ETH_MODULE_ID index )

  Summary:
    Identifies whether the RxBufferCountDecrement feature exists on the Ethernet module.

  Description:
    This function identifies whether the RxBufferCountDecrement feature is available on
    the Ethernet module. When this function returns true, this function is
    supported on the device:
    - PLIB_ETH_RxBufferCountDecrement

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RxBufferCountDecrement feature is supported on the device
    - false  - The RxBufferCountDecrement feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsRxBufferCountDecrement( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsReceiveBufferSize( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveBufferSize feature exists on the Ethernet module.

  Description:
    This function identifies whether the ReceiveBufferSize feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_ReceiveBufferSizeGet
    - PLIB_ETH_ReceiveBufferSizeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveBufferSize feature is supported on the device
    - false  - The ReceiveBufferSize feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsReceiveBufferSize( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsTxPacketDescriptorAddress( ETH_MODULE_ID index )

  Summary:
    Identifies whether the TxPacketDescriptorAddress feature exists on the Ethernet module.

  Description:
    This function identifies whether the TxPacketDescriptorAddress feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_TxPacketDescAddrSet
    - PLIB_ETH_TxPacketDescAddrGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TxPacketDescriptorAddress feature is supported on the device
    - false  - The TxPacketDescriptorAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsTxPacketDescriptorAddress( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRxPacketDescriptorAddress( ETH_MODULE_ID index )

  Summary:
    Identifies whether the RxPacketDescriptorAddress feature exists on the Ethernet module.

  Description:
    This function identifies whether the RxPacketDescriptorAddress feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RxPacketDescAddrSet
    - PLIB_ETH_RxPacketDescAddrGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RxPacketDescriptorAddress feature is supported on the device
    - false  - The RxPacketDescriptorAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsRxPacketDescriptorAddress( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsHashTable( ETH_MODULE_ID index )

  Summary:
    Identifies whether the HashTable feature exists on the Ethernet module.

  Description:
    This function identifies whether the HashTable feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_HashTableSet
    - PLIB_ETH_HashTableGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The HashTable feature is supported on the device
    - false  - The HashTable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsHashTable( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsPatternMatch( ETH_MODULE_ID index )

  Summary:
    Identifies whether the PatternMatch feature exists on the Ethernet module.

  Description:
    This function identifies whether the PatternMatch feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_PatternMatchSet
    - PLIB_ETH_PatternMatchGet
    - PLIB_ETH_PatternMatchChecksumSet
    - PLIB_ETH_PatternMatchChecksumGet
    - PLIB_ETH_PatternMatchOffsetSet
    - PLIB_ETH_PatternMatchOffsetGet
    - PLIB_ETH_PatternMatchModeSet
    - PLIB_ETH_PatternMatchModeGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PatternMatch feature is supported on the device
    - false  - The PatternMatch feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsPatternMatch( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsReceiveFilters( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveFilters feature exists on the Ethernet module.

  Description:
    This function identifies whether the ReceiveFilters feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_ReceiveFilterEnable
    - PLIB_ETH_ReceiveFilterDisable
    - PLIB_ETH_ReceiveFilterIsEnable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveFilters feature is supported on the device
    - false  - The ReceiveFilters feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsReceiveFilters( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsReceiveWmarks( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveWmarks feature exists on the Ethernet module.

  Description:
    This function identifies whether the ReceiveWmarks feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RxFullWmarkSet
    - PLIB_ETH_RxFullWmarkGet
    - PLIB_ETH_RxEmptyWmarkSet
    - PLIB_ETH_RxEmptyWmarkGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveWmarks feature is supported on the device
    - false  - The ReceiveWmarks feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsReceiveWmarks( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsInterrupt( ETH_MODULE_ID index )

  Summary:
    Identifies whether the Interrupt feature exists on the Ethernet module.

  Description:
    This function identifies whether the Interrupt feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_InterruptSourceEnable
    - PLIB_ETH_InterruptSourceDisable
    - PLIB_ETH_InterruptSourceIsEnabled
    - PLIB_ETH_InterruptSet
    - PLIB_ETH_InterruptClear
    - PLIB_ETH_InterruptStatusGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Interrupt feature is supported on the device
    - false  - The Interrupt feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsInterrupt( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsEthernetControllerStatus( ETH_MODULE_ID index )

  Summary:
    Identifies whether the EthernetControllerStatus feature exists on the Ethernet module.

  Description:
    This function identifies whether the EthernetControllerStatus feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RxPacketCountGet
    - PLIB_ETH_EthernetIsBus
    - PLIB_ETH_TransmitIsBusy
    - PLIB_ETH_ReceiveIsBusy

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EthernetControllerStatus feature is supported on the device
    - false  - The EthernetControllerStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsEthernetControllerStatus( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsReceiveOverflowCount( ETH_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveOverflowCount feature exists on the Ethernet module.

  Description:
    This function identifies whether the ReceiveOverflowCount feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RxOverflowCountClear
    - PLIB_ETH_RxOverflowCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveOverflowCount feature is supported on the device
    - false  - The ReceiveOverflowCount feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsReceiveOverflowCount( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsFramesTransmittedOK( ETH_MODULE_ID index )

  Summary:
    Identifies whether the FramesTransmittedOK feature exists on the Ethernet module.

  Description:
    This function identifies whether the FramesTransmittedOK feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_FramesTxdOkCountClear
    - PLIB_ETH_FramesTxdOkCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FramesTransmittedOK feature is supported on the device
    - false  - The FramesTransmittedOK feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsFramesTransmittedOK( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsCollisionCounts( ETH_MODULE_ID index )

  Summary:
    Identifies whether the CollisionCounts feature exists on the Ethernet module.

  Description:
    This function identifies whether the CollisionCounts feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_SingleCollisionCountClear
    - PLIB_ETH_SingleCollisionCountGet
    - PLIB_ETH_MultipleCollisionCountClear
    - PLIB_ETH_MultipleCollisionCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CollisionCounts feature is supported on the device
    - false  - The CollisionCounts feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsCollisionCounts( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsFramexReceivedOK( ETH_MODULE_ID index )

  Summary:
    Identifies whether the FramexReceivedOK feature exists on the Ethernet module.

  Description:
    This function identifies whether the FramexReceivedOK feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_FramesRxdOkCountClear
    - PLIB_ETH_FramesRxdOkCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FramexReceivedOK feature is supported on the device
    - false  - The FramexReceivedOK feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsFramexReceivedOK( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsFCSErrorCount( ETH_MODULE_ID index )

  Summary:
    Identifies whether the FCSErrorCount feature exists on the Ethernet module.

  Description:
    This function identifies whether the FCSErrorCount feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_FCSErrorCountClear
    - PLIB_ETH_FCSErrorCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FCSErrorCount feature is supported on the device
    - false  - The FCSErrorCount feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsFCSErrorCount( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsAlignmentErrorCount( ETH_MODULE_ID index )

  Summary:
    Identifies whether the AlignmentErrorCount feature exists on the Ethernet module.

  Description:
    This function identifies whether the AlignmentErrorCount feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_AlignErrorCountClear
    - PLIB_ETH_AlignErrorCountGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AlignmentErrorCount feature is supported on the device
    - false  - The AlignmentErrorCount feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsAlignmentErrorCount( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMAC_Resets( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MAC_Resets feature exists on the Ethernet module.

  Description:
    This function identifies whether the MAC_Resets feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MIIResetEnable
    - PLIB_ETH_MIIResetDisable
    - PLIB_ETH_MIIResetIsEnabled
    - PLIB_ETH_SimResetEnable
    - PLIB_ETH_SimResetDisable
    - PLIB_ETH_SimResetIsEnabled
    - PLIB_ETH_MCSRxResetEnable
    - PLIB_ETH_MCSRxResetDisable
    - PLIB_ETH_MCSRxResetIsEnabled
    - PLIB_ETH_RxFuncResetEnable
    - PLIB_ETH_RxFuncResetDisable
    - PLIB_ETH_RxFuncResetIsEnabled
    - PLIB_ETH_MCSTxResetEnable
    - PLIB_ETH_MCSTxResetDisable
    - PLIB_ETH_MCSTxResetIsEnabled
    - PLIB_ETH_TxFuncResetEnable
    - PLIB_ETH_TxFuncResetDisable
    - PLIB_ETH_TxFuncResetIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MAC_Resets feature is supported on the device
    - false  - The MAC_Resets feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMAC_Resets( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMAC_Configuration( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MAC_Configuration feature exists on the Ethernet module.

  Description:
    This function identifies whether the MAC_Configuration feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_LoopbackEnable
    - PLIB_ETH_LoopbackDisable
    - PLIB_ETH_LoopbackIsEnabled
    - PLIB_ETH_TxPauseEnable
    - PLIB_ETH_TxPauseDisable
    - PLIB_ETH_TxPauseIsEnabled
    - PLIB_ETH_RxPauseEnable
    - PLIB_ETH_RxPauseDisable
    - PLIB_ETH_RxPauseIsEnabled
    - PLIB_ETH_PassAllEnable
    - PLIB_ETH_PassAllDisable
    - PLIB_ETH_PassAllIsEnabled
    - PLIB_ETH_ReceiveEnable
    - PLIB_ETH_ReceiveDisable
    - PLIB_ETH_ReceiveIsEnabled
    - PLIB_ETH_ExcessDeferEnable
    - PLIB_ETH_ExcessDeferDisable
    - PLIB_ETH_ExcessDeferIsEnabled
    - PLIB_ETH_BackPresNoBackoffEnable
    - PLIB_ETH_BackPresNoBackoffDisable
    - PLIB_ETH_BackPresNoBackoffIsEnabled
    - PLIB_ETH_NoBackoffEnable
    - PLIB_ETH_NoBackoffDisable
    - PLIB_ETH_NoBackoffIsEnabled
    - PLIB_ETH_LongPreambleEnable
    - PLIB_ETH_LongPreambleDisable
    - PLIB_ETH_LongPreambleIsEnabled
    - PLIB_ETH_PurePreambleEnable
    - PLIB_ETH_PurePreambleDisable
    - PLIB_ETH_PurePreambleIsEnabled
    - PLIB_ETH_AutoDetectPadGet
    - PLIB_ETH_AutoDetectPadSet
    - PLIB_ETH_AutoDetectPadClear
    - PLIB_ETH_CRCEnable
    - PLIB_ETH_CRCDisable
    - PLIB_ETH_CRCIsEnabled
    - PLIB_ETH_DelayedCRCEnable
    - PLIB_ETH_DelayedCRCDisable
    - PLIB_ETH_DelayedCRCIsEnabled
    - PLIB_ETH_HugeFrameEnable
    - PLIB_ETH_HugeFrameDisable
    - PLIB_ETH_HugeFrameIsEnabled
    - PLIB_ETH_FrameLengthCheckEnable
    - PLIB_ETH_FrameLengthCheckDisable
    - PLIB_ETH_FrameLengthCheckIsEnabled
    - PLIB_ETH_FullDuplexEnable
    - PLIB_ETH_FullDuplexDisable
    - PLIB_ETH_FullDuplexIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MAC_Configuration feature is supported on the device
    - false  - The MAC_Configuration feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMAC_Configuration( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsInterPacketGaps( ETH_MODULE_ID index )

  Summary:
    Identifies whether the InterPacketGaps feature exists on the Ethernet module.

  Description:
    This function identifies whether the InterPacketGaps feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_BackToBackIPGGet
    - PLIB_ETH_BackToBackIPGSet
    - PLIB_ETH_NonBackToBackIPG1Get
    - PLIB_ETH_NonBackToBackIPG1Set
    - PLIB_ETH_NonBackToBackIPG2Get
    - PLIB_ETH_NonBackToBackIPG2Set

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The InterPacketGaps feature is supported on the device
    - false  - The InterPacketGaps feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsInterPacketGaps( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsCollisionWindow( ETH_MODULE_ID index )

  Summary:
    Identifies whether the CollisionWindow feature exists on the Ethernet module.

  Description:
    This function identifies whether the CollisionWindow feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_CollisionWindowGet
    - PLIB_ETH_CollisionWindowSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CollisionWindow feature is supported on the device
    - false  - The CollisionWindow feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsCollisionWindow( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRetransmissionMaximum( ETH_MODULE_ID index )

  Summary:
    Identifies whether the RetransmissionMaximum feature exists on the Ethernet module.

  Description:
    This function identifies whether the RetransmissionMaximum feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_ReTxMaxGet
    - PLIB_ETH_ReTxMaxSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:

    - true   - The RetransmissionMaximum feature is supported on the device
    - false  - The RetransmissionMaximum feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsRetransmissionMaximum( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMaxFrameLength( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MaxFrameLength feature exists on the Ethernet module.

  Description:
    This function identifies whether the MaxFrameLength feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MaxFrameLengthGet
    - PLIB_ETH_MaxFrameLengthSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MaxFrameLength feature is supported on the device
    - false  - The MaxFrameLength feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMaxFrameLength( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRMII_Support( ETH_MODULE_ID index )

  Summary:
    Identifies whether the RMII_Support feature exists on the Ethernet module.

  Description:
    This function identifies whether the RMII_Support feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_RMIIResetEnable
    - PLIB_ETH_RMIIResetDisable
    - PLIB_ETH_RMIIResetIsEnabled
    - PLIB_ETH_RMIISpeedGet
    - PLIB_ETH_RMIISpeedSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RMII_Support feature is supported on the device
    - false  - The RMII_Support feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsRMII_Support( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMAC_Testing( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MAC_Testing feature exists on the Ethernet module.

  Description:
    This function identifies whether the MAC_Testing feature is available on
    the Ethernet module. When this function returns true, these functions are
    supported on the device:
    - PLIB_ETH_TestBackPressEnable
    - PLIB_ETH_TestBackPressDisable
    - PLIB_ETH_TestBackPressIsEnabled
    - PLIB_ETH_TestPauseEnable
    - PLIB_ETH_TestPauseDisable
    - PLIB_ETH_TestPauseIsEnabled
    - PLIB_ETH_ShortcutQuantaEnable
    - PLIB_ETH_ShortcutQuantaDisable
    - PLIB_ETH_ShortcutQuantaIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MAC_Testing feature is supported on the device
    - false  - The MAC_Testing feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMAC_Testing( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIM_Config( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIM_Config feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIM_Config feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MIIMResetEnable
    - PLIB_ETH_MIIMResetDisable
    - PLIB_ETH_MIIMResetIsEnabled
    - PLIB_ETH_MIIMClockGet
    - PLIB_ETH_MIIMClockSet
    - PLIB_ETH_MIIMNoPreEnable
    - PLIB_ETH_MIIMNoPreDisable
    - PLIB_ETH_MIIMNoPreIsEnabled
    - PLIB_ETH_MIIMScanIncrEnable
    - PLIB_ETH_MIIMScanIncrDisable
    - PLIB_ETH_MIIMScanIncrIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIM_Config feature is supported on the device
    - false  - The MIIM_Config feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIM_Config( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIMScanMode( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIMScanMode feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIMScanMode feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MIIMScanModeEnable
    - PLIB_ETH_MIIMScanModeDisable
    - PLIB_ETH_MIIMScanModeIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIMScanMode feature is supported on the device
    - false  - The MIIMScanMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIMScanMode( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIMReadWrite( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIMReadWrite feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIMReadWrite feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MIIMReadStart
    - PLIB_ETH_MIIMWriteStart

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIMReadWrite feature is supported on the device
    - false  - The MIIMReadWrite feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIMReadWrite( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIMAddresses( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIMAddresses feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIMAddresses feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_PHYAddressGet
    - PLIB_ETH_PHYAddressSet
    - PLIB_ETH_RegisterAddressGet
    - PLIB_ETH_RegisterAddressSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIMAddresses feature is supported on the device
    - false  - The MIIMAddresses feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIMAddresses( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIWriteReadData( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIWriteReadData feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIWriteReadData feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_MIIMWriteDataSet
    - PLIB_ETH_MIIMReadDataGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIWriteReadData feature is supported on the device
    - false  - The MIIWriteReadData feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIWriteReadData( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsMIIM_Indicators( ETH_MODULE_ID index )

  Summary:
    Identifies whether the MIIM_Indicators feature exists on the Ethernet module.

  Description:
    This function identifies whether the MIIM_Indicators feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_LinkHasFailed
    - PLIB_ETH_DataNotValid
    - PLIB_ETH_MIIMIsScanning
    - PLIB_ETH_MIIMIsBusy

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MIIM_Indicators feature is supported on the device
    - false  - The MIIM_Indicators feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsMIIM_Indicators( ETH_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_ETH_ExistsStationAddress( ETH_MODULE_ID index )

  Summary:
    Identifies whether the StationAddress feature exists on the Ethernet module.

  Description:
    This function identifies whether the StationAddress feature is available on
    the Ethernet module. When this function returns true, these functions are 
    supported on the device:
    - PLIB_ETH_StationAddressGet
    - PLIB_ETH_StationAddressSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The StationAddress feature is supported on the device
    - false  - The StationAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_ETH_ExistsStationAddress( ETH_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_ETH_ExistsRxFiltersPMSet( ETH_MODULE_ID index )

  Summary:
    Identifies whether the Pattern Match Rx Filter feature 
	exists on the Ethernet module.

  Description:
    This function identifies whether the Pattern Match Rx Filter feature is 
	available on the Ethernet module. When this function returns true, 
	these functions are supported on the device:
    -  PLIB_ETH_RxFiltersPMSet

  Preconditions:
    None.

  Parameters:
    index   - Identifier for the device instance

  Returns:
    - true   - The feature is supported on the device
    - false  - The feature is not supported on the device

  Remarks:
    None.
*/
bool PLIB_ETH_ExistsRxFiltersPMSet( ETH_MODULE_ID index );

//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatRxOvflCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of dropped receive frames.

  Description:
    This function returns the current number of dropped receive frames by the
    Ethernet controller.  These are frames accepted by the RX filter but dropped due
    to internal receive error (RX FIFO overrun).

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of dropped receive frames

  Example:
    <code>
    droppedFrames = PLIB_ETH_StatRxOvflCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

    An RX overflow event is signalled by the ETH_EV_RXOVFLOW event (see
    PLIB_ETH_EVENTS definition).
 */

int PLIB_ETH_StatRxOvflCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define PLIB_ETH_StatRxOvflCnt  PLIB_ETH_RxOverflowCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatRxOkCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of successfully received frames.

  Description:
    This function returns the current number of successfully received frames by
    the Ethernet controller.  These are frames accepted by the RX filter (some of
    them may still be dropped because of an RX FIFO overrun).

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of frames received OK

  Example:
    <code>
    numReceived = PLIB_ETH_StatRxOkCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

    Frames with FCS or alignment errors will not increment this count.

 */

int PLIB_ETH_StatRxOkCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define  PLIB_ETH_StatRxOkCnt  PLIB_ETH_FramesRxdOkCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatRxFcsErrCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of frames with FCS received errors.

  Description:
    The function returns the current number of frames with FCS errors received
    by the Ethernet controller.  These are received frames that have an integral
    number of bytes.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of received frames having FCS errors

  Example:
    <code>
    numRxFcsErrors = PLIB_ETH_StatRxFcsErrCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

    Frames with alignment error do not increment this count.

 */

int PLIB_ETH_StatRxFcsErrCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
  #define  PLIB_ETH_StatRxFcsErrCnt  PLIB_ETH_FCSErrorCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatRxAlgnErrCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of frames with alignment received errors

  Description:
    The function returns the current number of frames with alignment errors
    received by the Ethernet controller.  These are received frames that have FCS
    error and they do not contain an integral number of bytes (aka
    dribble-nibble).

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of received frames having alignment errors

  Example:
    <code>
    numAlignErrors = PLIB_ETH_StatRxAlgnErrCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

 */

int PLIB_ETH_StatRxAlgnErrCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define  PLIB_ETH_StatRxAlgnErrCnt  PLIB_ETH_AlignErrorCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatTxOkCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of frames transmitted successfully

  Description:
    The function returns the current number of frames transmitted successfully by
    the Ethernet controller.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of frames transmitted OK

  Example:
    <code>
    numFramesTx = PLIB_ETH_StatTxOkCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

 */

int PLIB_ETH_StatTxOkCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define  PLIB_ETH_StatTxOkCnt  PLIB_ETH_FramesTxdOkCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatTxSColCnt ( ETH_MODULE_ID index )

  Summary:
    Returns the current number of successfully transmitted frames on the second try

  Description:
    This function returns the current number of transmitted frames that had
    a collision but were successfully transmitted by the Ethernet controller
    on the second try.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of single collision frames

  Example:
    <code>
    numSingleCollisions = PLIB_ETH_StatTxSColCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

 */

int PLIB_ETH_StatTxSColCnt ( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define  PLIB_ETH_StatTxSColCnt  PLIB_ETH_SingleCollisionCountGet
// DOM-IGNORE-END


//*******************************************************************************
/*  Function:
    int PLIB_ETH_StatTxMColCnt( ETH_MODULE_ID index )

  Summary:
    Returns the number of current frames transmitted after more than one
    collision occurred.

  Description:
    The function returns the current number of frames successfully transmitted
    by the Ethernet controller after there was more than one collision.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance.

  Returns:
    The number of multiple collision frames

  Example:
    <code>
    numMultiCollisions = PLIB_ETH_StatTxMColCnt(index);
    </code>

  Remarks:
    This statistics register is cleared by the read operation.

 */

int PLIB_ETH_StatTxMColCnt( ETH_MODULE_ID index );

// DOM-IGNORE-BEGIN
#define  PLIB_ETH_StatTxMColCnt  PLIB_ETH_MultipleCollisionCountClear

// DOM-IGNORE-END

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

// ****************************************************************************
// ****************************************************************************
// Section: Included Files (continued)
// ****************************************************************************
// ****************************************************************************
/*  The file included below  maps the interface definitions above to appropriate
    implementations defined in the implementation (imp) file(s).
*/
//#include "peripheral/eth/processor/eth_processor.h"

#endif //_PLIB_ETH_H

//******************************************************************************
/* End of File
*/
