/***************************************************************************//**
 * @file em_usbhint.c
 * @brief USB protocol stack library, USB host peripheral interrupt handlers.
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

#include "em_usbtypes.h"
#include "em_usbhal.h"
#include "em_usbh.h"
#if ( USB_VBUSOVRCUR_PORT != USB_VBUSOVRCUR_PORT_NONE )
#include "em_gpio.h"
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define HANDLE_INT( x ) if ( status & x ) { Handle_##x(); status &= ~x; }

#define FIFO_TXSTS_HCNUM_MASK  0x78000000
#define FIFO_TXSTS_HCNUM_SHIFT 27

static void Handle_HcInInt(  uint8_t hcnum );
static void Handle_HcOutInt( uint8_t hcnum );
static void Handle_USB_GINTSTS_DISCONNINT ( void );
static void Handle_USB_GINTSTS_HCHINT     ( void );
static void Handle_USB_GINTSTS_PRTINT     ( void );

/*
 * USB_IRQHandler() is the first level handler for the USB peripheral interrupt.
 */
void USB_IRQHandler( void )
{
  uint32_t status;

  INT_Disable();

  status = USBHAL_GetCoreInts();
  if ( status == 0 )
  {
    INT_Enable();
    DEBUG_USB_INT_LO_PUTS( "\nSinT" );
    return;
  }

  HANDLE_INT( USB_GINTSTS_HCHINT     )
  HANDLE_INT( USB_GINTSTS_PRTINT     )
  HANDLE_INT( USB_GINTSTS_DISCONNINT )

  INT_Enable();

  if ( status != 0 )
  {
    DEBUG_USB_INT_LO_PUTS( "\nUinT" );
  }
}

/*
 * Handle host channel IN transfer interrupt.
 */
static void Handle_HcInInt( uint8_t hcnum )
{
  USBH_Hc_TypeDef *hc;
  USB_Status_TypeDef result;
  uint32_t status, hcchar, eptype;
#ifdef DEBUG_USB_INT_HI
  uint32_t status2;
#endif

  hc = &hcs[ hcnum ];
  status = USBHHAL_GetHcInts( hcnum );
  hcchar = USB->HC[ hcnum ].CHAR;
  eptype = hcchar & _USB_HC_CHAR_EPTYPE_MASK;

  DEBUG_USB_INT_HI_PUTCHAR( 'i' );

  if ( status & USB_HC_INT_CHHLTD )
  {
    USB->HC[ hcnum ].INT = 0xFFFFFFFF;

#ifdef DEBUG_USB_INT_HI
    status2 = status;
#endif
    status &= USB_HC_INT_XFERCOMPL | USB_HC_INT_STALL | USB_HC_INT_XACTERR |
              USB_HC_INT_ACK | USB_HC_INT_NAK | USB_HC_INT_DATATGLERR |
              USB_HC_INT_BBLERR;

    if ( ( status & ( USB_HC_INT_ACK | USB_HC_INT_XFERCOMPL ) ) ==
         ( USB_HC_INT_ACK | USB_HC_INT_XFERCOMPL              )    )
    {
      DEBUG_USB_INT_HI_PUTCHAR( 'c' );

      hc->xferred = hc->hwXferSize -
                    ( ( USB->HC[ hcnum ].TSIZ & _USB_HC_TSIZ_XFERSIZE_MASK ) >>
                      _USB_HC_TSIZ_XFERSIZE_SHIFT );

      hc->remaining -= hc->xferred;
      hc->ep->toggle = ( USB->HC[ hcnum ].TSIZ & _USB_HC_TSIZ_PID_MASK ) ==
                       USB_HC_TSIZ_PID_DATA0 ? USB_PID_DATA0 : USB_PID_DATA1;

      result = USB_STATUS_OK;
    }

    else if ( status & USB_HC_INT_STALL )
    {
      result = USB_STATUS_EP_STALLED;
      DEBUG_USB_INT_LO_PUTS( "StaL" );
    }

    else if ( status & USB_HC_INT_BBLERR )
    {
      result = USB_STATUS_EP_ERROR;
      DEBUG_USB_INT_LO_PUTS( "BabL" );
    }

    else if ( ( ( status &
                  ( USB_HC_INT_DATATGLERR | USB_HC_INT_NAK ) )
                == USB_HC_INT_DATATGLERR                       ) &&
              ( !( hc->status & HCS_TIMEOUT )                  )    )
    {
      /* Toggle error but not nak or timeout */
      result = USB_STATUS_EP_ERROR;
      DEBUG_USB_INT_LO_PUTS( "TglE" );

      hc->errorCnt++;
      if ( hc->errorCnt < 3 )
      {
        USBHHAL_HCStart( hcnum );
        return;
      }
    }

    else if ( ( ( status &
                  ( USB_HC_INT_DATATGLERR | USB_HC_INT_NAK |
                    USB_HC_INT_XACTERR )                       )
                == USB_HC_INT_XACTERR                            ) &&
              ( !( hc->status & HCS_TIMEOUT )                    )    )
    {
      /* Exact error but not toggle err or nak or timeout */
      result = USB_STATUS_EP_ERROR;
      DEBUG_USB_INT_LO_PUTS( "XacT" );

      hc->errorCnt++;
      if ( hc->errorCnt < 3 )
      {
        USBHHAL_HCStart( hcnum );
        return;
      }
    }

    else if ( hc->status & HCS_TIMEOUT )
    {
      DEBUG_USB_INT_HI_PUTCHAR( 't' );
      result = USB_STATUS_TIMEOUT;
    }

    else
    {
#ifdef DEBUG_USB_INT_HI
      if ( !( ( eptype == HCCHAR_EPTYPE_INTR                  ) &&
              ( ( status & USB_HC_INT_NAK ) == USB_HC_INT_NAK )    ) )
        {
          USB_PRINTF( "0x%08lX", status2 );
        }
#endif
      return;
    }

    if ( eptype == HCCHAR_EPTYPE_CTRL )
      USBHEP_CtrlEpHandler( hc->ep, result );
    else
      USBHEP_EpHandler( hc->ep, result );
  }
}

