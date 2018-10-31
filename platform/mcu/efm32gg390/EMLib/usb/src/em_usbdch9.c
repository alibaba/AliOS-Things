/**************************************************************************//**
 * @file em_usbdch9.c
 * @brief USB protocol stack library, USB chapter 9 command handler.
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

#include "em_usbtypes.h"
#include "em_usbhal.h"
#include "em_usbd.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static int ClearFeature     ( USBD_Device_TypeDef *pDev );
static int GetConfiguration ( USBD_Device_TypeDef *pDev );
static int GetDescriptor    ( USBD_Device_TypeDef *pDev );
static int GetInterface     ( USBD_Device_TypeDef *pDev );
static int GetStatus        ( USBD_Device_TypeDef *pDev );
static int SetAddress       ( USBD_Device_TypeDef *pDev );
static int SetConfiguration ( USBD_Device_TypeDef *pDev );
static int SetFeature       ( USBD_Device_TypeDef *pDev );
static int SetInterface     ( USBD_Device_TypeDef *pDev );

static uint32_t txBuf;

int USBDCH9_SetupCmd( USBD_Device_TypeDef *device )
{
  int status;
  USB_Setup_TypeDef *p = device->setup;

  /* Vendor unique, Class or Standard setup commands override ? */
  if ( device->callbacks->setupCmd )
  {
    status = device->callbacks->setupCmd( p );

    if ( status != USB_STATUS_REQ_UNHANDLED )
    {
      return status;
    }
  }

  status = USB_STATUS_REQ_ERR;

  if ( p->Type == USB_SETUP_TYPE_STANDARD )
  {
    switch ( p->bRequest )
    {
      case GET_STATUS:
        status = GetStatus( device );
        break;

      case CLEAR_FEATURE:
        status = ClearFeature( device );
        break;

      case SET_FEATURE:
        status = SetFeature( device );
        break;

      case SET_ADDRESS:
        status = SetAddress( device );
        break;

      case GET_DESCRIPTOR:
        status = GetDescriptor( device );
        break;

      case GET_CONFIGURATION:
        status = GetConfiguration( device );
        break;

      case SET_CONFIGURATION:
        status = SetConfiguration( device );
        break;

      case GET_INTERFACE:
        status = GetInterface( device );
        break;

      case SET_INTERFACE:
        status = SetInterface( device );
        break;

      case SYNCH_FRAME:     /* Synch frame is for isochronous endpoints */
      case SET_DESCRIPTOR:  /* Set descriptor is optional */
      default:
        break;
    }
  }

  return status;
}

static int ClearFeature( USBD_Device_TypeDef *pDev )
{
  USBD_Ep_TypeDef *ep;
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;

  if ( p->wLength != 0 )
  {
    return USB_STATUS_REQ_ERR;
  }

  switch ( p->Recipient )
  {
    case USB_SETUP_RECIPIENT_DEVICE:
      if ( ( p->wIndex == 0                                 ) &&
           ( p->wValue == USB_FEATURE_DEVICE_REMOTE_WAKEUP  ) &&
           ( ( pDev->state == USBD_STATE_ADDRESSED     ) ||
             ( pDev->state == USBD_STATE_CONFIGURED    )    )    )
      {
        /* Remote wakeup feature clear */
        if ( pDev->configDescriptor->bmAttributes & CONFIG_DESC_BM_REMOTEWAKEUP )
          {
          /* The device is capable of signalling remote wakeup */
          pDev->remoteWakeupEnabled = false;
          retVal = USB_STATUS_OK;
          }
      }
      break;

    case USB_SETUP_RECIPIENT_ENDPOINT:
      ep = USBD_GetEpFromAddr( p->wIndex & 0xFF );
      if ( ep )
      {
        if ( ( ep->num > 0                            ) &&
             ( p->wValue == USB_FEATURE_ENDPOINT_HALT ) &&
             ( pDev->state == USBD_STATE_CONFIGURED   )    )
        {
          retVal = USBDHAL_UnStallEp( ep );
        }
      }
  }

  return retVal;
}

