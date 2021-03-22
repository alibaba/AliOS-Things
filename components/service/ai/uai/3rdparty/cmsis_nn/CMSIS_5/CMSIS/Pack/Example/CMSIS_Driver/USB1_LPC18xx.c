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
 * $Revision:    V1.1
 *
 * Project:      USB common (Device and Host) module for NXP LPC18xx
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.1
 *    Improved support for Host and Device
 *  Version 1.0
 *    Initial release
 */

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"

#include "Driver_USB.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

volatile uint8_t USB1_role  = ARM_USB_ROLE_NONE;
volatile uint8_t USB1_state = 0U;

#ifdef RTE_Drivers_USBH1
extern void USBH1_IRQ (void);
#endif
#ifdef RTE_Drivers_USBD1
extern void USBD1_IRQ (void);
#endif


// Common IRQ Routine **********************************************************

/**
  \fn          void USB1_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void USB1_IRQHandler (void) {
#if (defined(RTE_Drivers_USBH1) && defined(RTE_Drivers_USBD1))
  switch (USB1_role) {
#ifdef RTE_Drivers_USBH1
    case ARM_USB_ROLE_HOST:
      USBH1_IRQ ();
      break;
#endif
#ifdef RTE_Drivers_USBD1
    case ARM_USB_ROLE_DEVICE:
      USBD1_IRQ ();
      break;
#endif
    default:
      break;
  }
#else
#ifdef RTE_Drivers_USBH1
  USBH1_IRQ ();
#else
  USBD1_IRQ ();
#endif
#endif

}


// Public Functions ************************************************************

/**
  \fn          void USB1_PinsConfigure (void)
  \brief       Configure USB pins
*/
void USB1_PinsConfigure (void) {

  // Common (Device and Host) Pins
#if (RTE_USB1_IND0_PIN_EN)
  SCU_PinConfigure(RTE_USB1_IND0_PORT, RTE_USB1_IND0_BIT, RTE_USB1_IND0_FUNC);
#endif
#if (RTE_USB1_IND1_PIN_EN)
  SCU_PinConfigure(RTE_USB1_IND1_PORT, RTE_USB1_IND1_BIT, RTE_USB1_IND1_FUNC);
#endif

#if (RTE_USB_USB1_FS_PHY_EN)
#if (RTE_USB1_VBUS_PIN_EN)
  // Device Pin
  if (USB1_role == ARM_USB_ROLE_DEVICE) {
    SCU_PinConfigure(RTE_USB1_VBUS_PORT, RTE_USB1_VBUS_BIT, RTE_USB1_VBUS_FUNC | SCU_SFS_EPD | SCU_SFS_EZI);
  }
#endif
#endif

  // Host Pins
  if (USB1_role == ARM_USB_ROLE_HOST) {
#if (RTE_USB1_PPWR_PIN_EN)
    SCU_PinConfigure(RTE_USB1_PPWR_PORT,      RTE_USB1_PPWR_BIT,      RTE_USB1_PPWR_FUNC);
#endif
#if (RTE_USB1_PWR_FAULT_PIN_EN)
    SCU_PinConfigure(RTE_USB1_PWR_FAULT_PORT, RTE_USB1_PWR_FAULT_BIT, RTE_USB1_PWR_FAULT_FUNC);
#endif
  }

  // ULPI Pins
#if (RTE_USB_USB1_HS_PHY_EN)
  SCU_PinConfigure(RTE_USB1_ULPI_CLK_PORT, RTE_USB1_ULPI_CLK_BIT,  RTE_USB1_ULPI_CLK_FUNC);
  SCU_PinConfigure(RTE_USB1_ULPI_DIR_PORT, RTE_USB1_ULPI_DIR_BIT,  RTE_USB1_ULPI_DIR_FUNC);
  SCU_PinConfigure(RTE_USB1_ULPI_STP_PORT, RTE_USB1_ULPI_STP_BIT,  RTE_USB1_ULPI_STP_FUNC);
  SCU_PinConfigure(RTE_USB1_ULPI_NXT_PORT, RTE_USB1_ULPI_NXT_BIT,  RTE_USB1_ULPI_NXT_FUNC);
  SCU_PinConfigure(RTE_USB1_ULPI_D0_PORT,  RTE_USB1_ULPI_D0_BIT,   RTE_USB1_ULPI_D0_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D1_PORT,  RTE_USB1_ULPI_D1_BIT,   RTE_USB1_ULPI_D1_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D2_PORT,  RTE_USB1_ULPI_D2_BIT,   RTE_USB1_ULPI_D2_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D3_PORT,  RTE_USB1_ULPI_D3_BIT,   RTE_USB1_ULPI_D3_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D4_PORT,  RTE_USB1_ULPI_D4_BIT,   RTE_USB1_ULPI_D4_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D5_PORT,  RTE_USB1_ULPI_D5_BIT,   RTE_USB1_ULPI_D5_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D6_PORT,  RTE_USB1_ULPI_D6_BIT,   RTE_USB1_ULPI_D6_FUNC );
  SCU_PinConfigure(RTE_USB1_ULPI_D7_PORT,  RTE_USB1_ULPI_D7_BIT,   RTE_USB1_ULPI_D7_FUNC );
#endif
}