/*
 * Handle host channel OUT transfer interrupt.
 */
static void Handle_HcOutInt( uint8_t hcnum )
{
  USBH_Hc_TypeDef *hc;
  USB_Status_TypeDef result;
  uint32_t status, hcchar, eptype;
#ifdef DEBUG_USB_INT_HI
  uint32_t status2;
#endif

  hc = &hcs[ hcnum ];
  status = USBHHAL_GetHcInts( hcnum );
  hcchar = USB->HC[ hcnum ].CHAR;
  eptype = hcchar & _USB_HC_CHAR_EPTYPE_MASK;

  DEBUG_USB_INT_HI_PUTCHAR( 'o' );

  if ( status & USB_HC_INT_CHHLTD )
  {
    USB->HC[ hcnum ].INT = 0xFFFFFFFF;

#ifdef DEBUG_USB_INT_HI
    status2 = status;
#endif
    status &= USB_HC_INT_XFERCOMPL | USB_HC_INT_STALL | USB_HC_INT_XACTERR |
              USB_HC_INT_ACK | USB_HC_INT_NAK;

    if ( ( status & ( USB_HC_INT_ACK | USB_HC_INT_XFERCOMPL ) ) ==
         ( USB_HC_INT_ACK | USB_HC_INT_XFERCOMPL              )    )
    {
      DEBUG_USB_INT_HI_PUTCHAR( 'c' );

      hc->xferred   = hc->remaining;
      hc->remaining = 0;
      hc->ep->toggle = ( USB->HC[ hcnum ].TSIZ & _USB_HC_TSIZ_PID_MASK ) ==
                       USB_HC_TSIZ_PID_DATA0 ? USB_PID_DATA0 : USB_PID_DATA1;

      result = USB_STATUS_OK;
    }

    else if ( status & USB_HC_INT_STALL )
    {
      result = USB_STATUS_EP_STALLED;
      DEBUG_USB_INT_LO_PUTS( "StaL" );
    }

    else if ( status & USB_HC_INT_XACTERR )
    {
      DEBUG_USB_INT_LO_PUTS( "XacT" );
      if ( status & ( USB_HC_INT_ACK | USB_HC_INT_NAK ) )
      {
        hc->errorCnt = 0;
        USBHHAL_HCStart( hcnum );
        return;
      }
      else
      {
        hc->errorCnt++;
        if ( hc->errorCnt < 3 )
        {
          USBHHAL_HCStart( hcnum );
          return;
        }
      }
      result = USB_STATUS_EP_ERROR;
    }

    else if ( hc->status & HCS_TIMEOUT )
    {
      DEBUG_USB_INT_HI_PUTCHAR( 't' );
      result = USB_STATUS_TIMEOUT;
    }

    else
    {
#ifdef DEBUG_USB_INT_HI
      if ( !( ( eptype == HCCHAR_EPTYPE_INTR                  ) &&
              ( ( status & USB_HC_INT_NAK ) == USB_HC_INT_NAK )    ) )
        {
          USB_PRINTF( "0x%08lX", status2 );
        }
#endif
      return;
    }

    if ( eptype == HCCHAR_EPTYPE_CTRL )
      USBHEP_CtrlEpHandler( hc->ep, result );
    else
      USBHEP_EpHandler( hc->ep, result );
  }
}

