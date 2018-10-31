/**************************************************************************//**
 * @file em_usbdint.c
 * @brief USB protocol stack library, USB device peripheral interrupt handlers.
 * @version 3.20.7
 ******************************************************************************
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
#if defined( USB_DEVICE )

#include "em_cmu.h"
#include "em_usbtypes.h"
#include "em_usbhal.h"
#include "em_usbd.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define HANDLE_INT( x ) if ( status & x ) { Handle_##x(); status &= ~x; }

static void Handle_USB_GINTSTS_ENUMDONE  ( void );
static void Handle_USB_GINTSTS_IEPINT    ( void );
static void Handle_USB_GINTSTS_OEPINT    ( void );
static void Handle_USB_GINTSTS_RESETDET  ( void );
static void Handle_USB_GINTSTS_SOF       ( void );
static void Handle_USB_GINTSTS_USBRST    ( void );
static void Handle_USB_GINTSTS_USBSUSP   ( void );
static void Handle_USB_GINTSTS_WKUPINT   ( void );

#if ( USB_PWRSAVE_MODE )
/* Variables and prototypes for USB powerdown (suspend) functionality. */
volatile bool USBD_poweredDown = false;
static bool UsbPowerDown( void );
static bool UsbPowerUp(   void );

/* Storage for backing up USB core registers. */
static uint32_t  x_USB_GINTMSK;
static uint32_t  x_USB_GOTGCTL;
static uint32_t  x_USB_GAHBCFG;
static uint32_t  x_USB_GUSBCFG;
static uint32_t  x_USB_GRXFSIZ;
static uint32_t  x_USB_GNPTXFSIZ;
static uint32_t  x_USB_DCFG;
static uint32_t  x_USB_DCTL;
static uint32_t  x_USB_DAINTMSK;
static uint32_t  x_USB_DIEPMSK;
static uint32_t  x_USB_DOEPMSK;
static uint32_t  x_USB_PCGCCTL;

#if ( NUM_EP_USED > 0 )
static uint32_t  x_USB_EP_CTL[ NUM_EP_USED ];
static uint32_t  x_USB_EP_TSIZ[ NUM_EP_USED ];
static uint32_t  x_USB_EP_DMAADDR[ NUM_EP_USED ];
#endif

#if ( NUM_EP_USED > MAX_NUM_TX_FIFOS )
#define FIFO_CNT MAX_NUM_TX_FIFOS
#else
#define FIFO_CNT NUM_EP_USED
#endif

#if ( FIFO_CNT > 0 )
static uint32_t  x_USB_DIEPTXFS[ FIFO_CNT ];
#endif

#endif /* if ( USB_PWRSAVE_MODE ) */

/*
 * USB_IRQHandler() is the first level handler for the USB peripheral interrupt.
 */
