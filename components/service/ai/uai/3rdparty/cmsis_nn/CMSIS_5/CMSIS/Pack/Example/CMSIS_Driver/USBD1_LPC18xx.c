/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V2.7
 *
 * Driver:       Driver_USBD1
 * Configured:   via RTE_Device.h configuration file
 * Project:      USB Device Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                  Value
 *   ---------------------                  -----
 *   Connect to hardware via Driver_USBD# = 1
 * --------------------------------------------------------------------------
 * Defines used for driver configuration (at compile time):
 *
 *   USBD_MAX_ENDPOINT_NUM:  defines maximum number of IN/OUT Endpoint pairs 
 *                           that driver will support with Control Endpoint 0
 *                           not included, this value impacts driver memory
 *                           requirements
 *     - default value: 3
 *     - maximum value: 3
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.7
 *    Removed unnecessary __packed specifier for structures dQH_t and dTD_t
 *  Version 2.6
 *    Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.5
 *    PowerControl for Power OFF and Uninitialize functions made unconditional
 *  Version 2.4
 *    Corrected isochronous endpoint configuration
 *    Corrected transfer procedure
 *  Version 2.3
 *    Corrected isochronous transfer functionality
 *  Version 2.2
 *    Corrected return value in USBD_PowerControl function.
 *  Version 2.1
 *    Added USB_LPC18xx_USB1.h with register bit definitions
 *    Pin configuration moved to USB_LPC18xx.c
 *  Version 2.0
 *    Updated to 2.00 API
 *  Version 1.3
 *    Re-implementation of the driver
 *  Version 1.2
 *    Updated USB1 pin configurations
 *  Version 1.1
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    Initial release
 */


#include <stdint.h>
#include <string.h>

#include "Driver_USBD.h"

#include "LPC18xx.h"
#include "USB_LPC18xx.h"
#include "SCU_LPC18xx.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if      (RTE_USB_USB1 == 0)
#error   "USB1 is not enabled in the RTE_Device.h!"
#endif
#if      (RTE_USB_USB1_FS_PHY_EN && RTE_USB_USB1_HS_PHY_EN)
#error   "Both full-speed and high-speed PHY can not be selected at the same time!"
#endif

#ifndef USBD_MAX_ENDPOINT_NUM
#define USBD_MAX_ENDPOINT_NUM           3U
#endif
#if    (USBD_MAX_ENDPOINT_NUM > 3)
#error  Too many Endpoints, maximum IN/OUT Endpoint pairs that this driver supports is 3 !!!
#endif

extern uint8_t USB1_role;
extern uint8_t USB1_state;

extern void USB1_PinsConfigure   (void);
extern void USB1_PinsUnconfigure (void);


// USBD Driver *****************************************************************

#define ARM_USBD_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,7)

// Driver Version
static const ARM_DRIVER_VERSION usbd_driver_version = { ARM_USBD_API_VERSION, ARM_USBD_DRV_VERSION };

// Driver Capabilities
static const ARM_USBD_CAPABILITIES usbd_driver_capabilities = {
  0U,   // VBUS Detection
  0U,   // Event VBUS On
  0U    // Event VBUS Off
};

#define LPC_USBx                        LPC_USB1
#define ENDPTCTRL(ep_num)               (*(volatile uint32_t *)((uint32_t)(&LPC_USBx->ENDPTCTRL0) + 4U * ep_num))

#define EP_NUM(ep_addr)                 (ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK)
#define EP_DIR(ep_addr)                 ((ep_addr >>  7) & 1U)
#define EP_SLL(ep_addr)                 (EP_DIR(ep_addr) * 16U)
#define EP_IDX(ep_addr)                 ((EP_NUM(ep_addr) * 2U) + ((ep_addr >> 7) & 1U))
#define EP_MSK(ep_addr)                 (1UL << (EP_NUM(ep_addr) + EP_SLL(ep_addr)))

typedef struct {                        // USB Device Endpoint Queue Head
  uint32_t  cap;
  uint32_t  curr_dTD;
  uint32_t  next_dTD;
  uint32_t  dTD_token;
  uint32_t  buf[5];
  uint32_t  reserved;
  uint32_t  setup[2];
                                        // Use reminder of 64 bytes for transfer information
  uint8_t  *data;
  uint32_t  num;
  uint32_t  num_transferred_total;
  uint16_t  num_transferring;
  uint8_t   ep_type;
  uint8_t   ep_active;
} dQH_t;

