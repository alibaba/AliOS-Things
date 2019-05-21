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
 * Top-level HID class driver module
 * $Revision: 1.14 $
 */
#include "include.h"

#include "mu_arch.h"
#include "mu_bits.h"
#include "mu_cdi.h"
#include "mu_descs.h"
#include "mu_diag.h"
#include "mu_ghi.h"
#include "mu_kii.h"
#include "mu_mem.h"
#include "mu_spi.h"
#include "mu_stdio.h"
#include "mu_hid.h"
#include "mu_hidif.h"

extern void *MGC_HidKeyboardCreateInstance(MGC_HidDevice *pHidDevice,
        const MUSB_ConfigurationDescriptor *pConfig,
        const MUSB_InterfaceDescriptor *pIface);

extern void MGC_HidKeyboardDestroyInstance(void *pInstance);

extern void *MGC_HidMouseCreateInstance(MGC_HidDevice *pHidDevice,
                                        const MUSB_ConfigurationDescriptor *pConfig,
                                        const MUSB_InterfaceDescriptor *pIface);

extern void MGC_HidMouseDestroyInstance(void *pInstance);

/******************************** TYPES **********************************/

typedef struct
{
    uint32_t dwUsagePage;
    uint32_t dwLogicalMin;
    uint32_t dwLogicalMax;
    uint32_t dwPhysicalMin;
    uint32_t dwPhysicalMax;
    uint32_t dwUnitExp;
    uint32_t dwUnit;
    uint32_t dwReportSize;
    uint32_t dwReportId;
    uint32_t dwReportCount;
    uint32_t dwUsage;
    uint32_t dwUsageMin;
    uint32_t dwUsageMax;
} MGC_HidCurrentAttr;

/*************************** FORWARDS *****************************/
#if CFG_SUPPORT_HID
static uint32_t MGC_HidConfigureDevice(MGC_HidDevice *pHidDevice);
static uint32_t MGC_HidSetConfigCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);
static uint32_t MGC_HidEnableWakeup(MGC_HidDevice *pHidDevice);
static uint32_t MGC_HidEnableWakeupCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);
static uint32_t MGC_HidGetReportDescriptor(MGC_HidDevice *pHidDevice);
static uint32_t MGC_HidGetReportDescCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);
static void MGC_HidParseReportDesc(MGC_HidDevice *pHidDevice);
static void MGC_HidSetInputHandler(void *pDeviceData,
                                   MUSB_pfGhiInputReport pfInputHandler);
static void MGC_HidSetOutputHandler(void *pDeviceData,
                                    MUSB_pfGhiOutputComplete pfOutputCompletionHandler);
static MUSB_GhiStatus MGC_HidGetInput(void *pDeviceData,
                                      uint8_t bReportId, uint8_t *pInputBuffer, uint16_t wLength);
static MUSB_GhiStatus MGC_HidSetOutput(void *pDeviceData,
                                       uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength);
static MUSB_GhiStatus MGC_HidSetFeature(void *pDeviceData,
                                        uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength);

/**************************** GLOBALS *****************************/
static const uint8_t MGC_aHidPeripheralList1[] =
{
    /* Apple */
    MUSB_TARGET_VID, 0xac, 0x05,
    MUSB_TARGET_CONFIG, 0x01,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

static const uint8_t MGC_aHidPeripheralList2[] =
{
    /* boot keyboard/mouse */
    MUSB_TARGET_CLASS, MUSB_CLASS_PER_INTERFACE,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_HID,
    MUSB_TARGET_SUBCLASS, 1,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

/** HID class driver */
MUSB_DeviceDriver MGC_HidDeviceDriver =
{
    NULL,
    0,
    NULL,
    MUSB_HidConnect,
    MUSB_HidDisconnect,
    NULL,
    NULL,
};

/** SET_FEATURE(DEVICE_REMOTE_WAKEUP) request */
static uint8_t MGC_aHidSetRemoteWakeup[] =
{
    MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_SET_FEATURE,
    MUSB_FEATURE_DEVICE_REMOTE_WAKEUP, 0,
    0, 0,
    0, 0
};

/*************************** FUNCTIONS ****************************/

uint16_t MUSB_FillHidPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
                                    uint16_t wListRemain)
{
    uint16_t wCount;
    uint16_t wResult = 0;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aHidPeripheralList1),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aHidPeripheralList1, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aHidPeripheralList2),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aHidPeripheralList2, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    return wResult;
}

MUSB_DeviceDriver *MUSB_GetHidClassDriver()
{
    return &MGC_HidDeviceDriver;
}

/**
 * Allocate per-device data
 */
