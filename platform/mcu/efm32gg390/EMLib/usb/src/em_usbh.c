/***************************************************************************//**
 * @file em_usbh.c
 * @brief USB protocol stack library API for EFM32.
 * @version 3.20.7
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#if defined( USB_PRESENT ) && ( USB_COUNT == 1 )
#include "em_usb.h"
#if defined( USB_HOST )

#include "em_cmu.h"
#include "em_usbtypes.h"
#include "em_usbhal.h"
#include "em_usbh.h"
#if ( USB_VBUSOVRCUR_PORT != USB_VBUSOVRCUR_PORT_NONE )
#include "em_gpio.h"
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

USBH_Hc_TypeDef                   hcs[ NUM_HC_USED + 2 ];
int                               USBH_attachRetryCount;
USBH_Init_TypeDef                 USBH_initData;
volatile USBH_PortState_TypeDef   USBH_portStatus;
const USBH_AttachTiming_TypeDef   USBH_attachTiming[]=
{
  /* debounceTime resetTime */
  {  200, 15 },
  {  200, 20 },
  {  200, 10 },
};

#define PORT_VBUS_DELAY       250
#define DEFAULT_CTRL_TIMEOUT  1000

static void Timeout( int hcnum );

#if defined( __ICCARM__ )
#pragma diag_suppress=Pe175
#endif

static void Timeout0(void){ Timeout(0); }
static void Timeout1(void){ Timeout(1); }
static void Timeout2(void){ Timeout(2); }
static void Timeout3(void){ Timeout(3); }
static void Timeout4(void){ Timeout(4); }
static void Timeout5(void){ Timeout(5); }
static void Timeout6(void){ Timeout(6); }
static void Timeout7(void){ Timeout(7); }
static void Timeout8(void){ Timeout(8); }
static void Timeout9(void){ Timeout(9); }
static void Timeout10(void){ Timeout(10); }
static void Timeout11(void){ Timeout(11); }
static void Timeout12(void){ Timeout(12); }
static void Timeout13(void){ Timeout(13); }

#if defined( __ICCARM__ )
#pragma diag_default=Pe175
#endif

static const USBTIMER_Callback_TypeDef hcTimeoutFunc[ MAX_NUM_HOSTCHANNELS ]=
{
  Timeout0, Timeout1, Timeout2, Timeout3, Timeout4, Timeout5,
  Timeout6, Timeout7, Timeout8, Timeout9, Timeout10, Timeout11,
  Timeout12, Timeout13
};

static void Timeout( int hcnum )
{
  uint32_t hcchar;
  USBH_Hc_TypeDef *hc;
  USBH_Ep_TypeDef *ep;

  hc = &hcs[ hcnum ];
  hcchar = USBHHAL_GetHcChar( hcnum );
  ep = hc->ep;

  if ( ep->type != USB_EPTYPE_INTR )
  {
    USBHHAL_HCHalt( hcnum, hcchar );
    hc->status |= HCS_TIMEOUT;
    ep->timeout = 0;
    if ( !hc->idle )
      USBHEP_TransferDone( hc->ep, USB_STATUS_TIMEOUT );
  }
  else
  {
    if ( !ep->timeout )
    {
      /* Restart the channel */
      USBHHAL_HCStart( hcnum );
      USBTIMER_Start( hcnum + HOSTCH_TIMER_INDEX,
                      ep->epDesc.bInterval, hcTimeoutFunc[ hcnum ] );
    }
    else
    {
      ep->timeout -= EFM32_MIN( ep->timeout, ep->epDesc.bInterval );
      if ( ep->timeout )
      {
        /* Restart the channel */
        USBHHAL_HCStart( hcnum );
        USBTIMER_Start( hcnum + HOSTCH_TIMER_INDEX,
                        EFM32_MIN( ep->timeout, ep->epDesc.bInterval ),
                        hcTimeoutFunc[ hcnum ] );
      }
      else
      {
        USBHHAL_HCHalt( hcnum, hcchar );
        hc->status |= HCS_TIMEOUT;
        if ( !hc->idle )
          USBHEP_TransferDone( hc->ep, USB_STATUS_TIMEOUT );
      }
    }
  }
}

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Assign a host channel to a given endpoint.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] hcnum
 *   Host channel number (0..).
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 *
 * @details
 *   After assigning a host channel to an endpoint, all subsequent transfers
 *   to the endpoint will use the given host channel. Several endpoints can
 *   be assigned to the same host channel, but keep in mind that concurrent
 *   transfers can only be performed on endpoints assigned to different host
 *   channels.  @n The default endpoint (EP0) is assigned to host channels 0
 *   and 1 by the host stack.
 ******************************************************************************/
