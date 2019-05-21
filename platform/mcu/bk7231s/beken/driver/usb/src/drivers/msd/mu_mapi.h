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

#ifndef _MUSB_MSD_API_H_
#define _MUSB_MSD_API_H_

/*
 * Prototypes for MSD class driver API
 * $Revision: 1.11 $
 */
#include "include.h"
#include "mu_mpcsi.h"

/* Debug message values for success and error */
#define MUSB_MSD_DIAG_ERROR                     0x1
#define MUSB_MSD_DIAG_SUCCESS                   0x2

/* Macro to prepare setup packet for Mass Storage Class driver*/
#define MGC_MSD_PREPARE_SETUP_PACKET(pSetup,\
                                     bmActualRequestType,\
                                     bActualRequest,\
                                     wActualValue,\
                                     wActualIndex,\
                                     wActualLength)\
{\
    (pSetup)->bmRequestType = (uint8_t) bmActualRequestType;\
    (pSetup)->bRequest      = (uint8_t) bActualRequest;\
    (pSetup)->wValue        = (uint16_t) MUSB_SWAP16(wActualValue);\
    (pSetup)->wIndex        = (uint16_t) MUSB_SWAP16(wActualIndex);\
    (pSetup)->wLength       = (uint16_t) MUSB_SWAP16(wActualLength);\
}

/* Macro to fill control Irp for Mass Storage Class driver */
#define MGC_MSD_FILL_CONTROL_IRP(pMsdDevice,\
                                 pControlIrp,\
                                 pActualOutBuffer,\
                                 dwRequestedOutLength,\
                                 pActualInBuffer,\
                                 dwRequestedInLength,\
                                 pfControlIrpComplete)\
{\
    (pControlIrp)->pDevice           = pMsdDevice->pUsbDevice;\
    (pControlIrp)->pOutBuffer        = pActualOutBuffer;\
    (pControlIrp)->dwOutLength       = dwRequestedOutLength;\
    (pControlIrp)->pInBuffer         = pActualInBuffer;\
    (pControlIrp)->dwInLength        = dwRequestedInLength;\
    (pControlIrp)->dwStatus          = 0;\
    (pControlIrp)->dwActualOutLength = 0;\
    (pControlIrp)->dwActualInLength  = 0;\
    (pControlIrp)->pfIrpComplete     = pfControlIrpComplete;\
    (pControlIrp)->pCompleteParam    = (void *) pMsdDevice;\
}

typedef struct
{
    uint8_t                         bBusAddress;
    uint8_t                         bInterfaceNumber;
    uint8_t                         bAlternateSetting;
    uint8_t                         bProtocol;
    uint8_t                         bSubclass;
    MUSB_BusHandle                  hBus;
    MUSB_Device                    *pUsbDevice;
    MUSB_DeviceDriver              *pDriver;
    void                           *pDeviceId;
    MUSB_DeviceRequest      Setup;
    MUSB_ControlIrp         ControlIrp;
    const MUSB_EndpointDescriptor  *pBulkInEndpoint;
    const MUSB_EndpointDescriptor  *pBulkOutEndpoint;
    const MUSB_EndpointDescriptor  *pIntrInEndpoint;
    MUSB_HfiVolumeHandle            hVolume;
    MGC_MsdProtocol *pProtocol;
} MGC_MsdDevice;

#if CFG_SUPPORT_MSD
/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the mass-storage class driver, ending with the MUSB_TARGET_ACCEPT.
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals wListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 * @deprecated please use MUSB_FillMsdPeripheralList
 * @see MUSB_FillMsdPeripheralList
 */
extern uint16_t MGC_FillStorageClassPeripheralList(uint8_t *pList, uint16_t wListLength);

/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the mass-storage driver.
 * @param bDriverIndex the driver's index for use with MUSB_TARGET_ACCEPT
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals wListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 */
extern uint16_t MUSB_FillMsdPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
        uint16_t wListRemain);

/**
 * Get a pointer to the mass-storage class driver
 */
extern MUSB_DeviceDriver *MUSB_GetStorageClassDriver(void);
#define MGC_GetStorageClassDriver MUSB_GetStorageClassDriver

/** Connect handler for mass-storage class driver */
extern uint8_t MUSB_MsdConnect(  void              *pPrivateData,
                                 MUSB_BusHandle     hBus,
                                 MUSB_Device       *pUsbDevice,
                                 const uint8_t     *pPeripheralList);

/** Disconnect Handler for Mass Storage Device Driver */
extern void MUSB_MsdDisconnect (void           *pPrivateData,
                                MUSB_BusHandle  hBus,
                                MUSB_Device    *pUsbDevice);
#endif // CFG_SUPPORT_MSD
#endif /* _MUSB_MSD_API_H_ */
// eof