static MGC_HidDevice *MGC_HidDeviceInit(MUSB_Device *pUsbDevice,
                                        const MUSB_ConfigurationDescriptor *pConfig)
{
    const MUSB_HidDescriptor *pHidDescriptor;
    MGC_HidDevice            *pHidDevice;
    uint16_t wReportLength = 0;

    /* find HID descriptor */
    pHidDescriptor = (const MUSB_HidDescriptor *)MUSB_FindDescriptor(
                         pConfig, MUSB_HID_DESCRIPTOR, 0);
    if(pHidDescriptor)
    {
        /* assume report descriptor is first */
        if(MUSB_REPORT_DESCRIPTOR == pHidDescriptor->bClassDescriptorType)
        {
            wReportLength = MUSB_SWAP16(pHidDescriptor->wDescriptorLength);
        }
    }

    pHidDevice = (MGC_HidDevice *)MUSB_MemAlloc(sizeof(MGC_HidDevice) + wReportLength);
    if(!pHidDevice)
    {
        MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR, "HID Error: Insufficient memory");
        return (NULL);
    }
    MUSB_MemSet((void *)pHidDevice, 0, sizeof (MGC_HidDevice));

    pHidDevice->pDriver = &MGC_HidDeviceDriver;
    pHidDevice->pHidDescriptor = pHidDescriptor;
    if(wReportLength)
    {
        pHidDevice->wReportDescriptorLength = wReportLength;
        pHidDevice->pReportDescriptor = (uint8_t *)pHidDevice + sizeof(MGC_HidDevice);
    }
    pHidDevice->GhiDeviceInfo.bBusAddress = pUsbDevice->bBusAddress;
    pHidDevice->GhiDeviceInfo.wVendorId = pUsbDevice->DeviceDescriptor.idVendor;
    pHidDevice->GhiDeviceInfo.wProductId = pUsbDevice->DeviceDescriptor.idProduct;
    pHidDevice->GhiDeviceInfo.bcdDevice = pUsbDevice->DeviceDescriptor.bcdDevice;
    pHidDevice->GhiDevice.pPrivateData = pHidDevice;
    pHidDevice->GhiDevice.pfSetInputHandler = MGC_HidSetInputHandler;
    pHidDevice->GhiDevice.pfSetOutputCompletionHandler = MGC_HidSetOutputHandler;
    pHidDevice->GhiDevice.pfGetInput = MGC_HidGetInput;
    pHidDevice->GhiDevice.pfSetOutput = MGC_HidSetOutput;
    pHidDevice->GhiDevice.pfSetFeature = MGC_HidSetFeature;

    MUSB_DIAG_STRING(MUSB_HID_DIAG_SUCCESS, "HID: Initialization Completed");

    return pHidDevice;
}

/** This function is called when a HID device is connected */
uint8_t
MUSB_HidConnect(  void              *pPrivateData,
                  MUSB_BusHandle     hBus,
                  MUSB_Device       *pUsbDevice,
                  const uint8_t     *pPeripheralList)
{
    uint8_t bConfig, bIndex;
    MGC_HidDevice	         *pHidDevice;
    const MUSB_InterfaceDescriptor *pIface = NULL;
    const MUSB_InterfaceDescriptor *pGenericIface = NULL;
    const MUSB_InterfaceDescriptor *pKeyboardIface = NULL;
    const MUSB_InterfaceDescriptor *pMouseIface = NULL;
    MUSB_ConfigurationDescriptor *pConfig = NULL;
    uint8_t bStatus = FALSE;

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_HID_DIAG_SUCCESS, "HID Device Connected");

    /* find interfaces with supported subclass/protocol combinations and reject others */
    for(bConfig = 0; bConfig < pUsbDevice->DeviceDescriptor.bNumConfigurations; bConfig++)
    {
        pConfig = (MUSB_ConfigurationDescriptor *)pUsbDevice->apConfigDescriptors[bConfig];
        for(bIndex = 0; bIndex < pConfig->bNumInterfaces; bIndex++)
        {
            /* assume no alternates */
            pIface = MUSB_FindInterfaceDescriptor(pConfig, bIndex, 0);
            if(pIface && (MUSB_CLASS_HID == pIface->bInterfaceClass) &&
                    (pIface->bNumEndpoints >= 1)) /*  && (1 == pIface->bInterfaceSubClass) */
            {
                pUsbDevice->pCurrentConfiguration = pConfig;
                switch(pIface->bInterfaceProtocol)
                {
                case 0:
                    pGenericIface = pIface;
                    break;
                case 1:
                    pKeyboardIface = pIface;
                    break;
                case 2:
                    pMouseIface = pIface;
                    break;
                default:
                    break;
                }
            }
        }
    }

    /* if we found supported interfaces, configure and start */
    if(pGenericIface || pKeyboardIface || pMouseIface)
    {
        pHidDevice = MGC_HidDeviceInit(pUsbDevice, pConfig);
        if(pHidDevice)
        {
            pUsbDevice->pDriverPrivateData = pHidDevice;

            /* Initialize */
            pHidDevice->pUsbDevice     = pUsbDevice;
            pHidDevice->hBus           = hBus;
            pHidDevice->pKeyboardIface = pKeyboardIface;
            pHidDevice->pMouseIface    = pMouseIface;
            pHidDevice->pIfaceDescriptor = pGenericIface;

            /* Configure the device */
            bStatus = (uint8_t)MGC_HidConfigureDevice(pHidDevice);
            if (MUSB_STATUS_OK == bStatus)
            {
                bStatus = TRUE;
            }
        }
    }
    else
    {
        MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR,
                         "HID Error: No interface has supported subclass/protocol");
    }

    return (bStatus);
}

