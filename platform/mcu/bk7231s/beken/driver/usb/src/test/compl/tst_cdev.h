/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

#ifndef _MUSB_CTEST_H_
#define _MUSB_CTEST_H_

#define MGC_COMPL_SOUR_SINK_TEST    0x01
#define MGC_COMPL_NO_TEST           0x00

#define MUSB_TEST_SUCCESS   0x05
typedef enum
{
    MGC_NoMessage = 0x00,
    MGC_ControlRead,
    MGC_ControlWrite,
    MGC_BulkRead,
    MGC_BulkWrite,
    MGC_InterruptRead,
    MGC_InterruptWrite,
    MGC_IsochRead,
    MGC_IsochWrite,
    MGC_DoDisconnect,
    MGC_DoConnect,
    MGC_SetRemoteWakeupTo
} MGC_ComplTestMessages;

typedef struct
{
    uint32_t    dwStatus;
    MUSB_EndpointDescriptor endpointDescriptor;
} MGC_ComplTestPipe;

typedef uint32_t (*MGC_DeviceRequestHandler)(void *, MGC_ComplTestMessages, void *);

typedef struct
{
    MUSB_DeviceDriver       *pDeviceDriver;
    void                    *pTestDevice;
    MGC_ComplTestMessages   message;
    MUSB_ControlIrp         *pControlIrp;
    MUSB_BulkIrp            *pBulkIrp;
    MUSB_InterruptIrp       *pInterruptIrp;
    MUSB_IsochIrp           *pIsochIrp;
    MGC_DeviceRequestHandler    pfDeviceRequestHandler;
} MGC_ComplTestSetup;

typedef struct
{
    uint8_t                 bCurEndpoint;
    uint8_t                 bCurTest;
    uint8_t                 bCurPacketIndex;
    uint8_t                 bLoopbackInReceived;
    uint8_t                 aActualDescriptorArray[200];
    uint8_t                 aCurDescriptorArray[200];
    uint16_t                wActualDescriptorLength;
    uint16_t                wCurDescriptorLength;
    MUSB_EndpointDescriptor *pEndpointDscr;
    uint8_t                 aComplCapabilityTable[256 * 20];
    uint8_t                 gEpBuffer[20][2 * 1024];
    uint8_t                 bEpEnable[256];
    uint16_t                wExtraBuffer[256];
    uint32_t                dwPattern;
    uint32_t                dwModifier;
    MUSB_ComplCapabilityTable   ePCapabilityTable;
} MGC_ComplTestDevice;

extern MGC_ComplTestSetup   MGC_ComplTestInstance;
extern MUSB_DeviceDriver MGC_ComplDeviceDriver;

extern void *
MUSB_ComplDummyDeviceInit(uint8_t *pComplCapabilityTable) ;

extern void MGC_ComplDisplayControlIrp(MUSB_ControlIrp *pControlIrp);
extern void MGC_ComplDisplaySetupPkt(MUSB_DeviceRequest *pSetup);
extern uint32_t
MUSB_DummyDevice(void *pTestDevice, MGC_ComplTestMessages message, void *pPacket);

extern uint8_t
MGC_ComplTestGetEpIndex(MUSB_PipePtr hPipe, MUSB_ComplCapabilityTable *pCapabilityTable) ;


#endif