void USB_IRQHandler( void )
{
  uint32_t status;
  bool servedVbusInterrupt = false;

  INT_Disable();

#if ( USB_PWRSAVE_MODE )
  if ( USBD_poweredDown )
  {
    /* Switch USBC clock from 32kHz to HFCLK to be able to read USB */
    /* peripheral registers.                                        */
    /* If we woke up from EM2, HFCLK is now HFRCO.                  */

    CMU_OscillatorEnable( cmuOsc_HFXO, true, false);  /* Prepare HFXO. */
    CMU->CMD = CMU_CMD_USBCCLKSEL_HFCLKNODIV;
    while ( !( CMU->STATUS & CMU_STATUS_USBCHFCLKSEL ) ){}
  }
#endif /* if ( USB_PWRSAVE_MODE ) */

  if ( USB->IF && ( USB->CTRL & USB_CTRL_VREGOSEN ) )
  {
    if ( USB->IF & USB_IF_VREGOSH )
    {
      USB->IFC = USB_IFC_VREGOSH;

      if ( USB->STATUS & USB_STATUS_VREGOS )
      {
        servedVbusInterrupt = true;
        DEBUG_USB_INT_LO_PUTS( "\nVboN" );

#if ( USB_PWRSAVE_MODE )
#if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF )
        if ( UsbPowerUp() )
#endif
        {
          USBDHAL_EnableUsbResetAndSuspendInt();
        }

        USBD_SetUsbState( USBD_STATE_POWERED );
#endif
      }
    }

    if ( USB->IF & USB_IF_VREGOSL )
    {
      USB->IFC = USB_IFC_VREGOSL;

      if ( !( USB->STATUS & USB_STATUS_VREGOS ) )
      {
        servedVbusInterrupt = true;
        DEBUG_USB_INT_LO_PUTS( "\nVboF" );

#if ( USB_PWRSAVE_MODE )
        if ( !USBD_poweredDown )
        {
          USB->GINTMSK = 0;
          USB->GINTSTS = 0xFFFFFFFF;
        }

#if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF )
        UsbPowerDown();
#endif
        USBD_SetUsbState( USBD_STATE_NONE );
#endif
      }
    }
  }

  status = USBHAL_GetCoreInts();
  if ( status == 0 )
  {
    INT_Enable();
    if ( !servedVbusInterrupt )
    {
      DEBUG_USB_INT_LO_PUTS( "\nSinT" );
    }
    return;
  }

  HANDLE_INT( USB_GINTSTS_RESETDET   )
  HANDLE_INT( USB_GINTSTS_WKUPINT    )
  HANDLE_INT( USB_GINTSTS_USBSUSP    )
  HANDLE_INT( USB_GINTSTS_SOF        )
  HANDLE_INT( USB_GINTSTS_ENUMDONE   )
  HANDLE_INT( USB_GINTSTS_USBRST     )
  HANDLE_INT( USB_GINTSTS_IEPINT     )
  HANDLE_INT( USB_GINTSTS_OEPINT     )

  INT_Enable();

  if ( status != 0 )
  {
    DEBUG_USB_INT_LO_PUTS( "\nUinT" );
  }
}

/*
 * Handle port enumeration interrupt. This has nothing to do with normal
 * device enumeration.
 */
static void Handle_USB_GINTSTS_ENUMDONE( void )
{
#if ( USB_PWRSAVE_MODE )
  UsbPowerUp();
#endif /* if ( USB_PWRSAVE_MODE ) */

  USBDHAL_Ep0Activate( dev->ep0MpsCode );
  dev->ep[ 0 ].state = D_EP_IDLE;
  USBDHAL_EnableInts( dev );
  DEBUG_USB_INT_LO_PUTS( "EnumD" );
}

/*
 * Handle IN endpoint transfer interrupt.
 */
static void Handle_USB_GINTSTS_IEPINT( void )
{
  int epnum;
  uint16_t epint;
  uint16_t epmask;
  uint32_t status;
  USBD_Ep_TypeDef *ep;

  DEBUG_USB_INT_HI_PUTCHAR( 'i' );

  epint = USBDHAL_GetAllInEpInts();
  for ( epnum = 0,                epmask = 1;
        epnum <= MAX_NUM_IN_EPS;
        epnum++,                  epmask <<= 1 )
  {
    if ( epint & epmask )
    {
      ep = USBD_GetEpFromAddr( USB_SETUP_DIR_MASK | epnum );
      status = USBDHAL_GetInEpInts( ep );

      if ( status & USB_DIEP_INT_XFERCOMPL )
      {
        USB_DINEPS[ epnum ].INT = USB_DIEP_INT_XFERCOMPL;

        DEBUG_USB_INT_HI_PUTCHAR( 'c' );

        if ( epnum == 0 )
        {
          if ( ep->remaining > ep->packetSize )
          {
            ep->remaining -= ep->packetSize;
            ep->xferred += ep->packetSize;
          }
          else
          {
            ep->xferred += ep->remaining;
            ep->remaining = 0;
          }
          USBDEP_Ep0Handler( dev );
        }
        else
        {
          ep->xferred = ep->remaining -
                        ( ( USB_DINEPS[ epnum ].TSIZ      &
                            _USB_DIEP_TSIZ_XFERSIZE_MASK    ) >>
                          _USB_DIEP_TSIZ_XFERSIZE_SHIFT          );
          ep->remaining -= ep->xferred;
          USBDEP_EpHandler( ep->addr );
        }
      }
    }
  }
}