/** Disconnect Handler for HID Driver */
void MUSB_HidDisconnect (void           *pPrivateData,
                         MUSB_BusHandle  hBus,
                         MUSB_Device    *pUsbDevice)
{
    MGC_HidDevice            *pHidDevice;


    pHidDevice = (MGC_HidDevice *)pUsbDevice->pDriverPrivateData;


    /* Check against the USB device and bus handle */
    if( (hBus != pHidDevice->hBus) || (pUsbDevice != pHidDevice->pUsbDevice) )
    {
        MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR, "HID Error: HID Device Disconnect Callback");
        MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR, "HID Error: Wrong 'Usb Device' and 'Bus' handles ");
        return;
    }

    /* Release the resources held */
    if(pHidDevice->pKeyboardIface)
    {
        MGC_HidKeyboardDestroyInstance(pHidDevice->pKeyboardInstance);
    }
    if(pHidDevice->pMouseIface)
    {
        MGC_HidMouseDestroyInstance(pHidDevice->pMouseInstance);
    }

    if(pHidDevice->bGhiAccepted)
    {
        if(pHidDevice->GhiIntrIrp.hPipe)
        {
            MUSB_ClosePipe(pHidDevice->GhiIntrIrp.hPipe);
            pHidDevice->GhiIntrIrp.hPipe = NULL;
        }
        if(pHidDevice->pGhiInputBuffer)
        {
            MUSB_MemFree(pHidDevice->pGhiInputBuffer);
            pHidDevice->pGhiInputBuffer = NULL;
        }
        if(pHidDevice->pGhiOutputBuffer)
        {
            MUSB_MemFree(pHidDevice->pGhiOutputBuffer);
            pHidDevice->pGhiOutputBuffer = NULL;
        }
        if(pHidDevice->GhiDeviceInfo.aInputItem)
        {
            MUSB_MemFree((void *)pHidDevice->GhiDeviceInfo.aInputItem);
            /* these are allocated together */
            pHidDevice->GhiDeviceInfo.aInputItem = NULL;
            pHidDevice->GhiDeviceInfo.aOutputItem = NULL;
            pHidDevice->GhiDeviceInfo.aFeatureItem = NULL;
        }
        MUSB_GhiDeviceRemoved(pHidDevice->hGhiDevice);
    }

    pUsbDevice->pDriverPrivateData = NULL;
    MUSB_MemFree(pHidDevice);
    MUSB_DIAG_STRING(MUSB_HID_DIAG_SUCCESS, "HID Device Disconnected Successfully");
    return;
}

/** Configure the device */
static uint32_t MGC_HidConfigureDevice(MGC_HidDevice *pHidDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pSetup          = &(pHidDevice->Setup);
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for sending Set Config Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_CONFIGURATION;
    pSetup->wValue        = (uint16_t)pHidDevice->pUsbDevice->pCurrentConfiguration->bConfigurationValue;
    pSetup->wIndex        = (uint16_t)0;
    pSetup->wLength       = (uint16_t)0;

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidSetConfigCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Set Configuration Request failed, dwStatus: ", dwStatus, 16, FALSE);
    }
    return (dwStatus);
}

/** Callback function when device acknowledges set config reqeust. */
static uint32_t MGC_HidSetConfigCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    uint32_t dwStatus;
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Set Config Callback Status=", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_HID_DIAG_SUCCESS, "HID Device Configured Successfully");

    if(pHidDevice->pUsbDevice->pCurrentConfiguration->bmAttributes & (1 << 5))
    {
        dwStatus = MGC_HidEnableWakeup(pHidDevice);
    }
    else
    {
        dwStatus = MGC_HidGetReportDescriptor(pHidDevice);
    }

    if(dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: setup request status=", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    return TRUE;
}

static uint32_t MGC_HidEnableWakeup(MGC_HidDevice *pHidDevice)
{
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)MGC_aHidSetRemoteWakeup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidEnableWakeupCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: SetFeature Request failed, dwStatus: ", dwStatus, 16, FALSE);
    }
    return (dwStatus);
}

static uint32_t MGC_HidEnableWakeupCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp)
{
    uint32_t dwStatus;
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Set Feature Callback Status=", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    dwStatus = MGC_HidGetReportDescriptor(pHidDevice);
    if(dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Get report descriptor status=", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    return TRUE;
}

/** After getting connect callback, driver calls this function to get report descriptor */
static uint32_t MGC_HidGetReportDescriptor(MGC_HidDevice *pHidDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pSetup          = &(pHidDevice->Setup);
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_IN | MUSB_TYPE_STANDARD | MUSB_RECIP_INTERFACE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_GET_DESCRIPTOR;
    pSetup->wValue        = (uint16_t)MUSB_SWAP16(MUSB_REPORT_DESCRIPTOR << 8);
    pSetup->wIndex        = (uint16_t)MUSB_SWAP16(pHidDevice->pIfaceDescriptor->bInterfaceNumber);
    pSetup->wLength       = (uint16_t)MUSB_SWAP16(pHidDevice->wReportDescriptorLength);

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = pHidDevice->pReportDescriptor;
    pControlIrp->dwInLength        = pHidDevice->wReportDescriptorLength;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidGetReportDescCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: GetDescriptor Request failed, dwStatus: ", dwStatus, 16, FALSE);
    }
    return (dwStatus);
}

