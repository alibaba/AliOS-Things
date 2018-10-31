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
 *  @brief USB Driver Interface
 *  @details
 * ****************************************************************************/
/* header definition ******************************************************** */
#ifndef BCDS_USB_H_
#define BCDS_USB_H_

/* public interface declaration ********************************************* */
#include <stdint.h>
#include "em_usb.h"
/* public type and macro definitions */

/** Return status for the USB transfer  */
typedef enum USB_returnCode_e
{
    USB_SUCCESS = INT8_C(0),
    USB_FAILURE = INT8_C(-1),
} USB_returnCode_t;

/** Define the prototype to which USB receive callback functions must confirm.
 *
 * Functions implementing USB receive callbacks do not need to return, therefore their return type
 * is void none.
 *
 * Example task function prototype which confirms to the type declaration:
 * @code void EXL_exampleTxCallbackFunction(buffer,length); @endcode
 */
typedef void (*USB_rxCallback)(uint8_t *, uint16_t);

/** The serial port LINE CODING data structure, used to carry information
 *  about serial port baudrate, parity etc. between host and device.       
 */
typedef struct USB_lineCoding_s
{
    uint32_t Baudrate; /**< Baudrate                            */
    uint8_t charFormat; /**< Stop bits, 0=1 1=1.5 2=2            */
    uint8_t parityType; /**< 0=None 1=Odd 2=Even 3=Mark 4=Space  */
    uint8_t dataBits; /**< 5, 6, 7, 8 or 16                    */
    uint8_t dummy; /**< To ensure size is a multiple of 4 bytes.*/
    USB_rxCallback usbRxCallback; /**< received callback**/

}__attribute__ ((packed)) USB_lineCoding_t, *USB_lineCoding_tp;

/* public function prototype declarations */

/**
 * @brief       Initializes the USB device
 * @param[in]   lineCoding - The usb port LINE CODING data structure, used to carry information
 *                           about serial port baudrate, parity etc. between host and device.
 * @param[in]   usbRxCallback  - Application receive callback function to receive data over USB
 *
 * @warning     Please give the parameter usbRxCallback value as NULL.
 *
 * @note If the user wants to re-init USB please Refer code given as below for example
 *
 * @code
 * Example:
 * #include "XdkUsbResetUtility.h"
 *
 * USB_lineCoding_t BSP_USBHandle1;
 *
 * Retcode_T ReInitUSB(void)
 * {
 * Retcode_T retcode = RETCODE_OK;
 * BSP_USBHandle1.Baudrate = USB_SET_BAUDRATE;
 * BSP_USBHandle1.charFormat = USB_SET_STOPBITS_MODE;
 * BSP_USBHandle1.parityType = USB_SET_PARITY;
 * BSP_USBHandle1.dataBits = USB_SET_DATA_BITS;
 * BSP_USBHandle1.dummy = 0;
 * NVIC_SetPriority(USB_IRQn, 5);
 * USB_init(&BSP_USBHandle1, NULL); // Note - If required please give few seconds delay for USB enumeration
 * returnValue = UsbResetUtility_Init(); // Note - USB receive callback is set to UsbResetUtility_UsbISRCallback() by implementation.
 *   if (RETCODE_OK == returnValue)
 *   {
 *      // If user wants to handle receive then the below API calls are required
 *      returnValue = UsbResetUtility_RegAppISR(&ApplicationUsbRXcallback);
 *      if (RETCODE_OK == returnValue)
 *      {
 *      // application usb receive call back successfully registered
 *      }
 *   }
 * }
 *
 *  // user application receive USB call back sample implementation
 *  void ApplicationUsbRXcallback(uint8_t * usbRcvBuffer, uint32_t usbRcvBufLength)
 *  {
 *  // Print received data and length
 *  }
 * @endcode
 *
 */
void USB_init(USB_lineCoding_tp lineCoding, USB_rxCallback usbRxCallback);

/**
 * @brief       Transmit single byte to USB.(only for printf purpose)
 * @param[in]   byte : The byte to be sent
 * @retVal      USB_FAILURE : API call failed
 * @retVal      USB_SUCCESS : API call succeeded
 *
 * @warning : This API was made to transmit the data send through printf API. This API is blocking
 *          TODO interrupt based USB transfers must be implemented.
 *
 *@warning  Usage of #USB_transmitByte should not be used in application code , since this api is required only for usb library
 */