int USBH_AssignHostChannel( USBH_Ep_TypeDef *ep, uint8_t hcnum )
{
  ep->type        = ep->epDesc.bmAttributes & CONFIG_DESC_BM_TRANSFERTYPE;
  ep->in          = ( ep->epDesc.bEndpointAddress & USB_SETUP_DIR_MASK ) != 0;
  ep->packetSize  = ep->epDesc.wMaxPacketSize;
  ep->addr        = ep->epDesc.bEndpointAddress;
  ep->state       = H_EP_IDLE;

  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_AssignHostChannel(),"
                        " ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( hcnum >= MAX_NUM_HOSTCHANNELS )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_AssignHostChannel(),"
                        " Illegal host channel number" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( ep->in )
  {
    ep->hcIn  = hcnum;
  }
  else
  {
    ep->hcOut = hcnum;
  }

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Send a SETUP command to a device, non-blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is non-blocking and returns immediately.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] bmRequestType
 *   SETUP command request type. A suitable combination of values
 *   @ref USB_SETUP_DIR_D2H, @ref USB_SETUP_DIR_H2D,
 *   @ref USB_SETUP_TYPE_STANDARD_MASK, @ref USB_SETUP_TYPE_CLASS_MASK,
 *   @ref USB_SETUP_TYPE_VENDOR_MASK, @ref USB_SETUP_RECIPIENT_DEVICE,
 *   @ref USB_SETUP_RECIPIENT_INTERFACE, @ref USB_SETUP_RECIPIENT_ENDPOINT or
 *   @ref USB_SETUP_RECIPIENT_OTHER.@n Refer to the USB specification for details.
 *
 * @param[in] bRequest
 *   A specific SETUP command request.
 *
 * @param[in] wValue
 *   Word sized field that varies according to request.
 *
 * @param[in] wIndex
 *   Word sized field that varies according to request. Typically used to pass
 *   an index or offset.
 *
 * @param[in] wLength
 *   Number of bytes to transfer if there is a data stage.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @param[in] callback
 *   Function to be called on transfer completion. Supply NULL if no callback
 *   is needed. See @ref USB_XferCompleteCb_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_ControlMsg( USBH_Ep_TypeDef *ep,
                     uint8_t bmRequestType,
                     uint8_t bRequest,
                     uint16_t wValue,
                     uint16_t wIndex,
                     uint16_t wLength,
                     void *data,
                     int timeout,
                     USB_XferCompleteCb_TypeDef callback )
{
  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsg(), ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( !USBH_DeviceConnected() )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsg(), No device connected" );
    return USB_STATUS_ILLEGAL;
  }

  if ( (uint32_t)data & 3 )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsg(), Misaligned data buffer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  INT_Disable();
  if ( ep->state != H_EP_IDLE )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsg(), Endpoint is busy" );
    return USB_STATUS_EP_BUSY;
  }

  if ( !hcs[ ep->hcIn ].idle || !hcs[ ep->hcOut ].idle )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsg(), Host channel is busy" );
    return USB_STATUS_HC_BUSY;
  }

  ep->setup.bmRequestType = bmRequestType;
  ep->setup.bRequest      = bRequest;
  ep->setup.wValue        = wValue;
  ep->setup.wIndex        = wIndex;
  ep->setup.wLength       = wLength;

  ep->xferCompleted  = false;
  ep->state          = H_EP_SETUP;
  ep->xferCompleteCb = callback;
  ep->buf            = data;
  ep->xferred        = 0;
  ep->timeout        = timeout;
  ep->setupErrCnt    = 0;

  if ( timeout )
  {
    USBTIMER_Start( ep->hcOut + HOSTCH_TIMER_INDEX,
                    timeout, hcTimeoutFunc[ ep->hcOut ] );
  }

  USBH_CtlSendSetup( ep );

  INT_Enable();

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Send a SETUP command to a device, blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out or failed.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] bmRequestType
 *   SETUP command request type. A suitable combination of values
 *   @ref USB_SETUP_DIR_D2H, @ref USB_SETUP_DIR_H2D,
 *   @ref USB_SETUP_TYPE_STANDARD_MASK, @ref USB_SETUP_TYPE_CLASS_MASK,
 *   @ref USB_SETUP_TYPE_VENDOR_MASK, @ref USB_SETUP_RECIPIENT_DEVICE,
 *   @ref USB_SETUP_RECIPIENT_INTERFACE, @ref USB_SETUP_RECIPIENT_ENDPOINT or
 *   @ref USB_SETUP_RECIPIENT_OTHER.@n Refer to the USB specification for details.
 *
 * @param[in] bRequest
 *   A specific SETUP command request.
 *
 * @param[in] wValue
 *   Word sized field that varies according to request.
 *
 * @param[in] wIndex
 *   Word sized field that varies according to request. Typically used to pass
 *   an index or offset.
 *
 * @param[in] wLength
 *   Number of bytes to transfer if there is a data stage.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_ControlMsgB( USBH_Ep_TypeDef *ep,
                      uint8_t bmRequestType,
                      uint8_t bRequest,
                      uint16_t wValue,
                      uint16_t wIndex,
                      uint16_t wLength,
                      void *data,
                      int timeout )
{
  int retVal;

  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_ControlMsgB() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsg( ep,
                            bmRequestType,
                            bRequest,
                            wValue,
                            wIndex,
                            wLength,
                            data,
                            timeout,
                            NULL );
  INT_Enable();

  if ( retVal == USB_STATUS_OK )
  {
    while ( ! ep->xferCompleted );

    if ( ( retVal = ep->xferStatus ) == USB_STATUS_OK )
    {
      retVal = ep->xferred;
    }
  }

  return retVal;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

USB_Status_TypeDef USBH_CtlReceiveData( USBH_Ep_TypeDef *ep, uint16_t length )
{
  USBH_Hc_TypeDef *hc;

  ep->in = true;
  ep->toggle = USB_PID_DATA1;
  ep->remaining = length;

  hc = &hcs[ ep->hcIn ];
  hc->buf = ep->buf;
  hc->xferred = 0;
  hc->remaining = length;

  USBHHAL_HCStart( ep->hcIn );

  return USB_STATUS_OK;
}

#if defined( USB_RAW_API )
int USBH_CtlRxRaw( uint8_t pid, USBH_Ep_TypeDef *ep, void *data, int byteCount )
{
  uint16_t  packets, len;
  int       hcnum, retval;
  uint32_t  hcchar, status;

  hcnum = ep->hcIn;
  if ( byteCount > 0 )
    packets = ( byteCount + ep->packetSize - 1 ) / ep->packetSize;
  else
    packets = 1;
  len = packets * ep->packetSize;

  INT_Disable();

  USB->HC[ hcnum ].INTMSK = USB_HC_INT_STALL | USB_HC_INT_NAK | USB_HC_INT_ACK;
  USB->HC[ hcnum ].INT = 0xFFFFFFFF;      /* Clear all interrupt flags      */
  NVIC_ClearPendingIRQ( USB_IRQn );
  USB->HAINTMSK |= 1 << hcnum;            /* Enable host channel interrupt  */
  USB->HC[ hcnum ].CHAR =                 /* Program HCCHAR register        */
      ( ep->parentDevice->addr     <<   _USB_HC_CHAR_DEVADDR_SHIFT     ) |
      ( ( ep->addr & USB_EPNUM_MASK ) << _USB_HC_CHAR_EPNUM_SHIFT      ) |
      ( ep->type                   <<   _USB_HC_CHAR_EPTYPE_SHIFT      ) |
      ( ep->packetSize             <<   _USB_HC_CHAR_MPS_SHIFT         ) |
      ( USB_HC_CHAR_EPDIR                                              ) |
      ( ep->parentDevice->speed ==
                           HPRT_L_SPEED >> _USB_HPRT_PRTSPD_SHIFT
                                    ?   USB_HC_CHAR_LSPDDEV       : 0  );
  USB->HC[ hcnum ].TSIZ =
          ( ( len     << _USB_HC_TSIZ_XFERSIZE_SHIFT ) &
                         _USB_HC_TSIZ_XFERSIZE_MASK       ) |
          ( ( packets << _USB_HC_TSIZ_PKTCNT_SHIFT   ) &
                         _USB_HC_TSIZ_PKTCNT_MASK         ) |
          ( ( pid     << _USB_HC_TSIZ_PID_SHIFT      ) &
                         _USB_HC_TSIZ_PID_MASK            );
  USB->HC[ hcnum ].DMAADDR = (uint32_t)data;
  hcchar = ( USB->HC[ hcnum ].CHAR & ~USB_HC_CHAR_CHDIS ) | USB_HC_CHAR_CHENA;
  USB->HC[ hcnum ].CHAR = hcchar;

  /* Start polling for interrupt */
  retval = USB_STATUS_EP_ERROR;
  while ( ( USBHAL_GetCoreInts() & USB_GINTSTS_HCHINT ) == 0 ){}
  if ( USBHHAL_GetHostChannelInts() & ( 1 << hcnum ) )
  {
    status = USBHHAL_GetHcInts( hcnum );
    hcchar |= USB_HC_CHAR_CHDIS;
    USB->HC[ hcnum ].CHAR = hcchar;
    USB->HC[ hcnum ].INTMSK = 0;
    USB->HC[ hcnum ].INT = 0xFFFFFFFF;

    if ( status & USB_HC_INT_STALL )
      retval = USB_STATUS_EP_STALLED;
    else if ( status & USB_HC_INT_NAK )
      retval = USB_STATUS_EP_NAK;
    else if ( status & USB_HC_INT_ACK )
      retval = USB_STATUS_OK;
  }

  NVIC_ClearPendingIRQ( USB_IRQn );
  INT_Enable();

  return retval;
}
#endif

USB_Status_TypeDef USBH_CtlSendData( USBH_Ep_TypeDef *ep, uint16_t length )
{
  USBH_Hc_TypeDef *hc;

  ep->in = false;
  ep->toggle = USB_PID_DATA1;
  ep->remaining = length;

  hc = &hcs[ ep->hcOut ];
  hc->buf = ep->buf;
  hc->xferred = 0;
  hc->remaining = length;

  USBHHAL_HCStart( ep->hcOut );

  return USB_STATUS_OK;
}

USB_Status_TypeDef USBH_CtlSendSetup( USBH_Ep_TypeDef *ep )
{
  USBH_Hc_TypeDef *hc;

  hc = &hcs[ ep->hcOut ];
  hc->buf = (uint8_t*)&ep->setup;
  hc->xferred = 0;
  hc->remaining = USB_SETUP_PKT_SIZE;
  hc->ep = ep;
  hcs[ ep->hcIn ].ep = ep;

  ep->toggle = USB_PID_SETUP;
  ep->remaining = USB_SETUP_PKT_SIZE;

  ep->in = true;
  USBHHAL_HCInit( ep->hcIn );

  ep->in = false;
  USBHHAL_HCInit( ep->hcOut );

  USBHHAL_HCStart( ep->hcOut );

  return USB_STATUS_OK;
}

#if defined( USB_RAW_API )
int USBH_CtlTxRaw( uint8_t pid, USBH_Ep_TypeDef *ep, void *data, int byteCount )
{
  uint16_t  packets;
  int       hcnum, retval;
  uint32_t  hcchar, status;

  hcnum = ep->hcOut;
  if ( byteCount > 0 )
    packets = ( byteCount + ep->packetSize - 1 ) / ep->packetSize;
  else
    packets = 1;

  INT_Disable();

  USB->HC[ hcnum ].INTMSK = USB_HC_INT_STALL | USB_HC_INT_NAK | USB_HC_INT_ACK;
  USB->HC[ hcnum ].INT = 0xFFFFFFFF;      /* Clear all interrupt flags      */
  NVIC_ClearPendingIRQ( USB_IRQn );
  USB->HAINTMSK |= 1 << hcnum;            /* Enable host channel interrupt  */
  USB->HC[ hcnum ].CHAR =                 /* Program HCCHAR register        */
      ( ep->parentDevice->addr     <<   _USB_HC_CHAR_DEVADDR_SHIFT     ) |
      ( ( ep->addr & USB_EPNUM_MASK ) << _USB_HC_CHAR_EPNUM_SHIFT      ) |
      ( ep->type                   <<   _USB_HC_CHAR_EPTYPE_SHIFT      ) |
      ( ep->packetSize             <<   _USB_HC_CHAR_MPS_SHIFT         ) |
      ( ep->parentDevice->speed ==
                           HPRT_L_SPEED >> _USB_HPRT_PRTSPD_SHIFT
                                    ?   USB_HC_CHAR_LSPDDEV       : 0  );
  USB->HC[ hcnum ].TSIZ =
          ( ( byteCount << _USB_HC_TSIZ_XFERSIZE_SHIFT ) &
                           _USB_HC_TSIZ_XFERSIZE_MASK       ) |
          ( ( packets   << _USB_HC_TSIZ_PKTCNT_SHIFT   ) &
                           _USB_HC_TSIZ_PKTCNT_MASK         ) |
          ( ( pid       << _USB_HC_TSIZ_PID_SHIFT      ) &
                           _USB_HC_TSIZ_PID_MASK            );
  USB->HC[ hcnum ].DMAADDR = (uint32_t)data;
  hcchar = ( USB->HC[ hcnum ].CHAR & ~USB_HC_CHAR_CHDIS ) | USB_HC_CHAR_CHENA;
  USB->HC[ hcnum ].CHAR = hcchar;

  /* Start polling for interrupt */
  retval = USB_STATUS_EP_ERROR;
  while ( ( USBHAL_GetCoreInts() & USB_GINTSTS_HCHINT ) == 0 ){}
  if ( USBHHAL_GetHostChannelInts() & ( 1 << hcnum ) )
  {
    status = USBHHAL_GetHcInts( hcnum );
    hcchar |= USB_HC_CHAR_CHDIS;
    USB->HC[ hcnum ].CHAR = hcchar;
    USB->HC[ hcnum ].INTMSK = 0;
    USB->HC[ hcnum ].INT = 0xFFFFFFFF;

    if ( status & USB_HC_INT_STALL )
      retval = USB_STATUS_EP_STALLED;
    else if ( status & USB_HC_INT_NAK )
      retval = USB_STATUS_EP_NAK;
    else if ( status & USB_HC_INT_ACK )
      retval = USB_STATUS_OK;
  }

  NVIC_ClearPendingIRQ( USB_IRQn );
  INT_Enable();

  return retval;
}
#endif

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Check if a device is connected.
 *
 * @return
 *   True if device connected, false otherwise.
 ******************************************************************************/
bool USBH_DeviceConnected( void )
{
  return USBH_portStatus == H_PORT_CONNECTED;
}

/***************************************************************************//**
 * @brief
 *   Read a configuration descriptor from a device.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] buf
 *   Pointer to transfer data buffer.
 *
 * @param[in] len
 *   The number of bytes to request, must not exceed transfer data buffer size.
 *
 * @param[in] configIndex
 *   Configuration index, a zero based number indicating which configuration
 *   to request.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_GetConfigurationDescriptorB( USBH_Device_TypeDef *device,
                                      void *buf, int len,
                                      uint8_t configIndex )
{
  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetConfigurationDescriptorB(),"
                        " device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( buf == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetConfigurationDescriptorB(),"
                        " buf NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  return USBH_ControlMsgB( &device->ep0,
                    USB_SETUP_DIR_D2H | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    GET_DESCRIPTOR,                       /* bRequest      */
                    (USB_CONFIG_DESCRIPTOR << 8) | configIndex,/* wValue   */
                    0,                                    /* wIndex        */
                    len,                                  /* wLength       */
                    buf,                                  /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */
}

