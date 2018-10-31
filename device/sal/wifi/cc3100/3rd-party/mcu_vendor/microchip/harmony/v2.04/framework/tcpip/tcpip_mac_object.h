/*******************************************************************************
  Multiple MAC Module implementation for Microchip Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_object.h

  Summary:
   Multiple MAC module API definitions.
    
  Description:
   This file provides the API definitions for Multiple MAC module implementation.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _TCPIP_MAC_OBJECT_H_ 
#define _TCPIP_MAC_OBJECT_H_ 

#include "tcpip/tcpip_mac.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: MPLAB Harmony MAC Parameterized Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

typedef struct TCPIP_MAC_OBJECT_TYPE
{
    int                 macId;          // one of the TCPIP_MODULE_MAC_ID IDs
    const char*         macName;        // PIC32INT, MRF24WN, etc
    SYS_MODULE_OBJ      (*TCPIP_MAC_Initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
    void                (*TCPIP_MAC_Deinitialize)(SYS_MODULE_OBJ object);
    void                (*TCPIP_MAC_Reinitialize)(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);
    SYS_STATUS          (*TCPIP_MAC_Status)( SYS_MODULE_OBJ object );
    void                (*TCPIP_MAC_Tasks)( SYS_MODULE_OBJ object );
    DRV_HANDLE          (*TCPIP_MAC_Open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
    void                (*TCPIP_MAC_Close)(DRV_HANDLE hMac);
    bool                (*TCPIP_MAC_LinkCheck)(DRV_HANDLE hMac);
    TCPIP_MAC_RES       (*TCPIP_MAC_RxFilterHashTableEntrySet)(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);
    bool 	            (*TCPIP_MAC_PowerMode)(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);
    TCPIP_MAC_RES       (*TCPIP_MAC_PacketTx)(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
    TCPIP_MAC_PACKET*   (*TCPIP_MAC_PacketRx)(DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat);
    TCPIP_MAC_RES       (*TCPIP_MAC_Process)(DRV_HANDLE hMac);
    TCPIP_MAC_RES       (*TCPIP_MAC_StatisticsGet)(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
    TCPIP_MAC_RES       (*TCPIP_MAC_ParametersGet)(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);
    TCPIP_MAC_RES       (*TCPIP_MAC_RegisterStatisticsGet)(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
    size_t              (*TCPIP_MAC_ConfigGet)(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

    bool                (*TCPIP_MAC_EventMaskSet)(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
    bool                (*TCPIP_MAC_EventAcknowledge)(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
    TCPIP_MAC_EVENT     (*TCPIP_MAC_EventPendingGet)(DRV_HANDLE hMac);

}TCPIP_MAC_OBJECT;        // TCPIP MAC object descriptor

typedef struct
{
    const TCPIP_MAC_OBJECT* pObj;       // associated object
                                        // pointer to the object here is intended to allow
                                        // multiple MAC objects of the same type
                                        // to share an unique const object table
    void*               mac_data[0];    // specific MAC object data
}TCPIP_MAC_DCPT; 
    
// supported MAC objects
extern const TCPIP_MAC_OBJECT DRV_ETHMAC_PIC32MACObject;
extern const TCPIP_MAC_OBJECT DRV_GMAC_Object;
extern const TCPIP_MAC_OBJECT DRV_ENCX24J600_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_MRF24WN_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_WINC1500_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_WILC1000_MACObject;
extern const TCPIP_MAC_OBJECT DRV_PIC32WK_MACObject;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // _TCPIP_MAC_OBJECT_H_ 
