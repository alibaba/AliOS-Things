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

#ifndef _HUB_HTEST_H_
#define _HUB_HTEST_H_

typedef struct
{
    MUSB_Device *pUsbDevice;
    MUSB_BusHandle hBus;
    uint8_t *pHubDscr;
    uint8_t *pHubStatus;
    uint8_t *pPortStatus;
    uint8_t *pIntrIrpData;

    uint8_t *pPortStatusDuringReset;
    uint8_t *pPortStatusDuringDebounce;
    MUSB_pfDriverTimerExpired pfDriverTimerExpired;
    void *pTimerParam;
    uint8_t bTestState;
    MUSB_pfHubEnumerationComplete pfHubEnumerateDevice;
    MUSB_Device      *pChildUsbDevice;

} HubTestDeviceStruct;


typedef struct
{
    uint8_t bSetConfig;
    uint8_t bResetTt;
    uint8_t bGetStatus;
    uint8_t bSetFeature;
    uint8_t bClearFeature;
    uint8_t bDisconnect;
    uint8_t bSetInterface;
    uint8_t bClearTtBuffer;
    uint8_t bGetDescriptor;


    uint8_t bExpectedSetConfig;
    uint8_t bExpectedResetTt;
    uint8_t bExpectedGetStatus;
    uint8_t bExpectedSetFeature;
    uint8_t bExpectedClearFeature;
    uint8_t bExpectedDisconnect;
    uint8_t bExpectedSetInterface;
    uint8_t bExpectedClearTtBuffer;
    uint8_t bExpectedGetDescriptor;
} HubTestResultStruct;




#define MUSB_TEST_INIT                                      0x01
#define MUSB_TEST_CONNECT                                   0x02
#define MUSB_TEST_CONNECT_CTRL_CLB                          0x03
#define MUSB_TEST_CALL_INTR_CLB                             0x04
#define MUSB_TEST_SUBMIT_INTR_IRP                           0x05
#define MUSB_TEST_HUB_STATUS_CTRL_CLB                       0x06
#define MUSB_TEST_DISCON                                    0x07

#define MUSB_TEST_PORT_STATUS_CTRL_CLB                      0x08
#define MUSB_TEST_CALL_TIMER_CALLBACK                       0x09
#define MUSB_TEST_CALL_ENUMERATION_CALLBACK                 0x0A
#define MUSB_TEST_HUB_PORT_DISCON                           0x0B



#define MUSB_TEST_PORT_CONNECT_COMPLETE                     0x01
#define MUSB_TEST_PORT_CONNECT_START                        0x02


extern void MUSB_HubTestRunMain (void);
extern uint32_t  MGC_TestHubDriver (uint8_t bTestNumber);

extern void MGC_PrepareTestDeviceForHubConDiscon(uint8_t bTestCaseNum,
        MUSB_Device *pUsbDevice,
        HubTestDeviceStruct *pTestDevice);

extern void  MGC_HubTestPrepareUsbDevice(MUSB_Device *pUsbDevice, uint8_t bBusAddress, uint8_t bHubPort,
        MUSB_Device *pParentUsbDevice, MUSB_DeviceDescriptor *pDeviceDscr,
        uint8_t *pConfigurationDscrBuffer);

extern uint32_t MGC_TestHubDriverRootHubConDiscon (uint8_t bRootHubConDisconTestNumber);

extern uint32_t MGC_TestHubDriverHubStatusChage (uint8_t bRootHubStatusTestNumber);
extern void MGC_PrepareTestDeviceForHubStatus(uint8_t bTestCaseNum,
        MUSB_Device *pUsbDevice,
        HubTestDeviceStruct *pTestDevice);
extern void
MGC_PrepareTestDeviceForPortStatus(uint8_t bTestCaseNum,
                                   MUSB_Device *pUsbDevice,
                                   HubTestDeviceStruct *pTestDevice);

extern uint32_t
MGC_TestHubDriverPortStatusChage (uint8_t bRootHubStatusTestNumber);


extern uint32_t MGC_TestHubPortConnectDisconnect (uint8_t bRootHubStatusTestNumber);
extern void MGC_PrepareTestDevicePortConnectDisconnect(uint8_t bTestCaseNum,
        MUSB_Device *pUsbDevice,
        HubTestDeviceStruct *pTestDevice);

extern void MUSB_HubTestRunMain (void);
extern void MGC_HubDisplayHubTestDevice(HubTestDeviceStruct *pTestDevice);
extern void MGC_TestResultInit(HubTestResultStruct *pResult);
#endif