/*
 * Handle port disconnect interrupt.
 */
static void Handle_USB_GINTSTS_DISCONNINT( void )
{
  int i;
  uint32_t hcchar;

  USB->GINTSTS = USB_GINTSTS_DISCONNINT;
  USB->HAINTMSK = 0;

  USBH_portStatus = H_PORT_DISCONNECTED;
  USBTIMER_Stop( HOSTPORT_TIMER_INDEX );
  USBHHAL_PortReset( false );

  for ( i=0; i< NUM_HC_USED + 2; i++ )
  {
    hcchar = USB->HC[ i ].CHAR;                 /* Halt channel             */
    USBHHAL_HCHalt( i, hcchar );
    USB->HC[ i ].INT = 0xFFFFFFFF;              /* Clear pending interrupts */

    if ( !hcs[ i ].idle )
    {
      USBHEP_TransferDone( hcs[ i ].ep, USB_STATUS_DEVICE_REMOVED );
    }
  }

  DEBUG_USB_INT_LO_PUTS( "\nDisC" );
}

/*
 * Handle host channel interrupt. Call IN and OUT transfer handlers as needed.
 */
static void Handle_USB_GINTSTS_HCHINT( void )
{
  uint8_t hcnum;
  uint32_t hcints, hcmask;

  hcints = USBHHAL_GetHostChannelInts();

  for ( hcnum = 0,               hcmask = 1;
        hcnum < NUM_HC_USED + 2;
        hcnum++,                 hcmask <<= 1 )
  {
    if ( hcints & hcmask )
    {
      if ( USB->HC[ hcnum ].CHAR & USB_HC_CHAR_EPDIR )
      {
        Handle_HcInInt( hcnum );
      }
      else
      {
        Handle_HcOutInt( hcnum );
      }
    }
  }
}

/*
 * Callback function for port interrupt state machine.
 * Called on timeout of the port timer when a port reset is completed.
 */
static void PortResetComplete( void )
{
  if ( USB->HPRT & USB_HPRT_PRTCONNSTS ) /* Is device still connected ? */
  {
    DEBUG_USB_INT_LO_PUTCHAR( '5' );
  }
  else
  {
    USBH_portStatus = H_PORT_DISCONNECTED;
  }
  USBHHAL_PortReset( false );
}

/*
 * Callback function for port interrupt state machine.
 * Called on timeout of the port timer connection debounce time has expired.
 */
