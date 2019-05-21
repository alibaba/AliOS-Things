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
#include "include.h"

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_impl.h"
#include "mu_stdio.h"
#include "mu_strng.h"
#include "mu_hfi.h"
#include "uvc_driver.h"
#include "mu_none.h"
#include "usb_pub.h"
#include "schedule_pub.h"

#if CFG_SUPPORT_UVC
static void MGC_UvcNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State);
static void MGC_UvcOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus);
extern int app_lwip_udp_send_packet (UINT8 *data, UINT32 len);
extern int app_lwip_tcp_send_packet (UINT8 *data, UINT32 len);

/**************************** GLOBALS *****************************/
static MUSB_Port *MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aUvcPeripheralList[256];
static MUSB_DeviceDriver MGC_aDeviceDriver[2];

static MUSB_HostClient MGC_UvcHostClient =
{
    MGC_aUvcPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_UvcOtgClient =
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_UvcNewOtgState,
    MGC_UvcOtgError
};

static MUSB_SandList MGC_SendType;
static MUSB_LinkedList MGC_TestNciPortList;
static FUNCPTR uvc_configured_cb = NULL;
static FUNCPTR uvc_vstream_rxed_cb = NULL;
static uint8_t *vstream_buf = NULL;
static uint32_t vstream_buf_len = 0;

/*************************** FUNCTIONS ****************************/
void MGC_UvcRegisterRxVstreamBufPtr(uint8_t *ptr)
{
    vstream_buf = ptr;
}

void MGC_UvcRegisterRxVstreamBufLen(uint32_t len)
{
    vstream_buf_len = len;
}

void MGC_UvcRegisterConfiguredCallback(FUNCPTR cb)
{
    uvc_configured_cb = cb;
}

void MGC_UvcRegisterVSRxedCallback(FUNCPTR cb)
{
    uvc_vstream_rxed_cb = cb;
}

void MGC_UvcRegisterLink(uint8_t UDPFlag)
{
    if( 1 == UDPFlag )
    {
        MGC_SendType.Func = app_lwip_udp_send_packet;
        MGC_SendType.MaxLen = 1024;
    }
    else
    {
        MGC_SendType.Func = app_lwip_tcp_send_packet;
        MGC_SendType.MaxLen = 1460;
    }
}

void MGC_UvcSetConnect(void)
{
    UVC_Device.connectStatus = 1;

    if(uvc_configured_cb)
    {
        (*uvc_configured_cb)();
    }
}

void MGC_UvcSetDisconnect(void)
{
    UVC_Device.connectStatus = 0;
}

uint32_t MGC_UvcGetConnectStatus(void)
{
    return UVC_Device.connectStatus;
}

uint32_t MGC_UvcStartStream(void)
{
    uint32_t ret;

    ret = UVC_StartStreaming(&UVC_Device);

    (*uvc_vstream_rxed_cb)();

    return ret;
}

uint32_t MGC_UvcStopStream(void)
{
    uint32_t ret;

    ret =  UVC_StopStreaming(&UVC_Device);
    uvc_vstream_rxed_cb = 0;

    return ret;
}

uint32_t MGC_UvcUpdateVideoParamter(uint16_t pid)
{
    return UVC_UpdateVideoParamter(&UVC_Device, pid);
}

#ifdef UVC_DEMO_SUPPORT102
uint32_t MGC_UvcEnableMjpeg(void)
{
    UVC_Device.enable_h264 = 0;

    return TRUE;
}

uint32_t MGC_UvcEnableH264(void)
{
    if(NOUSUPPORT == UVC_Device.uvc_params.fmt_h264)
    {
        MUSB_PRT("non-support h264, and switch to mjpeg mode automatically\r\n");
        UVC_Device.enable_h264 = 0;
        MGC_UvcEnableMjpeg();
    }
    else
    {
        UVC_Device.enable_h264 = 1;
    }

    return TRUE;
}
#else
uint32_t MGC_UvcEnableMjpeg(void)
{
    MUSB_PRT("macro exceptional\r\n");
    return TRUE;
}

uint32_t MGC_UvcEnableH264(void)
{
    MUSB_PRT("macro exceptional\r\n");
    return TRUE;
}
#endif

void MGC_UvcSetParameter(uint32_t resolution_id,
                         uint32_t fps)
{
#ifdef UVC_DEMO_SUPPORT100
    UVC_SetParam(&UVC_Device, resolution_id, fps);

#elif defined(UVC_DEMO_SUPPORT102)
    ASSERT(UVC_Device.connectStatus);
    ASSERT(UVC_Device.uvc_params.pid);
    ASSERT(resolution_id < UVC_FRAME_COUNT);

    UVC_SetParam(&UVC_Device, UVC_Device.uvc_params.res_x_y[resolution_id], fps);
#else
#error "can you check the uvc demo version?"
#endif
}

