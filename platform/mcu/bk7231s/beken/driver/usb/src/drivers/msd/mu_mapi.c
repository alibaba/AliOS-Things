/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2005              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/
#include "include.h"

#include "mu_arch.h"
#include "mu_impl.h"
#include "mu_cdi.h"
#include "mu_descs.h"
#include "mu_diag.h"
#include "mu_hfi.h"
#include "mu_mem.h"
#include "mu_stdio.h"
#include "mu_mpcsi.h"
#include "mu_msd.h"
#include "mu_mapi.h"

#if CFG_SUPPORT_MSD
extern MGC_MsdCmdSet *MGC_GetScsiCmdSet(void);
extern MGC_MsdProtocol *MGC_CreateBotInstance(MUSB_Device *pDevice,
        MUSB_BusHandle hBus,
        const MUSB_InterfaceDescriptor *pIfaceDesc,
        const MUSB_EndpointDescriptor *pInEnd,
        const MUSB_EndpointDescriptor *pOutEnd,
        MGC_MsdCmdSet *pCmdSet);
extern void MGC_DestroyBotInstance(MGC_MsdProtocol *pProtocol);

static uint32_t MGC_MsdConfigureDevice(MGC_MsdDevice *pMsdDevice);
static uint32_t MGC_MsdSetConfigCallback(void *pContext, MUSB_ControlIrp *pControlIrp);

/**************************** GLOBALS *****************************/

static uint32_t MGC_aMsdTimerResolution[1] = { 10 };

static const uint8_t MGC_Msd_PeripheralList[] =
{
    /* BOT/SCSI */
    MUSB_TARGET_CLASS, MUSB_CLASS_PER_INTERFACE,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_MASS_STORAGE,
    MUSB_TARGET_SUBCLASS, MGC_MSD_SCSI_SUBCLASS,
    MUSB_TARGET_PROTOCOL, MGC_MSD_BOT_PROTOCOL,
    MUSB_TARGET_ACCEPT
};

static const uint8_t MGC_aMsdPeripheralList1[] =
{
    /* BOT/SCSI */
    MUSB_TARGET_CLASS, MUSB_CLASS_PER_INTERFACE,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_MASS_STORAGE,
    MUSB_TARGET_SUBCLASS, MGC_MSD_SCSI_SUBCLASS,
    MUSB_TARGET_PROTOCOL, MGC_MSD_BOT_PROTOCOL,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

static const uint8_t MGC_aMsdPeripheralList2[] =
{
    /* BOT/SFF-8070i */
    MUSB_TARGET_CLASS, MUSB_CLASS_PER_INTERFACE,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_MASS_STORAGE,
    MUSB_TARGET_SUBCLASS, MGC_MSD_SFF_8070i_SUBCLASS,
    MUSB_TARGET_PROTOCOL, MGC_MSD_BOT_PROTOCOL,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

/** Mass Storage device driver */
MUSB_DeviceDriver MGC_MsdDeviceDriver =
{
    NULL,
    sizeof(MGC_aMsdTimerResolution) / sizeof(uint32_t),
    MGC_aMsdTimerResolution,
    MUSB_MsdConnect,
    MUSB_MsdDisconnect,
    NULL,
    NULL,
};

/*************************** FUNCTIONS ****************************/

uint16_t MGC_FillStorageClassPeripheralList(uint8_t *pList, uint16_t wListLength)
{
    uint16_t wResult = MUSB_MIN((uint16_t)sizeof(MGC_Msd_PeripheralList), wListLength);

    MUSB_MemCopy(pList, MGC_Msd_PeripheralList, wResult);
    return wResult;
}

uint16_t MUSB_FillMsdPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
                                    uint16_t wListRemain)
{
    uint16_t wCount;
    uint16_t wResult = 0;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aMsdPeripheralList1),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aMsdPeripheralList1, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aMsdPeripheralList2),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aMsdPeripheralList2, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    return wResult;
}

MUSB_DeviceDriver *MUSB_GetStorageClassDriver(void)
{
    return &MGC_MsdDeviceDriver;
}

/**
 * Allocate per-device data
 */