/*
 * Handle OUT endpoint transfer interrupt.
 */
static void Handle_USB_GINTSTS_OEPINT( void )
{
  int epnum;
  uint16_t epint;
  uint16_t epmask;
  uint32_t status;
  USBD_Ep_TypeDef *ep;

  DEBUG_USB_INT_HI_PUTCHAR( 'o' );
  epint = USBDHAL_GetAllOutEpInts();
  for ( epnum = 0,                epmask = 1;
        epnum <= MAX_NUM_OUT_EPS;
        epnum++,                  epmask <<= 1 )
  {
    if ( epint & epmask )
    {
      ep = USBD_GetEpFromAddr( epnum );
      status = USBDHAL_GetOutEpInts( ep );

      if ( status & USB_DOEP_INT_XFERCOMPL )
      {
        USB_DOUTEPS[ epnum ].INT = USB_DOEP_INT_XFERCOMPL;

        DEBUG_USB_INT_HI_PUTCHAR( 'c' );

        if ( epnum == 0 )
        {
          if ( ep->remaining > ep->packetSize )
          {
            ep->remaining -= ep->packetSize;
            ep->xferred += ep->packetSize;
          }
          else
          {
            ep->xferred += ep->remaining;
            ep->remaining = 0;
          }
          USBDEP_Ep0Handler( dev );
        }
        else
        {
          ep->xferred = ep->hwXferSize -
              ( ( USB_DOUTEPS[ epnum ].TSIZ & _USB_DOEP_TSIZ_XFERSIZE_MASK ) >>
                _USB_DOEP_TSIZ_XFERSIZE_SHIFT );
          ep->remaining -= ep->xferred;
          USBDEP_EpHandler( ep->addr );
        }
      }

      /* Setup Phase Done */
      if ( status & USB_DOEP_INT_SETUP )
      {
        DEBUG_USB_INT_LO_PUTS( "\nSP" );

        if ( USB->DOEP0INT & USB_DOEP_INT_BACK2BACKSETUP )
        {                           /* Back to back setup packets received */
          USB->DOEP0INT = USB_DOEP_INT_BACK2BACKSETUP;
          DEBUG_USB_INT_LO_PUTS( "B2B" );

          dev->setup = (USB_Setup_TypeDef*)
                       ( USB->DOEP0DMAADDR - USB_SETUP_PKT_SIZE );
        }
        else
        {
          /* Read SETUP packet counter from hw. */
          int supCnt = ( USB->DOEP0TSIZ & _USB_DOEP0TSIZ_SUPCNT_MASK )
                       >> _USB_DOEP0TSIZ_SUPCNT_SHIFT;

          if ( supCnt == 3 )
            supCnt = 2;

          dev->setup = &dev->setupPkt[ 2 - supCnt ];
        }
        USB->DOEP0TSIZ |= 3 << _USB_DOEP0TSIZ_SUPCNT_SHIFT;
        USB->DOEP0DMAADDR = (uint32_t)dev->setupPkt;
        USB->DOEP0INT = USB_DOEP_INT_SETUP;
        USBDEP_Ep0Handler( dev );   /* Call the SETUP process for the EP0  */
      }
    }
  }
}

/*
 * Handle USB reset detectet interrupt in suspend mode.
 */
static void Handle_USB_GINTSTS_RESETDET  ( void )
{
  USB->GINTSTS = USB_GINTSTS_RESETDET;

#if ( USB_PWRSAVE_MODE )
  UsbPowerUp();

#if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF )
  /* Power down immediately if VBUS is off. */
  if ( ! ( USB->STATUS & USB_STATUS_VREGOS ) )
    UsbPowerDown();
#endif

#endif /* if ( USB_PWRSAVE_MODE ) */

  if ( USB->STATUS & USB_STATUS_VREGOS )
  {
    USBD_SetUsbState( USBD_STATE_DEFAULT );
  }
  else
  {
    USBD_SetUsbState( USBD_STATE_NONE );
  }
  DEBUG_USB_INT_LO_PUTS( "RsuP\n" );
}

