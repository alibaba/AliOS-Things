#include "include.h"
#include "mem_pub.h"

#include "mu_arch.h"
#include "uvc_driver.h"
#include "mu_mem.h"
#include "mu_diag.h"
#include "usb_video.h"
#include "mu_descs.h"
#include "schedule_pub.h"
#include "usb_uvc.h"
#include "usb_pub.h"
#include "mu_pippr.h"

#if CFG_SUPPORT_UVC
static const uint8_t UVC_peripheralList1[] =
{
    MUSB_TARGET_VID, 0x8F, 0x05,       /* Alcor Micro, Corp*/
    MUSB_TARGET_CONFIG, 0x01,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

static const uint8_t UVC_peripheralList2[] =
{
    MUSB_TARGET_CLASS, MUSB_CLASS_MISC,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_VIDEO,
    MUSB_TARGET_SUBCLASS, 1,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

/** uvc class driver */
MUSB_DeviceDriver UVC_DeviceDriver =
{
    NULL,
    0,
    NULL,
    UVC_Connect,
    UVC_Disonnect,
    NULL,
    NULL,
};
MGC_UvcDevice UVC_Device = {0};

/*************************** FUNCTIONS ****************************/
static uint32_t UVC_CtrlSyncCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_UvcDevice *pUvcDevice;

    MUSB_PRT("UVC_CtrlSyncCallback\r\n");
    pUvcDevice = (MGC_UvcDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_RejectDevice(pUvcDevice->hBus, pUvcDevice->pUsbDevice);
        return 1;
    }

    pUvcDevice->ControlSyncLock = 0;

    return 0;
}

uint32_t UVC_CtrlSync(MGC_UvcDevice *pUvcDevice,
                      uint32_t requestType,
                      uint32_t request,
                      uint32_t value,
                      uint32_t index,
                      uint8_t *pBuffer,
                      uint32_t len,
                      uint32_t timeout)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    ASSERT(0 == pUvcDevice->ControlSyncLock);

    pSetup          = &(pUvcDevice->Setup);
    pControlIrp     = &(pUvcDevice->ControlIrp);

    pUvcDevice->ControlSyncLock = 1;
    MGC_UVC_PREPARE_SETUP_PACKET(pSetup,
                                 requestType,
                                 request,
                                 value,
                                 index,
                                 len);
    MGC_UVC_FILL_CONTROL_IRP( pUvcDevice,
                              pControlIrp,
                              (uint8_t *)pSetup,
                              sizeof(MUSB_DeviceRequest),
                              pBuffer,
                              len,
                              UVC_CtrlSyncCallback);

    pControlIrp->resv = 0xaa;
    dwStatus = MUSB_StartControlTransfer (pUvcDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        MUSB_PRT("UVC_CtrlSync_Failed\r\n");
    }

    while(pUvcDevice->ControlSyncLock)
    {
        MUSB_PRT(".");
        schedule();
    }

    MUSB_PRT(".\r\n");
    pControlIrp->resv = 0x00;

    return (dwStatus);
}

