/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CustomClass_0.c
 * Purpose: USB Device Custom Class User module
 * Rev.:    V6.7.3
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_CustomClass_0.c is a code template for the Custom Class 0 
 * class request handling. It allows user to handle all Custom Class class 
 * requests.
 *
 * Uncomment "Example code" lines to see example that receives data on 
 * Endpoint 1 OUT and echoes it back on Endpoint 1 IN.
 * To try the example you also have to enable Bulk Endpoint 1 IN/OUT in Custom 
 * Class configuration in USBD_Config_CustomClass_0.h file.
 */

/**
 * \addtogroup usbd_custom_classFunctions
 *
 */


//! [code_USBD_User_CustomClass]

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cmsis_os2.h"
#define   osObjectsExternal
#include "osObjects.h"
#include "rl_usb.h"
#include "Driver_USBD.h"
#include "DAP_config.h"
#include "DAP.h"

static volatile uint16_t USB_RequestIndexI;     // Request  Index In
static volatile uint16_t USB_RequestIndexO;     // Request  Index Out
static volatile uint16_t USB_RequestCountI;     // Request  Count In
static volatile uint16_t USB_RequestCountO;     // Request  Count Out
static volatile uint8_t  USB_RequestIdle;       // Request  Idle  Flag

static volatile uint16_t USB_ResponseIndexI;    // Response Index In
static volatile uint16_t USB_ResponseIndexO;    // Response Index Out
static volatile uint16_t USB_ResponseCountI;    // Response Count In
static volatile uint16_t USB_ResponseCountO;    // Response Count Out
static volatile uint8_t  USB_ResponseIdle;      // Response Idle  Flag

static uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE] __attribute__((section(".bss.USB_IO")));  // Request  Buffer
static uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE] __attribute__((section(".bss.USB_IO")));  // Response Buffer
static uint16_t USB_RespSize[DAP_PACKET_COUNT];                                                           // Response Size

// \brief Callback function called during USBD_Initialize to initialize the USB Custom class instance
void USBD_CustomClass0_Initialize (void) {
  // Handle Custom Class Initialization

  // Initialize variables
  USB_RequestIndexI  = 0U;
  USB_RequestIndexO  = 0U;
  USB_RequestCountI  = 0U;
  USB_RequestCountO  = 0U;
  USB_RequestIdle    = 1U;
  USB_ResponseIndexI = 0U;
  USB_ResponseIndexO = 0U;
  USB_ResponseCountI = 0U;
  USB_ResponseCountO = 0U;
  USB_ResponseIdle   = 1U;
}

// \brief Callback function called during USBD_Uninitialize to de-initialize the USB Custom class instance
void USBD_CustomClass0_Uninitialize (void) {
  // Handle Custom Class De-initialization
}

// \brief Callback function called upon USB Bus Reset signaling
void USBD_CustomClass0_Reset (void) {
  // Handle USB Bus Reset Event
}

// \brief Callback function called when Endpoint Start was requested (by activating interface or configuration)
// \param[in]     ep_addr       endpoint address.
void USBD_CustomClass0_EndpointStart (uint8_t ep_addr) {
  // Start communication on Endpoint
  if (ep_addr == USB_ENDPOINT_OUT(1U)) {
    USB_RequestIdle = 0U;
    USBD_EndpointRead(0U, USB_ENDPOINT_OUT(1U), USB_Request[0], DAP_PACKET_SIZE);
  }
}

// \brief Callback function called when Endpoint Stop was requested (by de-activating interface or activating configuration 0)
// \param[in]     ep_addr       endpoint address.
void USBD_CustomClass0_EndpointStop (uint8_t ep_addr) {
  // Handle Endpoint communication stopped
  (void)ep_addr;
}