static MGC_MsdDevice *MGC_MsdDeviceInit(MUSB_Device *pUsbDevice)
{
    MGC_MsdDevice            *pMsdDevice;

    MUSB_PRT("[MGC] MsdDeviceInit is called\r\n");
    pMsdDevice = (MGC_MsdDevice *)MUSB_MemAlloc (sizeof(MGC_MsdDevice) );
    if (NULL == pMsdDevice)
    {
        MUSB_DIAG_STRING(MUSB_MSD_DIAG_ERROR, "MSD Error: Insufficient memory");
        return (NULL);
    }
    MUSB_MemSet((void *)pMsdDevice, 0, sizeof (MGC_MsdDevice));

    pMsdDevice->pDriver                 = &MGC_MsdDeviceDriver;

    MUSB_DIAG_STRING(MUSB_MSD_DIAG_SUCCESS, "MSD: Initialization Completed");

    return pMsdDevice;
}

/** This function is called when Mass Storage device is connected.*/
uint8_t
MUSB_MsdConnect(  void              *pPrivateData,
                  MUSB_BusHandle     hBus,
                  MUSB_Device       *pUsbDevice,
                  const uint8_t     *pPeripheralList)
{
    uint8_t bIndex, bEnd;
    uint8_t bSubclass, bProtocol;
    const MUSB_InterfaceDescriptor *pIface;
    const MUSB_EndpointDescriptor *pEnd;
    const MUSB_EndpointDescriptor *pInEnd;
    const MUSB_EndpointDescriptor *pOutEnd;
    MGC_MsdDevice *pMsdDevice;
    MGC_MsdProtocol *pProtocol = NULL;
    MGC_MsdCmdSet *pCmdSet = NULL;

    const MUSB_ConfigurationDescriptor *pConfig = pUsbDevice->apConfigDescriptors[0];
    uint8_t bStatus = FALSE;

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_MSD_DIAG_SUCCESS, "Mass Storage Device Connected");

    /* find first interface with supported subclass/protocol combination */
    for(bIndex = 0; bIndex < pConfig->bNumInterfaces; bIndex++)
    {
        /* assume no alternates */
        pIface = MUSB_FindInterfaceDescriptor(pConfig, bIndex, 0);
        if(pIface && (MUSB_CLASS_MASS_STORAGE == pIface->bInterfaceClass) &&
                (pIface->bNumEndpoints >= 2))
        {
            bSubclass = pIface->bInterfaceSubClass;
            bProtocol = pIface->bInterfaceProtocol;
            pInEnd = pOutEnd = NULL;
            if(MGC_MSD_BOT_PROTOCOL == bProtocol)
            {
                /* BOT: find bulk-in & bulk-out ends */
                for(bEnd = 0; bEnd < pIface->bNumEndpoints; bEnd++)
                {
                    pEnd = MUSB_FindEndpointDescriptor(pConfig, pIface, bEnd);
                    if(pEnd &&
                            (MUSB_ENDPOINT_XFER_BULK == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)))
                    {
                        if(MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress)
                        {
                            pInEnd = pEnd;
                        }
                        else
                        {
                            pOutEnd = pEnd;
                        }
                    }
                }
                /* continue if not found */
                if(!pInEnd || !pOutEnd)
                {
                    MUSB_DIAG1(1, "MSD: insufficient endpoints for BOT in interface ",
                               bIndex, 10, 0);
                    continue;
                }
                /* determine command-set or continue on unsupported */
                switch(bSubclass)
                {
                case MGC_MSD_SCSI_SUBCLASS:
                    pCmdSet = MGC_GetScsiCmdSet();
                    break;

                default:
                    continue;
                }
                pProtocol = MGC_CreateBotInstance(pUsbDevice, hBus, pIface,
                                                  pInEnd, pOutEnd, pCmdSet);
            }
        }
    }

    if(pProtocol)
    {
        pMsdDevice = MGC_MsdDeviceInit(pUsbDevice);
        if(pMsdDevice)
        {
            pMsdDevice->pProtocol = pProtocol;
            pUsbDevice->pDriverPrivateData = pMsdDevice;

            /* Initialize */
            pMsdDevice->pUsbDevice     = pUsbDevice;
            pMsdDevice->hBus           = hBus;

            /* Configure the device */
            bStatus = (uint8_t) MGC_MsdConfigureDevice(pMsdDevice);
            if (MUSB_STATUS_OK == bStatus)
            {
                bStatus = TRUE;
            }
        }
    }
    else
    {
        MUSB_DIAG_STRING(MUSB_MSD_DIAG_ERROR,
                         "MSD Error: No interface has supported subclass/protocol");
    }

    return (bStatus);
}/* End MUSB_MsdConnect() */