uint32_t UVC_CtrlGetCur(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetCur\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_CUR
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_SetInterface(MGC_UvcDevice *pUvcDevice, uint16_t InterfaseNumm, uint16_t AlterSetNum)
{
    uint32_t ret;

    ASSERT(pUvcDevice);

    MUSB_PRT("UVC_SetInterface\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_INTERFACE)
                       , MUSB_REQ_SET_INTERFACE
                       , AlterSetNum
                       , InterfaseNumm
                       , NULL
                       , 0, 0);

    return ret;
}

uint32_t UVC_CtrlGetMin(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetMin\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_MIN
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlGetMax(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetMax\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_MAX
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlGetDef(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetDef\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_DEF
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlGetRes(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetRes\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_RES
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlGetLen(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetLen\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_LEN
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlGetInfo(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlGetInfo\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_GET_INFO
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlSetCur(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlSetCur\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_OUT | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_SET_CUR
                       , 0x100
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

uint32_t UVC_CtrlCommitSetCur(MGC_UvcDevice *pUvcDevice, uint8_t *pBuffer, uint32_t len)
{
    uint32_t ret;

    ASSERT(pUvcDevice);
    ASSERT(pBuffer);
    MUSB_PRT("UVC_CtrlCommitSetCur\r\n");

    ret = UVC_CtrlSync(pUvcDevice, (MUSB_DIR_OUT | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
                       , UVC_SET_CUR
                       , 0x200
                       , 0x001
                       , pBuffer
                       , len, 0);

    return ret;
}

static uint32_t UVC_FpsToFrameInterval(uint32_t v)
{
    uint32_t ret = 0;

    switch(v)
    {
    case FPS_30:
        ret = (int)FPS30;
        break;

    case FPS_25:
        ret = (int)FPS25;
        break;

    case FPS_20:
        ret = (int)FPS20;
        break;

    case FPS_15:
        ret = (int)FPS15;
        break;

    case FPS_10:
        ret = (int)FPS10;
        break;

    case FPS_5:
        ret = (int)FPS05;
        break;

    default:
        ret = (uint32_t) - 1;
        break;
    }

    return ret;
}

static void UVC_SetFormatID(uint32_t id, uint8_t *pDesc)
{
    MUSB_PRT("UVC_SetFormatID = %d\r\n", id);
    pDesc[OFFSET_FORMAT] = id;
}

/*
 * Refer to USB_Video_Class_1.1.pdf 'Table 4-47 Video Probe and Commit
    Controls' */
static void UVC_SetResolution(uint32_t id, uint8_t *pDesc)
{
    MUSB_PRT("UVC_SetResolution = %d\r\n", id);
    pDesc[OFFSET_FRAME] = id;
}

/*
 * Refer to USB_Video_Class_1.1.pdf 'Table 4-47 Video Probe and Commit
Controls' */
static void UVC_SetFrameRate(uint32_t fps, uint8_t *pDesc)
{
    int frame_interval;

    MUSB_PRT("UVC_SetFrameRate = %d\r\n", fps);

    frame_interval = UVC_FpsToFrameInterval(fps);
    pDesc[OFFSET_FRAME_INTERVAL + 3] = (frame_interval & 0xff000000) >> 24;
    pDesc[OFFSET_FRAME_INTERVAL + 2] = (frame_interval & 0x00ff0000) >> 16;
    pDesc[OFFSET_FRAME_INTERVAL + 1] = (frame_interval & 0x0000ff00) >> 8;
    pDesc[OFFSET_FRAME_INTERVAL + 0] = (frame_interval & 0x000000ff);
}

/*
 * Refer to USB_Video_Class_1.1.pdf 'Table 4-47 Video Probe and Commit
Controls' */
static uint32_t UVC_GetMaxPayloadLength(uint8_t *pDesc)
{
    uint32_t len =
        pDesc[OFFSET_MAX_PAYLOAD] |
        pDesc[OFFSET_MAX_PAYLOAD + 1] << 8 |
        pDesc[OFFSET_MAX_PAYLOAD + 2] << 16 |
        pDesc[OFFSET_MAX_PAYLOAD + 3] << 24 ;     /*4 bytes length*/

    return len;
}

uint32_t UVC_StartStreaming(MGC_UvcDevice *pUvcDevice)
{
    uint32_t maxPayloadTransfer;
    uint8_t pDesc[LENGTH_COMMIT_CONTROL];

    if(NULL == pUvcDevice)
    {
        return FALSE;
    }

    MUSB_PRT("UVC_StartStreaming\r\n");
    UVC_CtrlGetCur(pUvcDevice, pDesc, LENGTH_COMMIT_CONTROL);

#ifdef UVC_DEMO_SUPPORT102
    if(pUvcDevice->enable_h264)
    {
        pUvcDevice->format_id = pUvcDevice->uvc_params.fmt_h264;
    }
    else
    {
        pUvcDevice->format_id = pUvcDevice->uvc_params.fmt_mjpeg;
    }

    UVC_SetFormatID(pUvcDevice->format_id, pDesc);
#endif
    UVC_SetResolution(pUvcDevice->resolution_id, pDesc);
    UVC_SetFrameRate(pUvcDevice->frame_per_second, pDesc);

    UVC_CtrlCommitSetCur(pUvcDevice, pDesc, LENGTH_COMMIT_CONTROL);

    UVC_CtrlGetCur(pUvcDevice, pDesc, LENGTH_COMMIT_CONTROL);
    maxPayloadTransfer = UVC_GetMaxPayloadLength(pDesc);
    MUSB_PRT("maxPayloadTransfer = %d\r\n", maxPayloadTransfer);
    UVC_SetInterface(pUvcDevice, 0x01, INTERFACE_ALTERSET);

    return TRUE;
}

uint32_t UVC_StopStreaming(MGC_UvcDevice *pUvcDevice)
{
    MUSB_Irp *vsIrp;

    if(NULL == pUvcDevice)
    {
        return FALSE;
    }

    vsIrp = &(pUvcDevice->videoStreamIrp);

    MUSB_CancelTransfer(vsIrp);

    return TRUE;
}

uint32_t UVC_ReceiveIoscVideoStream(MGC_UvcDevice *pUvcDevice,
                                    uint8_t *pBuffer,
                                    uint32_t len,
                                    MUSB_pfIsochIrpComplete pCallback,
                                    void *pCBParam)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)pUvcDevice->hInVideoStreamingPipe;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    uint16_t  wMaxPacketLength = bIsTx ? pEnd->wPacketSize : pEnd->wRxPacketSize;
    MUSB_IsochIrp   *pIsochIrp;
    uint32_t        wIndex;
    uint32_t        dwResidue;


    dwResidue = len % wMaxPacketLength;

    pIsochIrp = &(pUvcDevice->videoStreamIoscIrp);
    pIsochIrp->wFrameCount       = (uint16_t) (len / wMaxPacketLength);
    pIsochIrp->pBuffer           = pBuffer;

    for (wIndex = 0; wIndex < pIsochIrp->wFrameCount; wIndex++)
    {
        pIsochIrp->adwLength[wIndex]  = wMaxPacketLength;
        pIsochIrp->adwActualLength[wIndex] = 0;
        pIsochIrp->adwStatus[wIndex] = MUSB_STATUS_OK;
    }

    if (0 != dwResidue)
    {
        pIsochIrp->wFrameCount++;
        pIsochIrp->adwLength[wIndex]  = dwResidue;
        pIsochIrp->adwActualLength[wIndex] = 0;
        pIsochIrp->adwStatus[wIndex] = MUSB_STATUS_OK;
    }

    pIsochIrp->wCurrentFrame    = 0;
    pIsochIrp->wCallbackInterval = 0; /* Callback only when data transfer is completed */
    pIsochIrp->pfIrpComplete    = pCallback;
    pIsochIrp->pCompleteParam   = pCBParam;
    pIsochIrp->bIsrCallback		= FALSE;
    pIsochIrp->bAllowDma		= TRUE;

    return MUSB_ScheduleIsochTransfer(0, pIsochIrp);
}

uint32_t UVC_ReceiveVideoStream(MGC_UvcDevice *pUvcDevice,
                                uint8_t *pBuffer,
                                uint32_t len,
                                MUSB_pfIrpComplete pCallback,
                                void *pCBParam)
{
    MUSB_Irp *vsIrp;

    vsIrp = &(pUvcDevice->videoStreamIrp);
    vsIrp->pBuffer = pBuffer;
    vsIrp->dwLength = len;
    vsIrp->dwStatus = 0;
    vsIrp->dwActualLength = 0;
    vsIrp->pfIrpComplete = pCallback;
    vsIrp->pCompleteParam = pCBParam;
    vsIrp->bAllowShortTransfer = TRUE;
    vsIrp->bIsrCallback = FALSE;
    vsIrp->bAllowDma = TRUE;

    return MUSB_StartTransfer(vsIrp);
}

uint32_t UVC_UpdateVideoParamter(MGC_UvcDevice *pUvcDevice, uint16_t pid)
{
    uint32_t ret = FALSE;
#ifdef UVC_DEMO_SUPPORT102
    UVC_PARAMS *uvc_params_ptr;

    ASSERT(pUvcDevice);

    uvc_params_ptr = &pUvcDevice->uvc_params;
    MUSB_PRT("UVC_UpdateVideoParamter\r\n");
    switch(pid)
    {
    case PID_3894:
        ret = TRUE;

        MUSB_PRT("         PID_3894\r\n");
        uvc_params_ptr->pid = pid;
        uvc_params_ptr->fmt_h264 = AU3894_H264;
        uvc_params_ptr->fmt_mjpeg = AU3894_MJPEG;
        uvc_params_ptr->res_x_y[UVC_FRAME_352_288] = AU3894_U2_352_288;
        uvc_params_ptr->res_x_y[UVC_FRAME_320_240] = AU3894_U2_320_240;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_360] = AU3894_U2_640_360;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_480] = AU3894_U2_640_480;
        break;

    case PID_3822:
        ret = TRUE;
        MUSB_PRT("         PID_3822\r\n");

        uvc_params_ptr->pid = pid;
        uvc_params_ptr->fmt_h264 = NOUSUPPORT;
        uvc_params_ptr->fmt_mjpeg = AU3822_MJPEG;

#ifdef MUSB_FORCE_FULLSPEED
        uvc_params_ptr->res_x_y[UVC_FRAME_352_288] = AU3822_U1_160_120;
        uvc_params_ptr->res_x_y[UVC_FRAME_320_240] = NOUSUPPORT;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_360] = NOUSUPPORT;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_480] = AU3822_U1_640_480;
#else
        uvc_params_ptr->res_x_y[UVC_FRAME_352_288] = AU3822_U2_160_120;
        uvc_params_ptr->res_x_y[UVC_FRAME_320_240] = AU3822_U2_320_240;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_360] = AU3822_U2_640_360;
        uvc_params_ptr->res_x_y[UVC_FRAME_640_480] = AU3822_U2_640_480;
#endif

        break;

    default:
        uvc_params_ptr->pid = 0;
        break;
    }
#endif

    return ret;
}

uint32_t UVC_SetParam(MGC_UvcDevice *pUvcDevice,
                      uint32_t resolution_id,
                      uint32_t fps)
{
    ASSERT(pUvcDevice);

    pUvcDevice->resolution_id = resolution_id;
    pUvcDevice->frame_per_second = fps;

    return TRUE;
}

static MGC_UvcDevice *UVC_DeviceInit(MUSB_Device *pUsbDevice)
{
    MGC_UvcDevice *pUvcDevice;

    MUSB_PRT("UVC_DeviceInit\r\n");
    pUvcDevice = &UVC_Device;
    pUvcDevice->pDriver = &UVC_DeviceDriver;

    return pUvcDevice;
}

static uint32_t UVC_SetConfigCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    MGC_UvcDevice *pUvcDevice;

    MUSB_PRT("UVC_SetConfigCallback\r\n");

    pUvcDevice = (MGC_UvcDevice *) pContext;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_PRT("UVC_SetConfigCallback_ErrorStatus\r\n");

        MUSB_RejectDevice(pUvcDevice->hBus, pUvcDevice->pUsbDevice);
        return 1;
    }

    MGC_UvcSetConnect();

    /* Set the Current Configuration Descriptor to Default as Set Config is Success */
    pUvcDevice->pUsbDevice->pCurrentConfiguration = pUvcDevice->pUsbDevice->apConfigDescriptors[0];

    return 0;
}