uint32_t MGC_UvcRxedVStreamCallback(void *pCompleteParam, MUSB_Irp *pIrp)
{
    uint32_t ret = 0;
#if CFG_USE_LWIP_NETSTACK

    if(pIrp->dwActualLength)
    {
        int32_t len = pIrp->dwActualLength;
        uint32_t total = 0;
        uint8_t *buf_ptr;
        uint32_t count;

#ifdef UVC_DEMO_SUPPORT100
        uint32_t i;
        UINT32 pad_count = 0;

        i = len - 1;
        while(1)
        {
            if(0xd9 == vstream_buf[i])
            {
                ASSERT(0xff == vstream_buf[i - 1]);
                break;
            }
            else
            {
                pad_count ++;
            }

            i --;

            ASSERT(pad_count < 32);
        }

        len -= pad_count;

        vstream_buf[len ++] = 0x00;
        vstream_buf[len ++] = ((pIrp->dwActualLength - 12 - pad_count) >> 0) & 0xff;
        vstream_buf[len ++] = ((pIrp->dwActualLength - 12 - pad_count) >> 8) & 0xff;
        vstream_buf[len ++] = ((pIrp->dwActualLength - 12 - pad_count) >> 16) & 0xff;
        vstream_buf[len ++] = ((pIrp->dwActualLength - 12 - pad_count) >> 24) & 0xff;

        if(len < 12)
        {
            goto rxedCBExit;
        }
        else
        {
            len = len - 12;
        }

        while(len > 0)
        {
            count = ( (len < MGC_SendType.MaxLen) ? len : MGC_SendType.MaxLen );
            buf_ptr = &vstream_buf[total + 12];
            ret = MGC_SendType.Func(buf_ptr, count);
            if(0 == ret)
            {
                schedule();
            }

            total += ret;
            len -= ret;
        }
#elif defined(UVC_DEMO_SUPPORT102)
        if(len < 12)
        {
            goto rxedCBExit;
        }
        else
        {
            len = len - 12;
        }

        while(len > 0)
        {
            count = ( (len < MGC_SendType.MaxLen) ? len : MGC_SendType.MaxLen );
            buf_ptr = &vstream_buf[total + 12];
            ret = MGC_SendType.Func(buf_ptr, count);

            if(0 == ret)
            {
                schedule();
            }
            else
            {
                total += ret;
                len -= ret;
            }
        }
#endif
    }
#endif

rxedCBExit:
    (*uvc_vstream_rxed_cb)();

    return ret;
}
#include "gpio_pub.h"//Suny
uint32_t MGC_UvcRxedIsocVStreamCallback(void *pCompleteParam, struct _MUSB_IsochIrp *pIrp)
{
    uint32_t ret = 0;

#if CFG_USE_LWIP_NETSTACK

    uint32_t times = pIrp->wFrameCount;
    int32_t len = 0;
    int32_t index = 0;
    uint32_t total = 0;
    uint8_t *buf_ptr;
    uint32_t count;

    for(index = 0; index < times; index++)
    {
        len += pIrp->adwActualLength[index];
    }

    if( 0 != len )
    {
#ifdef UVC_DEMO_SUPPORT100
        uint32_t i;
        UINT32 pad_count = 0;
        uint32_t dwActualLength = len;

        i = len - 1;
        while(1)
        {
            if(0xd9 == vstream_buf[i])
            {
                ASSERT(0xff == vstream_buf[i - 1]);
                break;
            }
            else
            {
                pad_count ++;
            }

            i --;

            ASSERT(pad_count < 32);
        }

        len -= pad_count;

        vstream_buf[len ++] = 0x00;
        vstream_buf[len ++] = ((dwActualLength - 12 - pad_count) >> 0) & 0xff;
        vstream_buf[len ++] = ((dwActualLength - 12 - pad_count) >> 8) & 0xff;
        vstream_buf[len ++] = ((dwActualLength - 12 - pad_count) >> 16) & 0xff;
        vstream_buf[len ++] = ((dwActualLength - 12 - pad_count) >> 24) & 0xff;

        if(len < 12)
        {
            goto rxedCBExit;
        }
        else
        {
            len = len - 12;
        }

        while(len > 0)
        {
            count = ( (len < MGC_SendType.MaxLen) ? len : MGC_SendType.MaxLen );
            buf_ptr = &vstream_buf[total + 12];

            ret = MGC_SendType.Func(buf_ptr, count);
            if(0 == ret)
            {
                schedule();
            }

            total += ret;
            len -= ret;
        }
#elif defined(UVC_DEMO_SUPPORT102)
        if(len < 12)
        {
            goto rxedCBExit;
        }
        else
        {
            len = len - 12;
        }

        while(len > 0)
        {
            count = ( (len < MGC_SendType.MaxLen) ? len : MGC_SendType.MaxLen );
            buf_ptr = &vstream_buf[total + 12];
            ret = MGC_SendType.Func(buf_ptr, count);

            if(0 == ret)
            {
                schedule();
            }
            else
            {
                total += ret;
                len -= ret;
            }
        }
#endif
    }
#endif

rxedCBExit:
    (*uvc_vstream_rxed_cb)();
    return ret;
}