// \brief Callback function called when Custom Class 0 received SETUP PACKET on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     setup_packet            pointer to received setup packet.
// \param[out]    buf                     pointer to data buffer used for data stage requested by setup packet.
// \param[out]    len                     pointer to number of data bytes in data stage requested by setup packet.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (send Zero-Length Packet if no data stage)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_CustomClass0_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) {
  (void)setup_packet;
  (void)buf;
  (void)len;

  switch (setup_packet->bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      break;
    case USB_REQUEST_TO_INTERFACE:
      break;
    case USB_REQUEST_TO_ENDPOINT:
      break;
    default:
      break;
  }

  return usbdRequestNotProcessed;
}

// \brief Callback function called when SETUP PACKET was processed by USB library
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback nor by Custom Class callback)
// \param[in]     setup_packet            pointer to processed setup packet.
void USBD_CustomClass0_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet) {
  (void)setup_packet;

  switch (setup_packet->bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      break;
    case USB_REQUEST_TO_INTERFACE:
      break;
    case USB_REQUEST_TO_ENDPOINT:
      break;
    default:
      break;
  }
}

// \brief Callback function called when Custom Class 0 received OUT DATA on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     len                     number of received data bytes.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:         request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClass0_Endpoint0_OutDataReceived (uint32_t len) {
  (void)len;
  return usbdRequestNotProcessed;
}

// \brief Callback function called when Custom Class 0 sent IN DATA on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     len                     number of sent data bytes.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (return ACK)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:         request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClass0_Endpoint0_InDataSent (uint32_t len) {
  (void)len;
  return usbdRequestNotProcessed;
}

// \brief Callback function called when DATA was sent or received on Endpoint n
// \param[in]     event                   event on Endpoint:
//                                          - ARM_USBD_EVENT_OUT = data OUT received
//                                          - ARM_USBD_EVENT_IN  = data IN  sent
void USBD_CustomClass0_Endpoint1_Event  (uint32_t event) {
  // Handle Endpoint 1 events
  uint32_t n;

  if (event & ARM_USBD_EVENT_OUT) {
    n = USBD_EndpointReadGetResult(0U, USB_ENDPOINT_OUT(1U));
    if (n != 0U) {
      if (USB_Request[USB_RequestIndexI][0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1U;
      } else {
        USB_RequestIndexI++;
        if (USB_RequestIndexI == DAP_PACKET_COUNT) {
          USB_RequestIndexI = 0U;
        }
        USB_RequestCountI++;
        osThreadFlagsSet(DAP_ThreadId, 0x01);
      }
    }
    // Start reception of next request packet
    if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
      USBD_EndpointRead(0U, USB_ENDPOINT_OUT(1U), USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE);
    } else {
      USB_RequestIdle = 1U;
    }
  }
  if (event & ARM_USBD_EVENT_IN) {
    if (USB_ResponseCountI != USB_ResponseCountO) {
      // Load data from response buffer to be sent back
      USBD_EndpointWrite(0U, USB_ENDPOINT_IN(1U), USB_Response[USB_ResponseIndexO], USB_RespSize[USB_ResponseIndexO]);
      USB_ResponseIndexO++;
      if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
        USB_ResponseIndexO = 0U;
      }
      USB_ResponseCountO++;
    } else {
      USB_ResponseIdle = 1U;
    }
  }
}
void USBD_CustomClass0_Endpoint2_Event  (uint32_t event) {
  // Handle Endpoint 2 events
  if (event & ARM_USBD_EVENT_IN) {
    SWO_TransferComplete();
  }
}
void USBD_CustomClass0_Endpoint3_Event  (uint32_t event) {
  // Handle Endpoint 3 events
  (void)event;
}
void USBD_CustomClass0_Endpoint4_Event  (uint32_t event) {
  // Handle Endpoint 4 events
  (void)event;
}
void USBD_CustomClass0_Endpoint5_Event  (uint32_t event) {
  // Handle Endpoint 5 events
  (void)event;
}
void USBD_CustomClass0_Endpoint6_Event  (uint32_t event) {
  // Handle Endpoint 6 events
  (void)event;
}
void USBD_CustomClass0_Endpoint7_Event  (uint32_t event) {
  // Handle Endpoint 7 events
  (void)event;
}
void USBD_CustomClass0_Endpoint8_Event  (uint32_t event) {
  // Handle Endpoint 8 events
  (void)event;
}
void USBD_CustomClass0_Endpoint9_Event  (uint32_t event) {
  // Handle Endpoint 9 events
  (void)event;
}
void USBD_CustomClass0_Endpoint10_Event (uint32_t event) {
  // Handle Endpoint 10 events
  (void)event;
}
void USBD_CustomClass0_Endpoint11_Event (uint32_t event) {
  // Handle Endpoint 11 events
  (void)event;
}
void USBD_CustomClass0_Endpoint12_Event (uint32_t event) {
  // Handle Endpoint 12 events
  (void)event;
}
void USBD_CustomClass0_Endpoint13_Event (uint32_t event) {
  // Handle Endpoint 13 events
  (void)event;
}
void USBD_CustomClass0_Endpoint14_Event (uint32_t event) {
  // Handle Endpoint 14 events
  (void)event;
}
void USBD_CustomClass0_Endpoint15_Event (uint32_t event) {
  // Handle Endpoint 15 events
  (void)event;
}