/**
  \fn          void USB1_PinsUnconfigure (void)
  \brief       De-configure USB pins
*/
void USB1_PinsUnconfigure (void) {

  // Common (Device and Host) Pins
#if (RTE_USB1_IND0_PIN_EN)
  SCU_PinConfigure(RTE_USB1_IND0_PORT, RTE_USB1_IND0_BIT, 0);
#endif
#if (RTE_USB1_IND1_PIN_EN)
  SCU_PinConfigure(RTE_USB1_IND1_PORT, RTE_USB1_IND1_BIT, 0);
#endif

#if (RTE_USB_USB1_FS_PHY_EN)
#if (RTE_USB1_VBUS_PIN_EN)
  // Device Pin
  if (USB1_role == ARM_USB_ROLE_DEVICE) {
    SCU_PinConfigure(RTE_USB1_VBUS_PORT, RTE_USB1_VBUS_BIT, 0);
  }
#endif
#endif

  // Host Pins
  if (USB1_role == ARM_USB_ROLE_HOST) {
#if (RTE_USB1_PPWR_PIN_EN)
    SCU_PinConfigure(RTE_USB1_PPWR_PORT,      RTE_USB1_PPWR_BIT,      0);
#endif
#if (RTE_USB1_PWR_FAULT_PIN_EN)
    SCU_PinConfigure(RTE_USB1_PWR_FAULT_PORT, RTE_USB1_PWR_FAULT_BIT, 0);
#endif
  }

  // ULPI Pins
#if (RTE_USB_USB1_HS_PHY_EN)
  SCU_PinConfigure(RTE_USB1_ULPI_CLK_PORT, RTE_USB1_ULPI_CLK_BIT,  0);
  SCU_PinConfigure(RTE_USB1_ULPI_DIR_PORT, RTE_USB1_ULPI_DIR_BIT,  0);
  SCU_PinConfigure(RTE_USB1_ULPI_STP_PORT, RTE_USB1_ULPI_STP_BIT,  0);
  SCU_PinConfigure(RTE_USB1_ULPI_NXT_PORT, RTE_USB1_ULPI_NXT_BIT,  0);
  SCU_PinConfigure(RTE_USB1_ULPI_D0_PORT,  RTE_USB1_ULPI_D0_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D1_PORT,  RTE_USB1_ULPI_D1_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D2_PORT,  RTE_USB1_ULPI_D2_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D3_PORT,  RTE_USB1_ULPI_D3_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D4_PORT,  RTE_USB1_ULPI_D4_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D5_PORT,  RTE_USB1_ULPI_D5_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D6_PORT,  RTE_USB1_ULPI_D6_BIT,   0 );
  SCU_PinConfigure(RTE_USB1_ULPI_D7_PORT,  RTE_USB1_ULPI_D7_BIT,   0 );
#endif
}
