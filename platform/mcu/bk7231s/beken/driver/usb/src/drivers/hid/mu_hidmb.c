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

/*
 * Boot mouse functions
 * $Revision: 1.4 $
 */
#include "include.h"

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_bits.h"
#include "mu_descs.h"
#include "mu_stdio.h"
#include "mu_diag.h"
#include "mu_spi.h"
#include "mu_hid.h"
#include "mu_hidif.h"

/****************************** CONSTANTS ********************************/

/******************************** TYPES **********************************/

typedef struct
{
    MGC_HidDevice *pHidDevice;
    MUSB_SpiDeviceInfo SpiDeviceInfo;
    MUSB_SpiDevice SpiDevice;
    MUSB_SpiDeviceHandle SpiHandle;
    MUSB_Irp Irp;
    uint8_t abReport[4];
    uint8_t bLastButtonMask;
} MGC_HidMouseData;

/******************************* FORWARDS ********************************/
#if CFG_SUPPORT_HID
static uint32_t MGC_HidMouseIntrIrpCallback(void *pContext, MUSB_Irp *pIrp);
static MUSB_SpiStatus MGC_HidMouseGetName(void *pDeviceData,
        MUSB_SpiNameQuery NameQuery,
        uint8_t bIndex,
        uint16_t wLanguageCode,
        uint16_t *pName,
        uint8_t bNameLength);

/******************************** GLOBALS ********************************/

/******************************** FUNCTIONS ******************************/

void *MGC_HidMouseCreateInstance(MGC_HidDevice *pHidDevice,
                                 const MUSB_ConfigurationDescriptor *pConfig,
                                 const MUSB_InterfaceDescriptor *pIface)
{
    uint8_t bEnd;
    uint32_t dwStatus;
    const MUSB_EndpointDescriptor *pEnd;
    const MUSB_EndpointDescriptor *pInEnd = NULL;
    MUSB_DeviceEndpoint end;

    /* try to allocate */
    MGC_HidMouseData *pMouse = (MGC_HidMouseData *)MUSB_MemAlloc(
                                   sizeof(MGC_HidMouseData));
    if(pMouse)
    {
        /* good allocation; initialize */
        MUSB_MemSet(pMouse, 0, sizeof(MGC_HidMouseData));
        pMouse->pHidDevice = pHidDevice;
        pMouse->SpiDeviceInfo.bButtonCount = 3;
        pMouse->SpiDeviceInfo.bmWheels = 2;
        pMouse->SpiDeviceInfo.wVendorId = pHidDevice->pUsbDevice->DeviceDescriptor.idVendor;
        pMouse->SpiDeviceInfo.wProductId = pHidDevice->pUsbDevice->DeviceDescriptor.idProduct;
        pMouse->SpiDeviceInfo.bcdDevice = pHidDevice->pUsbDevice->DeviceDescriptor.bcdDevice;
        pMouse->SpiDeviceInfo.bBusAddress = pHidDevice->pUsbDevice->bBusAddress;
        pMouse->SpiDevice.pPrivateData = pMouse;
        pMouse->SpiDevice.pfGetName = MGC_HidMouseGetName;

        /* find suitable endpoint */
        for(bEnd = 0; bEnd < pIface->bNumEndpoints; bEnd++)
        {
            pEnd = MUSB_FindEndpointDescriptor(pConfig, pIface, bEnd);
            if(pEnd &&
                    (MUSB_ENDPOINT_XFER_INT == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)))
            {
                if(MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress)
                {
                    pInEnd = pEnd;
                }
            }
        }
        /* bail if endpoint not found */
        if(!pInEnd)
        {
            MUSB_DIAG1(1, "HID: insufficient endpoints for keyboard in interface ",
                       pIface->bInterfaceNumber, 10, 0);
            MUSB_MemFree(pMouse);
            pMouse = NULL;
        }
        else
        {
            /* endpoint found; try to open pipe */
            end.pDevice = pHidDevice->pUsbDevice;
            MUSB_MemCopy(&(end.UsbDescriptor), pInEnd, sizeof(end.UsbDescriptor));
            end.wNakLimit = 0xffff;
            pMouse->Irp.hPipe = MUSB_OpenPipe(pHidDevice->hBus, &end, NULL);
            if(!pMouse->Irp.hPipe)
            {
                /* bail on open failure */
                MUSB_DIAG1(1, "HID: pipe open error on endpoint ",
                           pInEnd->bEndpointAddress, 10, 0);
                MUSB_MemFree(pMouse);
                pMouse = NULL;
            }
            else
            {
                /* good open; start transfer */
                pMouse->Irp.pBuffer = pMouse->abReport;
                pMouse->Irp.dwLength = sizeof(pMouse->abReport);
                pMouse->Irp.pfIrpComplete = MGC_HidMouseIntrIrpCallback;
                pMouse->Irp.pCompleteParam = pMouse;
                pMouse->Irp.bAllowShortTransfer = FALSE;
                dwStatus = MUSB_StartTransfer(&(pMouse->Irp));
                if (dwStatus)
                {
                    /* bail on transfer start error */
                    MUSB_DIAG1(3, "HID: MUSB_StartTransfer() status=", dwStatus, 16, 0);
                    MUSB_MemFree(pMouse);
                    pMouse = NULL;
                }
                else
                {
                    /* announce to SPI implementation */
                    if(MUSB_SPI_SUCCESS !=
                            MUSB_SpiAddDevice(&(pMouse->SpiHandle), &(pMouse->SpiDeviceInfo), &(pMouse->SpiDevice)))
                    {
                        /* bail on SPI failure */
                        MUSB_MemFree(pMouse);
                        pMouse = NULL;
                    }
                }
            }
        }
    }
    return pMouse;
}