/***************************************************************************//**
 * @brief
 *   Read a device descriptor from a device.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] buf
 *   Pointer to transfer data buffer.
 *
 * @param[in] len
 *   The number of bytes to request, must not exceed transfer data buffer size.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_GetDeviceDescriptorB( USBH_Device_TypeDef *device,
                               void *buf, int len )
{
  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetDeviceDescriptorB(),"
                        " device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( buf == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetDeviceDescriptorB(),"
                        " buf NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  return USBH_ControlMsgB( &device->ep0,
                    USB_SETUP_DIR_D2H | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    GET_DESCRIPTOR,                       /* bRequest      */
                    USB_DEVICE_DESCRIPTOR << 8,           /* wValue        */
                    0,                                    /* wIndex        */
                    len,                                  /* wLength       */
                    buf,                                  /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */
}

/***************************************************************************//**
 * @brief
 *   Get the bus speed of the device attached to the USB port.
 *
 * @return
 *   @ref PORT_FULL_SPEED or @ref PORT_LOW_SPEED.
 ******************************************************************************/
uint8_t USBH_GetPortSpeed( void )
{
  return USBHHAL_GetPortSpeed();
}

/***************************************************************************//**
 * @brief
 *   Read a string descriptor from a device.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *   @n The maximum permitted USB string lenght is 255 bytes.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] buf
 *   Pointer to transfer data buffer.
 *
 * @param[in] bufLen
 *   Transfer data buffer size.
 *
 * @param[in] stringIndex
 *   String index, a zero based number indicating which string to request.
 *
 * @param[in] langID
 *   String language ID.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_GetStringB( USBH_Device_TypeDef *device, uint8_t *buf, int bufLen,
                     uint8_t stringIndex, uint16_t langID )
{
  int retVal;
  USB_StringDescriptor_TypeDef* sd;

  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetStringB(), device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( buf == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_GetStringB(), buf NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsgB(
                    &device->ep0,
                    USB_SETUP_DIR_D2H | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    GET_DESCRIPTOR,                       /* bRequest      */
                    (USB_STRING_DESCRIPTOR << 8) | stringIndex,/* wValue   */
                    langID,                               /* wIndex        */
                    EFM32_MIN( bufLen, 255 ),             /* wLength       */
                    buf,                                  /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */

  if ( retVal > 2 )
  {
    sd = (USB_StringDescriptor_TypeDef*)buf;
    if ( sd->len )
    {
      sd->name[ (sd->len - 2) / sizeof( char16_t ) ] = '\0';
    }
  }
  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Initialize host protocol stack data structures.
 *
 * @param[in] p
 *   Pointer to initialization structure. See @ref USBH_Init_TypeDef.
 *
 * @details
 *   Host stack internal data structures are initialized, no actions will be
 *   performed on the USB port. Use this function once before starting USB
 *   host operation. USB operation is initated with
 *   @ref USBH_WaitForDeviceConnectionB().
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_Init( const USBH_Init_TypeDef *p )
{
  if ( p == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Init(), init struct NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( (    p->rxFifoSize   < MIN_EP_FIFO_SIZE_INBYTES )
       || ( p->nptxFifoSize < MIN_EP_FIFO_SIZE_INBYTES )
       || ( p->ptxFifoSize  < MIN_EP_FIFO_SIZE_INBYTES )
       || ( (p->rxFifoSize + p->nptxFifoSize + p->ptxFifoSize) >
            (MAX_HOST_FIFO_SIZE_INWORDS * 4) ) )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Init(), Illegal Tx/Rx FIFO memory configuration" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }
  USBH_initData = *p;

  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );
  USBTIMER_Init();
  USBH_portStatus = H_PORT_DISCONNECTED;

  /* Enable USB clock. */
  INT_Disable();
  CMU->CMD = CMU_CMD_USBCCLKSEL_HFCLKNODIV;
  CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC;
  INT_Enable();

  /* Enable USB interrupt. */
  NVIC_ClearPendingIRQ( USB_IRQn );
  NVIC_EnableIRQ( USB_IRQn );

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Populate device and endpoint data structures with data retrieved during
 *   device enumeration.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] buf
 *   A data buffer containing enumeration data retrieved with
 *   @ref USBH_QueryDeviceB().
 *
 * @param[in] ep
 *   Pointer to an array of @ref USBH_Ep_TypeDef endpoint data structures.
 *
 * @param[in] numEp
 *   Number of elements in endpoint array.
 *
 * @param[in] deviceSpeed
 *   @ref PORT_FULL_SPEED or @ref PORT_LOW_SPEED.
 *
 * @details
 *   Use this function prior to moving a device out of default state.
 *   The application itself must allocate device and endpoint structures.
 *   Data from a prior call to @ref USBH_QueryDeviceB() must be passed in input
 *   parameter @em buf. The device speed can be determined with
 *   @ref USBH_GetPortSpeed() for devices directly attached to the USB port.
 *   Devices attached via a hub must retrieve this information by querying the
 *   hub.
 ******************************************************************************/
