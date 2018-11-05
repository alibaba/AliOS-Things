/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief
 * This module takes care of USB initialization , USB scanning and printing to USB API's.
 * USB communications device class (or USB CDC) is a composite Universal Serial Bus device class.
 * 
 * ****************************************************************************/

/* module includes ********************************************************** */
#include "BCDS_BSP.h"
/* system header files */
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_USB

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_USB
/* additional interface header files */
#include "em_device.h"
/** @TODO : The following condition and the above inclusion of "em_device.h" needs to be
 * removed once the dependency of "em_usb.h" is removed in USB_ih.h.
 * This condition is added because, all the EM device variants does not support USB,
 * and because of the inclusion of "em_usb.h" in the USB_ih.h, a dependency is created
 * which affects the EM device types which does not support USB(during package creation). */
#if defined( USB_PRESENT ) && ( USB_COUNT == 1 )

#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID  BCDS_HAL_MODULE_ID_USB

#include "BCDS_Assert.h"
#include "em_usb.h"
#include "BCDS_USB.h"
#include "BSP_USB_Descriptors.h"
#include "BSP_BoardShared.h"

/* local prototypes ********************************************************* */
#define USB_TX_RX_BUF_SIZE          UINT16_C(256)               /**< USB Transfer/receiver buffer size */

/** WORD (4 byte) aligned uint8_t array to store the received data*/
static UBUF( USB_usbRxBuffer_mau, USB_TX_RX_BUF_SIZE );

/**The callback function is called after a successful read of the received data .
 *
 */
static inline int USB_dataReceivedCallback(USB_Status_TypeDef status,
    uint32_t bytesReceived, uint32_t remainingBytes);

/**The callback function is called when a transfer has completed.
 *
 */
static inline int USB_dataTransferred(USB_Status_TypeDef status,
    uint32_t bytesTransfered, uint32_t remainingBytes);

/**
 * buffer to hold the data that to be transmitted over USB
 */
uint8_t *USB_usbbuffer_gpu =
    { 0 };

/**Called each time the USB device state is changed.
 *
 */
inline void USB_stateChange(USBD_State_TypeDef oldState,
    USBD_State_TypeDef newState);

/**Called each time the USB host sends a SETUP command.
 *
 */
inline int USB_setupCommand(const USB_Setup_TypeDef *setup);

/* constant definitions ***************************************************** */

/* local variables ********************************************************** */
static uint32_t USB_transferCount_mdu; /**< The count of bytes transferred */
static volatile USB_returnCode_t USB_transferDone_md; /**< The status whether the transfer was completed or not */
static USB_Status_TypeDef USB_transferStatus_md; /**< Detailed USB transfer status enumerator */
static USB_returnCode_t USB_configured_md; /**< Variable to determine whether USB is configured or not */

/** The LineCoding variable must be 4-byte aligned as it is used as USB
 * transmit and receive buffer
 */
USB_lineCoding_t __attribute__((aligned(4))) USB_lineCoding_mds;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/**
 * @brief Build a USB compliant string descriptor from a standard zero
 *        terminated string.
 *
 * @param[in] serialNumber
 *     The serialNumber for the USB
 *
 */
static inline void USB_setSerialNumber( char *serialNumber )
{
  uint8_t strLength = UINT8_C(0);

  while( *serialNumber )
  {
    USB_serialnumber_gau.name[ strLength++ ] = *serialNumber++;

    if ( strLength == USBDESC_STR_LEN )
    {
      break;
    }
  }
  USB_serialnumber_gau.len  = (strLength * 2) + 2;
  USB_serialnumber_gau.type = USB_STRING_DESCRIPTOR;
}

/**
 * @brief Callback called after reading of buffer is finished
 *
 * @param[in] status
 *     The transfer status. See @ref USB_Status_TypeDef.
 *
 * @param[in] bytesReceived
 *   Number of bytes actually Received.
 *
 * @param[in] remaining
 *   Number of bytes not Received. Not in use currently
 */
static inline int USB_dataReceivedCallback(USB_Status_TypeDef status,
    uint32_t bytesReceived, uint32_t remainingBytes)
{
    (void) remainingBytes;

    int returnStatus = UINT8_C(0);

    if ((status == USB_STATUS_OK) && (bytesReceived > 0))
    {
        /*execute callback function only if it is provided during usb initialization*/
        if (USB_lineCoding_mds.usbRxCallback != NULL)
        {
            (*(USB_lineCoding_mds.usbRxCallback))(USB_usbRxBuffer_mau, bytesReceived);
        }
    }

    /* enable the endpoint and wait for the packet received interrupt */
    returnStatus = USBD_Read(USB_EP_DATA_OUT, (void*) USB_usbRxBuffer_mau, USB_TX_RX_BUF_SIZE,
        USB_dataReceivedCallback);

    return (returnStatus);
}