/*
 * Handle Start Of Frame (SOF) interrupt.
 */
static void Handle_USB_GINTSTS_SOF( void )
{
  USB->GINTSTS = USB_GINTSTS_SOF;

  if ( dev->callbacks->sofInt )
  {
    dev->callbacks->sofInt(
      ( USB->DSTS & _USB_DSTS_SOFFN_MASK ) >> _USB_DSTS_SOFFN_SHIFT );
  }
}

/*
 * Handle USB port reset interrupt.
 */
static void Handle_USB_GINTSTS_USBRST( void )
{
  int i;

  DEBUG_USB_INT_LO_PUTS( "ReseT" );

  /* Clear Remote Wakeup Signalling */
  USB->DCTL &= ~( DCTL_WO_BITMASK | USB_DCTL_RMTWKUPSIG );
  USBHAL_FlushTxFifo( 0 );

  /* Clear pending interrupts */
  for ( i = 0; i <= MAX_NUM_IN_EPS; i++ )
  {
    USB_DINEPS[ i ].INT = 0xFFFFFFFF;
  }

  for ( i = 0; i <= MAX_NUM_OUT_EPS; i++ )
  {
    USB_DOUTEPS[ i ].INT = 0xFFFFFFFF;
  }

  USB->DAINTMSK = USB_DAINTMSK_INEPMSK0 | USB_DAINTMSK_OUTEPMSK0;
  USB->DOEPMSK  = USB_DOEPMSK_SETUPMSK  | USB_DOEPMSK_XFERCOMPLMSK;
  USB->DIEPMSK  = USB_DIEPMSK_XFERCOMPLMSK;

  /* Reset Device Address */
  USB->DCFG &= ~_USB_DCFG_DEVADDR_MASK;

  /* Setup EP0 to receive SETUP packets */
  USBDHAL_StartEp0Setup( dev );
  USBDHAL_EnableInts( dev );

  if ( dev->callbacks->usbReset )
  {
    dev->callbacks->usbReset();
  }

  USBD_SetUsbState( USBD_STATE_DEFAULT );
  USBDHAL_AbortAllTransfers( USB_STATUS_DEVICE_RESET );
}

/*
 * Handle USB port suspend interrupt.
 */
static void Handle_USB_GINTSTS_USBSUSP( void )
{
  USBD_State_TypeDef state;

  USB->GINTSTS = USB_GINTSTS_USBSUSP;
  USBDHAL_AbortAllTransfers( USB_STATUS_DEVICE_SUSPENDED );
  DEBUG_USB_INT_LO_PUTS( "\nSusP" );

  if ( USBD_GetUsbState() == USBD_STATE_NONE )
  {
    USBD_SetUsbState( USBD_STATE_POWERED );
  }

  state = USBD_GetUsbState();
  if ( ( state == USBD_STATE_POWERED    ) ||
       ( state == USBD_STATE_DEFAULT    ) ||
       ( state == USBD_STATE_ADDRESSED  ) ||
       ( state == USBD_STATE_CONFIGURED )    )
  {
#if ( USB_PWRSAVE_MODE )
    UsbPowerDown();
#endif /* if ( USB_PWRSAVE_MODE ) */
    USBD_SetUsbState( USBD_STATE_SUSPENDED );
  }
}

/*
 * Handle USB port wakeup interrupt.
 */
static void Handle_USB_GINTSTS_WKUPINT( void )
{
  USB->GINTSTS = USB_GINTSTS_WKUPINT;

#if ( USB_PWRSAVE_MODE )
  if ( UsbPowerUp() )
  {
    USBDHAL_StartEp0Setup( dev );
    USBDHAL_Ep0Activate( dev->ep0MpsCode );
  }
#endif /* if ( USB_PWRSAVE_MODE ) */

  USBD_SetUsbState( dev->savedState );
  DEBUG_USB_INT_LO_PUTS( "WkuP\n" );
}

#if ( USB_PWRSAVE_MODE )
/*
 * Backup essential USB core registers, and set the core in partial powerdown
 * mode. Optionally prepare entry into EM2.
 */