static uint32_t UVC_ConfigureDevice(MGC_UvcDevice *pUvcDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pSetup          = &(pUvcDevice->Setup);
    pControlIrp     = &(pUvcDevice->ControlIrp);

    MUSB_PRT("UVC_ConfigureDevice\r\n");
    MGC_UVC_PREPARE_SETUP_PACKET(pSetup,
                                 (MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE),
                                 MUSB_REQ_SET_CONFIGURATION,
                                 pUvcDevice->pUsbDevice->apConfigDescriptors[0]->bConfigurationValue,
                                 0,
                                 0);
    /** Fill Control Irp */
    MGC_UVC_FILL_CONTROL_IRP( pUvcDevice,
                              pControlIrp,
                              (uint8_t *)pSetup,
                              sizeof(MUSB_DeviceRequest),
                              NULL,
                              0,
                              UVC_SetConfigCallback);

#ifdef ENABLE_UVC_SET_CONFIGURATION
    dwStatus = MUSB_StartControlTransfer (pUvcDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        MUSB_PRT("MSD Error: Set Configuration Request failed, dwStatus:0x%x\r\n", dwStatus);
    }
#else
    MGC_UvcSetConnect();
#endif
    return (dwStatus);
}/* End MGC_MsdConfigureDevice () */

uint32_t UVC_InitPipes(MGC_UvcDevice *pUvcDevice,
                       MUSB_BusHandle hBus,
                       const MUSB_EndpointDescriptor *pInVCEnd,
                       const MUSB_EndpointDescriptor *pInVSEnd)
{
    MUSB_DeviceEndpoint InVCEnd;
    MUSB_DeviceEndpoint InVSEnd;
    MUSB_PipePtr hInVCPipe;
    MUSB_PipePtr hInVSPipe;
    MUSB_Device *pDevice = pUvcDevice->pUsbDevice;

    InVCEnd.pDevice = pDevice;
    InVSEnd.pDevice = pDevice;

    InVCEnd.wNakLimit = 0;
    InVSEnd.wNakLimit = 0xffff;

    MUSB_MemCopy(&(InVCEnd.UsbDescriptor), pInVCEnd, sizeof(MUSB_EndpointDescriptor));
    MUSB_MemCopy(&(InVSEnd.UsbDescriptor), pInVSEnd, sizeof(MUSB_EndpointDescriptor));

    hInVSPipe = MUSB_OpenPipe(hBus, &InVSEnd, NULL);
    if(!hInVSPipe)
    {
        MUSB_PRT("[MGC] OpenPipe VS fail\r\n");
        MUSB_ClosePipe(hInVSPipe);
        return FALSE;
    }

    pUvcDevice->hInVideoControlPipe = hInVCPipe;
    pUvcDevice->hInVideoStreamingPipe = hInVSPipe;
    pUvcDevice->videoStreamIrp.hPipe = hInVSPipe;
    pUvcDevice->videoStreamIoscIrp.hPipe = hInVSPipe;

    MUSB_PRT("UVC_InitVCPipe:0x%x\r\n", hInVCPipe);
    MUSB_PRT("UVC_InitVSPipe:0x%x\r\n", hInVSPipe);

    return TRUE;
}