static int GetConfiguration( USBD_Device_TypeDef *pDev )
{
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;
  uint8_t *pConfigValue = (uint8_t*)&txBuf;

  if ( ( p->wIndex    != 0                          ) ||
       ( p->wLength   != 1                          ) ||
       ( p->wValue    != 0                          ) ||
       ( p->Direction != USB_SETUP_DIR_IN           ) ||
       ( p->Recipient != USB_SETUP_RECIPIENT_DEVICE )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  if ( pDev->state == USBD_STATE_ADDRESSED )
  {
    *pConfigValue = 0;
    retVal = USBD_Write( 0, pConfigValue, 1, NULL );
  }

  else if ( pDev->state == USBD_STATE_CONFIGURED )
  {
    retVal = USBD_Write( 0, &pDev->configurationValue, 1, NULL );
  }

  return retVal;
}

static int GetDescriptor( USBD_Device_TypeDef *pDev )
{
  int index;
  uint16_t length = 0;
  const void *data = NULL;
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;

  if ( ( p->Recipient != USB_SETUP_RECIPIENT_DEVICE ) ||
       ( p->Direction != USB_SETUP_DIR_IN           )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  index = p->wValue & 0xFF;
  switch ( p->wValue >> 8 )
  {
    case USB_DEVICE_DESCRIPTOR :
      if ( index != 0 )
      {
        break;
      }
      data   = pDev->deviceDescriptor;
      length = pDev->deviceDescriptor->bLength;
      break;

    case USB_CONFIG_DESCRIPTOR :
      if ( index != 0 )
      {
        break;
      }
      data   = pDev->configDescriptor;
      length = pDev->configDescriptor->wTotalLength;
      break;

    case USB_STRING_DESCRIPTOR :
      if ( index < pDev->numberOfStrings )
      {
        USB_StringDescriptor_TypeDef *s;
        s = ((USB_StringDescriptor_TypeDef**)pDev->stringDescriptors)[index];

        data   = s;
        length = s->len;
      }
      break;
  }

  if ( length )
  {
    retVal = USBD_Write( 0, (void*)data, EFM32_MIN(length, p->wLength), NULL );
  }

  return retVal;
}

static int GetInterface( USBD_Device_TypeDef *pDev )
{
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;
  uint8_t *pAlternateSetting = (uint8_t*)&txBuf;

  /* There is currently no support for alternate interface settings. */

  if ( ( p->wIndex      >= pDev->numberOfInterfaces      ) ||
       ( p->wLength     != 1                             ) ||
       ( p->wValue      != 0                             ) ||
       ( p->Direction   != USB_SETUP_DIR_IN              ) ||
       ( p->Recipient   != USB_SETUP_RECIPIENT_INTERFACE )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  if ( pDev->state == USBD_STATE_CONFIGURED )
  {
    *pAlternateSetting = 0;
    retVal = USBD_Write( 0, pAlternateSetting, 1, NULL );
  }

  return retVal;
}

static int GetStatus( USBD_Device_TypeDef *pDev )
{
  USBD_Ep_TypeDef *ep;
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;
  uint16_t *pStatus = (uint16_t*)&txBuf;

  if ( ( p->wValue    != 0                ) ||
       ( p->wLength   != 2                ) ||
       ( p->Direction != USB_SETUP_DIR_IN )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  switch ( p->Recipient )
  {
    case USB_SETUP_RECIPIENT_DEVICE:
      if ( ( p->wIndex == 0                              ) &&
           ( ( pDev->state == USBD_STATE_ADDRESSED  ) ||
             ( pDev->state == USBD_STATE_CONFIGURED )    )    )
      {
        *pStatus = 0;

        /* Remote wakeup feature status */
        if ( pDev->remoteWakeupEnabled )
          *pStatus |= REMOTE_WAKEUP_ENABLED;

        /* Current self/bus power status */
        if ( pDev->callbacks->isSelfPowered != NULL )
        {
          if ( pDev->callbacks->isSelfPowered() )
          {
            *pStatus |= DEVICE_IS_SELFPOWERED;
          }
        }
        else
        {
          if ( pDev->configDescriptor->bmAttributes & CONFIG_DESC_BM_SELFPOWERED )
          {
            *pStatus |= DEVICE_IS_SELFPOWERED;
          }
        }

        retVal = USBD_Write( 0, pStatus, 2, NULL );
      }
      break;

    case USB_SETUP_RECIPIENT_INTERFACE:
      if ( ( ( pDev->state == USBD_STATE_ADDRESSED     ) &&
             ( p->wIndex   == 0                        )    ) ||
           ( ( pDev->state == USBD_STATE_CONFIGURED    ) &&
             ( p->wIndex   <  pDev->numberOfInterfaces )    )    )
      {
        *pStatus = 0;
        retVal = USBD_Write( 0, pStatus, 2, NULL );
      }
      break;

    case USB_SETUP_RECIPIENT_ENDPOINT:
      ep = USBD_GetEpFromAddr( p->wIndex & 0xFF );
      if ( ep )
      {
        if ( ( ( pDev->state == USBD_STATE_ADDRESSED     ) &&
               ( ep->num     == 0                        )    ) ||
             ( pDev->state   == USBD_STATE_CONFIGURED         )    )
        {
          /* Send 2 bytes w/halt status for endpoint */
          retVal = USBDHAL_GetStallStatusEp( ep, pStatus );
          if ( retVal == USB_STATUS_OK )
          {
            retVal = USBD_Write( 0, pStatus, 2, NULL );
          }
        }
      }
      break;
  }

  return retVal;
}

static int SetAddress( USBD_Device_TypeDef *pDev )
{
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;

  if ( ( p->wIndex    != 0                          ) ||
       ( p->wLength   != 0                          ) ||
       ( p->wValue    >  127                        ) ||
       ( p->Recipient != USB_SETUP_RECIPIENT_DEVICE )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  if ( pDev->state == USBD_STATE_DEFAULT )
  {
    if ( p->wValue != 0 )
    {
      USBD_SetUsbState( USBD_STATE_ADDRESSED );
    }
    USBDHAL_SetAddr( p->wValue );
    retVal = USB_STATUS_OK;
  }

  else if ( pDev->state == USBD_STATE_ADDRESSED )
  {
    if ( p->wValue == 0 )
    {
      USBD_SetUsbState( USBD_STATE_DEFAULT );
    }
    USBDHAL_SetAddr( p->wValue );
    retVal = USB_STATUS_OK;
  }

  return retVal;
}

static int SetConfiguration( USBD_Device_TypeDef *pDev )
{
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;

  if ( ( p->wIndex      != 0                          ) ||
       ( p->wLength     != 0                          ) ||
       ( (p->wValue>>8) != 0                          ) ||
       ( p->Recipient   != USB_SETUP_RECIPIENT_DEVICE )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  if ( pDev->state == USBD_STATE_ADDRESSED )
  {
    if ( ( p->wValue == 0                                           ) ||
         ( p->wValue == pDev->configDescriptor->bConfigurationValue )    )
    {
      pDev->configurationValue = p->wValue;
      if ( p->wValue == pDev->configDescriptor->bConfigurationValue )
      {
        USBD_ActivateAllEps( true );
        USBD_SetUsbState( USBD_STATE_CONFIGURED );
      }
      retVal = USB_STATUS_OK;
    }
  }

  else if ( pDev->state == USBD_STATE_CONFIGURED )
  {
    if ( ( p->wValue == 0                                           ) ||
         ( p->wValue == pDev->configDescriptor->bConfigurationValue )    )
    {
      pDev->configurationValue = p->wValue;
      if ( p->wValue == 0 )
      {
        USBD_SetUsbState( USBD_STATE_ADDRESSED );
        USBD_DeactivateAllEps( USB_STATUS_DEVICE_UNCONFIGURED );
      }
      else
      {
        /* Reenable device endpoints, will reset data toggles */
        USBD_ActivateAllEps( false );
      }
      retVal = USB_STATUS_OK;
    }
  }

  return retVal;
}

static int SetFeature( USBD_Device_TypeDef *pDev )
{
  USBD_Ep_TypeDef *ep;
  int retVal = USB_STATUS_REQ_ERR;
  USB_XferCompleteCb_TypeDef callback;
  USB_Setup_TypeDef *p = pDev->setup;

  if ( p->wLength != 0 )
  {
    return USB_STATUS_REQ_ERR;
  }

  switch ( p->Recipient )
  {
    case USB_SETUP_RECIPIENT_DEVICE:
      if ( ( p->wIndex == 0                                ) &&
           ( p->wValue == USB_FEATURE_DEVICE_REMOTE_WAKEUP ) &&
           ( pDev->state == USBD_STATE_CONFIGURED          )    )
      {
        /* Remote wakeup feature set */
        if ( pDev->configDescriptor->bmAttributes & CONFIG_DESC_BM_REMOTEWAKEUP )
          {
          /* The device is capable of signalling remote wakeup */
          pDev->remoteWakeupEnabled = true;
          retVal = USB_STATUS_OK;
          }
      }
      break;

    case USB_SETUP_RECIPIENT_ENDPOINT:
      ep = USBD_GetEpFromAddr( p->wIndex & 0xFF );
      if ( ep )
      {
        if ( ( ep->num > 0                            ) &&
             ( p->wValue == USB_FEATURE_ENDPOINT_HALT ) &&
             ( pDev->state == USBD_STATE_CONFIGURED   )    )
        {
          retVal = USBDHAL_StallEp( ep );

          ep->state = D_EP_IDLE;
          /* Call end of transfer callback for endpoint */
          if ( ( retVal == USB_STATUS_OK ) &&
               ( ep->state != D_EP_IDLE  ) &&
               ( ep->xferCompleteCb      )    )
          {
            callback = ep->xferCompleteCb;
            ep->xferCompleteCb = NULL;
            DEBUG_USB_API_PUTS( "\nEP cb(), EP stalled" );
            callback( USB_STATUS_EP_STALLED, ep->xferred, ep->remaining);
          }
        }
      }
  }

  return retVal;
}

static int SetInterface( USBD_Device_TypeDef *pDev )
{
  int retVal = USB_STATUS_REQ_ERR;
  USB_Setup_TypeDef *p = pDev->setup;

  /* There is currently no support for alternate interface settings. */

  if ( ( p->wIndex    <  pDev->numberOfInterfaces      ) &&
       ( p->wLength   == 0                             ) &&
       ( p->wValue    == 0                             ) &&
       ( pDev->state  == USBD_STATE_CONFIGURED         ) &&
       ( p->Recipient == USB_SETUP_RECIPIENT_INTERFACE )    )
  {
    /* Reset data toggles on EP's */
    USBD_ActivateAllEps( false );
    return USB_STATUS_OK;
  }
  return retVal;
}

/** @endcond */

#endif /* defined( USB_DEVICE ) */
#endif /* defined( USB_PRESENT ) && ( USB_COUNT == 1 ) */