void MGC_HidMouseDestroyInstance(void *pInstance)
{
    MGC_HidMouseData *pMouse = (MGC_HidMouseData *)pInstance;

    MUSB_ClosePipe(pMouse->Irp.hPipe);
    MUSB_SpiDeviceRemoved(&(pMouse->SpiHandle));
    MUSB_MemFree(pInstance);
}

static uint32_t MGC_HidMouseIntrIrpCallback(void *pContext, MUSB_Irp *pIrp)
{
    uint8_t buttonsNew, buttonsOld, buttonsChanged, button;
    int8_t bX, bY;
    int8_t motions[2];
    MGC_HidMouseData *pMouse = (MGC_HidMouseData *)pIrp->pCompleteParam;

    buttonsNew = pMouse->abReport[0];
    buttonsOld = pMouse->bLastButtonMask;
    buttonsChanged = (buttonsOld ^ buttonsNew) & 7;
    pMouse->bLastButtonMask = pMouse->abReport[0];
    if(buttonsChanged)
    {
        /* buttons changed */
        for(button = 0; button < 3; button++)
        {
            if(buttonsChanged & (1 << button))
            {
                if(buttonsNew & (1 << button)) MUSB_SpiButtonPressed(pMouse->SpiHandle, button);
                else MUSB_SpiButtonReleased(pMouse->SpiHandle, button);
            }
        }
    }
    bX = (int8_t)pMouse->abReport[1];
    bY = (int8_t)pMouse->abReport[2];
    if((bX != 0) || (bY != 0))
    {
        /* motion */
        motions[0] = bX;
        motions[1] = bY;
        MUSB_SpiWheelsMoved(pMouse->SpiHandle, (const int8_t *)&motions);
    }

    return TRUE;
}

static MUSB_SpiStatus MGC_HidMouseGetName(void *pDeviceData,
        MUSB_SpiNameQuery NameQuery,
        uint8_t bIndex,
        uint16_t wLanguageCode,
        uint16_t *pName,
        uint8_t bNameLength)
{
    /* TODO */
    *pName = 0;
    return MUSB_SPI_SUCCESS;
}
#endif // CFG_SUPPORT_HID
// eof