typedef struct {                        // USB Device Endpoint Transfer Descriptor
  uint32_t  next_dTD;
  uint32_t  dTD_token;
  uint32_t  buf[5];
  uint32_t  reserved;
} dTD_t;

static ARM_USBD_SignalDeviceEvent_t   SignalDeviceEvent;
static ARM_USBD_SignalEndpointEvent_t SignalEndpointEvent;

static ARM_USBD_STATE      usbd_state;

static uint32_t            setup_packet[2];     // Setup packet data
static volatile uint8_t    setup_received;      // Setup packet received

static dQH_t __align(2048) dQH[(USBD_MAX_ENDPOINT_NUM + 1U) * 2U];      // Queue Heads
static dTD_t __align(  32) dTD[(USBD_MAX_ENDPOINT_NUM + 1U) * 2U];      // Transfer Descriptors

// Function prototypes
static int32_t USBD_EndpointConfigure (uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size);


// Auxiliary functions

/**
  \fn          void USBD_HW_EndpointFlush (uint8_t ep_addr)
  \brief       Flush Endpoint.
*/
static void USBD_HW_EndpointFlush (uint8_t ep_addr) {
  uint32_t ep_msk;

  ep_msk = EP_MSK(ep_addr);

  LPC_USBx->ENDPTFLUSH = ep_msk;
  while (LPC_USBx->ENDPTFLUSH & ep_msk);
}

/**
  \fn          void USBD_Reset (void)
  \brief       Reset USB Endpoint settings and variables.
*/
static void USBD_Reset (void) {
  uint8_t i;

  // Reset global variables
  setup_packet[0] = 0U;
  setup_packet[1] = 0U;
  setup_received  = 0U;
  memset((void *)&usbd_state, 0, sizeof(usbd_state));
  memset((void *)dQH,         0, sizeof(dQH));
  memset((void *)dTD,         0, sizeof(dTD));
  for (i = 1U; i <= USBD_MAX_ENDPOINT_NUM; i++) {
    ENDPTCTRL(i) &= ~(USB_ENDPTCTRL_RXE | USB_ENDPTCTRL_TXE);
  }

  // Clear interrupts
  LPC_USBx->ENDPTNAK       = 0xFFFFFFFFUL;
  LPC_USBx->ENDPTNAKEN     = 0U;
  LPC_USBx->USBSTS_D       = 0xFFFFFFFFUL;
  LPC_USBx->ENDPTSETUPSTAT = LPC_USBx->ENDPTSETUPSTAT;
  LPC_USBx->ENDPTCOMPLETE  = LPC_USBx->ENDPTCOMPLETE;

  while (LPC_USBx->ENDPTPRIME);

  // Clear all Primed buffers
  LPC_USBx->ENDPTFLUSH = 0xFFFFFFFFUL;
  while (LPC_USBx->ENDPTFLUSH);

  // Interrupt threshold control: no threshold
  LPC_USBx->USBCMD_D &= ~(USB_USBCMD_D_ITC(0xFFUL));

  // Default Initialize Control Endpoint 0
  if (usbd_state.speed == ARM_USB_SPEED_HIGH) { // For High-speed
    USBD_EndpointConfigure (0x00U, ARM_USB_ENDPOINT_CONTROL, 64U);
    USBD_EndpointConfigure (0x80U, ARM_USB_ENDPOINT_CONTROL, 64U);
  } else {                                      // For Full/Low-speed
    USBD_EndpointConfigure (0x00U, ARM_USB_ENDPOINT_CONTROL,  8U);
    USBD_EndpointConfigure (0x80U, ARM_USB_ENDPOINT_CONTROL,  8U);
  }

  // Set start of endpoint list address
  LPC_USBx->ENDPOINTLISTADDR = (uint32_t)dQH;

  // Setup lockouts off
  LPC_USBx->USBMODE_D |= USB_USBMODE_D_SLOM;
}