uint32_t MGC_UvcRxVStream(uint8_t *buf, uint32_t len)
{
    uint32_t    dwStatus;
    MGC_Pipe *pPipe = (MGC_Pipe *)UVC_Device.hInVideoStreamingPipe;
    uint8_t bTrafficType = pPipe->bTrafficType;

    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_ISOC:
        dwStatus = UVC_ReceiveIoscVideoStream(&UVC_Device,
                                              buf,
                                              len,
                                              MGC_UvcRxedIsocVStreamCallback,
                                              0);
        break;
    case MUSB_ENDPOINT_XFER_INT:
    case MUSB_ENDPOINT_XFER_BULK:
        dwStatus = UVC_ReceiveVideoStream(&UVC_Device,
                                          buf,
                                          len,
                                          MGC_UvcRxedVStreamCallback,
                                          0);
        break;
    default:
        break;
    }

    return dwStatus;
}

uint32_t MGC_UvcReceiveVideoStream(void)
{
    if((NULL == vstream_buf)
            || (0 == vstream_buf_len))
    {
        return (uint32_t) - 1;
    }

    return MGC_UvcRxVStream(vstream_buf, vstream_buf_len);
}

static void MGC_UvcNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State)
{
    char aAnswer[4];

    switch(State)
    {
    case MUSB_AB_IDLE:
        MUSB_PrintLine("S - Start Session");
        MUSB_GetLine(aAnswer, 4);
        if(('s' == aAnswer[0]) || ('S' == aAnswer[0]))
        {
            MUSB_RequestBus(MGC_hCdiBus);
        }
        break;

    case MUSB_A_SUSPEND:
        MUSB_PrintLine("R - Resume bus");
        MUSB_GetLine(aAnswer, 4);
        if(('r' == aAnswer[0]) || ('R' == aAnswer[0]))
        {
            MUSB_ResumeBus(MGC_hCdiBus);
        }
        break;

    default:
        break;
    }
}

void MUSB_HfiDeviceRemoved(void)
{
}
static void MGC_UvcOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus)
{
    switch(dwStatus)
    {
    case MUSB_STATUS_UNSUPPORTED_DEVICE:
        MUSB_PRT("Device not supported\r\n");
        break;

    case MUSB_STATUS_UNSUPPORTED_HUB:
        MUSB_PRT("Hubs not supported\r\n");
        break;

    case MUSB_STATUS_OTG_VBUS_INVALID:
        MUSB_PRT("Vbus error\r\n");
        break;

    case MUSB_STATUS_OTG_NO_RESPONSE:
        MUSB_PRT("Device not responding\r\n");
        break;

    case MUSB_STATUS_OTG_SRP_FAILED:
        MUSB_PRT("Device not responding (SRP failed)\r\n");
        break;

    default:
        break;
    }
}

int usb_sw_init(void)
{
    uint8_t *pList;
    uint8_t bDriver = 0;
    uint16_t wCount = 0;
    uint16_t wSize = 0;
    uint16_t wRemain;
    MUSB_DeviceDriver *pDriver;

    wRemain = (uint16_t)sizeof(MGC_aUvcPeripheralList);
    pList = MGC_aUvcPeripheralList;

    wSize = UVC_FillPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = UVC_GetClassDriver();
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_UvcHostClient.aDeviceDriverList[bDriver]),
                         pDriver,
                         sizeof(MUSB_DeviceDriver));

            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;

            bDriver++;
        }
    }

    MGC_UvcHostClient.wPeripheralListLength = wCount;
    MGC_UvcHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        MUSB_PRT("[MGC] InitSystem failed\r\n");
        return -1;
    }

    MUSB_PRT("usb_sw_init\r\n");

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);

    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort,
                                         0,
                                         &MGC_UvcHostClient,
                                         &MGC_UvcOtgClient);

    MUSB_NoneRunBackground();

    return 0;
}

int usb_sw_uninit(void)
{
    return 0;
}
#endif // CFG_SUPPORT_UVC

// eof