/** Disconnect Handler for Mass Storage Device Driver */
void MUSB_MsdDisconnect (void           *pPrivateData,
                         MUSB_BusHandle  hBus,
                         MUSB_Device    *pUsbDevice)
{
    MGC_MsdDevice            *pMsdDevice;


    pMsdDevice = (MGC_MsdDevice *)pUsbDevice->pDriverPrivateData;


    /* Check against the USB device and bus handle */
    if( (hBus != pMsdDevice->hBus) || (pUsbDevice != pMsdDevice->pUsbDevice) )
    {
        MUSB_DIAG_STRING(MUSB_MSD_DIAG_ERROR, "MSD Error: MSD Device Disconnect Callback");
        MUSB_DIAG_STRING(MUSB_MSD_DIAG_ERROR, "MSD Error: Wrong 'Usb Device' and 'Bus' handles ");
        return;
    }

    /* Release the resources held */
    MGC_DestroyBotInstance(pMsdDevice->pProtocol);

    MUSB_MemFree(pMsdDevice);
    pUsbDevice->pDriverPrivateData = NULL;
    MUSB_DIAG_STRING(MUSB_MSD_DIAG_SUCCESS, "Mass Storage Device Disconnected Successfully");
    return;
}/* End MUSB_MsdDisconnect () */

/** After getting connect callback,  device driver calls this function to configure device */
static uint32_t MGC_MsdConfigureDevice(MGC_MsdDevice *pMsdDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pSetup          = &(pMsdDevice->Setup);
    pControlIrp     = &(pMsdDevice->ControlIrp);
    /*此处为设置setup configuration包*/

    /** Prepare the Setup Packet for sending Set Config Request */
    MGC_MSD_PREPARE_SETUP_PACKET(pSetup,
                                 (MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE),
                                 MUSB_REQ_SET_CONFIGURATION,
                                 pMsdDevice->pUsbDevice->apConfigDescriptors[0]->bConfigurationValue,
                                 0,
                                 0);
    /** Fill Control Irp */
    MGC_MSD_FILL_CONTROL_IRP( pMsdDevice,
                              pControlIrp,
                              (uint8_t *)pSetup,
                              sizeof(MUSB_DeviceRequest),
                              NULL,
                              0,
                              MGC_MsdSetConfigCallback);

    dwStatus = MUSB_StartControlTransfer (pMsdDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_MSD_DIAG_ERROR,
                   "MSD Error: Set Configuration Request failed, dwStatus: ", dwStatus, 16, FALSE);
    }
    return (dwStatus);
}/* End MGC_MsdConfigureDevice () */

/** Callback function when device acknowledges set config reqeust. */
static uint32_t MGC_MsdSetConfigCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_MsdDevice            *pMsdDevice;

    MUSB_PRT("[MGC] MsSetConfigCallback fun is called\r\n");

    pMsdDevice = (MGC_MsdDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_MSD_DIAG_ERROR,
                   "MSD Error: Set Config Callback Status", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pMsdDevice->hBus, pMsdDevice->pUsbDevice);
        return 1;
    }
    /* Set the Current Configuration Descriptor to Default as Set Config is Success */
    pMsdDevice->pUsbDevice->pCurrentConfiguration = pMsdDevice->pUsbDevice->apConfigDescriptors[0];

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_MSD_DIAG_SUCCESS, "Mass Storage Device Configured Successfully");

    //就是函数 MGC_BotProtocolStartDevice() 启动Get_Max_Lun
    if(!pMsdDevice->pProtocol->pfStartDevice(
                pMsdDevice->pProtocol->pProtocolData, pMsdDevice->pUsbDevice))
    {
        MUSB_DIAG_STRING(MUSB_MSD_DIAG_ERROR, "MSD Error: Failed to start device");
        return 1;
    }
    return 0;
}
#endif // CFG_SUPPORT_MSD
// eof