/**
  \fn          void USBD_HW_ReadSetupPacket (void)
  \brief       Read Setup Packet to buffer.
*/
static void USBD_HW_ReadSetupPacket (void) {

  do {
    LPC_USBx->USBCMD_D |= USB_USBCMD_D_SUTW;            // Setup trip wire

    // Copy Setup Packet Data to buffer
    setup_packet[0] = dQH[0].setup[0];
    setup_packet[1] = dQH[0].setup[1];
  } while (!(LPC_USBx->USBCMD_D & LPC_USBx->USBCMD_D));

  LPC_USBx->USBCMD_D &= ~USB_USBCMD_D_SUTW;             // Clear Setup trip wire

  LPC_USBx->ENDPTSETUPSTAT = 1U;                        // Clear Setup bit

}

/**
  \fn            void USBD_HW_EndpointTransfer (uint8_t ep_addr)
  \brief         Start transfer on Endpoint.
  \param[in]     ep_addr  Endpoint Address
                  - ep_addr.0..3: Address
                  - ep_addr.7:    Direction
*/
static void USBD_HW_EndpointTransfer (uint8_t ep_addr) {
  dQH_t   *ptr_dqh;
  dTD_t   *ptr_dtd;
  uint8_t *data;
  uint32_t ep_msk, num;
  uint8_t  ep_idx;

  ep_idx  =  EP_IDX(ep_addr);
  ep_msk  =  EP_MSK(ep_addr);
  ptr_dqh = &dQH[ep_idx];
  ptr_dtd = &dTD[ep_idx];

  data    =  ptr_dqh->data + ptr_dqh->num_transferred_total;
  num     =  ptr_dqh->num  - ptr_dqh->num_transferred_total;

  if (num > 0x4000U) { num = 0x4000U; } // Maximum transfer length is 16k

  while (LPC_USBx->ENDPTSTAT & ep_msk);

  memset (ptr_dtd, 0, sizeof(dTD_t));

  // Driver does not support linked endpoint descriptors
  // Next link pointer is not valid
  ptr_dtd->next_dTD = 1U;

  // Configure Transfer Descriptor
  ptr_dtd->dTD_token |=  USB_bTD_TOKEN_TOTAL_BYTES(num) |       // Bytes to transfer
                         USB_bTD_TOKEN_IOC              |       // Interrupt on complete
                         USB_bTD_TOKEN_STATUS_ACTIVE    ;       // Transfer Active

  // Set Buffer Addresses
  ptr_dtd->buf[0]     =  (uint32_t)(data          );
  ptr_dtd->buf[1]     =  (uint32_t)(data + 0x1000U);
  ptr_dtd->buf[2]     =  (uint32_t)(data + 0x2000U);
  ptr_dtd->buf[3]     =  (uint32_t)(data + 0x3000U);
  ptr_dtd->buf[4]     =  (uint32_t)(data + 0x4000U);

  ptr_dqh->dTD_token &= ~USB_bTD_TOKEN_STATUS_MSK;              // Clear status
  ptr_dqh->next_dTD   =  (uint32_t)(ptr_dtd);                   // Save Transfer Descriptor address to Queue Head overlay

  ptr_dqh->num_transferring = num;

  LPC_USBx->ENDPTPRIME |= ep_msk;       // Prime Endpoint -> Start Transfer
}


// USBD Driver functions

/**
  \fn          ARM_DRIVER_VERSION USBD_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBD_GetVersion (void) { return usbd_driver_version; }

/**
  \fn          ARM_USBD_CAPABILITIES USBD_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBD_CAPABILITIES
*/
static ARM_USBD_CAPABILITIES USBD_GetCapabilities (void) { return usbd_driver_capabilities; }

