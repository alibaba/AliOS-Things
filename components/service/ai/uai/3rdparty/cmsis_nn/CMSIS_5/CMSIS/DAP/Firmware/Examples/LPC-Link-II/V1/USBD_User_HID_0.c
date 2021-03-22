/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2017 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_HID_0.c
 * Purpose: USB Device Human Interface Device class (HID) User module
 * Rev.:    V6.2.3
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_hidFunctions
 *
 * USBD_User_HID_0.c implements the application specific functionality of the
 * HID class and is used to receive and send data reports to the USB Host.
 *
 * The implementation must match the configuration file USBD_Config_HID_0.h.
 * The following values in USBD_Config_HID_0.h affect the user code:
 *
 *  - 'Endpoint polling Interval' specifies the frequency of requests
 *    initiated by USB Host for \ref USBD_HIDn_GetReport.
 *
 *  - 'Number of Output Reports' configures the values for \em rid of
 *    \ref USBD_HIDn_SetReport.
 *
 *  - 'Number of Input Reports' configures the values for \em rid of
 *    \ref USBD_HIDn_GetReport and \ref USBD_HID_GetReportTrigger.
 *
 *  - 'Maximum Input Report Size' specifies the maximum value for:
 *       - return of \ref USBD_HIDn_GetReport
 *       - len of \ref USBD_HID_GetReportTrigger.
 *
 *  - 'Maximum Output Report Size' specifies the maximum value for \em len
 *    in \ref USBD_HIDn_SetReport for rtype=HID_REPORT_OUTPUT
 *
 *  - 'Maximum Feature Report Size' specifies the maximum value for \em len
 *    in \ref USBD_HIDn_SetReport for rtype=HID_REPORT_FEATURE
 *
 */


//! [code_USBD_User_HID]

#include <stdint.h>
#include <string.h>
#include "cmsis_os2.h"
#define   osObjectsExternal
#include "osObjects.h"
#include "rl_usb.h"
#include "RTE\USB\USBD_Config_HID_0.h"
#include "DAP_config.h"
#include "DAP.h"


#if (USBD_HID0_OUT_REPORT_MAX_SZ != DAP_PACKET_SIZE)
#error "USB HID0 Output Report Size must match DAP Packet Size"
#endif
#if (USBD_HID0_IN_REPORT_MAX_SZ != DAP_PACKET_SIZE)
#error "USB HID Input Report Size must match DAP Packet Size"
#endif

static volatile uint16_t USB_RequestIndexI;     // Request  Index In
static volatile uint16_t USB_RequestIndexO;     // Request  Index Out
static volatile uint16_t USB_RequestCountI;     // Request  Count In
static volatile uint16_t USB_RequestCountO;     // Request  Count Out

static volatile uint16_t USB_ResponseIndexI;    // Response Index In
static volatile uint16_t USB_ResponseIndexO;    // Response Index Out
static volatile uint16_t USB_ResponseCountI;    // Response Count In
static volatile uint16_t USB_ResponseCountO;    // Response Count Out
static volatile uint8_t  USB_ResponseIdle;      // Response Idle  Flag

static uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
static uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Response Buffer


// Called during USBD_Initialize to initialize the USB HID class instance.
void USBD_HID0_Initialize (void) {
  // Initialize variables
  USB_RequestIndexI  = 0U;
  USB_RequestIndexO  = 0U;
  USB_RequestCountI  = 0U;
  USB_RequestCountO  = 0U;
  USB_ResponseIndexI = 0U;
  USB_ResponseIndexO = 0U;
  USB_ResponseCountI = 0U;
  USB_ResponseCountO = 0U;
  USB_ResponseIdle   = 1U;
}


// Called during USBD_Uninitialize to de-initialize the USB HID class instance.
void USBD_HID0_Uninitialize (void) {
}


// \brief Prepare HID Report data to send.
// \param[in]   rtype   report type:
//                - HID_REPORT_INPUT           = input report requested
//                - HID_REPORT_FEATURE         = feature report requested
// \param[in]   req     request type:
//                - USBD_HID_REQ_EP_CTRL       = control endpoint request
//                - USBD_HID_REQ_PERIOD_UPDATE = idle period expiration request
//                - USBD_HID_REQ_EP_INT        = previously sent report on interrupt endpoint request
// \param[in]   rid     report ID (0 if only one report exists).
// \param[out]  buf     buffer containing report data to send.
// \return              number of report data bytes prepared to send or invalid report requested.
//              - value >= 0: number of report data bytes prepared to send
//              - value = -1: invalid report requested
int32_t USBD_HID0_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf) {
  (void)rid;

  switch (rtype) {
    case HID_REPORT_INPUT:
      switch (req) {
        case USBD_HID_REQ_EP_CTRL:        // Explicit USB Host request via Control OUT Endpoint
        case USBD_HID_REQ_PERIOD_UPDATE:  // Periodic USB Host request via Interrupt OUT Endpoint
          break;
        case USBD_HID_REQ_EP_INT:         // Called after USBD_HID_GetReportTrigger to signal data obtained.
          if (USB_ResponseCountI != USB_ResponseCountO) {
            // Load data from response buffer to be sent back
            memcpy(buf, USB_Response[USB_ResponseIndexO], DAP_PACKET_SIZE);
            USB_ResponseIndexO++;
            if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
              USB_ResponseIndexO = 0U;
            }
            USB_ResponseCountO++;
            return ((int32_t)DAP_PACKET_SIZE);
          } else {
            USB_ResponseIdle = 1U;
          }
          break;
      }
      break;
    case HID_REPORT_FEATURE:
      break;
  }
  return (0);
}


// \brief Process received HID Report data.
// \param[in]   rtype   report type:
//                - HID_REPORT_OUTPUT    = output report received
//                - HID_REPORT_FEATURE   = feature report received
// \param[in]   req     request type:
//                - USBD_HID_REQ_EP_CTRL = report received on control endpoint
//                - USBD_HID_REQ_EP_INT  = report received on interrupt endpoint
// \param[in]   rid     report ID (0 if only one report exists).
// \param[in]   buf     buffer that receives report data.
// \param[in]   len     length of received report data.
// \return      true    received report data processed.
// \return      false   received report data not processed or request not supported.
bool USBD_HID0_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len) {
  (void)req;
  (void)rid;

  switch (rtype) {
    case HID_REPORT_OUTPUT:
      if (len == 0) {
        break;
      }
      if (buf[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1U;
        break;
      }
      if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) == DAP_PACKET_COUNT) {
        osThreadFlagsSet(DAP_ThreadId, 0x80U);
        break;  // Discard packet when buffer is full
      }
      // Store received data into request buffer
      memcpy(USB_Request[USB_RequestIndexI], buf, (uint32_t)len);
      USB_RequestIndexI++;
      if (USB_RequestIndexI == DAP_PACKET_COUNT) {
        USB_RequestIndexI = 0U;
      }
      USB_RequestCountI++;
      osThreadFlagsSet(DAP_ThreadId, 0x01U);
      break;
    case HID_REPORT_FEATURE:
      break;
  }
  return true;
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
      DAP_ExecuteCommand(USB_Request[USB_RequestIndexO], USB_Response[USB_ResponseIndexI]);

      // Update Request Index and Count
      USB_RequestIndexO++;
      if (USB_RequestIndexO == DAP_PACKET_COUNT) {
        USB_RequestIndexO = 0U;
      }
      USB_RequestCountO++;

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
          USBD_HID_GetReportTrigger(0U, 0U, USB_Response[n], DAP_PACKET_SIZE);
        }
      }
    }
  }
}

//! [code_USBD_User_HID]