int USBH_InitDeviceData( USBH_Device_TypeDef *device,
                         const uint8_t *buf,
                         USBH_Ep_TypeDef *ep,
                         int numEp,
                         uint8_t deviceSpeed )
{
  int i;

  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_InitDeviceData(), device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  memset( device, 0, sizeof( USBH_Device_TypeDef ) );

  device->ep = ep;
  device->addr = 0;
  device->numEp = numEp;
  device->speed = deviceSpeed;

  device->ep0.packetSize = 8;
  if ( buf )      /* Copy EP0 data from buffer filled by USBH_QueryDevice() */
  {
    memcpy( &device->ep0,
            &((USBH_Device_TypeDef*)buf)->ep0, sizeof( USBH_Ep_TypeDef ) );
  }
  device->ep0.parentDevice = device;
  device->ep0.type = USB_EPTYPE_CTRL;
  device->ep0.hcOut = 0;
  device->ep0.hcIn = 1;

  if ( ep )
  {
    for ( i=0; i<numEp; i++ )
    {
      memset( &device->ep[i], 0, sizeof( USBH_Ep_TypeDef ) );
      device->ep[i].parentDevice = device;
      device->ep[i].toggle = USB_PID_DATA0;
    }
  }

  if ( buf )/* Copy descriptor data from buffer filled by USBH_QueryDevice() */
  {
    memcpy( &device->devDesc,
            &((USBH_Device_TypeDef*)buf)->devDesc, USB_DEVICE_DESCSIZE );
    memcpy( &device->confDesc,
            &((USBH_Device_TypeDef*)buf)->confDesc, USB_CONFIG_DESCSIZE );
    memcpy( &device->itfDesc,
            USBH_QGetInterfaceDescriptor( buf, 0, 0 ), USB_INTERFACE_DESCSIZE );

    if ( ep )
    {
      for ( i=0; i<numEp; i++ )
      {
        memcpy( &device->ep[i].epDesc,
                USBH_QGetEndpointDescriptor( buf, 0, 0, i ),
                USB_ENDPOINT_DESCSIZE );
      }
    }
  }

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Drive reset signalling on the USB port.
 *
 * @note
 *   This function is primarily meant for debugging a device.
 *   When returning the device will appear to be disconnected.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_PortReset( void )
{
  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_PortReset() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USBH_portStatus = H_PORT_CONNECTED_RESETTING;
  USBHHAL_PortReset( true );
  INT_Enable();
  USBTIMER_DelayMs( 30 );                 /* USB Reset delay */
  INT_Disable();
  USBHHAL_PortReset( false );
  INT_Enable();
  USBTIMER_DelayMs( 10 );                 /* Reset recovery time */
  USBH_portStatus = H_PORT_DISCONNECTED;

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Drive resume signalling on the USB port.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_PortResume( void )
{
  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_PortResume() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USBHHAL_PortResume( true );
  INT_Enable();
  USBTIMER_DelayMs( 30 );
  INT_Disable();
  USBHHAL_PortResume( false );
  INT_Enable();

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Set the USB port in suspend mode.
 ******************************************************************************/
void USBH_PortSuspend( void )
{
  INT_Disable();
  USBHHAL_PortSuspend();
  INT_Enable();
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

int USBH_PortVbusOn( bool on )
{
  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_PortVbusOn() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USBHHAL_VbusOn( on );

  if ( !on )
    USBH_portStatus = H_PORT_DISCONNECTED;

  INT_Enable();
  USBTIMER_DelayMs( PORT_VBUS_DELAY );

  return USB_STATUS_OK;
}

/** @endcond */

#if defined( USB_USE_PRINTF )
/***************************************************************************//**
 * @brief
 *   Pretty print a configuration descriptor on the debug serial port.
 *
 * @note
 *   This function is enabled when \#define USB_USE_PRINTF and
 *   \#define USER_PUTCHAR macros are properly defined when configuring the
 *   protocol stack in "usbconfig.h".
 *   The function is primarily meant for debugging purposes.
 *
 * @param[in] config
 *   Pointer to a @ref USB_ConfigurationDescriptor_TypeDef data structure.
 *
 * @param[in] maxLen
 *   The size of the data buffer passed as input parameter @em config.
 ******************************************************************************/
int USBH_PrintConfigurationDescriptor(
                            const USB_ConfigurationDescriptor_TypeDef *config,
                            int maxLen )
{
  char *c;
  int remaining;

  if ( config == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_PrintConfigurationDescriptor(),"
                        " config NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USB_PRINTF( "\n\nConfiguration descriptor:" );
  USB_PRINTF( "\n bLength              %d",     config->bLength             );
  USB_PRINTF( "\n bDescriptorType      0x%02X", config->bDescriptorType     );
  USB_PRINTF( "\n wTotalLength         %d",     config->wTotalLength        );
  USB_PRINTF( "\n bNumInterfaces       %d",     config->bNumInterfaces      );
  USB_PRINTF( "\n bConfigurationValue  %d",     config->bConfigurationValue );
  USB_PRINTF( "\n iConfiguration       %d",     config->iConfiguration      );
  USB_PRINTF( "\n bmAttributes         0x%02X", config->bmAttributes        );
  USB_PRINTF( "\n bMaxPower            %d\n",   config->bMaxPower           );

  maxLen -= config->bLength;
  if ( maxLen > 0 )
  {
    remaining = EFM32_MIN( config->wTotalLength - config->bLength, maxLen );
    if ( remaining > 0 )
    {
      USB_PUTCHAR( '\n' );
      c = (char*)(config + 1);
      while ( remaining-- )
      {
        USB_PRINTF( "0x%02X ", *c++ );
      }
      USB_PUTCHAR( '\n' );
    }
  }

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Pretty print a device descriptor on the debug serial port.
 *
 * @note
 *   This function is enabled when \#define USB_USE_PRINTF and
 *   \#define USER_PUTCHAR macros are properly defined when configuring the
 *   protocol stack in "usbconfig.h".
 *   The function is primarily meant for debugging purposes.
 *
 * @param[in] device
 *   Pointer to a @ref USB_DeviceDescriptor_TypeDef data structure.
 ******************************************************************************/
int USBH_PrintDeviceDescriptor( const USB_DeviceDescriptor_TypeDef *device )
{
  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_PrintDeviceDescriptor(),"
                        " device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USB_PRINTF( "\n\nDevice descriptor:" );
  USB_PRINTF( "\n bLength              %d",     device->bLength            );
  USB_PRINTF( "\n bDescriptorType      0x%02X", device->bDescriptorType    );
  USB_PRINTF( "\n bcdUSB               0x%04X", device->bcdUSB             );
  USB_PRINTF( "\n bDeviceClass         %d",     device->bDeviceClass       );
  USB_PRINTF( "\n bDeviceSubClass      %d",     device->bDeviceSubClass    );
  USB_PRINTF( "\n bDeviceProtocol      %d",     device->bDeviceProtocol    );
  USB_PRINTF( "\n bMaxPacketSize       %d",     device->bMaxPacketSize0    );
  USB_PRINTF( "\n idVendor             0x%04X", device->idVendor           );
  USB_PRINTF( "\n idProduct            0x%04X", device->idProduct          );
  USB_PRINTF( "\n bcdDevice            0x%04X", device->bcdDevice          );
  USB_PRINTF( "\n iManufacturer        %d",     device->iManufacturer      );
  USB_PRINTF( "\n iProduct             %d",     device->iProduct           );
  USB_PRINTF( "\n iSerialNumber        %d",     device->iSerialNumber      );
  USB_PRINTF( "\n bNumConfigurations   %d\n",   device->bNumConfigurations );

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Pretty print an endpoint descriptor on the debug serial port.
 *
 * @note
 *   This function is enabled when \#define USB_USE_PRINTF and
 *   \#define USER_PUTCHAR macros are properly defined when configuring the
 *   protocol stack in "usbconfig.h".
 *   The function is primarily meant for debugging purposes.
 *
 * @param[in] endpoint
 *   Pointer to a @ref USB_EndpointDescriptor_TypeDef data structure.
 ******************************************************************************/
int USBH_PrintEndpointDescriptor(
                            const USB_EndpointDescriptor_TypeDef *endpoint )
{
  if ( endpoint == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_PrintEndpointDescriptor(),"
                        " endpoint NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USB_PRINTF( "\n\nEndpoint descriptor:" );
  USB_PRINTF( "\n bLength              %d",     endpoint->bLength          );
  USB_PRINTF( "\n bDescriptorType      0x%02X", endpoint->bDescriptorType  );
  USB_PRINTF( "\n bEndpointAddress     0x%02X", endpoint->bEndpointAddress );
  USB_PRINTF( "\n bmAttributes         0x%02X", endpoint->bmAttributes     );
  USB_PRINTF( "\n wMaxPacketSize       %d",     endpoint->wMaxPacketSize   );
  USB_PRINTF( "\n bInterval            %d\n",   endpoint->bInterval        );

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Pretty print an interface descriptor on the debug serial port.
 *
 * @note
 *   This function is enabled when \#define USB_USE_PRINTF and
 *   \#define USER_PUTCHAR macros are properly defined when configuring the
 *   protocol stack in "usbconfig.h".
 *   The function is primarily meant for debugging purposes.
 *
 * @param[in] interface
 *   Pointer to a @ref USB_InterfaceDescriptor_TypeDef data structure.
 ******************************************************************************/
int USBH_PrintInterfaceDescriptor(
                            const USB_InterfaceDescriptor_TypeDef *interface )
{
  if ( interface == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_PrintInterfaceDescriptor(),"
                        " interface NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  USB_PRINTF( "\n\nInterface descriptor:" );
  USB_PRINTF( "\n bLength              %d",     interface->bLength            );
  USB_PRINTF( "\n bDescriptorType      0x%02X", interface->bDescriptorType    );
  USB_PRINTF( "\n bInterfaceNumber     %d",     interface->bInterfaceNumber   );
  USB_PRINTF( "\n bAlternateSetting    %d",     interface->bAlternateSetting  );
  USB_PRINTF( "\n bNumEndpoints        %d",     interface->bNumEndpoints      );
  USB_PRINTF( "\n bInterfaceClass      0x%02X", interface->bInterfaceClass    );
  USB_PRINTF( "\n bInterfaceSubClass   0x%02X", interface->bInterfaceSubClass );
  USB_PRINTF( "\n bInterfaceProtocol   0x%02X", interface->bInterfaceProtocol );
  USB_PRINTF( "\n iInterface           %d\n",   interface->iInterface         );

  return USB_STATUS_OK;
}
#endif /* defined( USB_USE_PRINTF ) */

#if defined( __ICCARM__ )
#pragma diag_suppress=Pa039
#endif
/***************************************************************************//**
 * @brief
 *   Print a USB string descriptor on the debug serial port.
 *
 * @note
 *   This function is enabled when the \#define USER_PUTCHAR macro is properly
 *   defined when configuring the protocol stack in "usbconfig.h".
 *   The function is primarily meant for debugging purposes.
 *
 * @param[in] pre
 *   Optional text string to prepend to the string descriptor. Pass NULL if
 *   not needed.
 *
 * @param[in] s
 *   Pointer to a @ref USB_StringDescriptor_TypeDef data structure.
 *
 * @param[in] post
 *   Optional text string to append to the string descriptor. Pass NULL if
 *   not needed.
 ******************************************************************************/
void USBH_PrintString( const char *pre,
                       const USB_StringDescriptor_TypeDef *s,
                       const char *post )
{
  char *c;

  if ( pre )
  {
    USB_PUTS( pre );
  }

  if ( s )
  {
    c = (char*)&s->name;
    while ( *c )
    {
      USB_PUTCHAR( *c );
      c += 2;
    }
  }

  if ( post )
  {
    USB_PUTS( post );
  }
}
#if defined( __ICCARM__ )
#pragma diag_default=Pa039
#endif

 /*
  * A request for a configuration descriptor returns the configuration
  * descriptor, all interface descriptors, and endpoint descriptors for all
  * of the interfaces in a single request.
  * The first interface descriptor follows the configuration descriptor.
  * The endpoint descriptors for the first interface follow the first interface
  * descriptor.
  * If there are additional interfaces, their interface descriptor and endpoint
  * descriptors follow the first interface’s endpoint descriptors.
  * Class-specific and/or vendor-specific descriptors follow the standard
  * descriptors they extend or modify.
  */

/***************************************************************************//**
 * @brief
 *   Return a pointer to a configuration descriptor.
 *
 * @note
 *   This function search through @em buf looking for a given configuration
 *   descriptor.
 *
 * @param[in] buf
 *   A data buffer containing enumeration data retrieved with
 *   @ref USBH_QueryDeviceB().
 *
 * @param[in] configIndex
 *   Configuration index, a zero based number indicating which configuration
 *   descriptor to find.
 *
 * @return
 *   A pointer to @ref USB_ConfigurationDescriptor_TypeDef. NULL if no
 *   descriptor found.
 ******************************************************************************/
USB_ConfigurationDescriptor_TypeDef *USBH_QGetConfigurationDescriptor(
                                    const uint8_t *buf, int configIndex )
{
  int i;
  const uint8_t *start, *end;
  USB_DeviceDescriptor_TypeDef *dev;

  dev = USBH_QGetDeviceDescriptor( buf );
  if ( dev )
  {
    if ( configIndex < dev->bNumConfigurations )
    {
      /* Start of first configuration descriptor */
      start = buf + sizeof( USBH_Device_TypeDef );

      /* Find end of avaiable data, NOTE: ep contains end of buf ! */
      end = EFM32_MIN(
          start + ((USB_ConfigurationDescriptor_TypeDef*)start)->wTotalLength,
          (uint8_t*)(((USBH_Device_TypeDef*)buf)->ep));

      /* Scan through, looking for correct configuration descriptor */
      i = 0;
      while ( start < end )
      {
        if ( *start == 0 )
          return NULL;

        if ( ( *start     == USB_CONFIG_DESCSIZE   ) &&
             ( *(start+1) == USB_CONFIG_DESCRIPTOR )    )
        {
          if ( i == configIndex )
          {
            return (USB_ConfigurationDescriptor_TypeDef*)start;
          }
        i++;
        }

        start += *start;
      }
    }
  }
  return NULL;
}

/***************************************************************************//**
 * @brief
 *   Return a pointer to the device descriptor.
 *
 * @param[in] buf
 *   A data buffer containing enumeration data retrieved with
 *   @ref USBH_QueryDeviceB().
 *
 * @return
 *   A pointer to @ref USB_DeviceDescriptor_TypeDef. NULL if no
 *   descriptor found.
 ******************************************************************************/
USB_DeviceDescriptor_TypeDef *USBH_QGetDeviceDescriptor( const uint8_t *buf )
{
  USB_DeviceDescriptor_TypeDef *dev = &((USBH_Device_TypeDef*)buf)->devDesc;

  if ( buf == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_QGetDeviceDescriptor(), buf NULL pointer" );
    EFM_ASSERT( false );
    return NULL;
  }

  if ( ( dev->bLength         == USB_DEVICE_DESCSIZE   ) &&
       ( dev->bDescriptorType == USB_DEVICE_DESCRIPTOR )    )
  {
    return dev;
  }
  return NULL;
}

/***************************************************************************//**
 * @brief
 *   Return a pointer to an endpoint descriptor.
 *
 * @note
 *   This function search through @em buf looking for a given endpoint
 *   descriptor.
 *
 * @param[in] buf
 *   A data buffer containing enumeration data retrieved with
 *   @ref USBH_QueryDeviceB().
 *
 * @param[in] configIndex
 *   Configuration index, a zero based number indicating in which configuration
 *   descriptor to look for the interface containing the endpoint descriptor.
 *
 * @param[in] interfaceIndex
 *   Interface index, a zero based number indicating the interface
 *   descriptor to look for the correct endpoint descriptor in.
 *
 * @param[in] endpointIndex
 *   Endpoint index, a zero based number indicating which endpoint
 *   descriptor to look for.
 *
 * @return
 *   A pointer to @ref USB_EndpointDescriptor_TypeDef. NULL if no
 *   descriptor found.
 ******************************************************************************/
USB_EndpointDescriptor_TypeDef *USBH_QGetEndpointDescriptor(
    const uint8_t *buf, int configIndex, int interfaceIndex, int endpointIndex )
{
  int i;
  uint8_t *start, *end;
  USB_InterfaceDescriptor_TypeDef *itf;

  itf = USBH_QGetInterfaceDescriptor( buf, configIndex, interfaceIndex );
  if ( itf )
  {
    if ( endpointIndex < itf->bNumEndpoints )
    {
      /* First possible endpoint descriptor */
      start = (uint8_t*)itf + USB_INTERFACE_DESCSIZE;

      /* Search until start of next interface decsriptor, or to end of buf */
      end = (uint8_t*)USBH_QGetInterfaceDescriptor(
                                        buf, configIndex, interfaceIndex + 1 );
      if ( end == NULL )
      {
        /* NOTE: ep contains end of buf ! */
        end = (uint8_t*)((USBH_Device_TypeDef*)buf)->ep;
      }

      /* Scan through, looking for correct endpoint descriptor */
      i = 0;
      while ( start < end )
      {
        if ( *start == 0 )
          return NULL;

        if ( ( *start     == USB_ENDPOINT_DESCSIZE   ) &&
             ( *(start+1) == USB_ENDPOINT_DESCRIPTOR )    )
        {
          if ( i == endpointIndex )
          {
            return (USB_EndpointDescriptor_TypeDef*)start;
          }
        i++;
        }

        start += *start;
      }
    }
  }
  return NULL;
}

/***************************************************************************//**
 * @brief
 *   Return a pointer to an interface descriptor.
 *
 * @note
 *   This function search through @em buf looking for a given interface
 *   descriptor.
 *
 * @param[in] buf
 *   A data buffer containing enumeration data retrieved with
 *   @ref USBH_QueryDeviceB().
 *
 * @param[in] configIndex
 *   Configuration index, a zero based number indicating in which configuration
 *   descriptor to look for the interface descriptor.
 *
 * @param[in] interfaceIndex
 *   Interface index, a zero based number indicating which interface
 *   descriptor to find.
 *
 * @return
 *   A pointer to @ref USB_InterfaceDescriptor_TypeDef. NULL if no
 *   descriptor found.
 ******************************************************************************/
USB_InterfaceDescriptor_TypeDef *USBH_QGetInterfaceDescriptor(
                      const uint8_t *buf, int configIndex, int interfaceIndex )
{
  int i;
  uint8_t *start, *end;
  USB_ConfigurationDescriptor_TypeDef *conf;

  conf = USBH_QGetConfigurationDescriptor( buf, configIndex );
  if ( conf )
  {
    if ( interfaceIndex < conf->bNumInterfaces )
    {
      /* First interface descriptor */
      start = (uint8_t*)conf + USB_CONFIG_DESCSIZE;

      /* Search until start of next config decsriptor, or to end of buf */
      end = (uint8_t*)USBH_QGetConfigurationDescriptor( buf, configIndex + 1 );
      if ( end == NULL )
      {
        /* NOTE: ep contains end of buf ! */
        end = (uint8_t*)((USBH_Device_TypeDef*)buf)->ep;
      }

      /* Scan through, looking for correct interface descriptor */
      i = 0;
      while ( start < end )
      {
        if ( *start == 0 )
          return NULL;

        if ( ( *start     == USB_INTERFACE_DESCSIZE   ) &&
             ( *(start+1) == USB_INTERFACE_DESCRIPTOR )    )
        {
          if ( ( i == interfaceIndex ) &&
               ( i ==
                 ((USB_InterfaceDescriptor_TypeDef*)start)->bInterfaceNumber ) )
          {
            return (USB_InterfaceDescriptor_TypeDef*)start;
          }
        i++;
        }

        start += *start;
      }
    }
  }
  return NULL;
}

/***************************************************************************//**
 * @brief
 *   Will request both the device descriptor and the entire configuration
 *   descriptor from the device at USB address 0.
 *
 * @note
 *   This function is normally used to retrieve the data needed by
 *   @ref USBH_InitDeviceData().
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] buf
 *   A data buffer with sufficent space for the descriptors. The data buffer
 *   size must be sizeof( @ref USBH_Device_TypeDef ) + the anticipated
 *   maximum size of the entire configuration descriptor.
 *
 * @param[in] bufsize
 *   The size of the data buffer.
 *
 * @param[in] deviceSpeed
 *   @ref PORT_FULL_SPEED or @ref PORT_LOW_SPEED.
 *
 * @details
 *   The device speed can be determined with @ref USBH_GetPortSpeed() for
 *   devices directly attached to the USB port. Devices attached via a hub
 *   must retrieve this information by querying the hub.
 ******************************************************************************/
int USBH_QueryDeviceB( uint8_t *buf, size_t bufsize, uint8_t deviceSpeed )
{
  USBH_Device_TypeDef *device = (USBH_Device_TypeDef*)buf;

  USBH_InitDeviceData( device, NULL, NULL, 0, deviceSpeed );

  device->speed = deviceSpeed;
  device->ep = (USBH_Ep_TypeDef*)(buf+bufsize);/* Save buffer end addr here ! */

  /* Get the first 8 bytes of the device desc to figure out device EP0 size */
  if ( 8 != USBH_GetDeviceDescriptorB( device,
                                       &device->devDesc,
                                       EFM32_MIN( 8U, bufsize ) ) )
  {
    return USB_STATUS_REQ_ERR;
  }
  device->ep0.packetSize = device->devDesc.bMaxPacketSize0;

  /* Get entire device descriptor */
  if ( USB_DEVICE_DESCSIZE != USBH_GetDeviceDescriptorB(
                           device,
                           buf + sizeof( USBH_Device_TypeDef ),
                           EFM32_MIN( (size_t)USB_DEVICE_DESCSIZE, bufsize ) ) )
  {
    return USB_STATUS_REQ_ERR;
  }
  memcpy( &device->devDesc,
          buf + sizeof( USBH_Device_TypeDef ), USB_DEVICE_DESCSIZE );

  /* Get configuration descriptor. */
  if ( USB_CONFIG_DESCSIZE != USBH_GetConfigurationDescriptorB(
                           device,
                           buf + sizeof( USBH_Device_TypeDef ),
                           EFM32_MIN( (size_t)USB_CONFIG_DESCSIZE,
                                      bufsize - (size_t)USB_DEVICE_DESCSIZE ),
                           0 ) )
  {
    return USB_STATUS_REQ_ERR;
  }
  memcpy( &device->confDesc,
          buf + sizeof( USBH_Device_TypeDef ), USB_CONFIG_DESCSIZE );

  /* Get complete configuration descriptor. NOTE: Descriptor data is    */
  /* appended at the end of the device data structure !                 */
  if ( ( USB_CONFIG_DESCSIZE + USB_INTERFACE_DESCSIZE ) >
       USBH_GetConfigurationDescriptorB(
                           device,
                           buf + sizeof( USBH_Device_TypeDef ),
                           EFM32_MIN( device->confDesc.wTotalLength,
                                      bufsize - sizeof( USBH_Device_TypeDef )),
                           0 ) )
  {
    return USB_STATUS_REQ_ERR;
  }

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Read data from device endpoint, non-blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is non-blocking and returns immediately.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] byteCount
 *   Number of bytes to transfer (zero is a valid value).
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @param[in] callback
 *   Function to be called on transfer completion. Supply NULL if no callback
 *   is needed. See @ref USB_XferCompleteCb_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_Read( USBH_Ep_TypeDef *ep, void *data, int byteCount, int timeout,
               USB_XferCompleteCb_TypeDef callback )
{
  USBH_Hc_TypeDef *hc = &hcs[ ep->hcIn ];

  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( !USBH_DeviceConnected() )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), No device connected" );
    return USB_STATUS_ILLEGAL;
  }

  if ( (   byteCount > MAX_XFER_LEN                           ) ||
       ( ( byteCount / ep->packetSize ) > MAX_PACKETS_PR_XFER )    )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), Illegal transfer size" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( (uint32_t)data & 3 )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), Misaligned data buffer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  INT_Disable();
  if ( ep->state != H_EP_IDLE )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), Endpoint is busy" );
    return USB_STATUS_EP_BUSY;
  }

  if ( !ep->in )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), Illegal EP direction" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( !hc->idle )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Read(), Host channel is busy" );
    return USB_STATUS_HC_BUSY;
  }

  ep->xferCompleted  = false;
  ep->state          = H_EP_DATA_IN;
  ep->xferCompleteCb = callback;
  ep->buf            = data;
  ep->remaining      = byteCount;
  ep->xferred        = 0;
  ep->timeout        = timeout;

  hc->buf = data;
  hc->xferred = 0;
  hc->remaining = byteCount;
  hc->ep = ep;

  if ( ep->type == USB_EPTYPE_INTR )
  {
    if ( timeout )
      timeout = EFM32_MIN( timeout, ep->epDesc.bInterval );
    else
      timeout = ep->epDesc.bInterval;
  }

  if ( timeout )
  {
    USBTIMER_Start( ep->hcIn + HOSTCH_TIMER_INDEX,
                    timeout, hcTimeoutFunc[ ep->hcIn ] );
  }

  USBHHAL_HCInit( ep->hcIn );
  USBHHAL_HCStart( ep->hcIn );

  INT_Enable();

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Read data from device endpoint, blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out or failed.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] byteCount
 *   Number of bytes to transfer (zero is a valid value).
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_ReadB( USBH_Ep_TypeDef *ep, void *data, int byteCount, int timeout )
{
  int retVal;

  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_ReadB() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_Read( ep, data, byteCount, timeout, NULL );
  INT_Enable();

  if ( retVal == USB_STATUS_OK )
  {
    while ( ! ep->xferCompleted );

    if ( ( retVal = ep->xferStatus ) == USB_STATUS_OK )
    {
      retVal = ep->xferred;
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Give a device an USB address.
 *
 * @note
 *   The device must currently have address 0. This command will move the
 *   device to the addressed state.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] deviceAddress
 *   The new device address. Provide a value between 0 and 127.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_SetAddressB( USBH_Device_TypeDef *device, uint8_t deviceAddress )
{
  int retVal;

  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_SetAddressB(), device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( deviceAddress > USB_MAX_DEVICE_ADDRESS )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_SetAddressB(), Illegal device address" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsgB(
                    &device->ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    SET_ADDRESS,                          /* bRequest      */
                    deviceAddress,                        /* wValue        */
                    0,                                    /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */

  if ( retVal == USB_STATUS_OK )
  {
    device->addr = deviceAddress;
  }
  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Activate a device interface within current device configuration.
 *
 * @note
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] interfaceIndex
 *   The interface index. A zero based value.
 *
 * @param[in] alternateSetting
 *   The alternate interface setting value.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_SetAltInterfaceB( USBH_Device_TypeDef *device,
                           uint8_t interfaceIndex, uint8_t alternateSetting )
{
  int i, retVal;

  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_SetAltInterfaceB(), device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsgB(
                    &device->ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    SET_INTERFACE,                        /* bRequest      */
                    alternateSetting,                     /* wValue        */
                    interfaceIndex,                       /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */

  if ( retVal == USB_STATUS_OK )
  {
    // Alternate settings are mutually exclusive within the interface

    // We should:
    // Deactivate ep's in previous alternate interface setting
    // Activate ep's in the new alternate setting of the interface

    for ( i=0; i<device->numEp; i++ )           /* Reset all data toggles */
    {
      device->ep[i].toggle = USB_PID_DATA0;
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Activate a device configuration.
 *
 * @note
 *   This command will move the device to the configured state.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] device
 *   Pointer to a @ref USBH_Device_TypeDef data structure.
 *
 * @param[in] configValue
 *   The configuration value. The value can be retrieved from
 *   device->confDesc.bConfigurationValue
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_SetConfigurationB( USBH_Device_TypeDef *device, uint8_t configValue )
{
  int i, retVal;

  if ( device == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_SetConfigurationB(), device NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsgB(
                    &device->ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_DEVICE |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    SET_CONFIGURATION,                    /* bRequest      */
                    configValue,                          /* wValue        */
                    0,                                    /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */

  if ( retVal == USB_STATUS_OK )
  {
    // Configurations are mutually exclusive within the device

    // Should:
    // Deactivate ep's in previous configuration
    // Activate ep's in new configuration

    for ( i=0; i<device->numEp; i++ )           /* Reset all data toggles */
    {
      device->ep[i].toggle = USB_PID_DATA0;
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Set an endpoint in the stalled (halted) state.
 *
 * @note
 *   This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_StallEpB( USBH_Ep_TypeDef *ep )
{
  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_StallEpB(), ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  return USBH_ControlMsgB( &ep->parentDevice->ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_ENDPOINT |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    SET_FEATURE,                          /* bRequest      */
                    USB_FEATURE_ENDPOINT_HALT,            /* wValue        */
                    ep->addr,                             /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */
}

/***************************************************************************//**
 * @brief
 *   Stop USB host operation.
 *
 * @details
 *   USB host operation is terminated and VBUS on the port is turned off.
 ******************************************************************************/
void USBH_Stop( void )
{
  int i;

  USBTIMER_Stop( HOSTPORT_TIMER_INDEX );
  for ( i = 0; i < NUM_HC_USED + 2; i++ )
  {
    USBTIMER_Stop( i + HOSTCH_TIMER_INDEX );
  }
  INT_Disable();
  USBHAL_DisableGlobalInt();
  INT_Enable();
  USBH_PortVbusOn( false );
  USBHAL_DisablePhyPins();
  /* Turn off USB clocks. */
  CMU->HFCORECLKEN0 &= ~(CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC);
}

/***************************************************************************//**
 * @brief
 *   Reset stall state on a stalled (halted) endpoint.
 *
 * @note
 *   This function is blocking and will not return before the transfer
 *   has completed, timed out (1 second) or failed.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_UnStallEpB( USBH_Ep_TypeDef *ep )
{
  int retVal;

  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_UnStallEpB(), ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_ControlMsgB( &ep->parentDevice->ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_ENDPOINT |
                    USB_SETUP_TYPE_STANDARD_MASK,         /* bmRequestType */
                    CLEAR_FEATURE,                        /* bRequest      */
                    USB_FEATURE_ENDPOINT_HALT,            /* wValue        */
                    ep->addr,                             /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    DEFAULT_CTRL_TIMEOUT );               /* int timeout   */

  if ( retVal == USB_STATUS_OK )
  {
    ep->toggle = USB_PID_DATA0;
  }

  return retVal;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void InitUsb( void )
{
  int i;

  memset( hcs, 0, sizeof( hcs ) );
  for ( i = 0; i < NUM_HC_USED + 2; i++ )
  {
    hcs[ i ].idle = true;
  }

  INT_Disable();
  USBHHAL_CoreInit( USBH_initData.rxFifoSize, USBH_initData.nptxFifoSize,
                    USBH_initData.ptxFifoSize );

  NVIC_ClearPendingIRQ( USB_IRQn );
  INT_Enable();
}

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Wait for device connection.
 *
 * @details
 *   This function will wait for device connection and try to read the 8 first
 *   bytes of the device descriptor.
 *   @n First the USB peripheral is initializet (reset) and VBUS is turned on.
 *   When a device is connected, an USB reset will be signalled on the USB port,
 *   and then a USB GetDescriptor command is performed.
 *   @n This procedure is repeated until success or timeout. On each iteration
 *   the duration of USB reset signalling is varied.
 *
 * @param[in] buf
 *   A data buffer with sufficent space for retrieving the first 8 bytes of
 *   a device descriptor. The data buffer size must be
 *   sizeof( @ref USBH_Device_TypeDef ) + 8.
 *
 * @param[in] timeoutInSeconds
 *   Timeout in seconds. A value of 0 means infinite.
 *
 * @return
 *   Returns @ref USB_STATUS_OK on success, @ref USB_STATUS_TIMEOUT on
 *   timeout, @ref USB_STATUS_DEVICE_MALFUNCTION if a device was attached
 *   but USB communications could not be succesfully established with the device,
 *   or @ref USB_STATUS_PORT_OVERCURRENT if a VBUS overcurrent condition exist.
 ******************************************************************************/
int USBH_WaitForDeviceConnectionB( uint8_t *buf, int timeoutInSeconds )
{
  bool xferError;
  USBH_Device_TypeDef *device;
  int accumulatedTime, deadLine;

  if ( buf == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_WaitForDeviceConnectionB(),"
                        " buf NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  xferError = false;
  accumulatedTime = 0;
  USBH_attachRetryCount = 0;
  deadLine = timeoutInSeconds * 1000;
  device = (USBH_Device_TypeDef*)buf;

  while ( 1 )
  {
    /* If USB is enabled, turn it off. */
    if ( USBHHAL_InitializedAndPowered() )
    {
      USBH_PortVbusOn( false );
      if ( deadLine )
        accumulatedTime += PORT_VBUS_DELAY;
    }

    /* Reset and init USB core. */
    InitUsb();
    if ( deadLine )
      accumulatedTime += 50;

    /* Turn on VBUS. */
    USBH_PortVbusOn( true );
    DEBUG_USB_INT_LO_PUTCHAR( '1' );
    if ( deadLine )
      accumulatedTime += PORT_VBUS_DELAY;

    /* Enable interrupts from USB core. */
    INT_Disable();
    USBHHAL_EnableInts();
    USBHAL_EnableGlobalInt();
    INT_Enable();

    /* Wait for device connection. */
    while ( 1 )
    {
      if ( USBH_DeviceConnected() )
        break;

      if ( ( deadLine                    ) &&
           ( accumulatedTime >= deadLine )    )
      {
        return USB_STATUS_TIMEOUT;
      }

#if ( USB_VBUSOVRCUR_PORT != USB_VBUSOVRCUR_PORT_NONE )
      if ( GPIO_PinInGet( USB_VBUSOVRCUR_PORT, USB_VBUSOVRCUR_PIN ) ==
           USB_VBUSOVRCUR_POLARITY )
      {
        DEBUG_USB_INT_LO_PUTCHAR( '~' );
        USBHHAL_PortReset( false );
        USBHHAL_VbusOn( false );
        USBTIMER_Stop( HOSTPORT_TIMER_INDEX );
        USBH_portStatus = H_PORT_OVERCURRENT;
        return USB_STATUS_PORT_OVERCURRENT;
      }
#endif
      USBTIMER_DelayMs( 50 );
      if ( deadLine )
        accumulatedTime += 50;
    }

    /* Reset recovery time. */
    USBTIMER_DelayMs( 10 );
    if ( deadLine )
      accumulatedTime += 10;

    /* Do one USB transfer to check if device connection is OK. */

    USBH_InitDeviceData( device, NULL, NULL, 0, USBH_GetPortSpeed() );

    /* Get the first 8 bytes of the device descriptor */
    if ( 8 == USBH_GetDeviceDescriptorB( device, &device->devDesc, 8  ) )
    {
      return USB_STATUS_OK;
    }
    else
    {
      xferError = true;
      if ( deadLine )
        accumulatedTime += DEFAULT_CTRL_TIMEOUT;
    }

    /* Disable USB, power down VBUS. */
    USBH_Stop();
    if ( deadLine )
      accumulatedTime += PORT_VBUS_DELAY;

    if ( ( deadLine                    ) &&
         ( accumulatedTime >= deadLine )    )
      break;

    USBH_attachRetryCount = ( USBH_attachRetryCount + 1 ) %
                            ( sizeof( USBH_attachTiming ) /
                              sizeof( USBH_AttachTiming_TypeDef ) );
  }

  if ( xferError )
    return USB_STATUS_DEVICE_MALFUNCTION;

  return USB_STATUS_TIMEOUT;
}

/***************************************************************************//**
 * @brief
 *   Write data to device endpoint, non-blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   @n This function is non-blocking and returns immediately.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] byteCount
 *   Number of bytes to transfer (zero is a valid value).
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @param[in] callback
 *   Function to be called on transfer completion. Supply NULL if no callback
 *   is needed. See @ref USB_XferCompleteCb_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBH_Write( USBH_Ep_TypeDef *ep, void *data, int byteCount,
                int timeout, USB_XferCompleteCb_TypeDef callback )
{
  USBH_Hc_TypeDef *hc = &hcs[ ep->hcOut ];

  if ( ep == NULL )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), ep NULL pointer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( !USBH_DeviceConnected() )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), No device connected" );
    return USB_STATUS_ILLEGAL;
  }

  if ( (   byteCount > MAX_XFER_LEN                           ) ||
       ( ( byteCount / ep->packetSize ) > MAX_PACKETS_PR_XFER )    )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), Illegal transfer size" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( (uint32_t)data & 3 )
  {
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), Misaligned data buffer" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  INT_Disable();
  if ( ep->state != H_EP_IDLE )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), Endpoint is busy" );
    return USB_STATUS_EP_BUSY;
  }

  if ( ep->in )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), Illegal EP direction" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  if ( !hc->idle )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_Write(), Host channel is busy" );
    return USB_STATUS_HC_BUSY;
  }

  ep->xferCompleted  = false;
  ep->state          = H_EP_DATA_OUT;
  ep->xferCompleteCb = callback;
  ep->buf            = data;
  ep->remaining      = byteCount;
  ep->xferred        = 0;
  ep->timeout        = timeout;

  hc->buf = data;
  hc->xferred = 0;
  hc->remaining = byteCount;
  hc->ep = ep;

  if ( ep->type == USB_EPTYPE_INTR )
  {
    if ( timeout )
      timeout = EFM32_MIN( timeout, ep->epDesc.bInterval );
    else
      timeout = ep->epDesc.bInterval;
  }

  if ( timeout )
  {
    USBTIMER_Start( ep->hcOut + HOSTCH_TIMER_INDEX,
                    timeout, hcTimeoutFunc[ ep->hcOut ] );
  }

  USBHHAL_HCInit( ep->hcOut );
  USBHHAL_HCStart( ep->hcOut );

  INT_Enable();

  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Write data to device endpoint, blocking version.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   @n This function is blocking and will not return before the transfer
 *   has completed, timed out or failed.
 *
 * @param[in] ep
 *   Pointer to a @ref USBH_Ep_TypeDef data structure.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] byteCount
 *   Number of bytes to transfer (zero is a valid value).
 *
 * @param[in] timeout
 *   Transfer timeout in milliseconds. The transfer will be terminated if not
 *   completed within @em timeout milliseconds. A value of 0 means
 *   infinite.
 *
 * @return
 *   A positive (or zero) value indicates number of bytes transferred.
 *   @n A negative value indicates a transfer error code enumerated in
 *   @ref USB_Status_TypeDef.
 ******************************************************************************/
int USBH_WriteB( USBH_Ep_TypeDef *ep, void *data, int byteCount, int timeout )
{
  int retVal;

  if ( INT_Disable() > 1 )
  {
    INT_Enable();
    DEBUG_USB_API_PUTS( "\nUSBH_WriteB() called with int's disabled" );
    EFM_ASSERT( false );
    return USB_STATUS_ILLEGAL;
  }

  retVal = USBH_Write( ep, data, byteCount, timeout, NULL );
  INT_Enable();

  if ( retVal == USB_STATUS_OK )
  {
    while ( ! ep->xferCompleted );

    if ( ( retVal = ep->xferStatus ) == USB_STATUS_OK )
    {
      retVal = ep->xferred;
    }
  }

  return retVal;
}

/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @{

@page usb_host USB host stack library

  The source files for the USB host stack resides in the usb directory
  and follows the naming convention: em_usbh<em>nnn</em>.c/h.

  @li @ref usb_host_intro
  @li @ref usb_host_getting_started
  @li @ref usb_host_api
  @li @ref usb_host_conf


@n @section usb_host_intro Introduction

  The USB host protocol stack provides an API which makes it possible to
  create USB hosts with a minimum of effort. The host stack supports control,
  bulk and interrupt transfers.

  The stack is highly configurable to suit various needs, it does also contain
  useful debugging features together with several demonstration projects to
  get you started fast.

  We recommend that you read through this documentation, then proceed to build
  and test a few example projects before you start designing your own USB host
  applications.

@n @section usb_host_getting_started Getting started

  To use an USB device, its pratical to divide the initial steps needed into :
  @li @ref usb_device_connection
  @li @ref usb_device_enumeration

  @n @subsection usb_device_connection Device connection

    This framework can be used to establish a device connection.
    @verbatim

// Initialize USB host stack
USBH_Init( &is );

for (;;)
{
  // Wait for ever on device attachment

  // The second parameter is timeout in seconds, 0 means for ever
  if ( USBH_WaitForDeviceConnectionB( tmpBuf, 0 ) == USB_STATUS_OK )
  {

    // Device is now connected and ready for enumeration !

  }

  // Wait for disconnection
  while ( USBH_DeviceConnected() ){}

  // Disable USB peripheral, power down USB port.
  USBH_Stop();
}@endverbatim

  @n @subsection usb_device_enumeration Device enumeration and configuration

    This framework can be used to enumerate and activate the device.
    @verbatim

  // Enumerate device, retrieve device and configuration descriptors from device
  USBH_QueryDeviceB( tmpBuf, sizeof( tmpBuf ), USBH_GetPortSpeed() );

  // Qualify the device
  if ( ( USBH_QGetDeviceDescriptor( tmpBuf )->idVendor  == 0x10C4               ) &&
       ( USBH_QGetDeviceDescriptor( tmpBuf )->idProduct == 0x0001               ) &&
       ( USBH_QGetDeviceDescriptor( tmpBuf )->bNumConfigurations == 1           ) &&
       ( USBH_QGetConfigurationDescriptor( tmpBuf, 0 )->bNumInterfaces  == 1    ) &&
       ( USBH_QGetInterfaceDescriptor(  tmpBuf, 0, 0 )->bInterfaceClass == 0xFF ) &&
       ( USBH_QGetInterfaceDescriptor(  tmpBuf, 0, 0 )->bNumEndpoints   == 2    )    )
  {
    // After having determined that the device is "our" device, it's time to
    // give it an USB address and activate the configuration.

    // Populate device and endpoint data structures with
    // data retrieved during enumeration.
    USBH_InitDeviceData( &device, tmpBuf, ep, 2, USBH_GetPortSpeed() );

    USBH_SetAddressB( &device, DEV_ADDR );
    USBH_SetConfigurationB( &device, device.confDesc.bConfigurationValue );

    // Assign host channels to device endpoints
    USBH_AssignHostChannel( &ep[ 0 ], 2 );
    USBH_AssignHostChannel( &ep[ 1 ], 3 );

    // We are now ready to use the device !

  }@endverbatim

@n @section usb_host_api The host stack API

  @li @ref usb_host_apiintro
  @li @ref usb_host_init
  @li @ref usb_host_transfer
  @li @ref usb_host_ch9
  @li @ref usb_host_port
  @li @ref usb_host_enum
  @li @ref usb_host_print

  @n @subsection usb_host_apiintro Introduction
    This section contains brief descriptions of all functions in the API. You will
    find detailed information on input and output parameters and return values by
    clicking on the hyperlinked function names. It is also a good idea to study
    the code in the USB demonstration projects.

    Your application code must include one header file: @em em_usb.h.

    The functions in the API come in two flavours, they are either blocking or
    non-blocking. The blocking functions have an uppercase letter B at the end
    of the function name. Blocking functions can not be called when interrupts
    are disabled. Note that all API callback functions are called from within
    the USB peripheral interrupt handler with interrupts disabled.

    The USB stack use a hardware timer to keep track of time. TIMER0 is the
    default choice, refer to @ref usb_host_conf for other possibilities.
    Your application must not use the selected timer.

    <b>Pitfalls:</b>@n
      An USB peripheral will fill host receive buffers in quantities of WORD's
      (4 bytes). When allocating storage for receive buffers, round size up to
      next WORD boundary. If it is possible that a device will send more data
      than host expects, round buffer size up to the next multiple of
      maxpacket size for the relevant endpoint to avoid buffer overflow. @n
      Transmit and receive buffers must also be WORD aligned. Macros are available
      for allocating buffers, see @ref UBUF and @ref STATIC_UBUF.

      Transmit buffers passed to non-blocking transfer functions must be
      statically allocated because these functions do not have their own buffers.
      The data in the transmit buffers must be valid until the transfer completes,
      times out or fails.

  @n @subsection usb_host_init Top level control functions
    @ref USBH_Init() @n
      Initial host stack initialization, call once in start of main().

    @ref USBH_Stop() @n
      Terminates host operation, turns off VBUS.

    @ref USBH_WaitForDeviceConnectionB() @n
      Wait for device connection with optional timeout.

    @ref USBH_AssignHostChannel() @n
      Associate a device endpoint with a host channel.

  @n @subsection usb_host_transfer USB transfer functions
    @ref USBH_ControlMsg(), @ref USBH_ControlMsgB() @n
      Perform a non-blocking or blocking USB control message transfer.

    @ref USBH_Read(), @ref USBH_ReadB() @n
      Perform a non-blocking or blocking USB IN data transfer. Data direction is
      @em from device @em to host.

    @ref USBH_Write(), @ref USBH_WriteB() @n
      Perform a non-blocking or blocking USB OUT data transfer. Data direction is
      @em from host @em to device.

  @n @subsection usb_host_ch9 USB Chapter 9 support functions
    All Chapter 9 support functions are blocking with a timeout of 1 second.

    @ref USBH_GetConfigurationDescriptorB() @n
      Read a configuration descriptor from a device.

    @ref USBH_GetDeviceDescriptorB() @n
      Read a device descriptor from a device.

    @ref USBH_GetStringB() @n
      Read a string descriptor from a device.

    @ref USBH_SetAddressB() @n
      Set new USB device address on device currently on USB address 0.

    @ref USBH_SetAltInterfaceB() @n
      Set alternate interface on a device.

    @ref USBH_SetConfigurationB() @n
      Set device configuration.

    @ref USBH_StallEpB(), @ref USBH_UnStallEpB() @n
      These functions stalls or un-stalls an endpoint. Uses USB standard
      requests SET_FEATURE and CLEAR_FEATURE.

  @n @subsection usb_host_port Host port control functions

    @ref USBH_DeviceConnected() @n
      Check if a device is connected on the USB host port.

    @ref USBH_GetPortSpeed() @n
      Get the bus speed (low speed or full speed) of the device currently
      connected to the USB host port.

    @ref USBH_PortReset() @n
      Drive reset signalling on the USB host port.

    @ref USBH_PortResume() @n
      Drive resume signalling on the USB host port.

    @ref USBH_PortSuspend() @n
      Set the USB host port in suspend mode.

  @n @subsection usb_host_enum Enumeration and query functions
      @ref USBH_QueryDeviceB() @n
        This function will read the device and configuration descriptors
        from a device at USB address 0. The application must allocate a buffer
        of sufficent size to hold the data. This data buffer can later be
        used by all USBH_Q<em>xxx</em> functions to retrieve pointers to
        any descriptor within any configuration descriptor. Data retrieved
        by this function must also be passed to @htmlonly USBH_InitDeviceData()
        @endhtmlonly before normal device communication can start.
        Ref. section @ref usb_device_enumeration.

      @ref USBH_QGetConfigurationDescriptor() @n
        Get a pointer to a given configuration descriptor. Parses through a
        data buffer which must have been previously populated by a call to
        @htmlonly USBH_QueryDeviceB() @endhtmlonly.

      @ref USBH_QGetDeviceDescriptor() @n
        Get a pointer to the device descriptor. Parses through a data buffer
        which must have been previously populated by a call to @htmlonly
        USBH_QueryDeviceB() @endhtmlonly.

      @ref USBH_QGetEndpointDescriptor() @n
        Get a pointer to a given endpoint descriptor within a given interface
        within a given configuration. Parses through a data buffer which must
        have been previously populated by a call to @htmlonly USBH_QueryDeviceB().
        @endhtmlonly

      @ref USBH_QGetInterfaceDescriptor() @n
        Get a pointer to an interface descriptor within a given configuration.
        Parses through a data buffer which must have been previously populated
        by a call to @htmlonly USBH_QueryDeviceB() @endhtmlonly.

      @ref USBH_InitDeviceData() @n
        Populates device and endpoint data structures with data which must
        have been retrieved from a device by a call to
        @htmlonly USBH_QueryDeviceB() @endhtmlonly.
        The application must allocate and provide device and endpoint data
        structures to the host stack.
        After this function is called the device and endpoint data structures
        can be used as parameters (@em handles) to other API functions as needed.

  @n @subsection usb_host_print Utility functions

      @ref USBH_PrintString() @n
        Print an USB string descriptor on the debug serial port with optional leader
        and trailer strings.

      @ref USBH_PrintConfigurationDescriptor(),
      @ref USBH_PrintDeviceDescriptor(),
      @ref USBH_PrintEndpointDescriptor(),
      @ref USBH_PrintInterfaceDescriptor() @n
        Pretty print descriptors on the debug serial port.

      USB_PUTCHAR() @n Transmit a single char on the debug serial port.

      USB_PUTS() @n Transmit a zero terminated string on the debug serial port.

      USB_PRINTF() @n Transmit "printf" formated data on the debug serial port.

      USB_GetErrorMsgString() @n Return an error message string for a given
      error code.

      USB_PrintErrorMsgString() @n Format and print a text string given an
      error code, prepends an optional user supplied leader string.

      USBTIMER_DelayMs() @n Active wait millisecond delay function. Can also be
      used inside interrupt handlers.

      USBTIMER_DelayUs() @n Active wait microsecond delay function. Can also be
      used inside interrupt handlers.

      USBTIMER_Init() @n Initialize the timer system. Called by @htmlonly
      USBH_Init(), @endhtmlonly but your application must call it again to
      reinitialize whenever you change the HFPERCLK frequency.

      USBTIMER_Start() @n Start a timer. You can configure the USB device stack
      to provide any number of timers. The timers have 1 ms resolution, your
      application is notified of timeout by means of a callback.

      USBTIMER_Stop() @n Stop a timer.

@n @section usb_host_conf Configuring the host stack

  Your application must provide a header file named @em usbconfig.h. This file
  must contain the following \#define's:@n @n
  @verbatim
#define USB_HOST         // Compile the stack for host mode.
#define NUM_HC_USED n    // Your application use 'n' host channels in addition
                         // to channels 0 and 1 which are assigned by the
                         // host stack for device endpoint 0 communication. @endverbatim

  @n @em usbconfig.h may define the following items: @n @n
  @verbatim
#define NUM_APP_TIMERS n // Your application needs 'n' timers

#define DEBUG_USB_API    // Turn on API debug diagnostics.

// Some utility functions in the API needs printf. These
// functions have "print" in their name. This macro enables
// these functions.
#define USB_USE_PRINTF   // Enable utility print functions.

// Define a function for transmitting a single char on the serial port.
extern int RETARGET_WriteChar(char c);
#define USER_PUTCHAR  RETARGET_WriteChar

#define USB_TIMER USB_TIMERn  // Select which hardware timer the USB stack
                              // is allowed to use. Valid values are n=0,1,2...
                              // corresponding to TIMER0, TIMER1, ...
                              // If not specified, TIMER0 is used @endverbatim

  @n You are strongly encouraged to start application development with DEBUG_USB_API
  turned on. When DEBUG_USB_API is turned on and USER_PUTCHAR is defined, useful
  debugging information will be output on the development kit serial port.
  Compiling with the DEBUG_EFM_USER flag will also enable all asserts
  in both @em emlib and in the USB stack. If asserts are enabled and
  USER_PUTCHAR defined, assert texts will be output on the serial port.

  You application must include @em retargetserial.c if DEBUG_USB_API is defined
  and @em retargetio.c if USB_USE_PRINTF is defined.
  These files reside in the @em drivers
  directory in the software package for your development board.

  @n The host stack can be configured to monitor a GPIO input pin for detection
  of VBUS overcurrent or short circuit conditions. The stack will by default
  use pin 2 on PortE and low polarity for this purpose.
  Override by using the following three \#define's:
  @n @n @verbatim
#define USB_VBUSOVRCUR_PORT       gpioPortB       // The port
#define USB_VBUSOVRCUR_PIN        7               // The pin number within the port
#define USB_VBUSOVRCUR_POLARITY   USB_VBUSOVRCUR_POLARITY_LOW @endverbatim

  Select any GPIO port for <b>USB_VBUSOVRCUR_PORT</b> or
  <b>USB_VBUSOVRCUR_PORT_NONE</b> if no overcurrent circuitry in the hw design.
  For <b>USB_VBUSOVRCUR_POLARITY</b> use <b>USB_VBUSOVRCUR_POLARITY_LOW</b>
  or <b>USB_VBUSOVRCUR_POLARITY_HIGH</b>.

 * @}**************************************************************************/

#endif /* defined( USB_HOST ) */
#endif /* defined( USB_PRESENT ) && ( USB_COUNT == 1 ) */