static void PortDebounceComplete( void )
{
  uint32_t hprt;

  hprt = USB->HPRT;                   /* Get port status */

  if ( hprt & USB_HPRT_PRTCONNSTS )   /* Is device still connected ? */
  {
    if ( ( hprt & _USB_HPRT_PRTSPD_MASK ) == HPRT_L_SPEED )
    {
      DEBUG_USB_INT_LO_PUTCHAR( '3' );
      USB->HFIR = 6000;
      /* Set 6 MHz PHY clock */
      USB->HCFG = ( USB->HCFG & ~_USB_HCFG_FSLSPCLKSEL_MASK ) |
                  ( 2 << _USB_HCFG_FSLSPCLKSEL_SHIFT        );
    }
    else if ( ( hprt & _USB_HPRT_PRTSPD_MASK ) == HPRT_F_SPEED )
    {
      DEBUG_USB_INT_LO_PUTCHAR( '4' );
      USB->HFIR = 48000;
      /* Set 48 MHz PHY clock */
      USB->HCFG = ( USB->HCFG & ~_USB_HCFG_FSLSPCLKSEL_MASK ) |
                  ( 1 << _USB_HCFG_FSLSPCLKSEL_SHIFT        );
    }

    USBH_portStatus = H_PORT_CONNECTED_RESETTING;
    USBTIMER_Start( HOSTPORT_TIMER_INDEX,
                    USBH_attachTiming[ USBH_attachRetryCount ].resetTime,
                    PortResetComplete );
    USBHHAL_PortReset( true );
  }
  else
  {
    USBH_portStatus = H_PORT_DISCONNECTED;
  }
}

/*
 * Handle port interrupt.
 */
static void Handle_USB_GINTSTS_PRTINT( void )
{
  uint32_t hprt;

  hprt = USB->HPRT;                               /* Get port status */

  DEBUG_USB_INT_LO_PUTCHAR( '^' );

  switch ( USBH_portStatus )
  {
    case H_PORT_DISCONNECTED:
    /***********************/
      if ( ( hprt & USB_HPRT_PRTCONNDET ) &&
           ( hprt & USB_HPRT_PRTCONNSTS )    )    /* Any device connected ? */
      {
        DEBUG_USB_INT_LO_PUTCHAR( '2' );
        USBH_portStatus = H_PORT_CONNECTED_DEBOUNCING;
        USBTIMER_Start( HOSTPORT_TIMER_INDEX,
                        USBH_attachTiming[ USBH_attachRetryCount ].debounceTime,
                        PortDebounceComplete );
      }
      break;

    case H_PORT_CONNECTED_DEBOUNCING:
    /***********************/
      DEBUG_USB_INT_LO_PUTCHAR( 'Y' );
      break;

    case H_PORT_CONNECTED_RESETTING:
    /***********************/
      if ( ( hprt & USB_HPRT_PRTENCHNG  ) &&    /* Port enable changed ?    */
           ( hprt & USB_HPRT_PRTENA     ) &&    /* Port enabled ?           */
           ( hprt & USB_HPRT_PRTCONNSTS )    )  /* Device still connected ? */
      {
        DEBUG_USB_INT_LO_PUTCHAR( '6' );
        USBH_portStatus = H_PORT_CONNECTED;
      }
      break;

    case H_PORT_CONNECTED:
    /***********************/
      if ( (    hprt & USB_HPRT_PRTENCHNG ) &&
           ( !( hprt & USB_HPRT_PRTENA )  )    )
      {
        DEBUG_USB_INT_LO_PUTCHAR( 'X' );
#if ( USB_VBUSOVRCUR_PORT != USB_VBUSOVRCUR_PORT_NONE )
        if ( GPIO_PinInGet( USB_VBUSOVRCUR_PORT, USB_VBUSOVRCUR_PIN ) ==
             USB_VBUSOVRCUR_POLARITY )
        {
          DEBUG_USB_INT_LO_PUTCHAR( '~' );
          USBHHAL_PortReset( false );
          USBHHAL_VbusOn( false );
          USBTIMER_Stop( HOSTPORT_TIMER_INDEX );
          USBH_portStatus = H_PORT_OVERCURRENT;
        }
#endif
      }
      break;

    case H_PORT_OVERCURRENT:
    /***********************/
      break;
  }

  if ( hprt & USB_HPRT_PRTOVRCURRCHNG ) /* Overcurrent change interrupt ? */
  {
    DEBUG_USB_INT_LO_PUTCHAR( '9' );
  }

  hprt &= ~HPRT_WC_MASK;              /* Mask off all write clear bits  */
  hprt |= USB_HPRT_PRTCONNDET | USB_HPRT_PRTENCHNG | USB_HPRT_PRTOVRCURRCHNG;
  USB->HPRT = hprt;                   /* Clear all port interrupt flags */
}

/** @endcond */

#endif /* defined( USB_HOST ) */
#endif /* defined( USB_PRESENT ) && ( USB_COUNT == 1 ) */