uint8_t UVC_Connect(void     *pPrivateData,
                    MUSB_BusHandle     hBus,
                    MUSB_Device       *pUsbDevice,
                    const uint8_t     *pPeripheralList)
{
    uint8_t bIndex, bEnd, bAlternateSet;
    uint8_t bSubclass;
    uint8_t bEndType;
    const MUSB_InterfaceDescriptor *pIface;
    const MUSB_EndpointDescriptor *pEnd;
    const MUSB_EndpointDescriptor *pInVideoControlEp;
    const MUSB_EndpointDescriptor *pInVideoStreamingEp;
    MGC_UvcDevice *pUvcDevice;
    const MUSB_ConfigurationDescriptor *pConfig = pUsbDevice->apConfigDescriptors[0];
    uint8_t bStatus = FALSE;

#ifdef UVC_DEMO_SUPPORT102
    MUSB_DeviceDescriptor *pDevDesc;
#endif

    /* Device is connected */
    MUSB_PRT("UVC_Connect\r\n");

    /* find first interface with supported subclass/protocol combination */
    pInVideoControlEp = NULL;
    pInVideoStreamingEp = NULL;
    for(bIndex = 0; bIndex < pConfig->bNumInterfaces; bIndex++)
    {

        /* assume no alternates */
        for(bAlternateSet = 0; bAlternateSet <= INTERFACE_ALTERSET; bAlternateSet++ )
        {
            pIface = MUSB_FindInterfaceDescriptor(pConfig, bIndex, bAlternateSet);
            if(pIface && (MUSB_CLASS_VIDEO == pIface->bInterfaceClass))
            {

                bSubclass = pIface->bInterfaceSubClass;

                for(bEnd = 0; bEnd < pIface->bNumEndpoints; bEnd++)
                {

                    pEnd = MUSB_FindEndpointDescriptor(pConfig, pIface, bEnd);

                    if(pEnd)
                    {
                        bEndType = pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK;

                        if(
                            (UVC_SC_VIDEOCONTROL == bSubclass)
                            &&
                            (MUSB_ENDPOINT_XFER_INT == bEndType)
                        )
                        {
                            pInVideoControlEp = pEnd;
                            break;
                        }

                        if(
                            UVC_SC_VIDEOSTREAMING == bSubclass
                            &&
                            (
                                (MUSB_ENDPOINT_XFER_BULK == bEndType)
                                ||
                                (MUSB_ENDPOINT_XFER_ISOC == bEndType)
                            )
                        )
                        {

                            pInVideoStreamingEp = pEnd;
                            break;
                        }

                    }

                }

            }
        }

    }

    MUSB_PRT("pInVideoControlEp[%x],pInVideoStreamingEp[%x]\r\n", pInVideoControlEp, pInVideoStreamingEp);
    if(pInVideoControlEp && pInVideoStreamingEp)
    {
#ifdef UVC_DEMO_SUPPORT102
        pDevDesc = &pUsbDevice->DeviceDescriptor;
        MGC_UvcUpdateVideoParamter(pDevDesc->idProduct);
#endif

        pUvcDevice = UVC_DeviceInit(pUsbDevice);
        if(pUvcDevice)
        {
            pUsbDevice->pDriverPrivateData = pUvcDevice;

            /* Initialize */
            pUvcDevice->pUsbDevice     = pUsbDevice;
            pUvcDevice->hBus           = hBus;

            bStatus = UVC_InitPipes(pUvcDevice,
                                    hBus,
                                    pInVideoControlEp,
                                    pInVideoStreamingEp);

            /* Configure the device */
            bStatus = (uint8_t) UVC_ConfigureDevice(pUvcDevice);
            if (MUSB_STATUS_OK == bStatus)
            {
                bStatus = TRUE;
            }
        }
    }

    return (bStatus);
}