/**
 * @brief
 *  USB transfer callback function.
 *  The callback function is called when a transfer has completed. An application
 *  should check the status, xferred and optionally the remaining parameters
 *  before deciding if the transfer is usable. In the case where the transfer
 *  is part of a control request data stage, the callback function should
 *  return an appropriate @ref USB_Status_TypeDef status. Right now it returns transferred status.
 *
 * @param[in] status
 *   The transfer status. See @ref USB_Status_TypeDef.
 *
 * @param[in] bytesTransferred
 *   Number of bytes actually Transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred. Not in use currently
 *
 * @return status of USB transfer
 */
static inline int USB_dataTransferred(USB_Status_TypeDef status,
    uint32_t bytesTransfered, uint32_t remainingBytes)
{
    (void) remainingBytes; /* Unused parameter */

    USB_transferStatus_md = status;
    USB_transferCount_mdu = bytesTransfered;
    USB_transferDone_md = USB_SUCCESS;
    return (USB_transferStatus_md);
}

/* refer interface header USB_ih.h for further details */
inline void USB_stateChange(USBD_State_TypeDef oldState,
    USBD_State_TypeDef newState)
{
    (void) oldState; /* Unused parameter */

    if (USBD_STATE_CONFIGURED == newState)
    {
        USB_configured_md = USB_SUCCESS;

        /*Inorder to receive data, endpoint should be enabled every time. For the first time , the reception of the data is
         * enabled here(i.e After USB successfully configured) and  re enabling the endpoint,for reception of data is handled
         * in the call back function(USB_dataReceivedCallback)  */
        USBD_Read(USB_EP_DATA_OUT, (void*) USB_usbRxBuffer_mau, USB_BULK_SIZE,
            USB_dataReceivedCallback);
    }
    else
    {
        USB_configured_md = USB_FAILURE;
    }
}

/* refer interface header USB_ih.h for further details */
inline int USB_setupCommand(const USB_Setup_TypeDef *setup)
{
    int retVal = USB_STATUS_REQ_UNHANDLED;

    if ((setup->Type == USB_SETUP_TYPE_CLASS)
        && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
        switch (setup->bRequest)
        {
        case USB_CDC_GETLINECODING:
            if ((USB_SETUP_VALUE == setup->wValue)
                && (USB_SETUP_INDEX == setup->wIndex) && /* Interface no.            */
                (USB_SETUP_LENGTH == setup->wLength) && /* Length of cdcLineCoding  */
                (USB_SETUP_DIR_IN == setup->Direction))
            {
                /* Send current settings to USB host. */
                USBD_Write(0, (void*) &USB_lineCoding_mds, USB_SETUP_LENGTH,
                NULL);
                retVal = USB_STATUS_OK;
            }
            break;

        case USB_CDC_SETLINECODING:
            if ((USB_SETUP_VALUE == setup->wValue)
                && (USB_SETUP_INDEX == setup->wIndex) && /* Interface no.            */
                (USB_SETUP_LENGTH == setup->wLength) && /* Length of cdcLineCoding  */
                (USB_SETUP_DIR_IN != setup->Direction))
            {
                /* Get new settings from USB host. */
                USBD_Read(0, (void*) &USB_lineCoding_mds, USB_SETUP_LENGTH,
                NULL);
                retVal = USB_STATUS_OK;
            }
            break;

        case USB_CDC_SETCTRLLINESTATE:
            if ((USB_SETUP_INDEX == setup->wIndex) && /* Interface no.  */
            (USB_SETUP_NO_DATA_LENGTH == setup->wLength)) /* No data        */
            {
                /* Do nothing ( Non compliant behaviour !! ) */
                retVal = USB_STATUS_OK;
            }
            break;
        default:
            break;

        }
    }
    return (retVal);
}

/* global functions ********************************************************* */

/* refer interface header USB_ih.h for further details */
void USB_init(USB_lineCoding_tp lineCoding, USB_rxCallback usbRxCallback)
{
    char usbSerialNumber[USBDESC_STR_LEN];


    assert(lineCoding != NULL);

    USB_lineCoding_mds.Baudrate = lineCoding->Baudrate;
    USB_lineCoding_mds.charFormat = lineCoding->charFormat;
    USB_lineCoding_mds.dataBits = lineCoding->dataBits;
    USB_lineCoding_mds.parityType = lineCoding->parityType;
    USB_configured_md = USB_FAILURE;
    USB_lineCoding_mds.usbRxCallback = usbRxCallback;

    /* Initialize USB Serial Number String with all Zeros */
    memset(usbSerialNumber, UINT8_C(0), USBDESC_STR_LEN);

    /* Format Unique ID of MCU in Hex format and put it into String */
    snprintf(usbSerialNumber, USBDESC_STR_LEN, "%lX%lX", DEVINFO->UNIQUEH, DEVINFO->UNIQUEL);

    /* Set USB Serial Number *before* USB Stack initialization */
    USB_setSerialNumber(usbSerialNumber);

    USBD_Init(&USB_initStruct_mds); /* Start USB CDC => USB communications device class functionality  */
}

