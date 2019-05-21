#ifndef _USB_UVC_H_
#define _USB_UVC_H_

#include "include.h"

typedef int (*LinkSandFormat)( uint8_t *, uint32_t );
typedef struct
{
    LinkSandFormat Func;
    uint32_t  MaxLen;
} MUSB_SandList;


/*******************************************************************************
* Function Declarations
*******************************************************************************/
#if CFG_SUPPORT_UVC
extern int usb_sw_init(void);
extern int usb_sw_uninit(void);
extern uint32_t MGC_UvcSetConnect(void);
extern uint32_t MGC_UvcSetDisconnect(void);
extern uint32_t MGC_UvcStartStream(void);
extern uint32_t MGC_UvcStopStream(void);
extern void MGC_UvcSetParameter(uint32_t resolution_id, uint32_t fps);
extern uint32_t MGC_UvcRxVStream(uint8_t *buf, uint32_t len);
extern void MGC_UvcRegisterRxVstreamBufPtr(uint8_t *ptr);
extern void MGC_UvcRegisterRxVstreamBufLen(uint32_t len);
extern void MGC_UvcRegisterConfiguredCallback(FUNCPTR cb);
extern void MGC_UvcRegisterVSRxedCallback(FUNCPTR cb);
extern void MGC_UvcRegisterLink(uint8_t UDPFlag);
extern uint32_t MGC_UvcReceiveVideoStream(void);
extern uint32_t MGC_UvcGetConnectStatus(void);
extern uint32_t MGC_UvcUpdateVideoParamter(uint16_t pid);

#ifdef UVC_DEMO_SUPPORT102
extern uint32_t MGC_UvcEnableH264(void);
extern uint32_t MGC_UvcEnableMjpeg(void);
#endif

#endif // CFG_SUPPORT_UVC

#endif // _USB_UVC_H_

// EOF