/**
  \fn          int32_t USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                        ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
  \brief       Initialize USB Device Interface.
  \param[in]   cb_device_event    Pointer to \ref ARM_USBD_SignalDeviceEvent
  \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBD_SignalEndpointEvent
  \return      \ref execution_status
*/
static int32_t USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) {

  if ((USB1_state & USBD_DRIVER_INITIALIZED) != 0U) { return ARM_DRIVER_OK; }

  SignalDeviceEvent   = cb_device_event;
  SignalEndpointEvent = cb_endpoint_event;

  USB1_role   =  ARM_USB_ROLE_DEVICE;
  USB1_PinsConfigure ();

  USB1_state  =  USBD_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_Uninitialize (void)
  \brief       De-initialize USB Device Interface.
  \return      \ref execution_status
*/
static int32_t USBD_Uninitialize (void) {

  USB1_PinsUnconfigure ();
  USB1_role   =  ARM_USB_ROLE_NONE;
  USB1_state &= ~USBD_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Device Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t USBD_PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      NVIC_DisableIRQ      (USB1_IRQn);                 // Disable interrupt
      NVIC_ClearPendingIRQ (USB1_IRQn);                 // Clear pending interrupt
      USB1_state &= ~USBD_DRIVER_POWERED;               // Clear powered flag
                                                        // Reset variables
      setup_received =  0U;
      memset((void *)&usbd_state, 0, sizeof(usbd_state));
      memset((void *)dQH,         0, sizeof(dQH));
      memset((void *)dTD,         0, sizeof(dTD));

#if (!RTE_USB_USB1_HS_PHY_EN)
      SCU_USB1_PinConfigure (SCU_USB1_PIN_CFG_ESEA);    // Reset SCU Register
#endif

      if ((LPC_CGU->BASE_USB1_CLK & 1U) ==  0U) {
        LPC_CCU1->CLK_USB1_CFG    &= ~1U;               // Disable USB1 Base Clock
        while (LPC_CCU1->CLK_USB1_STAT    & 1U);
        LPC_CCU1->CLK_M3_USB1_CFG &= ~1U;               // Disable USB1 Register Interface Clock
        while (LPC_CCU1->CLK_M3_USB1_STAT & 1U);
        LPC_CGU->BASE_USB1_CLK     =  1U;               // Disable Base Clock
      }
      break;

    case ARM_POWER_FULL:
      if ((USB1_state & USBD_DRIVER_INITIALIZED) == 0U) { return ARM_DRIVER_ERROR; }
      if ((USB1_state & USBD_DRIVER_POWERED)     != 0U) { return ARM_DRIVER_OK; }

      LPC_CGU->BASE_USB1_CLK     = (0x01U << 11) |      // Auto-block Enable
                                   (0x0CU << 24) ;      // Clock source: IDIVA
      LPC_CCU1->CLK_M3_USB1_CFG |=  1U;                 // Enable USB1 Register Interface Clock
      while (!(LPC_CCU1->CLK_M3_USB1_STAT & 1U));
      LPC_CCU1->CLK_USB1_CFG    |=  1U;                 // Enable USB1 Base Clock
      while (!(LPC_CCU1->CLK_USB1_STAT    & 1U));

      // Reset USB Controller
      LPC_USBx->USBCMD_D = USB_USBCMD_D_RST;
      while (LPC_USBx->USBCMD_D & (USB_USBCMD_D_RS | USB_USBCMD_D_RST));

      // Force device mode and set Setup lockouts off
      LPC_USBx->USBMODE_D =  USB_USBMODE_D_CM1_0(2U) | USB_USBMODE_D_SLOM;

      // Clear Transceiver Selection
      LPC_USBx->PORTSC1_D &= ~(USB_PORTSC1_D_PTS_MSK | USB_PORTSC1_D_PFSC | USB_PORTSC1_D_PHCD);

      USBD_Reset ();                                    // Reset variables and endpoint settings

#if  (RTE_USB_USB1_HS_PHY_EN)
      // ULPI Selected
      LPC_USBx->PORTSC1_D |=   USB_PORTSC1_D_PTS(2U);   // Activate ULPI
#else
      // Serial/1.1 PHY selected and Full-speed forced
      LPC_USBx->PORTSC1_D |=   USB_PORTSC1_D_PTS(3UL) | USB_PORTSC1_D_PFSC;
      SCU_USB1_PinConfigure (SCU_USB1_PIN_CFG_ESEA |
                             SCU_USB1_PIN_CFG_EPWR);
#endif

      // Enable interrupts
      LPC_USBx->USBINTR_D  = (USB_USBINTR_D_UE  |       // USB interrupt enable
                              USB_USBINTR_D_PCE |       // Port change detect interrupt enable
                              USB_USBINTR_D_SLE |       // Suspend interrupt enable
                              USB_USBINTR_D_URE);       // Reset interrupt enable

      USB1_state |=  USBD_DRIVER_POWERED;               // Set powered flag
      NVIC_EnableIRQ   (USB1_IRQn);                     // Enable interrupt
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceConnect (void)
  \brief       Connect USB Device.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceConnect (void) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  LPC_USBx->USBCMD_D |= USB_USBCMD_D_RS;                // Attach Device

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceDisconnect (void)
  \brief       Disconnect USB Device.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceDisconnect (void) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  LPC_USBx->USBCMD_D &= ~USB_USBCMD_D_RS;               // Detach Device

#if (RTE_USB1_IND0_PIN_EN)
  LPC_USBx->PORTSC1_D &= ~USB_PORTSC1_D_PIC1_0(1);      // Clear indicator LED0
#endif

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USBD_STATE USBD_DeviceGetState (void)
  \brief       Get current USB Device State.
  \return      Device State \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE USBD_DeviceGetState (void) {
  ARM_USBD_STATE dev_state = { 0U, 0U, 0U };
  uint32_t       portsc1_d;

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return dev_state; }

  portsc1_d = LPC_USBx->PORTSC1_D;
  dev_state = usbd_state;

  dev_state.active = ((portsc1_d & USB_PORTSC1_D_CCS) != 0U) &&
                     ((portsc1_d & USB_USBDSTS_D_SLI) == 0U)  ;

  return dev_state;
}

/**
  \fn          int32_t USBD_DeviceRemoteWakeup (void)
  \brief       Trigger USB Remote Wakeup.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceRemoteWakeup (void) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  LPC_USBx->PORTSC1_D &= ~USB_PORTSC1_D_PHCD;           // Enable PHY Clock
  LPC_USBx->PORTSC1_D |=  USB_PORTSC1_D_FPR;            // Force Port Resume

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceSetAddress (uint8_t dev_addr)
  \brief       Set USB Device Address.
  \param[in]   dev_addr  Device Address
  \return      \ref execution_status
*/
static int32_t USBD_DeviceSetAddress (uint8_t dev_addr) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  LPC_USBx->DEVICEADDR  = (dev_addr << USB_DEVICEADDR_USBADR_POS) & USB_DEVICEADDR_USBADR_MSK;
  LPC_USBx->DEVICEADDR |=  USB_DEVICEADDR_USBADRA;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_ReadSetupPacket (uint8_t *setup)
  \brief       Read setup packet received over Control Endpoint.
  \param[out]  setup  Pointer to buffer for setup packet
  \return      \ref execution_status
*/
static int32_t USBD_ReadSetupPacket (uint8_t *setup) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }
  if (setup_received                     == 0U) { return ARM_DRIVER_ERROR; }

  setup_received = 0U;
  memcpy(setup, setup_packet, 8);

  if (setup_received != 0U) {           // If new setup packet was received while this was being read
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointConfigure (uint8_t  ep_addr,
                                               uint8_t  ep_type,
                                               uint16_t ep_max_packet_size)
  \brief       Configure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type  Endpoint Type (ARM_USB_ENDPOINT_xxx)
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \return      \ref execution_status
*/
static int32_t USBD_EndpointConfigure (uint8_t  ep_addr,
                                       uint8_t  ep_type,
                                       uint16_t ep_max_packet_size) {
  dQH_t   *ptr_dqh;
  uint32_t ep_mult;
  uint32_t ep_mps;
  uint8_t  ep_num, ep_sll;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM)           { return ARM_DRIVER_ERROR; }
  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  ptr_dqh = &dQH[EP_IDX(ep_addr)];
  if (ptr_dqh->ep_active != 0U)                 { return ARM_DRIVER_ERROR_BUSY; }

  ep_num  =  EP_NUM(ep_addr);
  ep_sll  =  EP_SLL(ep_addr);
  ep_mult = (ep_max_packet_size & ARM_USB_ENDPOINT_MICROFRAME_TRANSACTIONS_MASK) >> 11;
  ep_mps  =  ep_max_packet_size & ARM_USB_ENDPOINT_MAX_PACKET_SIZE_MASK;

  // Clear Endpoint Queue Head
  memset((void *)ptr_dqh, 0, sizeof(dQH_t));

  ptr_dqh->ep_type = ep_type;
  if (ep_type == ARM_USB_ENDPOINT_ISOCHRONOUS) {
    // For isochronous endpoints number of transactions per microframe in high-speed (or frame in full-speed)
    // has to be 1 more than additional transactions per microframe for high-speed (or 1 for full-speed)
    ep_mult++;
  }

  if ((ep_mult > 1U) && (usbd_state.speed == ARM_USB_SPEED_FULL)) { ep_mult = 1U; }

  ptr_dqh->cap       = ((ep_mult << USB_EPQH_CAP_MULT_POS) & USB_EPQH_CAP_MULT_MSK) |
                        (USB_EPQH_CAP_MAX_PACKET_LEN(ep_mps))                       |
                        (USB_EPQH_CAP_ZLT)                                          |
                       ((ep_addr == 0U) * USB_EPQH_CAP_IOS);
  ptr_dqh->next_dTD  = 1U;
  ptr_dqh->dTD_token = 0U;

  USBD_HW_EndpointFlush(ep_addr);

  // Clear Endpoint Control Settings
  ENDPTCTRL(ep_num) &= ~((USB_ENDPTCTRL_RXS     |
                          USB_ENDPTCTRL_RXT_MSK |
                          USB_ENDPTCTRL_RXI     |
                          USB_ENDPTCTRL_RXR     |
                          USB_ENDPTCTRL_RXE     )
                          << ep_sll);

  // Set Endpoint Control Settings
  ENDPTCTRL(ep_num) |=   (USB_ENDPTCTRL_RXT(ep_type) |  // Endpoint Type
                          USB_ENDPTCTRL_RXR          |  // Data Tggle Rset
                          USB_ENDPTCTRL_RXE          )  // Endpoint Enable
                          << ep_sll;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointUnconfigure (uint8_t ep_addr)
  \brief       Unconfigure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t USBD_EndpointUnconfigure (uint8_t ep_addr) {
  dQH_t   *ptr_dqh;
  dTD_t   *ptr_dtd;
  uint8_t  ep_idx, ep_num, ep_sll;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM)           { return ARM_DRIVER_ERROR; }
  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  ep_idx  =  EP_IDX(ep_addr);
  ptr_dqh = &dQH[ep_idx];
  if (ptr_dqh->ep_active != 0U)                 { return ARM_DRIVER_ERROR_BUSY; }

  ptr_dtd = &dTD[ep_idx];
  ep_sll  =  EP_SLL(ep_addr);

  // Clear Endpoint Control Settings
  ENDPTCTRL(ep_num) &= ~((USB_ENDPTCTRL_RXS     |
                          USB_ENDPTCTRL_RXT_MSK |
                          USB_ENDPTCTRL_RXI     |
                          USB_ENDPTCTRL_RXR     |
                          USB_ENDPTCTRL_RXE     )
                          << ep_sll);

  ENDPTCTRL(ep_num)  |=  (USB_ENDPTCTRL_RXR << ep_sll);         // Data toggle reset

  // Clear Endpoint Queue Head and Endpoint Transfer Descriptor
  memset((void *)ptr_dqh, 0, sizeof(dQH_t));
  memset((void *)ptr_dtd, 0, sizeof(dTD_t));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointStall (uint8_t ep_addr, bool stall)
  \brief       Set/Clear Stall for USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   stall  Operation
                - \b false Clear
                - \b true Set
  \return      \ref execution_status
*/
static int32_t USBD_EndpointStall (uint8_t ep_addr, bool stall) {
  dQH_t   *ptr_dqh;
  uint8_t  ep_num, ep_sll;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM)           { return ARM_DRIVER_ERROR; }
  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  ptr_dqh = &dQH[EP_IDX(ep_addr)];
  if (ptr_dqh->ep_active != 0U)                 { return ARM_DRIVER_ERROR_BUSY; }

  ep_sll  =  EP_SLL(ep_addr);

  if (stall != 0U) {                    // Set endpoint stall
    ENDPTCTRL(ep_num)  |=  (USB_ENDPTCTRL_RXS << ep_sll);
  } else {                              // Clear endpoint stall
    ENDPTCTRL(ep_num)  &= ~(USB_ENDPTCTRL_RXS << ep_sll);

    ptr_dqh->dTD_token  = 0U;

    USBD_HW_EndpointFlush(ep_addr);

    ENDPTCTRL(ep_num)  |=  (USB_ENDPTCTRL_RXR << ep_sll);       // Data toggle reset
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)
  \brief       Read data from or Write data to USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[out]  data Pointer to buffer for data to read or with data to write
  \param[in]   num  Number of data bytes to transfer
  \return      \ref execution_status
*/
static int32_t USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num) {
  dQH_t   *ptr_dqh;

  if (EP_NUM(ep_addr) > USBD_MAX_ENDPOINT_NUM)  { return ARM_DRIVER_ERROR; }
  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  ptr_dqh = &dQH[EP_IDX(ep_addr)];
  if (ptr_dqh->ep_active != 0U)                 { return ARM_DRIVER_ERROR_BUSY; }

  ptr_dqh->ep_active = 1U;

  ptr_dqh->data                  = data;
  ptr_dqh->num                   = num;
  ptr_dqh->num_transferred_total = 0U;
  ptr_dqh->num_transferring      = 0U;

  USBD_HW_EndpointTransfer(ep_addr);    // Start transfer

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t USBD_EndpointTransferGetResult (uint8_t ep_addr)
  \brief       Get result of USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      number of successfully transferred data bytes
*/
static uint32_t USBD_EndpointTransferGetResult (uint8_t ep_addr) {

  if (EP_NUM(ep_addr) > USBD_MAX_ENDPOINT_NUM) { return 0U; }

  return (dQH[EP_IDX(ep_addr)].num_transferred_total);
}

/**
  \fn          int32_t USBD_EndpointTransferAbort (uint8_t ep_addr)
  \brief       Abort current USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t USBD_EndpointTransferAbort (uint8_t ep_addr) {
  dQH_t   *ptr_dqh;
  uint32_t ep_msk;
  uint8_t  ep_num, ep_sll;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM)           { return ARM_DRIVER_ERROR; }
  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  ptr_dqh = &dQH[EP_IDX(ep_addr)];
  ep_msk  =  EP_MSK(ep_addr);
  ep_sll  =  EP_SLL(ep_addr);

  USBD_HW_EndpointFlush(ep_addr);

  LPC_USBx->ENDPTCOMPLETE = ep_msk;                     // Clear Completed Flag
  ENDPTCTRL(ep_num)  |=  (USB_ENDPTCTRL_RXR << ep_sll); // Data toggle reset

  ptr_dqh->dTD_token &= ~0xFFU;

  ptr_dqh->ep_active  =  0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          uint16_t USBD_GetFrameNumber (void)
  \brief       Get current USB Frame Number.
  \return      Frame Number
*/
static uint16_t USBD_GetFrameNumber (void) {

  if ((USB1_state & USBD_DRIVER_POWERED) == 0U) { return 0U; }

  return ((LPC_USBx->FRINDEX_D & USB_FRINDEX_D_FRINDEX13_3_MSK) >> USB_FRINDEX_D_FRINDEX13_3_POS);
}

/**
  \fn          void USBD1_IRQ (void)
  \brief       USB1 Device Interrupt Routine (IRQ).
*/
void USBD1_IRQ (void) {
  dQH_t   *ptr_dqh;
  uint32_t sts, cmpl;
  uint16_t ep_mps, num_transferred;
  uint8_t  ep_num, ep_addr;

  sts  = LPC_USBx->USBSTS_D & LPC_USBx->USBINTR_D;      // Read active interrupts
  cmpl = LPC_USBx->ENDPTCOMPLETE;                       // Read Endpoint completed status
  LPC_USBx->USBSTS_D      = sts;                        // Clear interrupts
  LPC_USBx->ENDPTCOMPLETE = cmpl;                       // Clear Endpoint completed status

  if ((sts & USB_USBDSTS_D_URI) != 0U) {                // Reset interrupt
    USBD_Reset();
    SignalDeviceEvent(ARM_USBD_EVENT_RESET);
  }

  if ((sts & USB_USBDSTS_D_SLI) != 0U) {                // Suspend interrupt
    SignalDeviceEvent(ARM_USBD_EVENT_SUSPEND);

#if (RTE_USB1_IND0_PIN_EN)
    LPC_USBx->PORTSC1_D &= ~USB_PORTSC1_D_PIC1_0(1);    // Clear indicator LED0
#endif
  }

  if ((sts & USB_USBDSTS_D_PCI) != 0U) {                // Port change detect interrupt
    if (((LPC_USBx->PORTSC1_D & USB_PORTSC1_D_PSPD_MSK) >> USB_PORTSC1_D_PSPD_POS) == 2U) {
      usbd_state.speed = ARM_USB_SPEED_HIGH;
      SignalDeviceEvent(ARM_USBD_EVENT_HIGH_SPEED);
    } else {
      usbd_state.speed = ARM_USB_SPEED_FULL;
    }

#if (RTE_USB1_IND0_PIN_EN)
    LPC_USBx->PORTSC1_D |= USB_PORTSC1_D_PIC1_0(1);     // Set indicator LED0
#endif
    SignalDeviceEvent(ARM_USBD_EVENT_RESUME);
  }

  if ((sts & USB_USBDSTS_D_UI) != 0U) {                 // USB interrupt - completed transfer
    if ((LPC_USBx->ENDPTSETUPSTAT) != 0U) {             // Setup Packet Received
      USBD_HW_ReadSetupPacket();
      setup_received = 1U;
      SignalEndpointEvent(0, ARM_USBD_EVENT_SETUP);
    }

    if ((cmpl & USB_ENDPTCOMPLETE_ETCE_MSK) != 0U) {    // IN Data Sent
      for (ep_num = 0U; ep_num <= USBD_MAX_ENDPOINT_NUM; ep_num++) {
        if ((cmpl & USB_ENDPTCOMPLETE_ETCE_MSK) & (1U << (ep_num + USB_ENDPTCOMPLETE_ETCE_POS))) {
          ep_addr =  ep_num | ARM_USB_ENDPOINT_DIRECTION_MASK;
          ptr_dqh = &dQH[EP_IDX(ep_addr)];

          ptr_dqh->num_transferred_total += ptr_dqh->num_transferring;

          // Check if all required IN data was sent
          if (ptr_dqh->num == ptr_dqh->num_transferred_total) {
            ptr_dqh->ep_active = 0U;                            // Clear Endpoint busy flag
            SignalEndpointEvent(ep_addr, ARM_USBD_EVENT_IN);    // Send IN event
          } else if (ptr_dqh->ep_active != 0U) {
            USBD_HW_EndpointTransfer (ep_addr);                 // If this was not last transfer, start next
          }
        }
      }
    }

    if ((cmpl & USB_ENDPTCOMPLETE_ERCE_MSK) != 0U) {    // OUT Data Received
      for (ep_num = 0U; ep_num <= USBD_MAX_ENDPOINT_NUM; ep_num++) {
        if ((cmpl & USB_ENDPTCOMPLETE_ERCE_MSK) & (1 << ep_num)) {
          ep_addr =  ep_num;
          ptr_dqh = &dQH[EP_IDX(ep_addr)];
          ep_mps  = (ptr_dqh->cap & USB_EPQH_CAP_MAX_PACKET_LEN_MSK) >> USB_EPQH_CAP_MAX_PACKET_LEN_POS;

          num_transferred = ptr_dqh->num_transferring - 
                           ((ptr_dqh->dTD_token & USB_bTD_TOKEN_TOTAL_BYTES_MSK) >> USB_bTD_TOKEN_TOTAL_BYTES_POS);
          ptr_dqh->num_transferred_total += num_transferred;

          // Check if all OUT data was received:
          //  - data terminated with ZLP or short packet or
          //  - all required data received
          if (((num_transferred % ep_mps) != 0U) || (ptr_dqh->num == ptr_dqh->num_transferred_total)) {
            ptr_dqh->ep_active = 0U;                            // Clear Endpoint busy flag
            SignalEndpointEvent(ep_addr, ARM_USBD_EVENT_OUT);   // Send OUT event
          } else if (ptr_dqh->ep_active != 0U) {
            USBD_HW_EndpointTransfer (ep_addr);                 // If this was not last transfer, start next
          }
        }
      }
    }
  }
}

ARM_DRIVER_USBD Driver_USBD1 = {
  USBD_GetVersion,
  USBD_GetCapabilities,
  USBD_Initialize,
  USBD_Uninitialize,
  USBD_PowerControl,
  USBD_DeviceConnect,
  USBD_DeviceDisconnect,
  USBD_DeviceGetState,
  USBD_DeviceRemoteWakeup,
  USBD_DeviceSetAddress,
  USBD_ReadSetupPacket,
  USBD_EndpointConfigure,
  USBD_EndpointUnconfigure,
  USBD_EndpointStall,
  USBD_EndpointTransfer,
  USBD_EndpointTransferGetResult,
  USBD_EndpointTransferAbort,
  USBD_GetFrameNumber
};
