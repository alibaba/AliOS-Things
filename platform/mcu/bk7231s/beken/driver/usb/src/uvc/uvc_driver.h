#ifndef _UVC_DRIVER_H_
#define _UVC_DRIVER_H_

#include "mu_cdi.h"
#include "usb_pub.h"

#define ENABLE_UVC_SET_CONFIGURATION

#define FORMAT_MJPEG_ID        1
#define CONFIG_DESC_LENGTH_U20 0x02CE
#define CONFIG_DESC_LENGTH_U11 0x0100

/*
 * Refer to USB_Video_Class_1.1.pdf Table 4-47 Video Probe and Commit Controls
 */
#define OFFSET_FORMAT         2
#define OFFSET_FRAME          3
#define OFFSET_FRAME_INTERVAL 4
#define OFFSET_MAX_PAYLOAD    22
#define LENGTH_COMMIT_CONTROL 26

/*
 * The value is defined in field dwFrameInterval of  'Video Streaming MJPEG
Frame Type Descriptor' */
#define FPS30               0x00051615
#define FPS25               0x00061A80
#define FPS20               0x0007A120
#define FPS15               0x000A2C2A
#define FPS10               0x000F4240
#define FPS05               0x001E8480

#ifdef UVC_DEMO_SUPPORT102

#define PID_3822               0x3822
#define PID_3894               0x3894

/*
 * The value is defined in field wWidth and wHeight in 'Video Streaming MJPEG
Frame Type Descriptor'
 */

#define NOUSUPPORT            0xff

#define AU3822_MJPEG          1
#define AU3822_U2_640_480     1
#define AU3822_U2_640_360     2
#define AU3822_U2_320_240     3
#define AU3822_U2_160_120     4

#define AU3822_U1_640_480     1
#define AU3822_U1_160_120     2


#define AU3894_MJPEG         2
#define AU3894_H264          1

#define AU3894_U2_640_480    1
#define AU3894_U2_640_360    2
#define AU3894_U2_320_240    3
#define AU3894_U2_352_288    4

#define AU3894_U1_640_480    1
#define AU3894_U1_640_360    2
#define AU3894_U1_320_240    3
#define AU3894_U1_352_288    4

typedef struct
{
    uint32_t pid;
    uint16_t fmt_mjpeg;
    uint16_t fmt_h264;
    uint16_t res_x_y[UVC_FRAME_COUNT];
    uint16_t resv;
} UVC_PARAMS;
#endif

#define MGC_UVC_PREPARE_SETUP_PACKET(pSetup,\
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

/* Macro to fill control Irp for usb video Class driver */
#define MGC_UVC_FILL_CONTROL_IRP(pUvcDevice,\
                                 pControlIrp,\
                                 pActualOutBuffer,\
                                 dwRequestedOutLength,\
                                 pActualInBuffer,\
                                 dwRequestedInLength,\
                                 pfControlIrpComplete)\
{\
    (pControlIrp)->pDevice           = pUvcDevice->pUsbDevice;\
    (pControlIrp)->pOutBuffer        = pActualOutBuffer;\
    (pControlIrp)->dwOutLength       = dwRequestedOutLength;\
    (pControlIrp)->pInBuffer         = pActualInBuffer;\
    (pControlIrp)->dwInLength        = dwRequestedInLength;\
    (pControlIrp)->dwStatus          = 0;\
    (pControlIrp)->dwActualOutLength = 0;\
    (pControlIrp)->dwActualInLength  = 0;\
    (pControlIrp)->pfIrpComplete     = pfControlIrpComplete;\
    (pControlIrp)->pCompleteParam    = (void *) pUvcDevice;\
}

typedef struct
{
    uint32_t                connectStatus;
    MUSB_BusHandle          hBus;
    MUSB_Device             *pUsbDevice;
    MUSB_DeviceDriver       *pDriver;

    void                    *pDeviceId;
    MUSB_DeviceRequest      Setup;
    MUSB_ControlIrp         ControlIrp;
    uint32_t                ControlSyncLock;

    MUSB_PipePtr hInVideoControlPipe;
    MUSB_PipePtr hInVideoStreamingPipe;
    const MUSB_EndpointDescriptor  *pVCInEndpoint;
    const MUSB_EndpointDescriptor  *pVSInEndpoint;

    uint32_t vs_max_payload_transfer;
    uint32_t resolution_id;

#ifdef UVC_DEMO_SUPPORT102
    uint16_t enable_h264;
    uint16_t format_id;
    UVC_PARAMS uvc_params;
#endif

    uint32_t frame_per_second;
    MUSB_Irp videoStreamIrp;
    MUSB_IsochIrp videoStreamIoscIrp;

} MGC_UvcDevice;

extern MGC_UvcDevice UVC_Device;
extern uint8_t UVC_Connect(void          *pPrivateData,
                           MUSB_BusHandle     hBus,
                           MUSB_Device       *pUsbDevice,
                           const uint8_t     *pPeripheralList);
extern void UVC_Disonnect (void     *pPrivateData,
                           MUSB_BusHandle  hBus,
                           MUSB_Device    *pUsbDevice);
extern uint16_t UVC_FillPeripheralList(uint8_t bDriverIndex,
                                       uint8_t *pList,
                                       uint16_t wListRemain);
extern MUSB_DeviceDriver *UVC_GetClassDriver(void);

extern uint32_t UVC_StartStreaming(MGC_UvcDevice *pUvcDevice);
extern uint32_t UVC_StopStreaming(MGC_UvcDevice *pUvcDevice);
extern uint32_t UVC_UpdateVideoParamter(MGC_UvcDevice *pUvcDevice, uint16_t pid);
extern uint32_t UVC_SetParam(MGC_UvcDevice *pUvcDevice,
                             uint32_t resolution_id,
                             uint32_t fps);
extern uint32_t UVC_ReceiveVideoStream(MGC_UvcDevice *pUvcDevice,
                                       uint8_t *pBuffer,
                                       uint32_t len,
                                       MUSB_pfIrpComplete pCallback,
                                       void *pCBParam);
extern uint32_t UVC_ReceiveIoscVideoStream(MGC_UvcDevice *pUvcDevice,
        uint8_t *pBuffer,
        uint32_t len,
        MUSB_pfIsochIrpComplete pCallback,
        void *pCBParam);

#endif // _UVC_DRIVER_H_

// EOF