static uint32_t MGC_HidGhiIntrIrpCallback(void *pContext, MUSB_Irp *pIrp)
{
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *) pContext;
    if(pHidDevice && pHidDevice->pfInputHandler)
    {
        pHidDevice->pfInputHandler(pHidDevice->hGhiDevice, pIrp->pBuffer,
                                   pIrp->dwActualLength);
    }

    return TRUE;
}

static void MGC_HidOpenIntrPipe(MGC_HidDevice *pHidDevice)
{
    uint8_t bEnd;
    uint32_t dwStatus;
    const MUSB_EndpointDescriptor *pEnd;
    const MUSB_EndpointDescriptor *pInEnd;
    MUSB_DeviceEndpoint end;

    /* find suitable endpoint */
    for(bEnd = 0; bEnd < pHidDevice->pIfaceDescriptor->bNumEndpoints; bEnd++)
    {
        pEnd = MUSB_FindEndpointDescriptor(pHidDevice->pUsbDevice->pCurrentConfiguration, pHidDevice->pIfaceDescriptor, bEnd);
        if(pEnd &&
                (MUSB_ENDPOINT_XFER_INT == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)))
        {
            if(MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress)
            {
                pInEnd = pEnd;
            }
        }
    }
    if(pInEnd)
    {
        /* endpoint found; try to open pipe */
        end.pDevice = pHidDevice->pUsbDevice;
        MUSB_MemCopy(&(end.UsbDescriptor), pInEnd, sizeof(end.UsbDescriptor));
        end.wNakLimit = 0xffff;
        pHidDevice->GhiIntrIrp.hPipe = MUSB_OpenPipe(pHidDevice->hBus, &end, NULL);
        if(!pHidDevice->GhiIntrIrp.hPipe)
        {
            /* warn on open failure */
            MUSB_DIAG1(1, "HID: pipe open error on endpoint ",
                       pInEnd->bEndpointAddress, 10, 0);
        }
        else
        {
            /* good open; start transfer */
            pHidDevice->GhiIntrIrp.pBuffer = pHidDevice->pGhiInputBuffer;
            pHidDevice->GhiIntrIrp.dwLength = pHidDevice->wGhiInputLength;
            pHidDevice->GhiIntrIrp.pfIrpComplete = MGC_HidGhiIntrIrpCallback;
            pHidDevice->GhiIntrIrp.pCompleteParam = pHidDevice;
            pHidDevice->GhiIntrIrp.bAllowShortTransfer = FALSE;
            dwStatus = MUSB_StartTransfer(&(pHidDevice->GhiIntrIrp));
            if (dwStatus)
            {
                /* warn on transfer start error */
                MUSB_DIAG1(3, "HID: MUSB_StartTransfer() status=", dwStatus, 16, 0);
            }
        }
    }
}

static uint32_t MGC_HidGetReportDescCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MUSB_GhiStatus GhiStatus;
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: GetDescriptor Callback Status=", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    /* start boot-keyboard functionality if available */
    if(pHidDevice->pKeyboardIface)
    {
        MGC_HidParseReportDesc(pHidDevice);
        pHidDevice->pKeyboardInstance = MGC_HidKeyboardCreateInstance(pHidDevice,
                                        pHidDevice->pUsbDevice->pCurrentConfiguration,
                                        pHidDevice->pKeyboardIface);
        if(!pHidDevice->pKeyboardInstance)
        {
            MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR, "HID Error: Failed to start keyboard device");
        }
    }

    /* start boot-mouse functionality if available */
    if(pHidDevice->pMouseIface)
    {
        MGC_HidParseReportDesc(pHidDevice);
        pHidDevice->pMouseInstance = MGC_HidMouseCreateInstance(pHidDevice,
                                     pHidDevice->pUsbDevice->pCurrentConfiguration,
                                     pHidDevice->pMouseIface);
        if(!pHidDevice->pMouseInstance)
        {
            MUSB_DIAG_STRING(MUSB_HID_DIAG_ERROR, "HID Error: Failed to start mouse device");
        }
    }

    /* start generic functionality if available */
    if(pHidDevice->pIfaceDescriptor)
    {
        MGC_HidParseReportDesc(pHidDevice);
        MGC_HidOpenIntrPipe(pHidDevice);
        GhiStatus = MUSB_GhiAddDevice(&(pHidDevice->hGhiDevice),
                                      &(pHidDevice->GhiDeviceInfo), &(pHidDevice->GhiDevice));
        if(MUSB_GHI_SUCCESS == GhiStatus)
        {
            pHidDevice->bGhiAccepted = TRUE;
        }
        else
        {
            MUSB_ClosePipe(pHidDevice->GhiIntrIrp.hPipe);
            pHidDevice->GhiIntrIrp.hPipe = NULL;
        }
    }

    return TRUE;
}

static void MGC_HidParseItemType(uint32_t dwData, MUSB_GhiItemInfo *pInfo)
{
    pInfo->bConstant = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_CONSTANT)) ? TRUE : FALSE;
    pInfo->bVariable = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_VARIABLE)) ? TRUE : FALSE;
    pInfo->bRelative = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_RELATIVE)) ? TRUE : FALSE;
    pInfo->bWrap = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_WRAP)) ? TRUE : FALSE;
    pInfo->bNonLinear = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NONLINEAR)) ? TRUE : FALSE;
    pInfo->bPreferredState = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NO_PREFERRED)) ? FALSE : TRUE;
    pInfo->bNullState = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NULL_STATE)) ? TRUE : FALSE;
    pInfo->bVolatile = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_VOLATILE)) ? TRUE : FALSE;
    pInfo->bBufferedBytes = (dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_BUFFERED_BYTES)) ? TRUE : FALSE;
}