static bool UsbPowerDown( void )
{
#if ( NUM_EP_USED > 0 ) || ( FIFO_CNT > 0 )
  int i;
#endif
#if ( NUM_EP_USED > 0 )
  int epNum;
  USBD_Ep_TypeDef *ep;
#endif

  if ( !USBD_poweredDown )
  {
    USBD_poweredDown = true;
    DEBUG_USB_INT_LO_PUTCHAR( '\\' );

    /* Backup USB core registers. */
    x_USB_GINTMSK   = USB->GINTMSK;
    x_USB_GOTGCTL   = USB->GOTGCTL;
    x_USB_GAHBCFG   = USB->GAHBCFG;
    x_USB_GUSBCFG   = USB->GUSBCFG;
    x_USB_GRXFSIZ   = USB->GRXFSIZ;
    x_USB_GNPTXFSIZ = USB->GNPTXFSIZ;
    x_USB_DCFG      = USB->DCFG;
    x_USB_DCTL      = USB->DCTL;
    x_USB_DAINTMSK  = USB->DAINTMSK;
    x_USB_DIEPMSK   = USB->DIEPMSK;
    x_USB_DOEPMSK   = USB->DOEPMSK;
    x_USB_PCGCCTL   = USB->PCGCCTL;

#if ( NUM_EP_USED > 0 )
    for ( i = 0; i < NUM_EP_USED; i++ )
    {
      ep = &dev->ep[ i+1 ];
      epNum = ep->num;
      if ( ep->in )
      {
        x_USB_EP_CTL[     i ] = USB_DINEPS[ epNum ].CTL;
        x_USB_EP_TSIZ[    i ] = USB_DINEPS[ epNum ].TSIZ;
        x_USB_EP_DMAADDR[ i ] = USB_DINEPS[ epNum ].DMAADDR;
      }
      else
      {
        x_USB_EP_CTL[     i ] = USB_DOUTEPS[ epNum ].CTL;
        x_USB_EP_TSIZ[    i ] = USB_DOUTEPS[ epNum ].TSIZ;
        x_USB_EP_DMAADDR[ i ] = USB_DOUTEPS[ epNum ].DMAADDR;
      }
    }
#endif

#if ( FIFO_CNT > 0 )
    for ( i = 0; i < FIFO_CNT; i++ )
    {
      x_USB_DIEPTXFS[ i ] = USB_DIEPTXFS[ i ];
    }
#endif

    /* Prepare for wakeup on resume and reset. */
    USB->DCFG    = (USB->DCFG & ~_USB_DCFG_RESVALID_MASK) |
                   (4 << _USB_DCFG_RESVALID_SHIFT);
    USB->DCFG   |= USB_DCFG_ENA32KHZSUSP;
    USB->GINTMSK = USB_GINTMSK_RESETDETMSK | USB_GINTMSK_WKUPINTMSK;

    /* Enter partial powerdown mode. */
    USB->PCGCCTL |= USB_PCGCCTL_PWRCLMP;
    USB->PCGCCTL |= USB_PCGCCTL_RSTPDWNMODULE;
    USB->PCGCCTL |= USB_PCGCCTL_STOPPCLK;

#if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ENTEREM2 )
    /* Enter EM2 on interrupt exit. */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk;
#endif

    /* Switch USBC clock to 32 kHz. */
#if ( USB_USBC_32kHz_CLK == USB_USBC_32kHz_CLK_LFXO )
    CMU->CMD = CMU_CMD_USBCCLKSEL_LFXO;
#else
    CMU->CMD = CMU_CMD_USBCCLKSEL_LFRCO;
#endif
    while ( CMU->STATUS & CMU_STATUS_USBCHFCLKSEL ){}

    return true;
  }
  return false;
}
#endif /* if ( USB_PWRSAVE_MODE ) */

#if ( USB_PWRSAVE_MODE )
/*
 * Exit USB core partial powerdown mode, restore essential USB core registers.
 * Optionally prevent re-entry back to EM2.
 * Returns true if a powerup sequence was performed.
 */
