/***********************************************************************
TCP/IP Stack Ethernet PHY and MAC Driver Definitions Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_ethernet.h

  Summary:
    Common Ethernet definitions file

  Description:
    This file provides the definition of commonly-used enumerations
    for use with the Ethernet PHY and Ethernet MAC Drivers.
  ***********************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef _TCPIP_ETHERNET_H_
#define _TCPIP_ETHERNET_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
/* Ethernet Open Configuration Settings

  Summary:
    Supported open configuration flags for the Ethernet module (EthMACOpen).

  Description:
    This enumeration defines the various configuration options for the Ethernet
    module.  These values can be ORed together to create a configuration mask
    passed to the EthMACOpen routine.

  Remarks:
    When Auto-negotiation is specified:
        - If multiple capability flags are set (TCPIP_ETH_OPEN_FDUPLEX,
          TCPIP_ETH_OPEN_HDUPLEX, TCPIP_ETH_OPEN_100, TCPIP_ETH_OPEN_10 ) they are all advertised
          as this link side capabilities.
        - If no setting is passed, the lowest one is taken, i.e.,
          TCPIP_ETH_OPEN_HDUPLEX and TCPIP_ETH_OPEN_10.
        - Auto-MDIX requires Auto-Negotiation; TCPIP_ETH_OPEN_MDIX_NORM or
          TCPIP_ETH_OPEN_MDIX_SWAP setting irrelevant.

    When No Auto-negotiation is specified:
        - If multiple settings, the highest priority setting is taken, i.e.
          TCPIP_ETH_OPEN_FDUPLEX over TCPIP_ETH_OPEN_HDUPLEX and TCPIP_ETH_OPEN_100 over
          TCPIP_ETH_OPEN_10.
        - If no setting, the lowest setting is taken, i.e., TCPIP_ETH_OPEN_HDUPLEX and
          TCPIP_ETH_OPEN_10.
        - The MDIX is set based on the TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
          setting.
*/

typedef enum
{
    // Link capabilities flags:

    // Use auto negotiation. set the following flags to specify your choices
    TCPIP_ETH_OPEN_AUTO
        /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Use full duplex or full duplex negotiation capability needed
    TCPIP_ETH_OPEN_FDUPLEX
        /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // Use half duplex or half duplex negotiation capability needed
    TCPIP_ETH_OPEN_HDUPLEX
        /*DOM-IGNORE-BEGIN*/ = 0x4 /*DOM-IGNORE-END*/,

    // Use 100MBps or 100MBps negotiation capability needed
    TCPIP_ETH_OPEN_100
        /*DOM-IGNORE-BEGIN*/ = 0x8 /*DOM-IGNORE-END*/,

    // Use 10MBps or 10MBps negotiation capability needed
    TCPIP_ETH_OPEN_10
        /*DOM-IGNORE-BEGIN*/ = 0x10 /*DOM-IGNORE-END*/,

    // Allow huge packets RX/TX
    TCPIP_ETH_OPEN_HUGE_PKTS
        /*DOM-IGNORE-BEGIN*/ = 0x20 /*DOM-IGNORE-END*/,

    // Loopbacked at the MAC level
    TCPIP_ETH_OPEN_MAC_LOOPBACK
        /*DOM-IGNORE-BEGIN*/ = 0x40 /*DOM-IGNORE-END*/,

    /*When PHY is loopbacked, negotiation will be disabled!*/
    TCPIP_ETH_OPEN_PHY_LOOPBACK
        /*DOM-IGNORE-BEGIN*/ = 0x80 /*DOM-IGNORE-END*/,

    // Use Auto MDIX
    TCPIP_ETH_OPEN_MDIX_AUTO
        /*DOM-IGNORE-BEGIN*/ = 0x100 /*DOM-IGNORE-END*/,

    // Use normal MDIX when Auto MDIX disabled
    TCPIP_ETH_OPEN_MDIX_NORM
        /*DOM-IGNORE-BEGIN*/ = 0x0 /*DOM-IGNORE-END*/,

    // Use swapped MDIX when Auto MDIX disabled
    TCPIP_ETH_OPEN_MDIX_SWAP
        /*DOM-IGNORE-BEGIN*/ = 0x200 /*DOM-IGNORE-END*/,

    // MII/RMII flags:

    // RMII connection
    TCPIP_ETH_OPEN_RMII
        /*DOM-IGNORE-BEGIN*/ = 0x400 /*DOM-IGNORE-END*/,

    // MII connection
    TCPIP_ETH_OPEN_MII
        /*DOM-IGNORE-BEGIN*/ = 0x000 /*DOM-IGNORE-END*/,


    // All capabilities default
    TCPIP_ETH_OPEN_DEFAULT = (TCPIP_ETH_OPEN_AUTO|TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX|
                        TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10|TCPIP_ETH_OPEN_MDIX_AUTO)

} TCPIP_ETH_OPEN_FLAGS; 




// *****************************************************************************
/* TCP/IP Ethernet Pause Types

  Summary:
    Defines the possible TCP/IP Ethernet pause types.

  Description:
    This enumeration defines the pause capabilities of the TCP/IP Ethernet.

  Remarks:
    The pause type is mainly supported over Ethernet links and takes part
    in the PHY link negotiation process.

*/

typedef enum
{
    // No PAUSE capabilities
    TCPIP_ETH_PAUSE_TYPE_NONE     /*DOM-IGNORE-BEGIN*/ = 0x0 /*DOM-IGNORE-END*/,

    // Supports symmetric PAUSE
    TCPIP_ETH_PAUSE_TYPE_PAUSE    /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Supports ASM_DIR
    TCPIP_ETH_PAUSE_TYPE_ASM_DIR  /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // The previous two values converted to TX/RX capabilities:

    // Enable MAC TX pause support
    TCPIP_ETH_PAUSE_TYPE_EN_TX    /*DOM-IGNORE-BEGIN*/ = 0x4 /*DOM-IGNORE-END*/,

    // Enable MAC RX pause support
    TCPIP_ETH_PAUSE_TYPE_EN_RX    /*DOM-IGNORE-BEGIN*/ = 0x8 /*DOM-IGNORE-END*/,

    // All types of pause
    TCPIP_ETH_PAUSE_TYPE_ALL    = (TCPIP_ETH_PAUSE_TYPE_PAUSE|TCPIP_ETH_PAUSE_TYPE_ASM_DIR|
                           TCPIP_ETH_PAUSE_TYPE_EN_TX|TCPIP_ETH_PAUSE_TYPE_EN_RX),

} TCPIP_ETH_PAUSE_TYPE; 


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif//_TCPIP_ETHERNET_H_