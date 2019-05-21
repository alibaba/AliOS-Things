/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

#ifndef _MUSB_HID_API_H_
#define _MUSB_HID_API_H_

#include "class/mu_hid.h"
#include "mu_ghi.h"
#include "include.h"

/*
 * Prototypes for HID class driver API
 * $Revision: 1.8 $
 */

/* Debug message values for success and error */
#define MUSB_HID_DIAG_ERROR                     0x1
#define MUSB_HID_DIAG_SUCCESS                   0x2

typedef struct
{
    MUSB_BusHandle                  hBus;
    MUSB_Device                    *pUsbDevice;
    MUSB_DeviceDriver              *pDriver;
    MUSB_DeviceRequest      Setup;
    MUSB_ControlIrp         ControlIrp;
    const MUSB_InterfaceDescriptor *pKeyboardIface;
    void *pKeyboardInstance;
    const MUSB_InterfaceDescriptor *pMouseIface;
    void *pMouseInstance;
    const MUSB_InterfaceDescriptor *pIfaceDescriptor;
    const MUSB_HidDescriptor *pHidDescriptor;
    MUSB_GhiDeviceHandle hGhiDevice;
    MUSB_Irp GhiIntrIrp;
    MUSB_pfGhiInputReport pfInputHandler;
    MUSB_pfGhiOutputComplete pfOutputCompletionHandler;
    MUSB_GhiDeviceInfo GhiDeviceInfo;
    MUSB_GhiDevice GhiDevice;
    uint8_t *pGhiInputBuffer;
    uint8_t *pGhiOutputBuffer;
    uint16_t wGhiInputLength;
    uint16_t wGhiOutputLength;
    uint8_t *pReportDescriptor;
    uint16_t wReportDescriptorLength;
    uint8_t bGhiAccepted;
} MGC_HidDevice;

#if CFG_SUPPORT_HID
/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the HID driver.
 * @param bDriverIndex the driver's index for use with MUSB_TARGET_ACCEPT
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals wListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 */
extern uint16_t MUSB_FillHidPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
        uint16_t wListRemain);

/**
 * Get a pointer to the HID class driver
 */
extern MUSB_DeviceDriver *MUSB_GetHidClassDriver(void);

/** Connect handler for HID class driver */
extern uint8_t MUSB_HidConnect(  void              *pPrivateData,
                                 MUSB_BusHandle     hBus,
                                 MUSB_Device       *pUsbDevice,
                                 const uint8_t     *pPeripheralList);

/** Disconnect Handler for HID class Driver */
extern void MUSB_HidDisconnect (void           *pPrivateData,
                                MUSB_BusHandle  hBus,
                                MUSB_Device    *pUsbDevice);
#endif // CFG_SUPPORT_HID
#endif /* _MUSB_MSD_API_H_ */
// eof

