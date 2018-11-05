/*******************************************************************************
  WILC1000 Wireless Driver Internal Functions

  File Name:
    wdrv_wilc1000_main.h

  Summary:
    Internal functions for WILC1000 wireless driver.

  Description:
    Internal functions for WILC1000 wireless driver.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc. All rights reserved.

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

#ifndef _WDRV_WILC1000_MAIN_H
#define _WDRV_WILC1000_MAIN_H

#include "wdrv_wilc1000_api.h"
#include "tcpip/tcpip_mac.h"
#include "tcpip/src/tcpip_manager_control.h"
#include "tcpip/src/tcpip_packet.h"
#include "tcpip/src/link_list.h"

#if !defined(TCPIP_STACK_USE_EVENT_NOTIFICATION)
#error "TCPIP_STACK_USE_EVENT_NOTIFICATION must be defined for Wi-Fi demos"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#define WDRV_MAX_CLIENT_TABLE_SLOTS 10

// *****************************************************************************
/*  Wi-Fi PS-Poll Listen Interval default settings

  Summary:
    Default values for Wi-Fi PS PS-Poll Listen Interval settings

  Description
    These defines identify various default Wi-Fi PS-Poll settings that can
    be used in the WDRV_PS_POLL_CONTEXT structure.
*/
#define WDRV_DEFAULT_PS_LISTEN_INTERVAL ((uint16_t)1) // 100 ms multiplier, e.g. 1 * 100 ms = 100 ms
#define WDRV_DEFAULT_PS_DTIM_INTERVAL ((uint16_t)2) // number of beacon periods
#define WDRV_DEFAULT_PS_DTIM_ENABLED true // DTIM wake-up enabled (normally the case)

typedef struct
{
    uint8_t trafficEvents;
    uint16_t trafficEventInfo;
} WDRV_WILC1000_USREV_DCPT;

// stack internal notification
typedef struct
{
    bool                     mrfEnabledEvents; // group enabled notification events
    volatile TCPIP_MAC_EVENT mrfPendingEvents; // group notification events that are set, waiting to be re-acknowledged
    TCPIP_MAC_EventF         mrfNotifyFnc; // group notification handler
    const void              *mrfNotifyParam; // notification parameter
} WDRV_WILC1000_EVGROUP_DCPT; // event descriptor

typedef struct {
    uint8_t addr[6];
    uint32_t timeStamp;
} MAC_ADDR;

typedef struct 
{
    MAC_ADDR mac[WDRV_MAX_CLIENT_TABLE_SLOTS];
    uint16_t bitMap;
    uint32_t seqNum;
    uint16_t updated;
} WDRV_CLIENT_CACHE;

typedef struct {
    uint8_t macAddr[6];
    bool isDriverOpen;
    bool updateMacAddressRequired;
    bool isScanDone;
    bool initConn;
    bool isDisconnectRequested;
    OSAL_SEM_HANDLE_TYPE disconnectDoneSync;
    WDRV_CLIENT_CACHE clientCache;
    bool isOtaFwUpdateRequested;
    uint8_t fwOtaServerUrl[128];
} WDRV_WILC1000_PRIV;

bool WDRV_CONFIG_Load(void);
void WDRV_CONFIG_Save(void);
void WDRV_CONFIG_Delete(void);

bool isLinkUp();
void WDRV_Connect(void);
void WDRV_Disconnect(void);

void ConnectEventCB(bool connected, bool isServer, uint8_t const *const client);
void WPSDoneCB(void);

WDRV_CONNECTION_STATES WDRV_ConnectionState_Get(void);

bool isClientCacheUpdated(bool *connected, uint8_t *mac);
void ClientCacheUpdate(bool connected, uint8_t const *const mac);

bool isEventPending(void);
void WDRV_PendingEventProcess(void);

void WDRV_TrafficEventInit(TCPIP_MAC_EventF eventF, const void *eventParam);
void WDRV_TrafficEventDeinit(void);
void WDRV_TrafficEventReq(uint16_t event, uint16_t eventInfo);
bool WDRV_TrafficEventMask(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
bool WDRV_TrafficEventAck(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
TCPIP_MAC_EVENT WDRV_TrafficEventGet(TCPIP_MAC_HANDLE hMac);
void WDRV_AllEventClear(void);
void WDRV_EventSet(uint8_t event);
bool isWdrvExtReady(void);


TCPIP_MAC_RES WDRV_WILC1000_MulticastFilterSet(TCPIP_MAC_HANDLE hMac, const TCPIP_MAC_ADDR *DestMACAddr);

extern WDRV_SCAN_STATUS g_scanStatus;
extern WDRV_SCAN_RESULT g_scanResults[];
extern WDRV_WILC1000_PRIV g_wdrv_priv;

#endif /* _WDRV_WILC1000_MAIN_H */

// DOM-IGNORE-END