USB_returnCode_t USB_transmitByte(uint8_t byte);

/**
 * @brief       Get single byte from USB
 * @param[in]   byte : Pointer to which the received byte must be written
 * @retVal      USB_FAILURE : API call failed
 * @retVal      USB_SUCCESS : API call succeeded
 *
 * @warning : This API was made to scan the data received through scanf API. This API is blocking
 *          TODO interrupt based USB transfers must be implemented
 *
 * @warning  Usage of #USB_receiveByte should not be used in application code , since this api is required only for usb library
 */
USB_returnCode_t USB_receiveByte(uint8_t* byte);

/**
 * @brief       Transmit Data to USB.(only for printf purpose)
 * @param[in]   data : buffer of data that to be transmitted
 * @param[in]   len  : length of data that to be transmitted
 * @retVal      USB_FAILURE : API call failed
 * @retVal      USB_SUCCESS : API call succeeded
 *
 * @warning : This API was made to transmit the data send through printf API.
 *          This API is blocking and other API(TBD) must be used for interrupt based USB transfers
 *
 * @warning  Usage of #USB_transmitData should not be used in application code , since this api is required only for usb library
 *
 */
USB_returnCode_t USB_transmitData(uint8_t *data, uint32_t len);

/**
 * @brief       Called each time the USB device state is changed.
 *              Starts CDC operation when device has been configured by USB host
 * @param[in]   oldState : previous USB device state enumerator. Unused parameter.
 * @param[in]   newState : present USB device state enumerator
 * @return      void
 *
 * @warning     Usage of #USB_stateChange should not be used in application code , since this api is required only for usb library
 */

void USB_stateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

/**
 * @brief       Called each time the USB host sends a SETUP command.
 *              Implements CDC class specific commands.
 * @param[in]   setup  : USB Setup request package
 * @return      USB_STATUS_REQ_UNHANDLED :  Setup request not handled.
 * @return      USB_STATUS_OK            : No errors detected
 *
 * @warning    Usage of #USB_setupCommand should not be used in application code , since this api is required only for usb library
 *
 */
int USB_setupCommand(const USB_Setup_TypeDef *setup);

/**
 * @brief       This API used to map the call back that needed to be called whenever a USB interrupt occurs
 *
 * @param[in]   usbcallback - function pointer of the callback to  be mapped
 *
 * @retVal      USB_FAILURE : API call failed
 * @retVal      USB_SUCCESS : API call succeeded
 *
 * @warning     This API will over-write any previous callback if configured.
 * @warning     Usage of #USB_callBackMapping should not be used in application code , since this api is required only for usb library
 *
 */
USB_returnCode_t USB_callBackMapping(USB_rxCallback usbcallback);

/**
 * @brief       Disconnect the USB device
 *
 */
void USB_DisConnect(void);

/**
 * @brief       RETARGET API used by printf to Get single byte from USB.
 * @param[in]   void
 * @see      retargetio.c
 *
 * @return  byte : The byte which was received from USB
 * @warning This API was made to retarget the scanf API to transmit data through USB
 *          The USB_receiveByte API is blocking and other API(TBD) must be used for interrupt based USB transfers.
 * @warning Usage of #RETARGET_ReadChar should not be used in application code , since this api is required only for usb library
 */
uint8_t RETARGET_ReadChar(void);

/**
 * @brief       RETARGET API used by printf to transmit single byte to USB
 * @param[in]   data : The byte to be sent
 * @see         retargetio.c
 * @retval      USB_returnCode_t
 *                       USB_FAILURE : USB transfer was not successful
 *                       USB_SUCCESS : USB transfer was successful
 * @warning : This API was made to retarget the printf API to transmit data through USB
 *             The USB_transmitByte API is blocking and other API(TBD) must be used for interrupt based USB transfers.
 * @warning   Usage of #RETARGET_WriteChar should not be used in application code , since this api is required only for usb library
 */
USB_returnCode_t RETARGET_WriteChar(uint8_t *data, uint32_t len);

/* public global variable declarations */

/* inline function definitions */

#endif /* BCDS_USB_H_ */

/**@}*/

/** ************************************************************************* */