#if MUSB_DIAG > 1
static void MGC_HidPrintItemType(const char *name, uint32_t dwData)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, name);
    MUSB_StringConcat(aLine, 80, "=");
    MUSB_Stringize(aNumber, 12, dwData, 16, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    MUSB_StringConcat(aLine, 80, ":");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_CONSTANT))
        MUSB_StringConcat(aLine, 80, " const");
    else
        MUSB_StringConcat(aLine, 80, " data");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_VARIABLE))
        MUSB_StringConcat(aLine, 80, " var");
    else
        MUSB_StringConcat(aLine, 80, " array");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_RELATIVE))
        MUSB_StringConcat(aLine, 80, " rel");
    else
        MUSB_StringConcat(aLine, 80, " abs");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_WRAP))
        MUSB_StringConcat(aLine, 80, " wrap");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NONLINEAR))
        MUSB_StringConcat(aLine, 80, " non-linear");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NO_PREFERRED))
        MUSB_StringConcat(aLine, 80, " no-pref");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_NULL_STATE))
        MUSB_StringConcat(aLine, 80, " has-null");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_VOLATILE))
        MUSB_StringConcat(aLine, 80, " volatile");
    if(dwData & (1 << MUSB_S_HID_MAIN_IOF_DATA_BUFFERED_BYTES))
        MUSB_StringConcat(aLine, 80, " buf-bytes");
    else
        MUSB_StringConcat(aLine, 80, " bitfield");

    MUSB_PrintLine(aLine);
}
#else
#define MGC_HidPrintItemType(name, dwData)
#endif

static const char *MGC_aHidCollectionNames[] =
{
    "Physical Collection",
    "Application Collection",
    "Logical Collection",
    "Report Collection",
    "Named Array Collection",
    "Usage Switch Collection",
    "Usage Modifier Collection"
};

static void MGC_HidParseCollectionType(uint32_t dwData)
{
    if(dwData <= MUSB_HID_MAIN_COLL_DATA_USAGE_MODIFIER)
    {
        MUSB_PrintLine(MGC_aHidCollectionNames[dwData]);
    }
    else
    {
        MUSB_PrintLine("Unknown collection type");
    }
}

static void MGC_HidUpdateGlobal(uint8_t bTag, uint32_t dwData,
                                MGC_HidCurrentAttr *pAttr)
{
    switch(bTag)
    {
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_USAGE_PAGE:
        pAttr->dwUsagePage = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MIN:
        pAttr->dwLogicalMin = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MAX:
        pAttr->dwLogicalMax = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MIN:
        pAttr->dwPhysicalMin = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MAX:
        pAttr->dwPhysicalMax = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT_EXPONENT:
        pAttr->dwUnitExp = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT:
        pAttr->dwUnit = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_SIZE:
        pAttr->dwReportSize = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_ID:
        pAttr->dwReportId = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_COUNT:
        pAttr->dwReportCount = dwData;
        break;
    }
}