static bool UsbPowerUp( void )
{
#if ( NUM_EP_USED > 0 ) || ( FIFO_CNT > 0 )
  int i;
#endif
#if ( NUM_EP_USED > 0 )
  int epNum;
  uint32_t tmp;
  USBD_Ep_TypeDef *ep;
#endif

  if ( USBD_poweredDown )
  {
    USBD_poweredDown = false;
    DEBUG_USB_INT_LO_PUTCHAR( '/' );

    /* Switch CPU core clock to 48MHz */
    CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

    /* Switch USBC clock to HFCLK */
    CMU_ClockSelectSet( cmuClock_USBC, cmuSelect_HFCLK );

    /* Exit partial powerdown mode. */
    USB->PCGCCTL &= ~USB_PCGCCTL_STOPPCLK;
    USB->PCGCCTL &= ~(USB_PCGCCTL_PWRCLMP | USB_PCGCCTL_RSTPDWNMODULE);

    /* Restore USB core registers. */
    USB->GUSBCFG = x_USB_GUSBCFG;
    USB->DCFG    = x_USB_DCFG;

#if ( FIFO_CNT > 0 )
    for ( i = 0; i < FIFO_CNT; i++ )
    {
      USB_DIEPTXFS[ i ] = x_USB_DIEPTXFS[ i ];
    }
#endif

#if ( NUM_EP_USED > 0 )
    for ( i = 0; i < NUM_EP_USED; i++ )
    {
      ep = &dev->ep[ i+1 ];
      epNum = ep->num;

      tmp = x_USB_EP_CTL[ i ] &
            ~( USB_DIEP_CTL_CNAK       | USB_DIEP_CTL_SNAK       |
               USB_DIEP_CTL_SETD0PIDEF | USB_DIEP_CTL_SETD1PIDOF   );

      if ( x_USB_EP_CTL[ i ] & USB_DIEP_CTL_DPIDEOF )
        tmp |= USB_DIEP_CTL_SETD1PIDOF;
      else
        tmp |= USB_DIEP_CTL_SETD0PIDEF;

      if ( x_USB_EP_CTL[ i ] & USB_DIEP_CTL_NAKSTS )
        tmp |= USB_DIEP_CTL_SNAK;
      else
        tmp |= USB_DIEP_CTL_CNAK;

      if ( ep->in )
      {

        USB_DINEPS[ epNum ].CTL     = tmp;
        USB_DINEPS[ epNum ].TSIZ    = x_USB_EP_TSIZ[    i ];
        USB_DINEPS[ epNum ].DMAADDR = x_USB_EP_DMAADDR[ i ];
      }
      else
      {
        USB_DOUTEPS[ epNum ].CTL     = tmp;
        USB_DOUTEPS[ epNum ].TSIZ    = x_USB_EP_TSIZ[    i ];
        USB_DOUTEPS[ epNum ].DMAADDR = x_USB_EP_DMAADDR[ i ];
      }
    }
#endif

    USB->PCGCCTL   = x_USB_PCGCCTL;
    USB->DOEPMSK   = x_USB_DOEPMSK;
    USB->DIEPMSK   = x_USB_DIEPMSK;
    USB->DAINTMSK  = x_USB_DAINTMSK;
    USB->DCTL      = x_USB_DCTL;
    USB->GNPTXFSIZ = x_USB_GNPTXFSIZ;
    USB->GRXFSIZ   = x_USB_GRXFSIZ;
    USB->GAHBCFG   = x_USB_GAHBCFG;
    USB->GOTGCTL   = x_USB_GOTGCTL;
    USB->GINTMSK   = x_USB_GINTMSK;

    USB->DCTL |= USB_DCTL_PWRONPRGDONE;

#if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ENTEREM2 )
    /* Do not reenter EM2 on interrupt exit. */
    SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
#endif

    return true;
  }
  return false;
}
#endif /* if ( USB_PWRSAVE_MODE ) */

/** @endcond */

#endif /* defined( USB_DEVICE ) */
#endif /* defined( USB_PRESENT ) && ( USB_COUNT == 1 ) */
