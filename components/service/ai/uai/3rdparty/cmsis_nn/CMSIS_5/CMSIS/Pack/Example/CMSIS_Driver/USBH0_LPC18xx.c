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
 * $Revision:    V2.4
 *
 * Driver:       Driver_USBH0_HCI
 * Configured:   via RTE_Device.h configuration file
 * Project:      USB Host 0 HCI Controller (EHCI) Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                  Value
 *   ---------------------                  -----
 *   Connect to hardware via Driver_USBH# = 0
 *   USB Host controller interface        = EHCI
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.4
 *    Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.3
 *    PowerControl for Power OFF and Uninitialize functions made unconditional
 *  Version 2.2
 *    Updated in accordance with USB Device Driver
 *  Version 2.1
 *    Moved register initialization and uninitialization to PowerControl
 *    function and removed from Initialize/Uninitialize functions
 *    Pin configuration moved to USB_LPC18xx_USB0.c
 *  Version 2.0
 *    Initial release for USB Host EHCI Driver API v2.0
 *  Version 1.0
 *    Initial release
 */


#include "Driver_USBH.h"

#include "LPC18xx.h"
#include "USB_LPC18xx.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if      (RTE_USB_USB0 == 0)
#error   "USB0 is not enabled in the RTE_Device.h!"
#endif

extern uint8_t USB0_role;
extern uint8_t USB0_state;

extern void USB0_PinsConfigure   (void);
extern void USB0_PinsUnconfigure (void);


// USBH EHCI Driver ************************************************************

#define ARM_USBH_EHCI_DRIVER_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,4)

// Driver Version
static const ARM_DRIVER_VERSION usbh_ehci_driver_version = { ARM_USBH_API_VERSION, ARM_USBH_EHCI_DRIVER_VERSION };

// Driver Capabilities
static const ARM_USBH_HCI_CAPABILITIES usbh_ehci_driver_capabilities = {
  0x0001U       // Root HUB available Ports Mask
};

static ARM_USBH_HCI_Interrupt_t EHCI_IRQ;

// USBH EHCI Driver functions

/**
  \fn          ARM_DRIVER_VERSION USBH_HCI_GetVersion (void)
  \brief       Get USB Host HCI (OHCI/EHCI) driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBH_HCI_GetVersion (void) { return usbh_ehci_driver_version; }

/**
  \fn          ARM_USBH_HCI_CAPABILITIES USBH_HCI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBH_HCI_CAPABILITIES
*/
static ARM_USBH_HCI_CAPABILITIES USBH_HCI_GetCapabilities (void) { return usbh_ehci_driver_capabilities; }

/**
  \fn          int32_t USBH_HCI_Initialize (ARM_USBH_HCI_Interrupt_t *cb_interrupt)
  \brief       Initialize USB Host HCI (OHCI/EHCI) Interface.
  \param[in]   cb_interrupt Pointer to Interrupt Handler Routine
  \return      \ref execution_status
*/
static int32_t USBH_HCI_Initialize (ARM_USBH_HCI_Interrupt_t cb_interrupt) {

  if ((USB0_state & USBH_DRIVER_INITIALIZED) != 0U) { return ARM_DRIVER_OK; }

  EHCI_IRQ = cb_interrupt;

  USB0_role   =  ARM_USB_ROLE_HOST;
  USB0_PinsConfigure ();

  USB0_state  =  USBH_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_HCI_Uninitialize (void)
  \brief       De-initialize USB Host HCI (OHCI/EHCI) Interface.
  \return      \ref execution_status
*/
static int32_t USBH_HCI_Uninitialize (void) {

  USB0_PinsUnconfigure ();
  USB0_role   =  ARM_USB_ROLE_NONE;
  USB0_state &= ~USBH_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_HCI_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Host HCI (OHCI/EHCI) Interface Power.
  \param[in]   state Power state
  \return      \ref execution_status
*/
static int32_t USBH_HCI_PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      NVIC_DisableIRQ      (USB0_IRQn);                 // Disable interrupt
      NVIC_ClearPendingIRQ (USB0_IRQn);                 // Clear pending interrupt
      USB0_state &= ~USBH_DRIVER_POWERED;               // Clear powered flag
      if ((LPC_CGU->BASE_USB0_CLK & 1U) == 0U) {
        LPC_CREG->CREG0 |=  (1U << 5);                  // Disable USB0 PHY
        LPC_CCU1->CLK_USB0_CFG    &= ~1U;               // Disable USB0 Base Clock
        while (LPC_CCU1->CLK_USB0_STAT    & 1U);
        LPC_CCU1->CLK_M3_USB0_CFG &= ~1U;               // Disable USB0 Register Interface Clock
        while (LPC_CCU1->CLK_M3_USB0_STAT & 1U);
        LPC_CGU->BASE_USB0_CLK     =  1U;               // Disable Base Clock
      }
      break;

    case ARM_POWER_FULL:
      if ((USB0_state & USBH_DRIVER_INITIALIZED) == 0U) { return ARM_DRIVER_ERROR; }
      if ((USB0_state & USBH_DRIVER_POWERED)     != 0U) { return ARM_DRIVER_OK; }

      LPC_CGU->BASE_USB0_CLK     = (0x01U << 11) |      // Auto-block Enable
                                   (0x07U << 24) ;      // Clock source: PLL0USB
      LPC_CCU1->CLK_M3_USB0_CFG |=  1U;                 // Enable USB0 Register Interface Clock
      while (!(LPC_CCU1->CLK_M3_USB0_STAT & 1U));
      LPC_CCU1->CLK_USB0_CFG    |=  1U;                 // Enable USB0 Base Clock
      while (!(LPC_CCU1->CLK_USB0_STAT    & 1U));
      LPC_CREG->CREG0 &= ~(1U << 5);                    // Enable USB0 PHY

      USB0_state |=  USBH_DRIVER_POWERED;               // Set powered flag
      NVIC_EnableIRQ   (USB0_IRQn);                     // Enable interrupt
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_HCI_PortVbusOnOff (uint8_t port, bool vbus)
  \brief       USB Host HCI (OHCI/EHCI) Root HUB Port VBUS on/off.
  \param[in]   port  Root HUB Port Number
  \param[in]   vbus
                - \b false VBUS off
                - \b true  VBUS on
  \return      \ref execution_status
*/
static int32_t USBH_HCI_PortVbusOnOff (uint8_t port, bool power) {
  // No GPIO pins used for VBUS control it is controlled by EHCI Controller

  if (((1U << port) & usbh_ehci_driver_capabilities.port_mask) == 0U) { return ARM_DRIVER_ERROR; }
  return ARM_DRIVER_OK;
}

/**
  \fn          void USBH0_IRQ (void)
  \brief       USB0 Host Interrupt Routine (IRQ).
*/
void USBH0_IRQ (void) {
  EHCI_IRQ();
}

ARM_DRIVER_USBH_HCI Driver_USBH0_HCI = {
  USBH_HCI_GetVersion,
  USBH_HCI_GetCapabilities,
  USBH_HCI_Initialize,
  USBH_HCI_Uninitialize,
  USBH_HCI_PowerControl,
  USBH_HCI_PortVbusOnOff
};