void UVC_Disonnect (void *pPrivateData,
                    MUSB_BusHandle hBus,
                    MUSB_Device *pUsbDevice)
{
    MGC_UvcDevice *pUvcDevice;

    pUvcDevice = (MGC_UvcDevice *)pUsbDevice->pDriverPrivateData;

    MUSB_ClosePipe(pUvcDevice->hInVideoControlPipe);
    pUvcDevice->hInVideoControlPipe = NULL;
    MUSB_ClosePipe(pUvcDevice->hInVideoStreamingPipe);
    pUvcDevice->hInVideoStreamingPipe = NULL;

    pUsbDevice->pDriverPrivateData = NULL;

    MUSB_PRT("UVC_Disonnect\r\n");
}

uint16_t UVC_FillPeripheralList(uint8_t bDriverIndex,
                                uint8_t *pList,
                                uint16_t wListRemain)
{
    uint16_t wCount;
    uint16_t wResult = 0;

    wCount = MUSB_MIN((uint16_t)sizeof(UVC_peripheralList1),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), UVC_peripheralList1, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    wCount = MUSB_MIN((uint16_t)sizeof(UVC_peripheralList2),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), UVC_peripheralList2, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    return wResult;
}

MUSB_DeviceDriver *UVC_GetClassDriver(void)
{
    return &UVC_DeviceDriver;
}
#endif // CFG_SUPPORT_UVC
// eof