// DAP Thread.
__NO_RETURN void DAP_Thread (void *argument) {
  uint32_t flags;
  uint32_t n;
  (void)   argument;

  for (;;) {
    osThreadFlagsWait(0x81U, osFlagsWaitAny, osWaitForever);

    // Process pending requests
    while (USB_RequestCountI != USB_RequestCountO) {

      // Handle Queue Commands
      n = USB_RequestIndexO;
      while (USB_Request[n][0] == ID_DAP_QueueCommands) {
        USB_Request[n][0] = ID_DAP_ExecuteCommands;
        n++;
        if (n == DAP_PACKET_COUNT) {
          n = 0U;
        }
        if (n == USB_RequestIndexI) {
          flags = osThreadFlagsWait(0x81U, osFlagsWaitAny, osWaitForever);
          if (flags & 0x80U) {
            break;
          }
        }
      }

      // Execute DAP Command (process request and prepare response)
      USB_RespSize[USB_ResponseIndexI] =
        (uint16_t)DAP_ExecuteCommand(USB_Request[USB_RequestIndexO], USB_Response[USB_ResponseIndexI]);

      // Update Request Index and Count
      USB_RequestIndexO++;
      if (USB_RequestIndexO == DAP_PACKET_COUNT) {
        USB_RequestIndexO = 0U;
      }
      USB_RequestCountO++;

      if (USB_RequestIdle) {
        if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
          USB_RequestIdle = 0U;
          USBD_EndpointRead(0U, USB_ENDPOINT_OUT(1U), USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE);
        }
      }

      // Update Response Index and Count
      USB_ResponseIndexI++;
      if (USB_ResponseIndexI == DAP_PACKET_COUNT) {
        USB_ResponseIndexI = 0U;
      }
      USB_ResponseCountI++;

      if (USB_ResponseIdle) {
        if (USB_ResponseCountI != USB_ResponseCountO) {
          // Load data from response buffer to be sent back
          n = USB_ResponseIndexO++;
          if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
            USB_ResponseIndexO = 0U;
          }
          USB_ResponseCountO++;
          USB_ResponseIdle = 0U;
          USBD_EndpointWrite(0U, USB_ENDPOINT_IN(1U), USB_Response[n], USB_RespSize[n]);
        }
      }
    }
  }
}

// SWO Data Queue Transfer
//   buf:    pointer to buffer with data
//   num:    number of bytes to transfer
void SWO_QueueTransfer (uint8_t *buf, uint32_t num) {
  USBD_EndpointWrite(0U, USB_ENDPOINT_IN(2U), buf, num);
}

// SWO Data Abort Transfer
void SWO_AbortTransfer (void) {
  USBD_EndpointAbort(0U, USB_ENDPOINT_IN(2U));
}

//! [code_USBD_User_CustomClass]