/* refer interface header USB_ih.h for further details */
USB_returnCode_t USB_transmitByte(uint8_t byte)
{
    USB_returnCode_t retVal = USB_FAILURE;
    USB_transferDone_md = USB_FAILURE;
    {
        uint8_t buffer[1];

        buffer[0] = byte;

        if (USB_SUCCESS == USB_configured_md)
        {
            USBD_Write(USB_EP_DATA_IN, buffer, 1, USB_dataTransferred);
            while (USB_FAILURE == USB_transferDone_md)
            {
#ifdef USB_CC_TEST_GTEST
                break;
#endif
            }
            if (USB_STATUS_OK == USB_transferStatus_md)
            {
                retVal = USB_SUCCESS;
            }
            else
            {
                retVal = USB_FAILURE;
            }
        }
    }
    return (retVal);
}

/* refer interface header USB_ih.h for further details */
USB_returnCode_t USB_receiveByte(uint8_t* byte)
{
    USB_returnCode_t retVal = USB_FAILURE;
    USB_transferDone_md = USB_FAILURE;
    {
        uint8_t buffer[1];

        assert(byte != NULL);

        if (USB_SUCCESS == USB_configured_md)
        {
            USBD_Read(USB_EP_DATA_OUT, buffer, 1, USB_dataTransferred);
            while (USB_FAILURE == USB_transferDone_md)
            {
#ifdef USB_CC_TEST_GTEST
                break;
#endif
            }
            if (USB_STATUS_OK == USB_transferStatus_md)
            {
                *byte = buffer[0];
                retVal = USB_SUCCESS;
            }
            else
            {
                retVal = USB_FAILURE;
            }
        }
    }
    return (retVal);
}

#ifdef USB_USE_PRINTF

/**
 * @brief       RETARGET API used by printf to transmit single byte to USB
 * @param[in]   data : The byte to be sent
 * @see         retargetio.c
 * @retval      USB_returnCode_t
 *                       USB_FAILURE : USB transfer was not successful
 *                       USB_SUCCESS : USB transfer was successful
 * @warning : This API was made to retarget the printf API to transmit data through USB
 *          The USB_transmitByte API is blocking and other API(TBD) must be used for interrupt based USB transfers.
 */
USB_returnCode_t RETARGET_WriteChar(uint8_t *data, uint32_t len)
{
    USB_returnCode_t retVal = USB_FAILURE;

    retVal = USB_transmitData(data, len);
    return retVal;
}

/* refer interface header USB_ih.h for further details */
USB_returnCode_t USB_transmitData(uint8_t *data, uint32_t len)
{
    USB_returnCode_t retVal = USB_FAILURE;
    unsigned int counter = USB_ZERO_VALUE;
    USB_transferDone_md = USB_FAILURE;

    if (USB_SUCCESS == USB_configured_md)
    {
        USB_usbbuffer_gpu = data;
        USBD_Write(USB_EP_DATA_IN, USB_usbbuffer_gpu, len,
            USB_dataTransferred);
        while (USB_FAILURE == USB_transferDone_md)
        {
            USBTIMER_DelayMs(USB_ONE_MILLI_SECOND_DELAY);
            counter++;
            if (USB_DELAY_LIMIT == counter)
            {
                USB_transferStatus_md = USB_STATUS_EP_BUSY;
                break;
            }
        }
        if (USB_STATUS_OK == USB_transferStatus_md)
        {
            retVal = USB_SUCCESS;
        }
        else
        {
            retVal = USB_FAILURE;
        }
    }
    return (retVal);
}

/**
 * @brief       RETARGET API used by printf to Get single byte from USB.
 * @param[in]   void
 * @see      retargetio.c
 *
 * @return  byte : The byte which was received from USB
 * @warning This API was made to retarget the scanf API to transmit data through USB
 *          The USB_receiveByte API is blocking and other API(TBD) must be used for interrupt based USB transfers.

 */
uint8_t RETARGET_ReadChar(void)
{
    uint8_t byte = 0;

    USB_receiveByte(&byte);
    return (byte);
}

#endif /* USB_USE_PRINTF */


USB_returnCode_t USB_callBackMapping(USB_rxCallback usbcallback)
{
    USB_returnCode_t returnStatus = USB_FAILURE;
    if (usbcallback != NULL)
    {
        USB_lineCoding_mds.usbRxCallback = usbcallback;
        returnStatus = USB_SUCCESS;
    }
    return (returnStatus);
}

/* API documentation is in interface header file  */
void USB_DisConnect(void)
{
    USBD_Disconnect();
}
#endif /* if (USB_PRESENT ) && ( USB_COUNT == 1 ) */

#endif /* BCDS_FEATURE_BSP_USB */

/** ************************************************************************* */
