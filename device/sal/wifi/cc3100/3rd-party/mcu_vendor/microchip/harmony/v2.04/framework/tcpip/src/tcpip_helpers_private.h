/*******************************************************************************
  Header file for tcpip_helpers_private

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_helpers_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef __TCPIP_HELPERS_PRIVATE_H_
#define __TCPIP_HELPERS_PRIVATE_H_



unsigned char   TCPIP_Helper_FindCommonPrefix (unsigned char * addr1, unsigned char * addr2, unsigned char bytes);

TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str);

const char*     TCPIP_Helper_PowerModeToString(TCPIP_MAC_POWER_MODE mode);

uint16_t        TCPIP_Helper_CalcIPChecksum(uint8_t* buffer, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_PacketChecksum(TCPIP_MAC_PACKET* pPkt, uint8_t* startAdd, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_ChecksumFold(uint32_t checksum);

uint16_t        TCPIP_Helper_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, uint8_t* pDest, uint8_t** pStartAdd, uint16_t len, bool srchTransport);

void            TCPIP_Helper_FormatNetBIOSName(uint8_t Name[16]);


// Protocols understood by the TCPIP_Helper_ExtractURLFields() function.  IMPORTANT: If you 
// need to reorder these (change their constant values), you must also reorder 
// the constant arrays in TCPIP_Helper_ExtractURLFields().
typedef enum
{
	PROTOCOL_HTTP = 0u,
	PROTOCOL_HTTPS,
	PROTOCOL_MMS,
	PROTOCOL_RTSP
} PROTOCOLS;

uint8_t         TCPIP_Helper_ExtractURLFields(uint8_t *vURL, PROTOCOLS *protocol,
                                 uint8_t *vUsername, uint16_t *wUsernameLen,
                                 uint8_t *vPassword, uint16_t *wPasswordLen,
                                 uint8_t *vHostname, uint16_t *wHostnameLen,
                                 uint16_t *wPort, uint8_t *vFilePath, uint16_t *wFilePathLen);


#endif  // __TCPIP_HELPERS_PRIVATE_H_