#if MUSB_DIAG > 1
static void MGC_HidPrintGlobal(uint8_t bTag, uint32_t dwData)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Global=");
    MUSB_Stringize(aNumber, 12, bTag, 16, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    switch(bTag)
    {
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_USAGE_PAGE:
        MUSB_StringConcat(aLine, 80, " (usage page=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MIN:
        MUSB_StringConcat(aLine, 80, " (logical min=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_LOGICAL_MAX:
        MUSB_StringConcat(aLine, 80, " (logical max=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MIN:
        MUSB_StringConcat(aLine, 80, " (physical min=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PHYSICAL_MAX:
        MUSB_StringConcat(aLine, 80, " (physical max=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT_EXPONENT:
        MUSB_StringConcat(aLine, 80, " (unit exponent=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_UNIT:
        MUSB_StringConcat(aLine, 80, " (unit=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_SIZE:
        MUSB_StringConcat(aLine, 80, " (report size=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_ID:
        MUSB_StringConcat(aLine, 80, " (report ID=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_REPORT_COUNT:
        MUSB_StringConcat(aLine, 80, " (report count=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PUSH:
        MUSB_StringConcat(aLine, 80, "Push");
        break;
    case MUSB_HID_REPORT_ITEMTAG_GLOBAL_POP:
        MUSB_StringConcat(aLine, 80, "Pop");
        break;
    }
    MUSB_PrintLine(aLine);
}
#else
#define MGC_HidPrintGlobal(bTag, dwData)
#endif

static void MGC_HidUpdateLocal(uint8_t bTag, uint32_t dwData,
                               MGC_HidCurrentAttr *pAttr)
{
    switch(bTag)
    {
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE:
        pAttr->dwUsage = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MIN:
        pAttr->dwUsageMin = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MAX:
        pAttr->dwUsageMax = dwData;
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_INDEX:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MIN:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MAX:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_INDEX:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MIN:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MAX:
        /* TODO? */
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DELIMITER:
        /* TODO? */
        break;
    }
}

#if MUSB_DIAG > 1
static void MGC_HidPrintLocal(uint8_t bTag, uint32_t dwData)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Local=");
    MUSB_Stringize(aNumber, 12, bTag, 16, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    switch(bTag)
    {
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE:
        MUSB_StringConcat(aLine, 80, " (usage=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MIN:
        MUSB_StringConcat(aLine, 80, " (usage min=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_USAGE_MAX:
        MUSB_StringConcat(aLine, 80, " (usage max=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_INDEX:
        MUSB_StringConcat(aLine, 80, " (designator index=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MIN:
        MUSB_StringConcat(aLine, 80, " (designator min=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DESIGNATOR_MAX:
        MUSB_StringConcat(aLine, 80, " (designator max=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_INDEX:
        MUSB_StringConcat(aLine, 80, " (string index=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MIN:
        MUSB_StringConcat(aLine, 80, " (string min=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_STRING_MAX:
        MUSB_StringConcat(aLine, 80, " (string max=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    case MUSB_HID_REPORT_ITEMTAG_LOCAL_DELIMITER:
        MUSB_StringConcat(aLine, 80, " (local delimiter=");
        MUSB_Stringize(aNumber, 12, dwData, 16, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_StringConcat(aLine, 80, ")");
        break;
    }
    MUSB_PrintLine(aLine);
}
#else
#define MGC_HidPrintLocal(bTag, dwData)
#endif

static void MGC_HidFillItemFromAttr(MUSB_GhiItemInfo *pInfo,
                                    MGC_HidCurrentAttr *pAttr)
{
    pInfo->pParentItem = NULL; /* TODO */
    pInfo->wSize = (uint16_t)pAttr->dwReportSize;
    pInfo->bRepeatCount = (uint8_t)pAttr->dwReportCount;
    pInfo->Type = MUSB_GHI_ITEM_UNKNOWN; /* TODO */
    pInfo->bId = (uint8_t)pAttr->dwReportId;
}

#define MGC_HID_MAX_STACK	4

static void MGC_HidParseReportDesc(MGC_HidDevice *pHidDevice)
{
    MGC_HidCurrentAttr aAttr[MGC_HID_MAX_STACK];
    MGC_HidCurrentAttr *pAttr;
    MUSB_GhiItemInfo *pInfo;
    MUSB_GhiItemInfo *aInput;
    MUSB_GhiItemInfo *aOutput;
    MUSB_GhiItemInfo *aFeature;
    uint32_t dwData, dwSize;
    uint8_t bTemp, bType, bTag, bFilling, bStack, bAlloc;
    uint16_t wIndex, wInput, wOutput, wFeature;
    const uint8_t *pDesc;

    /* first pass, count items; then allocate and fill on 2nd pass */
    wInput = wOutput = wFeature = 0;
    aInput = aOutput = aFeature = NULL;
    for(bAlloc = bFilling = 0; bFilling < 2; bFilling++)
    {
        bStack = 0;
        wIndex = 0;
        pDesc = pHidDevice->pReportDescriptor;
        /* allocate */
        if(bFilling && !bAlloc)
        {
            bAlloc = TRUE;
            dwSize = sizeof(MUSB_GhiItemInfo) *
                     (pHidDevice->GhiDeviceInfo.wInputItemCount +
                      pHidDevice->GhiDeviceInfo.wOutputItemCount +
                      pHidDevice->GhiDeviceInfo.wFeatureItemCount);
            pInfo = (MUSB_GhiItemInfo *)MUSB_MemAlloc(dwSize);
            if(!pInfo)
            {
                MUSB_PrintLine("HID parser memory allocation failure (item info array)");
                return;
            }
            if(pHidDevice->wGhiOutputLength)
            {
                pHidDevice->pGhiOutputBuffer =
                    (uint8_t *)MUSB_MemAlloc(pHidDevice->wGhiOutputLength);
                if(!pHidDevice->pGhiOutputBuffer)
                {
                    MUSB_MemFree(pInfo);
                    MUSB_PrintLine("HID parser memory allocation failure (out buffer)");
                    return;
                }
            }
            if(pHidDevice->wGhiInputLength)
            {
                pHidDevice->pGhiInputBuffer =
                    (uint8_t *)MUSB_MemAlloc(pHidDevice->wGhiInputLength);
                if(!pHidDevice->pGhiInputBuffer)
                {
                    MUSB_MemFree(pHidDevice->pGhiOutputBuffer);
                    MUSB_MemFree(pInfo);
                    MUSB_PrintLine("HID parser memory allocation failure (in buffer)");
                    return;
                }
            }
            aInput = pInfo;
            aOutput = pInfo + pHidDevice->GhiDeviceInfo.wInputItemCount;
            aFeature = pInfo + pHidDevice->GhiDeviceInfo.wOutputItemCount;
            pHidDevice->GhiDeviceInfo.aInputItem = aInput;
            pHidDevice->GhiDeviceInfo.aOutputItem = aOutput;
            pHidDevice->GhiDeviceInfo.aFeatureItem = aFeature;
        }
        /* parse */
        while(wIndex < pHidDevice->wReportDescriptorLength)
        {
            pAttr = &(aAttr[bStack]);
            bTemp = (*pDesc >> MUSB_S_HID_REPORT_ITEMTAG_SIZE) & MUSB_M_HID_REPORT_ITEMTAG_SIZE;
            bType = (*pDesc >> MUSB_S_HID_REPORT_ITEMTAG_TYPE) & MUSB_M_HID_REPORT_ITEMTAG_TYPE;
            bTag = (*pDesc >> MUSB_S_HID_REPORT_ITEMTAG_TAG) & MUSB_M_HID_REPORT_ITEMTAG_TAG;
            switch(bTemp)
            {
            case MUSB_HID_REPORT_ITEMTAG_SIZE_0:
                dwData = 0xffffffff;
                wIndex++;
                pDesc++;
                break;
            case MUSB_HID_REPORT_ITEMTAG_SIZE_1:
                dwData = pDesc[1];
                pDesc += 2;
                wIndex += 2;
                break;
            case MUSB_HID_REPORT_ITEMTAG_SIZE_2:
                dwData = (pDesc[2] << 8) | pDesc[1];
                pDesc += 3;
                wIndex += 3;
                break;
            default:
                dwData = (pDesc[4] << 24) | (pDesc[3] << 16) | (pDesc[2] << 8) | pDesc[1];
                pDesc += 5;
                wIndex += 5;
            }

            switch(bType)
            {
            case MUSB_HID_REPORT_ITEMTAG_TYPE_MAIN:
                /* TODO: reset locals to defaults */
                switch(bTag)
                {
                case MUSB_HID_REPORT_ITEMTAG_MAIN_INPUT:
                    if(bFilling)
                    {
                        pInfo = &(aInput[wInput++]);
                        MGC_HidParseItemType(dwData, pInfo);
                        MGC_HidFillItemFromAttr(pInfo, pAttr);
                    }
                    else
                    {
                        pHidDevice->GhiDeviceInfo.wInputItemCount++;
                        pHidDevice->wGhiInputLength +=
                            (uint16_t)(pAttr->dwReportSize * pAttr->dwReportCount);
                    }
                    MGC_HidPrintItemType("Input", dwData);
                    break;
                case MUSB_HID_REPORT_ITEMTAG_MAIN_OUTPUT:
                    if(bFilling)
                    {
                        pInfo = &(aOutput[wOutput++]);
                        MGC_HidParseItemType(dwData, pInfo);
                        MGC_HidFillItemFromAttr(pInfo, pAttr);
                    }
                    else
                    {
                        pHidDevice->GhiDeviceInfo.wOutputItemCount++;
                        pHidDevice->wGhiOutputLength +=
                            (uint16_t)(pAttr->dwReportSize * pAttr->dwReportCount);
                    }
                    MGC_HidPrintItemType("Output", dwData);
                    break;
                case MUSB_HID_REPORT_ITEMTAG_MAIN_COLLECTION:
                    MGC_HidParseCollectionType(dwData);
                    break;
                case MUSB_HID_REPORT_ITEMTAG_MAIN_FEATURE:
                    if(bFilling)
                    {
                        pInfo = &(aFeature[wFeature++]);
                        MGC_HidParseItemType(dwData, pInfo);
                        MGC_HidFillItemFromAttr(pInfo, pAttr);
                    }
                    else
                    {
                        pHidDevice->GhiDeviceInfo.wFeatureItemCount++;
                        pHidDevice->wGhiOutputLength +=
                            (uint16_t)(pAttr->dwReportSize * pAttr->dwReportCount);
                    }
                    MGC_HidPrintItemType("Feature", dwData);
                    break;
                case MUSB_HID_REPORT_ITEMTAG_MAIN_END_COLLECTION:
                    MUSB_PrintLine("EndCollection");
                    break;
                }
                break;

            case MUSB_HID_REPORT_ITEMTAG_TYPE_GLOBAL:
                MGC_HidPrintGlobal(bTag, dwData);
                switch(bTag)
                {
                case MUSB_HID_REPORT_ITEMTAG_GLOBAL_PUSH:
                    if((bStack + 1) < MGC_HID_MAX_STACK)
                    {
                        bStack++;
                        MUSB_MemCopy(&(aAttr[bStack]), pAttr, sizeof(*pAttr));
                    }
                    else
                    {
                        MUSB_PrintLine("HID parser stack overflow");
                        /* TODO: cleanup and return */
                    }
                    break;
                case MUSB_HID_REPORT_ITEMTAG_GLOBAL_POP:
                    if(bStack)
                    {
                        pAttr = &(aAttr[--bStack]);
                    }
                    else
                    {
                        MUSB_PrintLine("HID parser stack underflow");
                        /* TODO: cleanup and return */
                    }
                    break;
                default:
                    MGC_HidUpdateGlobal(bTag, dwData, pAttr);
                }
                break;

            case MUSB_HID_REPORT_ITEMTAG_TYPE_LOCAL:
                MGC_HidPrintLocal(bTag, dwData);
                MGC_HidUpdateLocal(bTag, dwData, pAttr);
                break;
            }
        }
    }
}

static void MGC_HidSetInputHandler(void *pDeviceData,
                                   MUSB_pfGhiInputReport pfInputHandler)
{
    MGC_HidDevice *pHidDevice = (MGC_HidDevice *)pDeviceData;

    if(pHidDevice)
    {
        pHidDevice->pfInputHandler = pfInputHandler;
    }
}

static void MGC_HidSetOutputHandler(void *pDeviceData,
                                    MUSB_pfGhiOutputComplete pfOutputCompletionHandler)
{
    MGC_HidDevice *pHidDevice = (MGC_HidDevice *)pDeviceData;

    if(pHidDevice)
    {
        pHidDevice->pfOutputCompletionHandler = pfOutputCompletionHandler;
    }
}

static uint32_t MGC_HidGetInputCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *) pContext;
    if(pHidDevice && pHidDevice->pfInputHandler)
    {
        pHidDevice->pfInputHandler(pHidDevice->hGhiDevice,
                                   pHidDevice->ControlIrp.pInBuffer,
                                   pHidDevice->ControlIrp.dwActualInLength);
    }

    return TRUE;
}

static MUSB_GhiStatus MGC_HidGetInput(void *pDeviceData,
                                      uint8_t bReportId, uint8_t *pInputBuffer, uint16_t wLength)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    MGC_HidDevice *pHidDevice = (MGC_HidDevice *)pDeviceData;
    MUSB_GhiStatus result = MUSB_GHI_SUCCESS;

    pSetup          = &(pHidDevice->Setup);
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_GET_REPORT;
    pSetup->wValue        = (uint16_t)MUSB_SWAP16((MUSB_HID_INPUT_REPORT << 8) | bReportId);
    pSetup->wIndex        = (uint16_t)MUSB_SWAP16(pHidDevice->pIfaceDescriptor->bInterfaceNumber);
    pSetup->wLength       = (uint16_t)MUSB_SWAP16(wLength);

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = pInputBuffer;
    pControlIrp->dwInLength        = wLength;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidGetInputCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: SetOutput Request failed, dwStatus: ", dwStatus, 16, FALSE);
        result = MUSB_GHI_ERROR_TRANSFER;
    }
    return (result);
}

static uint32_t MGC_HidSetOutputCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *)pContext;
    if(pHidDevice && pHidDevice->pfOutputCompletionHandler)
    {
        pHidDevice->pfOutputCompletionHandler(pHidDevice->hGhiDevice,
                                              pHidDevice->ControlIrp.dwActualOutLength - 8);
    }

    return TRUE;
}

static MUSB_GhiStatus MGC_HidSetOutput(void *pDeviceData,
                                       uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    MGC_HidDevice *pHidDevice = (MGC_HidDevice *)pDeviceData;
    MUSB_GhiStatus result = MUSB_GHI_SUCCESS;

    pSetup          = (MUSB_DeviceRequest *)pHidDevice->pGhiOutputBuffer;
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_REPORT;
    pSetup->wValue        = (uint16_t)MUSB_SWAP16((MUSB_HID_OUTPUT_REPORT << 8) | bReportId);
    pSetup->wIndex        = (uint16_t)MUSB_SWAP16(pHidDevice->pIfaceDescriptor->bInterfaceNumber);
    pSetup->wLength       = (uint16_t)MUSB_SWAP16(wLength);
    MUSB_MemCopy(&(pHidDevice->pGhiOutputBuffer[8]), pOutput, wLength);

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest) + wLength;
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidSetOutputCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: SetOutput Request failed, dwStatus: ", dwStatus, 16, FALSE);
        result = MUSB_GHI_ERROR_TRANSFER;
    }
    return (result);
}

static uint32_t MGC_HidSetFeatureCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_HidDevice            *pHidDevice;

    pHidDevice = (MGC_HidDevice *)pContext;
    if(pHidDevice && pHidDevice->pfOutputCompletionHandler)
    {
        pHidDevice->pfOutputCompletionHandler(pHidDevice->hGhiDevice,
                                              pHidDevice->ControlIrp.dwActualOutLength - 8);
    }

    return TRUE;
}

static MUSB_GhiStatus MGC_HidSetFeature(void *pDeviceData,
                                        uint8_t bReportId, const uint8_t *pOutput, uint16_t wLength)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    MGC_HidDevice *pHidDevice = (MGC_HidDevice *)pDeviceData;
    MUSB_GhiStatus result = MUSB_GHI_SUCCESS;

    pSetup          = (MUSB_DeviceRequest *)pHidDevice->pGhiOutputBuffer;
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_FEATURE;
    pSetup->wValue        = (uint16_t)MUSB_SWAP16((MUSB_HID_FEATURE_REPORT << 8) | bReportId);
    pSetup->wIndex        = (uint16_t)MUSB_SWAP16(pHidDevice->pIfaceDescriptor->bInterfaceNumber);
    pSetup->wLength       = (uint16_t)MUSB_SWAP16(wLength);
    MUSB_MemCopy(&(pHidDevice->pGhiOutputBuffer[8]), pOutput, wLength);

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest) + wLength;
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidSetFeatureCallback;
    pControlIrp->pCompleteParam    = (void *)pHidDevice;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: SetFeature Request failed, dwStatus: ", dwStatus, 16, FALSE);
        result = MUSB_GHI_ERROR_TRANSFER;
    }
    return (result);
}
#endif // CFG_SUPPORT_HID
// eof
