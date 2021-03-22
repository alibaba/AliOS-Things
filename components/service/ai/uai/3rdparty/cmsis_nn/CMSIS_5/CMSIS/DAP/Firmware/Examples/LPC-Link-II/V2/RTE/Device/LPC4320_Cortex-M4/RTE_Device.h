/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2016 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        25. April 2016
 * $Revision:    V2.2.1
 *
 * Project:      RTE Device Configuration for NXP LPC43xx
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


// <e> USB0 Controller [Driver_USBD0 and Driver_USBH0]
// <i> Configuration settings for Driver_USBD0 in component ::Drivers:USB Device
// <i> Configuration settings for Driver_USBH0 in component ::Drivers:USB Host
#define   RTE_USB_USB0                  1

//   <h> Pin Configuration
//     <o> USB0_PPWR (Host) <0=>Not used <1=>P1_7 <2=>P2_0 <3=>P2_3 <4=>P6_3
//     <i> VBUS drive signal (towards external charge pump or power management unit).
#define   RTE_USB0_PPWR_ID              0
#if      (RTE_USB0_PPWR_ID == 0)
  #define RTE_USB0_PPWR_PIN_EN          0
#elif    (RTE_USB0_PPWR_ID == 1)
  #define RTE_USB0_PPWR_PORT            1
  #define RTE_USB0_PPWR_BIT             7
  #define RTE_USB0_PPWR_FUNC            4
#elif    (RTE_USB0_PPWR_ID == 2)
  #define RTE_USB0_PPWR_PORT            2
  #define RTE_USB0_PPWR_BIT             0
  #define RTE_USB0_PPWR_FUNC            3
#elif    (RTE_USB0_PPWR_ID == 3)
  #define RTE_USB0_PPWR_PORT            2
  #define RTE_USB0_PPWR_BIT             3
  #define RTE_USB0_PPWR_FUNC            7
#elif    (RTE_USB0_PPWR_ID == 4)
  #define RTE_USB0_PPWR_PORT            6
  #define RTE_USB0_PPWR_BIT             3
  #define RTE_USB0_PPWR_FUNC            1
#else
  #error "Invalid RTE_USB0_PPWR Pin Configuration!"
#endif
#ifndef   RTE_USB0_PPWR_PIN_EN
  #define RTE_USB0_PPWR_PIN_EN          1
#endif
//     <o> USB0_PWR_FAULT (Host) <0=>Not used <1=>P1_5 <2=>P2_1 <3=>P2_4 <4=>P6_6 <5=>P8_0
//     <i> Port power fault signal indicating overcurrent condition.
//     <i> This signal monitors over-current on the USB bus
//        (external circuitry required to detect over-current condition).
#define   RTE_USB0_PWR_FAULT_ID         0
#if      (RTE_USB0_PWR_FAULT_ID == 0)
  #define RTE_USB0_PWR_FAULT_PIN_EN     0
#elif    (RTE_USB0_PWR_FAULT_ID == 1)
  #define RTE_USB0_PWR_FAULT_PORT       1
  #define RTE_USB0_PWR_FAULT_BIT        5
  #define RTE_USB0_PWR_FAULT_FUNC       4
#elif    (RTE_USB0_PWR_FAULT_ID == 2)
  #define RTE_USB0_PWR_FAULT_PORT       2
  #define RTE_USB0_PWR_FAULT_BIT        1
  #define RTE_USB0_PWR_FAULT_FUNC       3
#elif    (RTE_USB0_PWR_FAULT_ID == 3)
  #define RTE_USB0_PWR_FAULT_PORT       2
  #define RTE_USB0_PWR_FAULT_BIT        4
  #define RTE_USB0_PWR_FAULT_FUNC       7
#elif    (RTE_USB0_PWR_FAULT_ID == 4)
  #define RTE_USB0_PWR_FAULT_PORT       6
  #define RTE_USB0_PWR_FAULT_BIT        6
  #define RTE_USB0_PWR_FAULT_FUNC       3
#elif    (RTE_USB0_PWR_FAULT_ID == 5)
  #define RTE_USB0_PWR_FAULT_PORT       8
  #define RTE_USB0_PWR_FAULT_BIT        0
  #define RTE_USB0_PWR_FAULT_FUNC       1
#else
  #error "Invalid RTE_USB0_PWR_FAULT Pin Configuration!"
#endif
#ifndef   RTE_USB0_PWR_FAULT_PIN_EN
  #define RTE_USB0_PWR_FAULT_PIN_EN     1
#endif
//     <o> USB0_IND0 <0=>Not used <1=>P1_4 <2=>P2_5 <3=>P2_6 <4=>P6_8 <5=>P8_2
//     <i> USB0 port indicator LED control output 0
#define   RTE_USB0_IND0_ID              0
#if      (RTE_USB0_IND0_ID == 0)
  #define RTE_USB0_IND0_PIN_EN          0
#elif    (RTE_USB0_IND0_ID == 1)
  #define RTE_USB0_IND0_PORT            1
  #define RTE_USB0_IND0_BIT             4
  #define RTE_USB0_IND0_FUNC            4
#elif    (RTE_USB0_IND0_ID == 2)
  #define RTE_USB0_IND0_PORT            2
  #define RTE_USB0_IND0_BIT             5
  #define RTE_USB0_IND0_FUNC            7
#elif    (RTE_USB0_IND0_ID == 3)
  #define RTE_USB0_IND0_PORT            2
  #define RTE_USB0_IND0_BIT             6
  #define RTE_USB0_IND0_FUNC            3
#elif    (RTE_USB0_IND0_ID == 4)
  #define RTE_USB0_IND0_PORT            6
  #define RTE_USB0_IND0_BIT             8
  #define RTE_USB0_IND0_FUNC            3
#elif    (RTE_USB0_IND0_ID == 5)
  #define RTE_USB0_IND0_PORT            8
  #define RTE_USB0_IND0_BIT             2
  #define RTE_USB0_IND0_FUNC            1
#else
  #error "Invalid RTE_USB0_IND0 Pin Configuration!"
#endif
#ifndef   RTE_USB0_IND0_PIN_EN
  #define RTE_USB0_IND0_PIN_EN          1
#endif
//     <o> USB0_IND1 <0=>Not used <1=>P1_3 <2=>P2_2 <3=>P6_7 <4=>P8_1
//     <i> USB0 port indicator LED control output 1
#define   RTE_USB0_IND1_ID              0
#if      (RTE_USB0_IND1_ID == 0)
  #define RTE_USB0_IND1_PIN_EN          0
#elif    (RTE_USB0_IND1_ID == 1)
  #define RTE_USB0_IND1_PORT            1
  #define RTE_USB0_IND1_BIT             3
  #define RTE_USB0_IND1_FUNC            4
#elif    (RTE_USB0_IND1_ID == 2)
  #define RTE_USB0_IND1_PORT            2
  #define RTE_USB0_IND1_BIT             2
  #define RTE_USB0_IND1_FUNC            3
#elif    (RTE_USB0_IND1_ID == 3)
  #define RTE_USB0_IND1_PORT            6
  #define RTE_USB0_IND1_BIT             7
  #define RTE_USB0_IND1_FUNC            3
#elif    (RTE_USB0_IND1_ID == 4)
  #define RTE_USB0_IND1_PORT            8
  #define RTE_USB0_IND1_BIT             1
  #define RTE_USB0_IND1_FUNC            1
#else
  #error "Invalid RTE_USB0_IND1 Pin Configuration!"
#endif
#ifndef   RTE_USB0_IND1_PIN_EN
  #define RTE_USB0_IND1_PIN_EN          1
#endif
//   </h> Pin Configuration

//   <h> Device [Driver_USBD0]
//   <i> Configuration settings for Driver_USBD0 in component ::Drivers:USB Device
//     <o.0> High-speed
//     <i> Enable high-speed functionality
#define   RTE_USB_USB0_HS_EN            1
//   </h> Device [Driver_USBD0]
// </e> USB0 Controller [Driver_USBD0 and Driver_USBH0]

// <e> USB1 Controller [Driver_USBD1 and Driver_USBH1]
// <i> Configuration settings for Driver_USBD1 in component ::Drivers:USB Device
// <i> Configuration settings for Driver_USBH1 in component ::Drivers:USB Host
#define   RTE_USB_USB1                  0

//   <h> Pin Configuration
//     <o> USB1_PPWR (Host) <0=>Not used <1=>P9_5
//     <i> VBUS drive signal (towards external charge pump or power management unit).
#define   RTE_USB1_PPWR_ID              1
#if      (RTE_USB1_PPWR_ID == 0)
  #define RTE_USB1_PPWR_PIN_EN          0
#elif    (RTE_USB1_PPWR_ID == 1)
  #define RTE_USB1_PPWR_PORT            9
  #define RTE_USB1_PPWR_BIT             5
  #define RTE_USB1_PPWR_FUNC            2
#else
  #error "Invalid RTE_USB1_PPWR Pin Configuration!"
#endif
#ifndef   RTE_USB1_PPWR_PIN_EN
  #define RTE_USB1_PPWR_PIN_EN          1
#endif
//     <o> USB1_PWR_FAULT (Host) <0=>Not used <1=>P9_6
//     <i> Port power fault signal indicating overcurrent condition.
//     <i> This signal monitors over-current on the USB bus
//        (external circuitry required to detect over-current condition).
#define   RTE_USB1_PWR_FAULT_ID         1
#if      (RTE_USB1_PWR_FAULT_ID == 0)
  #define RTE_USB1_PWR_FAULT_PIN_EN     0
#elif    (RTE_USB1_PWR_FAULT_ID == 1)
  #define RTE_USB1_PWR_FAULT_PORT       9
  #define RTE_USB1_PWR_FAULT_BIT        6
  #define RTE_USB1_PWR_FAULT_FUNC       2
#else
  #error "Invalid RTE_USB1_PWR_FAULT Pin Configuration!"
#endif
#ifndef   RTE_USB1_PWR_FAULT_PIN_EN
  #define RTE_USB1_PWR_FAULT_PIN_EN     1
#endif
//     <o> USB1_IND0 <0=>Not used <1=>P3_2 <2=>P9_4
//     <i> USB1 port indicator LED control output 0
#define   RTE_USB1_IND0_ID              1
#if      (RTE_USB1_IND0_ID == 0)
  #define RTE_USB1_IND0_PIN_EN          0
#elif    (RTE_USB1_IND0_ID == 1)
  #define RTE_USB1_IND0_PORT            3
  #define RTE_USB1_IND0_BIT             2
  #define RTE_USB1_IND0_FUNC            3
#elif    (RTE_USB1_IND0_ID == 2)
  #define RTE_USB1_IND0_PORT            9
  #define RTE_USB1_IND0_BIT             4
  #define RTE_USB1_IND0_FUNC            2
#else
  #error "Invalid RTE_USB1_IND0 Pin Configuration!"
#endif
#ifndef   RTE_USB1_IND0_PIN_EN
  #define RTE_USB1_IND0_PIN_EN          1
#endif
//     <o> USB1_IND1 <0=>Not used <1=>P3_1 <2=>P9_3
//     <i> USB1 port indicator LED control output 1
#define   RTE_USB1_IND1_ID              1
#if      (RTE_USB1_IND1_ID == 0)
  #define RTE_USB1_IND1_PIN_EN          0
#elif    (RTE_USB1_IND1_ID == 1)
  #define RTE_USB1_IND1_PORT            3
  #define RTE_USB1_IND1_BIT             1
  #define RTE_USB1_IND1_FUNC            3
#elif    (RTE_USB1_IND1_ID == 2)
  #define RTE_USB1_IND1_PORT            9
  #define RTE_USB1_IND1_BIT             3
  #define RTE_USB1_IND1_FUNC            2
#else
  #error "Invalid RTE_USB1_IND1 Pin Configuration!"
#endif
#ifndef   RTE_USB1_IND1_PIN_EN
  #define RTE_USB1_IND1_PIN_EN          1
#endif

//     <e> On-chip full-speed PHY
#define   RTE_USB_USB1_FS_PHY_EN        1

//       <o> USB1_VBUS (Device) <0=>Not used <1=>P2_5
//       <i> Monitors the presence of USB1 bus power.
#define   RTE_USB1_VBUS_ID              1
#if      (RTE_USB1_VBUS_ID == 0)
  #define RTE_USB1_VBUS_PIN_EN          0
#elif    (RTE_USB1_VBUS_ID == 1)
  #define RTE_USB1_VBUS_PORT            2
  #define RTE_USB1_VBUS_BIT             5
  #define RTE_USB1_VBUS_FUNC            2
#else
  #error "Invalid RTE_USB1_VBUS Pin Configuration!"
#endif
#ifndef   RTE_USB1_VBUS_PIN_EN
  #define RTE_USB1_VBUS_PIN_EN          1
#endif
//     </e> On-chip full-speed PHY

//     <e> External high-speed ULPI PHY (UTMI+ Low Pin Interface)
#define   RTE_USB_USB1_HS_PHY_EN        0

//       <o> USB1_ULPI_CLK <0=>P8_8 <1=>PC_0
//       <i> USB1 ULPI link CLK signal.
//       <i> 60 MHz clock generated by the PHY.
#define   RTE_USB1_ULPI_CLK_ID          0
#if      (RTE_USB1_ULPI_CLK_ID == 0)
  #define RTE_USB1_ULPI_CLK_PORT        8
  #define RTE_USB1_ULPI_CLK_BIT         8
  #define RTE_USB1_ULPI_CLK_FUNC        1
#elif    (RTE_USB1_ULPI_CLK_ID == 1)
  #define RTE_USB1_ULPI_CLK_PORT        0xC
  #define RTE_USB1_ULPI_CLK_BIT         0
  #define RTE_USB1_ULPI_CLK_FUNC        1
#else
  #error "Invalid RTE_USB1_ULPI_CLK Pin Configuration!"
#endif
//       <o> USB1_ULPI_DIR <0=>PB_1 <1=>PC_11
//       <i> USB1 ULPI link DIR signal.
//       <i> Controls the ULPI data line direction.
#define   RTE_USB1_ULPI_DIR_ID          0
#if      (RTE_USB1_ULPI_DIR_ID == 0)
  #define RTE_USB1_ULPI_DIR_PORT        0xB
  #define RTE_USB1_ULPI_DIR_BIT         1
  #define RTE_USB1_ULPI_DIR_FUNC        1
#elif    (RTE_USB1_ULPI_DIR_ID == 1)
  #define RTE_USB1_ULPI_DIR_PORT        0xC
  #define RTE_USB1_ULPI_DIR_BIT         11
  #define RTE_USB1_ULPI_DIR_FUNC        1
#else
  #error "Invalid RTE_USB1_ULPI_DIR Pin Configuration!"
#endif
//       <o> USB1_ULPI_STP <0=>P8_7 <1=>PC_10
//       <i> USB1 ULPI link STP signal.
//       <i> Asserted to end or interrupt transfers to the PHY.
#define   RTE_USB1_ULPI_STP_ID          0
#if      (RTE_USB1_ULPI_STP_ID == 0)
  #define RTE_USB1_ULPI_STP_PORT        8
  #define RTE_USB1_ULPI_STP_BIT         7
  #define RTE_USB1_ULPI_STP_FUNC        1
#elif    (RTE_USB1_ULPI_STP_ID == 1)
  #define RTE_USB1_ULPI_STP_PORT        0xC
  #define RTE_USB1_ULPI_STP_BIT         10
  #define RTE_USB1_ULPI_STP_FUNC        1
#else
  #error "Invalid RTE_USB1_ULPI_STP Pin Configuration!"
#endif
//       <o> USB1_ULPI_NXT <0=>P8_6 <1=>PC_9
//       <i> USB1 ULPI link NXT signal.
//       <i> Data flow control signal from the PHY.
#define   RTE_USB1_ULPI_NXT_ID          0
#if      (RTE_USB1_ULPI_NXT_ID == 0)
  #define RTE_USB1_ULPI_NXT_PORT        8
  #define RTE_USB1_ULPI_NXT_BIT         6
  #define RTE_USB1_ULPI_NXT_FUNC        1
#elif    (RTE_USB1_ULPI_NXT_ID == 1)
  #define RTE_USB1_ULPI_NXT_PORT        0xC
  #define RTE_USB1_ULPI_NXT_BIT         9
  #define RTE_USB1_ULPI_NXT_FUNC        1
#else
  #error "Invalid RTE_USB1_ULPI_NXT Pin Configuration!"
#endif
//       <o> USB1_ULPI_D0 <0=>P8_5 <1=>PC_8 <2=>PD_11
//       <i> USB1 ULPI link bidirectional data line 0.
#define   RTE_USB1_ULPI_D0_ID           0
#if      (RTE_USB1_ULPI_D0_ID == 0)
  #define RTE_USB1_ULPI_D0_PORT         8
  #define RTE_USB1_ULPI_D0_BIT          5
  #define RTE_USB1_ULPI_D0_FUNC         1
#elif    (RTE_USB1_ULPI_D0_ID == 1)
  #define RTE_USB1_ULPI_D0_PORT         0xC
  #define RTE_USB1_ULPI_D0_BIT          8
  #define RTE_USB1_ULPI_D0_FUNC         1
#elif    (RTE_USB1_ULPI_D0_ID == 2)
  #define RTE_USB1_ULPI_D0_PORT         0xD
  #define RTE_USB1_ULPI_D0_BIT          11
  #define RTE_USB1_ULPI_D0_FUNC         5
#else
  #error "Invalid RTE_USB1_ULPI_D0 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D1 <0=>P8_4 <1=>PC_7
//       <i> USB1 ULPI link bidirectional data line 1.
#define   RTE_USB1_ULPI_D1_ID           0
#if      (RTE_USB1_ULPI_D1_ID == 0)
  #define RTE_USB1_ULPI_D1_PORT         8
  #define RTE_USB1_ULPI_D1_BIT          4
  #define RTE_USB1_ULPI_D1_FUNC         1
#elif    (RTE_USB1_ULPI_D1_ID == 1)
  #define RTE_USB1_ULPI_D1_PORT         0xC
  #define RTE_USB1_ULPI_D1_BIT          7
  #define RTE_USB1_ULPI_D1_FUNC         1
#else
  #error "Invalid RTE_USB1_ULPI_D1 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D2 <0=>P8_3 <1=>PC_6
//       <i> USB1 ULPI link bidirectional data line 2.
#define   RTE_USB1_ULPI_D2_ID           0
#if      (RTE_USB1_ULPI_D2_ID == 0)
  #define RTE_USB1_ULPI_D2_PORT         8
  #define RTE_USB1_ULPI_D2_BIT          3
  #define RTE_USB1_ULPI_D2_FUNC         1
#elif    (RTE_USB1_ULPI_D2_ID == 1)
  #define RTE_USB1_ULPI_D2_PORT         0xC
  #define RTE_USB1_ULPI_D2_BIT          6
  #define RTE_USB1_ULPI_D2_FUNC         1
#else
  #error "Invalid RTE_USB1_ULPI_D2 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D3 <0=>PB_6 <1=>PC_5
//       <i> USB1 ULPI link bidirectional data line 3.
#define   RTE_USB1_ULPI_D3_ID           0
#if      (RTE_USB1_ULPI_D3_ID == 0)
  #define RTE_USB1_ULPI_D3_PORT         0xB
  #define RTE_USB1_ULPI_D3_BIT          6
  #define RTE_USB1_ULPI_D3_FUNC         1
#elif    (RTE_USB1_ULPI_D3_ID == 1)
  #define RTE_USB1_ULPI_D3_PORT         0xC
  #define RTE_USB1_ULPI_D3_BIT          5
  #define RTE_USB1_ULPI_D3_FUNC         1
#else
  #error "Invalid RTE_USB1_ULPI_D3 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D4 <0=>PB_5 <1=>PC_4
//       <i> USB1 ULPI link bidirectional data line 4.
#define   RTE_USB1_ULPI_D4_ID           0
#if      (RTE_USB1_ULPI_D4_ID == 0)
  #define RTE_USB1_ULPI_D4_PORT         0xB
  #define RTE_USB1_ULPI_D4_BIT          5
  #define RTE_USB1_ULPI_D4_FUNC         1
#elif    (RTE_USB1_ULPI_D4_ID == 1)
  #define RTE_USB1_ULPI_D4_PORT         0xC
  #define RTE_USB1_ULPI_D4_BIT          4
  #define RTE_USB1_ULPI_D4_FUNC         1
#else
  #error "Invalid RTE_USB1_ULPI_D4 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D5 <0=>PB_4 <1=>PC_3
//       <i> USB1 ULPI link bidirectional data line 5.
#define   RTE_USB1_ULPI_D5_ID           0
#if      (RTE_USB1_ULPI_D5_ID == 0)
  #define RTE_USB1_ULPI_D5_PORT         0xB
  #define RTE_USB1_ULPI_D5_BIT          4
  #define RTE_USB1_ULPI_D5_FUNC         1
#elif    (RTE_USB1_ULPI_D5_ID == 1)
  #define RTE_USB1_ULPI_D5_PORT         0xC
  #define RTE_USB1_ULPI_D5_BIT          3
  #define RTE_USB1_ULPI_D5_FUNC         0
#else
  #error "Invalid RTE_USB1_ULPI_D5 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D6 <0=>PB_3 <1=>PC_2
//       <i> USB1 ULPI link bidirectional data line 6.
#define   RTE_USB1_ULPI_D6_ID           0
#if      (RTE_USB1_ULPI_D6_ID == 0)
  #define RTE_USB1_ULPI_D6_PORT         0xB
  #define RTE_USB1_ULPI_D6_BIT          3
  #define RTE_USB1_ULPI_D6_FUNC         1
#elif    (RTE_USB1_ULPI_D6_ID == 1)
  #define RTE_USB1_ULPI_D6_PORT         0xC
  #define RTE_USB1_ULPI_D6_BIT          2
  #define RTE_USB1_ULPI_D6_FUNC         0
#else
  #error "Invalid RTE_USB1_ULPI_D6 Pin Configuration!"
#endif
//       <o> USB1_ULPI_D7 <0=>PB_2 <1=>PC_1
//       <i> USB1 ULPI link bidirectional data line 7.
#define   RTE_USB1_ULPI_D7_ID           0
#if      (RTE_USB1_ULPI_D7_ID == 0)
  #define RTE_USB1_ULPI_D7_PORT         0xB
  #define RTE_USB1_ULPI_D7_BIT          2
  #define RTE_USB1_ULPI_D7_FUNC         1
#elif    (RTE_USB1_ULPI_D7_ID == 1)
  #define RTE_USB1_ULPI_D7_PORT         0xC
  #define RTE_USB1_ULPI_D7_BIT          1
  #define RTE_USB1_ULPI_D7_FUNC         0
#else
  #error "Invalid RTE_USB1_ULPI_D7 Pin Configuration!"
#endif
//     </e> External high-speed ULPI PHY (UTMI+ Low Pin Interface)
//   </h> Pin Configuration
// </e> USB1 Controller [Driver_USBD1 and Driver_USBH1]

// <e> ENET (Ethernet Interface) [Driver_ETH_MAC0]
// <i> Configuration settings for Driver_ETH_MAC0 in component ::Drivers:Ethernet MAC
#define   RTE_ENET                      0

//   <e> MII (Media Independent Interface)
#define   RTE_ENET_MII                  0

//     <o> ENET_TXD0 Pin <0=>P1_18
#define   RTE_ENET_MII_TXD0_PORT_ID     0
#if      (RTE_ENET_MII_TXD0_PORT_ID == 0)
  #define RTE_ENET_MII_TXD0_PORT        1
  #define RTE_ENET_MII_TXD0_PIN         18
  #define RTE_ENET_MII_TXD0_FUNC        3
#else
  #error "Invalid ENET_TXD0 Pin Configuration!"
#endif
//     <o> ENET_TXD1 Pin <0=>P1_20
#define   RTE_ENET_MII_TXD1_PORT_ID     0
#if      (RTE_ENET_MII_TXD1_PORT_ID == 0)
  #define RTE_ENET_MII_TXD1_PORT        1
  #define RTE_ENET_MII_TXD1_PIN         20
  #define RTE_ENET_MII_TXD1_FUNC        3
#else
  #error "Invalid ENET_TXD1 Pin Configuration!"
#endif
//     <o> ENET_TXD2 Pin <0=>P9_4 <1=>PC_2
#define   RTE_ENET_MII_TXD2_PORT_ID     0
#if      (RTE_ENET_MII_TXD2_PORT_ID == 0)
  #define RTE_ENET_MII_TXD2_PORT        9
  #define RTE_ENET_MII_TXD2_PIN         4
  #define RTE_ENET_MII_TXD2_FUNC        5
#elif    (RTE_ENET_MII_TXD2_PORT_ID == 1)
  #define RTE_ENET_MII_TXD2_PORT        0xC
  #define RTE_ENET_MII_TXD2_PIN         2
  #define RTE_ENET_MII_TXD2_FUNC        3
#else
  #error "Invalid ENET_TXD2 Pin Configuration!"
#endif
//     <o> ENET_TXD3 Pin <0=>P9_5 <1=>PC_3
#define   RTE_ENET_MII_TXD3_PORT_ID     0
#if      (RTE_ENET_MII_TXD3_PORT_ID == 0)
  #define RTE_ENET_MII_TXD3_PORT        9
  #define RTE_ENET_MII_TXD3_PIN         5
  #define RTE_ENET_MII_TXD3_FUNC        5
#elif    (RTE_ENET_MII_TXD3_PORT_ID == 1)
  #define RTE_ENET_MII_TXD3_PORT        0xC
  #define RTE_ENET_MII_TXD3_PIN         3
  #define RTE_ENET_MII_TXD3_FUNC        3
#else
  #error "Invalid ENET_TXD3 Pin Configuration!"
#endif
//     <o> ENET_TX_EN Pin <0=>P0_1 <1=>PC_4
#define   RTE_ENET_MII_TX_EN_PORT_ID    0
#if      (RTE_ENET_MII_TX_EN_PORT_ID == 0)
  #define RTE_ENET_MII_TX_EN_PORT       0
  #define RTE_ENET_MII_TX_EN_PIN        1
  #define RTE_ENET_MII_TX_EN_FUNC       6
#elif    (RTE_ENET_MII_TX_EN_PORT_ID == 1)
  #define RTE_ENET_MII_TX_EN_PORT       0xC
  #define RTE_ENET_MII_TX_EN_PIN        4
  #define RTE_ENET_MII_TX_EN_FUNC       3
#else
  #error "Invalid ENET_TX_EN Pin Configuration!"
#endif
//     <o> ENET_TX_CLK Pin <0=>P1_19 <1=>CLK0
#define   RTE_ENET_MII_TX_CLK_PORT_ID   0
#if      (RTE_ENET_MII_TX_CLK_PORT_ID == 0)
  #define RTE_ENET_MII_TX_CLK_PORT      1
  #define RTE_ENET_MII_TX_CLK_PIN       19
  #define RTE_ENET_MII_TX_CLK_FUNC      0
#elif    (RTE_ENET_MII_TX_CLK_PORT_ID == 1)
  #define RTE_ENET_MII_TX_CLK_PORT      0x10
  #define RTE_ENET_MII_TX_CLK_PIN       0
  #define RTE_ENET_MII_TX_CLK_FUNC      7
#else
  #error "Invalid ENET_TX_CLK Pin Configuration!"
#endif
//     <o> ENET_TX_ER Pin <0=>Not used <1=>PC_5 <2=>PC_14
//     <i> Optional signal, rarely used
#define   RTE_ENET_MII_TX_ER_PORT_ID    0
#if      (RTE_ENET_MII_TX_ER_PORT_ID == 0)
  #define RTE_ENET_MII_TX_ER_PIN_EN     0
#elif    (RTE_ENET_MII_TX_ER_PORT_ID == 1)
  #define RTE_ENET_MII_TX_ER_PORT       0xC
  #define RTE_ENET_MII_TX_ER_PIN        5
  #define RTE_ENET_MII_TX_ER_FUNC       3
#elif    (RTE_ENET_MII_TX_ER_PORT_ID == 2)
  #define RTE_ENET_MII_TX_ER_PORT       0xC
  #define RTE_ENET_MII_TX_ER_PIN        14
  #define RTE_ENET_MII_TX_ER_FUNC       6
#else
  #error "Invalid ENET_TX_ER Pin Configuration!"
#endif
#ifndef   RTE_ENET_MII_TX_ER_PIN_EN
  #define RTE_ENET_MII_TX_ER_PIN_EN     1
#endif
//     <o> ENET_RXD0 Pin <0=>P1_15
#define   RTE_ENET_MII_RXD0_PORT_ID     0
#if      (RTE_ENET_MII_RXD0_PORT_ID == 0)
  #define RTE_ENET_MII_RXD0_PORT        1
  #define RTE_ENET_MII_RXD0_PIN         15
  #define RTE_ENET_MII_RXD0_FUNC        3
#else
  #error "Invalid ENET_RXD0 Pin Configuration!"
#endif
//     <o> ENET_RXD1 Pin <0=>P0_0
#define   RTE_ENET_MII_RXD1_PORT_ID     0
#if      (RTE_ENET_MII_RXD1_PORT_ID == 0)
  #define RTE_ENET_MII_RXD1_PORT        0
  #define RTE_ENET_MII_RXD1_PIN         0
  #define RTE_ENET_MII_RXD1_FUNC        2
#else
  #error "Invalid ENET_RXD1 Pin Configuration!"
#endif
//     <o> ENET_RXD2 Pin <0=>P9_3 <1=>PC_6
#define   RTE_ENET_MII_RXD2_PORT_ID     0
#if      (RTE_ENET_MII_RXD2_PORT_ID == 0)
  #define RTE_ENET_MII_RXD2_PORT        9
  #define RTE_ENET_MII_RXD2_PIN         3
  #define RTE_ENET_MII_RXD2_FUNC        5
#elif    (RTE_ENET_MII_RXD2_PORT_ID == 1)
  #define RTE_ENET_MII_RXD2_PORT        0xC
  #define RTE_ENET_MII_RXD2_PIN         6
  #define RTE_ENET_MII_RXD2_FUNC        3
#else
  #error "Invalid ENET_RXD2 Pin Configuration!"
#endif
//     <o> ENET_RXD3 Pin <0=>P9_2 <1=>PC_7
#define   RTE_ENET_MII_RXD3_PORT_ID     0
#if      (RTE_ENET_MII_RXD3_PORT_ID == 0)
  #define RTE_ENET_MII_RXD3_PORT        9
  #define RTE_ENET_MII_RXD3_PIN         2
  #define RTE_ENET_MII_RXD3_FUNC        5
#elif    (RTE_ENET_MII_RXD3_PORT_ID == 1)
  #define RTE_ENET_MII_RXD3_PORT        0xC
  #define RTE_ENET_MII_RXD3_PIN         7
  #define RTE_ENET_MII_RXD3_FUNC        3
#else
  #error "Invalid ENET_RXD3 Pin Configuration!"
#endif
//     <o> ENET_RX_DV Pin <0=>P1_16 <1=>PC_8
#define   RTE_ENET_MII_RX_DV_PORT_ID    0
#if      (RTE_ENET_MII_RX_DV_PORT_ID == 0)
  #define RTE_ENET_MII_RX_DV_PORT       1
  #define RTE_ENET_MII_RX_DV_PIN        16
  #define RTE_ENET_MII_RX_DV_FUNC       7
#elif    (RTE_ENET_MII_RX_DV_PORT_ID == 1)
  #define RTE_ENET_MII_RX_DV_PORT       0xC
  #define RTE_ENET_MII_RX_DV_PIN        8
  #define RTE_ENET_MII_RX_DV_FUNC       3
#else
  #error "Invalid ENET_RX_DV Pin Configuration!"
#endif
//     <o> ENET_RX_CLK Pin <0=>PC_0
#define   RTE_ENET_MII_RX_CLK_PORT_ID   0
#if      (RTE_ENET_MII_RX_CLK_PORT_ID == 0)
  #define RTE_ENET_MII_RX_CLK_PORT      0xC
  #define RTE_ENET_MII_RX_CLK_PIN       0
  #define RTE_ENET_MII_RX_CLK_FUNC      3
#else
  #error "Invalid ENET_RX_CLK Pin Configuration!"
#endif
//     <o> ENET_RX_ER Pin <0=>P9_1 <1=>PC_9
#define   RTE_ENET_MII_RX_ER_PORT_ID    0
#if      (RTE_ENET_MII_RX_ER_PORT_ID == 0)
  #define RTE_ENET_MII_RX_ER_PORT       9
  #define RTE_ENET_MII_RX_ER_PIN        1
  #define RTE_ENET_MII_RX_ER_FUNC       5
#elif    (RTE_ENET_MII_RX_ER_PORT_ID == 1)
  #define RTE_ENET_MII_RX_ER_PORT       0xC
  #define RTE_ENET_MII_RX_ER_PIN        9
  #define RTE_ENET_MII_RX_ER_FUNC       3
#else
  #error "Invalid ENET_RX_ER Pin Configuration!"
#endif
//     <o> ENET_COL Pin <0=>P0_1 <1=>P4_1 <2=>P9_6
#define   RTE_ENET_MII_COL_PORT_ID      0
#if      (RTE_ENET_MII_COL_PORT_ID == 0)
  #define RTE_ENET_MII_COL_PORT         0
  #define RTE_ENET_MII_COL_PIN          1
  #define RTE_ENET_MII_COL_FUNC         2
#elif    (RTE_ENET_MII_COL_PORT_ID == 1)
  #define RTE_ENET_MII_COL_PORT         4
  #define RTE_ENET_MII_COL_PIN          1
  #define RTE_ENET_MII_COL_FUNC         7
#elif    (RTE_ENET_MII_COL_PORT_ID == 2)
  #define RTE_ENET_MII_COL_PORT         9
  #define RTE_ENET_MII_COL_PIN          6
  #define RTE_ENET_MII_COL_FUNC         5
#else
  #error "Invalid ENET_COL Pin Configuration!"
#endif
//     <o> ENET_CRS Pin <0=>P1_16 <1=>P9_0
#define   RTE_ENET_MII_CRS_PORT_ID      0
#if      (RTE_ENET_MII_CRS_PORT_ID == 0)
  #define RTE_ENET_MII_CRS_PORT         1
  #define RTE_ENET_MII_CRS_PIN          16
  #define RTE_ENET_MII_CRS_FUNC         3
#elif    (RTE_ENET_MII_CRS_PORT_ID == 1)
  #define RTE_ENET_MII_CRS_PORT         9
  #define RTE_ENET_MII_CRS_PIN          0
  #define RTE_ENET_MII_CRS_FUNC         5
#else
  #error "Invalid ENET_CRS Pin Configuration!"
#endif
//   </e> MII (Media Independent Interface)

//   <e> RMII (Reduced Media Independent Interface)
#define   RTE_ENET_RMII                 0

//     <o> ENET_TXD0 Pin <0=>P1_18
#define   RTE_ENET_RMII_TXD0_PORT_ID    0
#if      (RTE_ENET_RMII_TXD0_PORT_ID == 0)
  #define RTE_ENET_RMII_TXD0_PORT       1
  #define RTE_ENET_RMII_TXD0_PIN        18
  #define RTE_ENET_RMII_TXD0_FUNC       3
#else
  #error "Invalid ENET_TXD0 Pin Configuration!"
#endif
//     <o> ENET_TXD1 Pin <0=>P1_20
#define   RTE_ENET_RMII_TXD1_PORT_ID    0
#if      (RTE_ENET_RMII_TXD1_PORT_ID == 0)
  #define RTE_ENET_RMII_TXD1_PORT       1
  #define RTE_ENET_RMII_TXD1_PIN        20
  #define RTE_ENET_RMII_TXD1_FUNC       3
#else
  #error "Invalid ENET_TXD1 Pin Configuration!"
#endif
//     <o> ENET_TX_EN Pin <0=>P0_1 <1=>PC_4
#define   RTE_ENET_RMII_TX_EN_PORT_ID   0
#if      (RTE_ENET_RMII_TX_EN_PORT_ID == 0)
  #define RTE_ENET_RMII_TX_EN_PORT      0
  #define RTE_ENET_RMII_TX_EN_PIN       1
  #define RTE_ENET_RMII_TX_EN_FUNC      6
#elif    (RTE_ENET_RMII_TX_EN_PORT_ID == 1)
  #define RTE_ENET_RMII_TX_EN_PORT      0xC
  #define RTE_ENET_RMII_TX_EN_PIN       4
  #define RTE_ENET_RMII_TX_EN_FUNC      3
#else
  #error "Invalid ENET_TX_EN Pin Configuration!"
#endif
//     <o> ENET_REF_CLK Pin <0=>P1_19 <1=>CLK0
#define   RTE_ENET_RMII_REF_CLK_PORT_ID 0
#if      (RTE_ENET_RMII_REF_CLK_PORT_ID == 0)
  #define RTE_ENET_RMII_REF_CLK_PORT    1
  #define RTE_ENET_RMII_REF_CLK_PIN     19
  #define RTE_ENET_RMII_REF_CLK_FUNC    0
#elif    (RTE_ENET_RMII_REF_CLK_PORT_ID == 1)
  #define RTE_ENET_RMII_REF_CLK_PORT    0x10
  #define RTE_ENET_RMII_REF_CLK_PIN     0
  #define RTE_ENET_RMII_REF_CLK_FUNC    7
#else
  #error "Invalid ENET_REF_CLK Pin Configuration!"
#endif
//     <o> ENET_RXD0 Pin <0=>P1_15
#define   RTE_ENET_RMII_RXD0_PORT_ID    0
#if      (RTE_ENET_RMII_RXD0_PORT_ID == 0)
  #define RTE_ENET_RMII_RXD0_PORT       1
  #define RTE_ENET_RMII_RXD0_PIN        15
  #define RTE_ENET_RMII_RXD0_FUNC       3
#else
  #error "Invalid ENET_RXD0 Pin Configuration!"
#endif
//     <o> ENET_RXD1 Pin <0=>P0_0
#define   RTE_ENET_RMII_RXD1_PORT_ID    0
#if      (RTE_ENET_RMII_RXD1_PORT_ID == 0)
  #define RTE_ENET_RMII_RXD1_PORT       0
  #define RTE_ENET_RMII_RXD1_PIN        0
  #define RTE_ENET_RMII_RXD1_FUNC       2
#else
  #error "Invalid ENET_RXD1 Pin Configuration!"
#endif
//     <o> ENET_RX_DV Pin <0=>P1_16 <1=>PC_8
#define   RTE_ENET_RMII_RX_DV_PORT_ID   0
#if      (RTE_ENET_RMII_RX_DV_PORT_ID == 0)
  #define RTE_ENET_RMII_RX_DV_PORT      1
  #define RTE_ENET_RMII_RX_DV_PIN       16
  #define RTE_ENET_RMII_RX_DV_FUNC      7
#elif    (RTE_ENET_RMII_RX_DV_PORT_ID == 1)
  #define RTE_ENET_RMII_RX_DV_PORT      0xC
  #define RTE_ENET_RMII_RX_DV_PIN       8
  #define RTE_ENET_RMII_RX_DV_FUNC      3
#else
  #error "Invalid ENET_RX_DV Pin Configuration!"
#endif
//   </e> RMII (Reduced Media Independent Interface)

//   <h> MIIM (Management Data Interface)
//     <o> ENET_MDIO Pin <0=>P1_17
#define   RTE_ENET_MDI_MDIO_PORT_ID     0
#if      (RTE_ENET_MDI_MDIO_PORT_ID == 0)
  #define RTE_ENET_MDI_MDIO_PORT        1
  #define RTE_ENET_MDI_MDIO_PIN         17
  #define RTE_ENET_MDI_MDIO_FUNC        3
#else
  #error "Invalid ENET_MDIO Pin Configuration!"
#endif
//     <o> ENET_MDC Pin <0=>P2_0 <1=>P7_7 <2=>PC_1
#define   RTE_ENET_MDI_MDC_PORT_ID      2
#if      (RTE_ENET_MDI_MDC_PORT_ID == 0)
  #define RTE_ENET_MDI_MDC_PORT         2
  #define RTE_ENET_MDI_MDC_PIN          0
  #define RTE_ENET_MDI_MDC_FUNC         7
#elif    (RTE_ENET_MDI_MDC_PORT_ID == 1)
  #define RTE_ENET_MDI_MDC_PORT         7
  #define RTE_ENET_MDI_MDC_PIN          7
  #define RTE_ENET_MDI_MDC_FUNC         6
#elif    (RTE_ENET_MDI_MDC_PORT_ID == 2)
  #define RTE_ENET_MDI_MDC_PORT         0xC
  #define RTE_ENET_MDI_MDC_PIN          1
  #define RTE_ENET_MDI_MDC_FUNC         3
#else
  #error "Invalid ENET_MDC Pin Configuration!"
#endif
//   </h> MIIM (Management Data Interface)
// </e> ENET (Ethernet Interface) [Driver_ETH_MAC0]

// <e> SD/MMC Interface [Driver_MCI0]
// <i> Configuration settings for Driver_MCI0 in component ::Drivers:MCI
#define RTE_SDMMC                       0

//   <h> SD/MMC Peripheral Bus
//     <o> SD_CLK Pin <0=>PC_0 <1=>CLK0 <2=>CLK2
#define   RTE_SD_CLK_PORT_ID            0
#if      (RTE_SD_CLK_PORT_ID == 0)
  #define RTE_SD_CLK_PORT               0xC
  #define RTE_SD_CLK_PIN                0
  #define RTE_SD_CLK_FUNC               7
#elif    (RTE_SD_CLK_PORT_ID == 1)
  #define RTE_SD_CLK_PORT               0x10
  #define RTE_SD_CLK_PIN                0
  #define RTE_SD_CLK_FUNC               4
#elif    (RTE_SD_CLK_PORT_ID == 2)
  #define RTE_SD_CLK_PORT               0x10
  #define RTE_SD_CLK_PIN                2
  #define RTE_SD_CLK_FUNC               4
#else
  #error "Invalid SD_CLK Pin Configuration!"
#endif
//     <o> SD_CMD Pin <0=>P1_6 <1=>PC_10
#define   RTE_SD_CMD_PORT_ID            0
#if      (RTE_SD_CMD_PORT_ID == 0)
  #define RTE_SD_CMD_PORT               1
  #define RTE_SD_CMD_PIN                6
  #define RTE_SD_CMD_FUNC               7
#elif    (RTE_SD_CMD_PORT_ID == 1)
  #define RTE_SD_CMD_PORT               0xC
  #define RTE_SD_CMD_PIN                10
  #define RTE_SD_CMD_FUNC               7
#else
  #error "Invalid SD_CMD Pin Configuration!"
#endif
//     <o> SD_DAT0 Pin <0=>P1_9 <1=>PC_4
#define   RTE_SD_DAT0_PORT_ID           0
#if      (RTE_SD_DAT0_PORT_ID == 0)
  #define RTE_SD_DAT0_PORT              1
  #define RTE_SD_DAT0_PIN               9
  #define RTE_SD_DAT0_FUNC              7
#elif    (RTE_SD_DAT0_PORT_ID == 1)
  #define RTE_SD_DAT0_PORT              0xC
  #define RTE_SD_DAT0_PIN               4
  #define RTE_SD_DAT0_FUNC              7
#else
  #error "Invalid SD_DAT0 Pin Configuration!"
#endif
//     <e> SD_DAT[1 .. 3]
#define   RTE_SDMMC_BUS_WIDTH_4         0
//       <o> SD_DAT1 Pin <0=>P1_10 <1=>PC_5
#define   RTE_SD_DAT1_PORT_ID           0
#if      (RTE_SD_DAT1_PORT_ID == 0)
  #define RTE_SD_DAT1_PORT              1
  #define RTE_SD_DAT1_PIN               10
  #define RTE_SD_DAT1_FUNC              7
#elif    (RTE_SD_DAT1_PORT_ID == 1)
  #define RTE_SD_DAT1_PORT              0xC
  #define RTE_SD_DAT1_PIN               5
  #define RTE_SD_DAT1_FUNC              7
#else
  #error "Invalid SD_DAT1 Pin Configuration!"
#endif
//       <o> SD_DAT2 Pin <0=>P1_11 <1=>PC_6
#define   RTE_SD_DAT2_PORT_ID           0
#if      (RTE_SD_DAT2_PORT_ID == 0)
  #define RTE_SD_DAT2_PORT              1
  #define RTE_SD_DAT2_PIN               11
  #define RTE_SD_DAT2_FUNC              7
#elif    (RTE_SD_DAT2_PORT_ID == 1)
  #define RTE_SD_DAT2_PORT              0xC
  #define RTE_SD_DAT2_PIN               6
  #define RTE_SD_DAT2_FUNC              7
#else
  #error "Invalid SD_DAT2 Pin Configuration!"
#endif
//       <o> SD_DAT3 Pin <0=>P1_12 <1=>PC_7
#define   RTE_SD_DAT3_PORT_ID           0
#if      (RTE_SD_DAT3_PORT_ID == 0)
  #define RTE_SD_DAT3_PORT              1
  #define RTE_SD_DAT3_PIN               12
  #define RTE_SD_DAT3_FUNC              7
#elif    (RTE_SD_DAT3_PORT_ID == 1)
  #define RTE_SD_DAT3_PORT              0xC
  #define RTE_SD_DAT3_PIN               7
  #define RTE_SD_DAT3_FUNC              7
#else
  #error "Invalid SD_DAT3 Pin Configuration!"
#endif
//     </e> SD_DAT[1 .. 3]
//     <e> SD_DAT[4 .. 7]
#define   RTE_SDMMC_BUS_WIDTH_8         0
//       <o> SD_DAT4 Pin <0=>PC_11
#define   RTE_SD_DAT4_PORT_ID           0
#if      (RTE_SD_DAT4_PORT_ID == 0)
  #define RTE_SD_DAT4_PORT              0xC
  #define RTE_SD_DAT4_PIN               11
  #define RTE_SD_DAT4_FUNC              7
#else
  #error "Invalid SD_DAT4 Pin Configuration!"
#endif
//       <o> SD_DAT5 Pin <0=>PC_12
#define   RTE_SD_DAT5_PORT_ID           0
#if      (RTE_SD_DAT5_PORT_ID == 0)
  #define RTE_SD_DAT5_PORT              0xC
  #define RTE_SD_DAT5_PIN               12
  #define RTE_SD_DAT5_FUNC              7
#else
  #error "Invalid SD_DAT5 Pin Configuration!"
#endif
//       <o> SD_DAT6 Pin <0=>PC_13
#define   RTE_SD_DAT6_PORT_ID           0
#if      (RTE_SD_DAT6_PORT_ID == 0)
  #define RTE_SD_DAT6_PORT              0xC
  #define RTE_SD_DAT6_PIN               13
  #define RTE_SD_DAT6_FUNC              7
#else
  #error "Invalid SD_DAT6 Pin Configuration!"
#endif
//       <o> SD_DAT7 Pin <0=>PC_14
#define   RTE_SD_DAT7_PORT_ID           0
#if      (RTE_SD_DAT7_PORT_ID == 0)
  #define RTE_SD_DAT7_PORT              0xC
  #define RTE_SD_DAT7_PIN               14
  #define RTE_SD_DAT7_FUNC              7
#else
  #error "Invalid SD_DAT7 Pin Configuration!"
#endif
//     </e> SD_DAT[4 .. 7]
//   </h> SD/MMC Peripheral Bus

//   <o> SD_CD (Card Detect) Pin <0=>Not used <1=>P1_13 <2=>PC_8
//   <i> Configure Pin if exists
#define   RTE_SD_CD_PORT_ID             0
#if      (RTE_SD_CD_PORT_ID == 0)
  #define RTE_SD_CD_PIN_EN              0
#elif    (RTE_SD_CD_PORT_ID == 1)
  #define RTE_SD_CD_PORT                1
  #define RTE_SD_CD_PIN                 13
  #define RTE_SD_CD_FUNC                7
#elif    (RTE_SD_CD_PORT_ID == 2)
  #define RTE_SD_CD_PORT                0xC
  #define RTE_SD_CD_PIN                 8
  #define RTE_SD_CD_FUNC                7
#else
  #error "Invalid SD_CD Pin Configuration!"
#endif
#ifndef   RTE_SD_CD_PIN_EN
  #define RTE_SD_CD_PIN_EN              1
#endif
//   <o> SD_WP (Write Protect) Pin <0=>Not used <1=>PD_15 <2=>PF_10
//   <i> Configure Pin if exists
#define   RTE_SD_WP_PORT_ID             0
#if      (RTE_SD_WP_PORT_ID == 0)
  #define RTE_SD_WP_PIN_EN              0
#elif    (RTE_SD_WP_PORT_ID == 1)
  #define RTE_SD_WP_PORT                0xD
  #define RTE_SD_WP_PIN                 15
  #define RTE_SD_WP_FUNC                5
#elif    (RTE_SD_WP_PORT_ID == 2)
  #define RTE_SD_WP_PORT                0xF
  #define RTE_SD_WP_PIN                 10
  #define RTE_SD_WP_FUNC                6
#else
  #error "Invalid SD_WP Pin Configuration!"
#endif
#ifndef   RTE_SD_WP_PIN_EN
  #define RTE_SD_WP_PIN_EN              1
#endif
//   <o> SD_POW (Power) Pin <0=>Not used <1=>P1_5 <2=>PC_9 <3=>PD_1
//   <i> Configure Pin if exists
#define   RTE_SD_POW_PORT_ID            0
#if      (RTE_SD_POW_PORT_ID == 0)
  #define RTE_SD_POW_PIN_EN             0
#elif    (RTE_SD_POW_PORT_ID == 1)
  #define RTE_SD_POW_PORT               1
  #define RTE_SD_POW_PIN                5
  #define RTE_SD_POW_FUNC               7
#elif    (RTE_SD_POW_PORT_ID == 2)
  #define RTE_SD_POW_PORT               0xC
  #define RTE_SD_POW_PIN                9
  #define RTE_SD_POW_FUNC               7
#elif    (RTE_SD_POW_PORT_ID == 3)
  #define RTE_SD_POW_PORT               0xD
  #define RTE_SD_POW_PIN                1
  #define RTE_SD_POW_FUNC               5
#else
  #error "Invalid SD_POW Pin Configuration!"
#endif
#ifndef   RTE_SD_POW_PIN_EN
  #define RTE_SD_POW_PIN_EN             1
#endif
//   <o> SD_RST (Card Reset for MMC4.4) Pin <0=>Not used <1=>P1_3 <2=>PC_2
//   <i> Configure Pin if exists
#define   RTE_SD_RST_PORT_ID            0
#if      (RTE_SD_RST_PORT_ID == 0)
  #define RTE_SD_RST_PIN_EN             0
#elif    (RTE_SD_RST_PORT_ID == 1)
  #define RTE_SD_RST_PORT               1
  #define RTE_SD_RST_PIN                3
  #define RTE_SD_RST_FUNC               7
#elif    (RTE_SD_RST_PORT_ID == 2)
  #define RTE_SD_RST_PORT               0xC
  #define RTE_SD_RST_PIN                2
  #define RTE_SD_RST_FUNC               7
#else
  #error "Invalid SD_RST Pin Configuration!"
#endif
#ifndef   RTE_SD_RST_PIN_EN
  #define RTE_SD_RST_PIN_EN             1
#endif
// </e> SD/MMC Interface [Driver_MCI0]

// <e> I2C0 (Inter-integrated Circuit Interface 0) [Driver_I2C0]
// <i> Configuration settings for Driver_I2C0 in component ::Drivers:I2C
// </e> I2C0 (Inter-integrated Circuit Interface 0) [Driver_I2C0]
#define   RTE_I2C0                      0

// <e> I2C1 (Inter-integrated Circuit Interface 1) [Driver_I2C1]
// <i> Configuration settings for Driver_I2C1 in component ::Drivers:I2C
#define   RTE_I2C1                      0

//   <o> I2C1_SCL Pin <0=>P2_4 <1=>PE_15
#define   RTE_I2C1_SCL_PORT_ID          0
#if      (RTE_I2C1_SCL_PORT_ID == 0)
  #define RTE_I2C1_SCL_PORT             2
  #define RTE_I2C1_SCL_PIN              4
  #define RTE_I2C1_SCL_FUNC             1
#elif    (RTE_I2C1_SCL_PORT_ID == 1)
  #define RTE_I2C1_SCL_PORT             0xE
  #define RTE_I2C1_SCL_PIN              15
  #define RTE_I2C1_SCL_FUNC             2
#else
  #error "Invalid I2C1_SCL Pin Configuration!"
#endif
//   <o> I2C1_SDA Pin <0=>P2_3 <1=>PE_13
#define   RTE_I2C1_SDA_PORT_ID          0
#if      (RTE_I2C1_SDA_PORT_ID == 0)
  #define RTE_I2C1_SDA_PORT             2
  #define RTE_I2C1_SDA_PIN              3
  #define RTE_I2C1_SDA_FUNC             1
#elif    (RTE_I2C1_SDA_PORT_ID == 1)
  #define RTE_I2C1_SDA_PORT             0xE
  #define RTE_I2C1_SDA_PIN              13
  #define RTE_I2C1_SDA_FUNC             2
#else
  #error "Invalid I2C1_SDA Pin Configuration!"
#endif
// </e> I2C1 (Inter-integrated Circuit Interface 1) [Driver_I2C1]

// <e> USART0 (Universal synchronous asynchronous receiver transmitter) [Driver_USART0]
#define   RTE_USART0                    0

//   <h> Pin Configuration
//     <o> TX <0=>Not used <1=>P2_0 <2=>P6_4 <3=>P9_5 <4=>PF_10
//     <i> USART0 Serial Output pin
#define   RTE_USART0_TX_ID              0
#if      (RTE_USART0_TX_ID == 0)
  #define RTE_USART0_TX_PIN_EN          0
#elif    (RTE_USART0_TX_ID == 1)
  #define RTE_USART0_TX_PORT            2
  #define RTE_USART0_TX_BIT             0
  #define RTE_USART0_TX_FUNC            1
#elif    (RTE_USART0_TX_ID == 2)
  #define RTE_USART0_TX_PORT            6
  #define RTE_USART0_TX_BIT             4
  #define RTE_USART0_TX_FUNC            2
#elif    (RTE_USART0_TX_ID == 3)
  #define RTE_USART0_TX_PORT            9
  #define RTE_USART0_TX_BIT             5
  #define RTE_USART0_TX_FUNC            7
#elif    (RTE_USART0_TX_ID == 4)
  #define RTE_USART0_TX_PORT            0xF
  #define RTE_USART0_TX_BIT             10
  #define RTE_USART0_TX_FUNC            1
#else
  #error "Invalid USART0_TX Pin Configuration!"
#endif
#ifndef   RTE_USART0_TX_PIN_EN
  #define RTE_USART0_TX_PIN_EN          1
#endif
//     <o> RX <0=>Not used <1=>P2_1 <2=>P6_5 <3=>P9_6 <4=>PF_11
//     <i> USART0 Serial Input pin
#define   RTE_USART0_RX_ID              0
#if      (RTE_USART0_RX_ID == 0)
  #define RTE_USART0_RX_PIN_EN          0
#elif    (RTE_USART0_RX_ID == 1)
  #define RTE_USART0_RX_PORT            2
  #define RTE_USART0_RX_BIT             1
  #define RTE_USART0_RX_FUNC            1
#elif    (RTE_USART0_RX_ID == 2)
  #define RTE_USART0_RX_PORT            6
  #define RTE_USART0_RX_BIT             5
  #define RTE_USART0_RX_FUNC            2
#elif    (RTE_USART0_RX_ID == 3)
  #define RTE_USART0_RX_PORT            9
  #define RTE_USART0_RX_BIT             6
  #define RTE_USART0_RX_FUNC            7
#elif    (RTE_USART0_RX_ID == 4)
  #define RTE_USART0_RX_PORT            0xF
  #define RTE_USART0_RX_BIT             11
  #define RTE_USART0_RX_FUNC            1
#else
  #error "Invalid USART0_RX Pin Configuration!"
#endif
#ifndef   RTE_USART0_RX_PIN_EN
  #define RTE_USART0_RX_PIN_EN          1
#endif
//     <o> UCLK (Synchronous and SmartCard mode) <0=>Not used <1=>P2_2 <2=>P6_1 <3=>PF_8
//     <i> USART0 Serial Clock input/output synchronous mode
#define   RTE_USART0_UCLK_ID            0
#if      (RTE_USART0_UCLK_ID == 0)
  #define RTE_USART0_UCLK_PIN_EN        0
#elif    (RTE_USART0_UCLK_ID == 1)
  #define RTE_USART0_UCLK_PORT          2
  #define RTE_USART0_UCLK_BIT           2
  #define RTE_USART0_UCLK_FUNC          1
#elif    (RTE_USART0_UCLK_ID == 2)
  #define RTE_USART0_UCLK_PORT          6
  #define RTE_USART0_UCLK_BIT           1
  #define RTE_USART0_UCLK_FUNC          2
#elif    (RTE_USART0_UCLK_ID == 3)
  #define RTE_USART0_UCLK_PORT          0xF
  #define RTE_USART0_UCLK_BIT           8
  #define RTE_USART0_UCLK_FUNC          1
#else
  #error "Invalid USART0_UCLK Pin Configuration!"
#endif
#ifndef   RTE_USART0_UCLK_PIN_EN
  #define RTE_USART0_UCLK_PIN_EN        1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>1 (DMAMUXPER1)  <1=>11 (DMAMUXPER11)
//     </e>
#define   RTE_USART0_DMA_TX_EN          0
#define   RTE_USART0_DMA_TX_CH          0
#define   RTE_USART0_DMA_TX_PERI_ID     0
#if      (RTE_USART0_DMA_TX_PERI_ID == 0)
  #define RTE_USART0_DMA_TX_PERI        1
  #define RTE_USART0_DMA_TX_PERI_SEL    1
#elif    (RTE_USART0_DMA_TX_PERI_ID == 1)
  #define RTE_USART0_DMA_TX_PERI        11
  #define RTE_USART0_DMA_TX_PERI_SEL    2
#endif
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral <0=>2 (DMAMUXPER2)  <1=>12 (DMAMUXPER12)
//     </e>
#define   RTE_USART0_DMA_RX_EN          0
#define   RTE_USART0_DMA_RX_CH          1
#define   RTE_USART0_DMA_RX_PERI_ID     0
#if      (RTE_USART0_DMA_RX_PERI_ID == 0)
  #define RTE_USART0_DMA_RX_PERI        2
  #define RTE_USART0_DMA_RX_PERI_SEL    1
#elif    (RTE_USART0_DMA_RX_PERI_ID == 1)
  #define RTE_USART0_DMA_RX_PERI        12
  #define RTE_USART0_DMA_RX_PERI_SEL    2
#endif
//   </h> DMA
// </e> USART0 (Universal synchronous asynchronous receiver transmitter) [Driver_USART0]

// <e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]
#define   RTE_UART1                     1

//   <h> Pin Configuration
//     <o> TX <0=>Not used <1=>P1_13 <2=>P3_4 <3=>P5_6 <4=>PC_13 <5=>PE_11
//     <i> UART0 Serial Output pin
#define   RTE_UART1_TX_ID               0
#if      (RTE_UART1_TX_ID == 0)
  #define RTE_UART1_TX_PIN_EN           0
#elif    (RTE_UART1_TX_ID == 1)
  #define RTE_UART1_TX_PORT             1
  #define RTE_UART1_TX_BIT              13
  #define RTE_UART1_TX_FUNC             1
#elif    (RTE_UART1_TX_ID == 2)
  #define RTE_UART1_TX_PORT             3
  #define RTE_UART1_TX_BIT              4
  #define RTE_UART1_TX_FUNC             4
#elif    (RTE_UART1_TX_ID == 3)
  #define RTE_UART1_TX_PORT             5
  #define RTE_UART1_TX_BIT              6
  #define RTE_UART1_TX_FUNC             4
#elif    (RTE_UART1_TX_ID == 4)
  #define RTE_UART1_TX_PORT             0xC
  #define RTE_UART1_TX_BIT              13
  #define RTE_UART1_TX_FUNC             2
#elif    (RTE_UART1_TX_ID == 5)
  #define RTE_UART1_TX_PORT             0xE
  #define RTE_UART1_TX_BIT              11
  #define RTE_UART1_TX_FUNC             2
#else
  #error "Invalid UART1_TX Pin Configuration!"
#endif
#ifndef   RTE_UART1_TX_PIN_EN
  #define RTE_UART1_TX_PIN_EN           1
#endif
//   <o> RX <0=>Not used <1=>P1_14 <2=>P3_5 <3=>P5_7 <4=>PC_14 <5=>PE_12
//   <i> UART1 Serial Input pin
#define   RTE_UART1_RX_ID               1
#if      (RTE_UART1_RX_ID == 0)
  #define RTE_UART1_RX_PIN_EN           0
#elif    (RTE_UART1_RX_ID == 1)
  #define RTE_UART1_RX_PORT             1
  #define RTE_UART1_RX_BIT              14
  #define RTE_UART1_RX_FUNC             1
#elif    (RTE_UART1_RX_ID == 2)
  #define RTE_UART1_RX_PORT             3
  #define RTE_UART1_RX_BIT              5
  #define RTE_UART1_RX_FUNC             4
#elif    (RTE_UART1_RX_ID == 3)
  #define RTE_UART1_RX_PORT             5
  #define RTE_UART1_RX_BIT              7
  #define RTE_UART1_RX_FUNC             4
#elif    (RTE_UART1_RX_ID == 4)
  #define RTE_UART1_RX_PORT             0xC
  #define RTE_UART1_RX_BIT              14
  #define RTE_UART1_RX_FUNC             2
#elif    (RTE_UART1_RX_ID == 5)
  #define RTE_UART1_RX_PORT             0xE
  #define RTE_UART1_RX_BIT              12
  #define RTE_UART1_RX_FUNC             2
#else
  #error "Invalid UART1_RX Pin Configuration!"
#endif
#ifndef   RTE_UART1_RX_PIN_EN
  #define RTE_UART1_RX_PIN_EN           1
#endif

//     <h> Modem Lines
//       <o> CTS <0=>Not used <1=>P1_11 <2=>P5_4 <3=>PC_2 <4=>PE_7
#define   RTE_UART1_CTS_ID              0
#if      (RTE_UART1_CTS_ID == 0)
  #define RTE_UART1_CTS_PIN_EN          0
#elif    (RTE_UART1_CTS_ID == 1)
  #define RTE_UART1_CTS_PORT            1
  #define RTE_UART1_CTS_BIT             11
  #define RTE_UART1_CTS_FUNC            1
#elif    (RTE_UART1_CTS_ID == 2)
  #define RTE_UART1_CTS_PORT            5
  #define RTE_UART1_CTS_BIT             4
  #define RTE_UART1_CTS_FUNC            4
#elif    (RTE_UART1_CTS_ID == 3)
  #define RTE_UART1_CTS_PORT            0xC
  #define RTE_UART1_CTS_BIT             2
  #define RTE_UART1_CTS_FUNC            2
#elif    (RTE_UART1_CTS_ID == 4)
  #define RTE_UART1_CTS_PORT            0xE
  #define RTE_UART1_CTS_BIT             7
  #define RTE_UART1_CTS_FUNC            2
#else
  #error "Invalid UART1_CTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_CTS_PIN_EN
  #define RTE_UART1_CTS_PIN_EN          1
#endif
//       <o> RTS <0=>Not used <1=>P1_9  <2=>P5_2 <3=>PC_3 <4=>PE_5
#define   RTE_UART1_RTS_ID              0
#if      (RTE_UART1_RTS_ID == 0)
  #define RTE_UART1_RTS_PIN_EN          0
#elif    (RTE_UART1_RTS_ID == 1)
  #define RTE_UART1_RTS_PORT            1
  #define RTE_UART1_RTS_BIT             9
  #define RTE_UART1_RTS_FUNC            1
#elif    (RTE_UART1_RTS_ID == 2)
  #define RTE_UART1_RTS_PORT            5
  #define RTE_UART1_RTS_BIT             2
  #define RTE_UART1_RTS_FUNC            4
#elif    (RTE_UART1_RTS_ID == 3)
  #define RTE_UART1_RTS_PORT            0xC
  #define RTE_UART1_RTS_BIT             3
  #define RTE_UART1_RTS_FUNC            2
#elif    (RTE_UART1_RTS_ID == 4)
  #define RTE_UART1_RTS_PORT            0xE
  #define RTE_UART1_RTS_BIT             5
  #define RTE_UART1_RTS_FUNC            2
#else
  #error "Invalid UART1_RTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_RTS_PIN_EN
  #define RTE_UART1_RTS_PIN_EN          1
#endif
//       <o> DCD <0=>Not used <1=>P1_12 <2=>P5_5 <3=>PC_11 <4=>PE_9
#define   RTE_UART1_DCD_ID              0
#if      (RTE_UART1_DCD_ID == 0)
  #define RTE_UART1_DCD_PIN_EN          0
#elif    (RTE_UART1_DCD_ID == 1)
  #define RTE_UART1_DCD_PORT            1
  #define RTE_UART1_DCD_BIT             12
  #define RTE_UART1_DCD_FUNC            1
#elif    (RTE_UART1_DCD_ID == 2)
  #define RTE_UART1_DCD_PORT            5
  #define RTE_UART1_DCD_BIT             5
  #define RTE_UART1_DCD_FUNC            4
#elif    (RTE_UART1_DCD_ID == 3)
  #define RTE_UART1_DCD_PORT            0xC
  #define RTE_UART1_DCD_BIT             11
  #define RTE_UART1_DCD_FUNC            2
#elif    (RTE_UART1_DCD_ID == 4)
  #define RTE_UART1_DCD_PORT            0xE
  #define RTE_UART1_DCD_BIT             9
  #define RTE_UART1_DCD_FUNC            2
#else
  #error "Invalid UART1_DCD Pin Configuration!"
#endif
#ifndef   RTE_UART1_DCD_PIN_EN
  #define RTE_UART1_DCD_PIN_EN          1
#endif
//       <o> DSR <0=>Not used <1=>P1_7 <2=>P5_0 <3=>PC_10 <4=>PE_8
#define   RTE_UART1_DSR_ID              0
#if      (RTE_UART1_DSR_ID == 0)
  #define RTE_UART1_DSR_PIN_EN          0
#elif    (RTE_UART1_DSR_ID == 1)
  #define RTE_UART1_DSR_PORT            1
  #define RTE_UART1_DSR_BIT             7
  #define RTE_UART1_DSR_FUNC            1
#elif    (RTE_UART1_DSR_ID == 2)
  #define RTE_UART1_DSR_PORT            5
  #define RTE_UART1_DSR_BIT             0
  #define RTE_UART1_DSR_FUNC            4
#elif    (RTE_UART1_DSR_ID == 3)
  #define RTE_UART1_DSR_PORT            0xC
  #define RTE_UART1_DSR_BIT             10
  #define RTE_UART1_DSR_FUNC            2
#elif    (RTE_UART1_DSR_ID == 4)
  #define RTE_UART1_DSR_PORT            0xE
  #define RTE_UART1_DSR_BIT             8
  #define RTE_UART1_DSR_FUNC            2
#else
  #error "Invalid UART1_DSR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DSR_PIN_EN
  #define RTE_UART1_DSR_PIN_EN          1
#endif
//       <o> DTR <0=>Not used <1=>P1_8  <2=>P5_1 <3=>PC_12 <4=>PE_10
#define   RTE_UART1_DTR_ID              0
#if      (RTE_UART1_DTR_ID == 0)
  #define RTE_UART1_DTR_PIN_EN          0
#elif    (RTE_UART1_DTR_ID == 1)
  #define RTE_UART1_DTR_PORT            1
  #define RTE_UART1_DTR_BIT             8
  #define RTE_UART1_DTR_FUNC            1
#elif    (RTE_UART1_DTR_ID == 2)
  #define RTE_UART1_DTR_PORT            5
  #define RTE_UART1_DTR_BIT             1
  #define RTE_UART1_DTR_FUNC            4
#elif    (RTE_UART1_DTR_ID == 3)
  #define RTE_UART1_DTR_PORT            0xC
  #define RTE_UART1_DTR_BIT             12
  #define RTE_UART1_DTR_FUNC            2
#elif    (RTE_UART1_DTR_ID == 4)
  #define RTE_UART1_DTR_PORT            0xE
  #define RTE_UART1_DTR_BIT             10
  #define RTE_UART1_DTR_FUNC            2
#else
  #error "Invalid UART1_DTR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DTR_PIN_EN
  #define RTE_UART1_DTR_PIN_EN          1
#endif
//       <o> RI <0=>Not used <1=>P1_10 <2=>P5_3 <3=>PC_1 <4=>PE_6
#define   RTE_UART1_RI_ID               0
#if      (RTE_UART1_RI_ID == 0)
  #define RTE_UART1_RI_PIN_EN           0
#elif    (RTE_UART1_RI_ID == 1)
  #define RTE_UART1_RI_PORT             1
  #define RTE_UART1_RI_BIT              10
  #define RTE_UART1_RI_FUNC             1
#elif    (RTE_UART1_RI_ID == 2)
  #define RTE_UART1_RI_PORT             5
  #define RTE_UART1_RI_BIT              3
  #define RTE_UART1_RI_FUNC             4
#elif    (RTE_UART1_RI_ID == 3)
  #define RTE_UART1_RI_PORT             0xC
  #define RTE_UART1_RI_BIT              1
  #define RTE_UART1_RI_FUNC             2
#elif    (RTE_UART1_RI_ID == 4)
  #define RTE_UART1_RI_PORT             0xE
  #define RTE_UART1_RI_BIT              6
  #define RTE_UART1_RI_FUNC             2
#else
  #error "Invalid UART1_RI Pin Configuration!"
#endif
#ifndef   RTE_UART1_RI_PIN_EN
  #define RTE_UART1_RI_PIN_EN           1
#endif
//     </h> Modem Lines
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>3 (DMAMUXPER3)
//     </e>
#define   RTE_UART1_DMA_TX_EN           0
#define   RTE_UART1_DMA_TX_CH           0
#define   RTE_UART1_DMA_TX_PERI_ID      0
#if      (RTE_UART1_DMA_TX_PERI_ID == 0)
  #define RTE_UART1_DMA_TX_PERI         3
  #define RTE_UART1_DMA_TX_PERI_SEL     1
#endif
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral <0=>4 (DMAMUXPER4)
//     </e>
#define   RTE_UART1_DMA_RX_EN           1
#define   RTE_UART1_DMA_RX_CH           1
#define   RTE_UART1_DMA_RX_PERI_ID      0
#if      (RTE_UART1_DMA_RX_PERI_ID == 0)
  #define RTE_UART1_DMA_RX_PERI         4
  #define RTE_UART1_DMA_RX_PERI_SEL     1
#endif
//   </h> DMA
// </e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]

// <e> USART2 (Universal synchronous asynchronous receiver transmitter) [Driver_USART2]
#define   RTE_USART2                    0

//   <h> Pin Configuration
//     <o> TX <0=>Not used <1=>P1_15 <2=>P2_10 <3=>P7_1 <4=>PA_1
//     <i> USART2 Serial Output pin
#define   RTE_USART2_TX_ID              0
#if      (RTE_USART2_TX_ID == 0)
  #define RTE_USART2_TX_PIN_EN          0
#elif    (RTE_USART2_TX_ID == 1)
  #define RTE_USART2_TX_PORT            1
  #define RTE_USART2_TX_BIT             15
  #define RTE_USART2_TX_FUNC            1
#elif    (RTE_USART2_TX_ID == 2)
  #define RTE_USART2_TX_PORT            2
  #define RTE_USART2_TX_BIT             10
  #define RTE_USART2_TX_FUNC            2
#elif    (RTE_USART2_TX_ID == 3)
  #define RTE_USART2_TX_PORT            7
  #define RTE_USART2_TX_BIT             1
  #define RTE_USART2_TX_FUNC            6
#elif    (RTE_USART2_TX_ID == 4)
  #define RTE_USART2_TX_PORT            0xA
  #define RTE_USART2_TX_BIT             1
  #define RTE_USART2_TX_FUNC            3
#else
  #error "Invalid USART2_TX Pin Configuration!"
#endif
#ifndef   RTE_USART2_TX_PIN_EN
  #define RTE_USART2_TX_PIN_EN          1
#endif
//     <o> RX <0=>Not used <1=>P1_16 <2=>P2_11 <3=>P7_2 <4=>PA_2
//     <i> USART2 Serial Input pin
#define   RTE_USART2_RX_ID              0
#if      (RTE_USART2_RX_ID == 0)
  #define RTE_USART2_RX_PIN_EN          0
#elif    (RTE_USART2_RX_ID == 1)
  #define RTE_USART2_RX_PORT            1
  #define RTE_USART2_RX_BIT             16
  #define RTE_USART2_RX_FUNC            1
#elif    (RTE_USART2_RX_ID == 2)
  #define RTE_USART2_RX_PORT            2
  #define RTE_USART2_RX_BIT             11
  #define RTE_USART2_RX_FUNC            2
#elif    (RTE_USART2_RX_ID == 3)
  #define RTE_USART2_RX_PORT            7
  #define RTE_USART2_RX_BIT             2
  #define RTE_USART2_RX_FUNC            6
#elif    (RTE_USART2_RX_ID == 4)
  #define RTE_USART2_RX_PORT            0xA
  #define RTE_USART2_RX_BIT             2
  #define RTE_USART2_RX_FUNC            3
#else
  #error "Invalid USART2_RX Pin Configuration!"
#endif
#ifndef   RTE_USART2_RX_PIN_EN
  #define RTE_USART2_RX_PIN_EN          1
#endif
//       <o> UCLK (Synchronous and SmartCard mode) <0=>Not used <1=>P1_17 <2=>P2_12
//       <i> USART2 Serial Clock input/output synchronous mode
#define   RTE_USART2_UCLK_ID            0
#if      (RTE_USART2_UCLK_ID == 0)
  #define RTE_USART2_UCLK_PIN_EN        0
#elif    (RTE_USART2_UCLK_ID == 1)
  #define RTE_USART2_UCLK_PORT          1
  #define RTE_USART2_UCLK_BIT           17
  #define RTE_USART2_UCLK_FUNC          1
#elif    (RTE_USART2_UCLK_ID == 2)
  #define RTE_USART2_UCLK_PORT          2
  #define RTE_USART2_UCLK_BIT           12
  #define RTE_USART2_UCLK_FUNC          7
#else
  #error "Invalid USART2_UCLK Pin Configuration!"
#endif
#ifndef   RTE_USART2_UCLK_PIN_EN
  #define RTE_USART2_UCLK_PIN_EN        1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>5 (DMAMUXPER5)
//     </e>
#define   RTE_USART2_DMA_TX_EN          0
#define   RTE_USART2_DMA_TX_CH          0
#define   RTE_USART2_DMA_TX_PERI_ID     0
#if      (RTE_USART2_DMA_TX_PERI_ID == 0)
  #define RTE_USART2_DMA_TX_PERI        5
  #define RTE_USART2_DMA_TX_PERI_SEL    1
#endif
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral <0=>6 (DMAMUXPER6)
//     </e>
#define   RTE_USART2_DMA_RX_EN          0
#define   RTE_USART2_DMA_RX_CH          1
#define   RTE_USART2_DMA_RX_PERI_ID     0
#if      (RTE_USART2_DMA_RX_PERI_ID == 0)
  #define RTE_USART2_DMA_RX_PERI        6
  #define RTE_USART2_DMA_RX_PERI_SEL    1
#endif
//   </h> DMA
// </e> USART2 (Universal synchronous asynchronous receiver transmitter) [Driver_USART2]

// <e> USART3 (Universal synchronous asynchronous receiver transmitter) [Driver_USART3]
#define   RTE_USART3                    0

//   <h> Pin Configuration
//     <o> TX <0=>Not used <1=>P2_3 <2=>P4_1 <3=>P9_3 <4=>PF_2
//     <i> USART3 Serial Output pin
#define   RTE_USART3_TX_ID              0
#if      (RTE_USART3_TX_ID == 0)
  #define RTE_USART3_TX_PIN_EN          0
#elif    (RTE_USART3_TX_ID == 1)
  #define RTE_USART3_TX_PORT            2
  #define RTE_USART3_TX_BIT             3
  #define RTE_USART3_TX_FUNC            2
#elif    (RTE_USART3_TX_ID == 2)
  #define RTE_USART3_TX_PORT            4
  #define RTE_USART3_TX_BIT             1
  #define RTE_USART3_TX_FUNC            6
#elif    (RTE_USART3_TX_ID == 3)
  #define RTE_USART3_TX_PORT            9
  #define RTE_USART3_TX_BIT             3
  #define RTE_USART3_TX_FUNC            7
#elif    (RTE_USART3_TX_ID == 4)
  #define RTE_USART3_TX_PORT            0xF
  #define RTE_USART3_TX_BIT             2
  #define RTE_USART3_TX_FUNC            1
#else
  #error "Invalid USART3_TX Pin Configuration!"
#endif
#ifndef   RTE_USART3_TX_PIN_EN
  #define RTE_USART3_TX_PIN_EN          1
#endif
//     <o> RX <0=>Not used <1=>P2_4 <2=>P4_2 <3=>P9_4 <4=>PF_3
//     <i> USART3 Serial Input pin
#define   RTE_USART3_RX_ID              0
#if      (RTE_USART3_RX_ID == 0)
  #define RTE_USART3_RX_PIN_EN          0
#elif    (RTE_USART3_RX_ID == 1)
  #define RTE_USART3_RX_PORT            2
  #define RTE_USART3_RX_BIT             4
  #define RTE_USART3_RX_FUNC            2
#elif    (RTE_USART3_RX_ID == 2)
  #define RTE_USART3_RX_PORT            4
  #define RTE_USART3_RX_BIT             2
  #define RTE_USART3_RX_FUNC            6
#elif    (RTE_USART3_RX_ID == 3)
  #define RTE_USART3_RX_PORT            9
  #define RTE_USART3_RX_BIT             4
  #define RTE_USART3_RX_FUNC            7
#elif    (RTE_USART3_RX_ID == 4)
  #define RTE_USART3_RX_PORT            0xF
  #define RTE_USART3_RX_BIT             3
  #define RTE_USART3_RX_FUNC            1
#else
  #error "Invalid USART3_RX Pin Configuration!"
#endif
#ifndef   RTE_USART3_RX_PIN_EN
  #define RTE_USART3_RX_PIN_EN          1
#endif
//     <o> UCLK (Synchronous and SmartCard mode) <0=>Not used <1=>P2_7 <2=>P4_0 <3=>PF_5
//     <i> USART3 Serial Clock input/output synchronous mode
#define   RTE_USART3_UCLK_ID            0
#if      (RTE_USART3_UCLK_ID == 0)
  #define RTE_USART3_UCLK_PIN_EN        0
#elif    (RTE_USART3_UCLK_ID == 1)
  #define RTE_USART3_UCLK_PORT          2
  #define RTE_USART3_UCLK_BIT           7
  #define RTE_USART3_UCLK_FUNC          2
#elif    (RTE_USART3_UCLK_ID == 2)
  #define RTE_USART3_UCLK_PORT          4
  #define RTE_USART3_UCLK_BIT           0
  #define RTE_USART3_UCLK_FUNC          6
#elif    (RTE_USART3_UCLK_ID == 3)
  #define RTE_USART3_UCLK_PORT          0xF
  #define RTE_USART3_UCLK_BIT           5
  #define RTE_USART3_UCLK_FUNC          1
#else
  #error "Invalid USART3_UCLK Pin Configuration!"
#endif
#ifndef   RTE_USART3_UCLK_PIN_EN
  #define RTE_USART3_UCLK_PIN_EN        1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>7 (DMAMUXPER7)  <1=>14 (DMAMUXPER14)
//     </e>
#define   RTE_USART3_DMA_TX_EN          0
#define   RTE_USART3_DMA_TX_CH          0
#define   RTE_USART3_DMA_TX_PERI_ID     0
#if      (RTE_USART3_DMA_TX_PERI_ID == 0)
  #define RTE_USART3_DMA_TX_PERI        7
  #define RTE_USART3_DMA_TX_PERI_SEL    1
#elif    (RTE_USART3_DMA_TX_PERI_ID == 1)
  #define RTE_USART3_DMA_TX_PERI        14
  #define RTE_USART3_DMA_TX_PERI_SEL    3
#endif
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral <0=>8 (DMAMUXPER8)  <1=>13 (DMAMUXPER13)
//     </e>
#define   RTE_USART3_DMA_RX_EN          0
#define   RTE_USART3_DMA_RX_CH          1
#define   RTE_USART3_DMA_RX_PERI_ID     0
#if      (RTE_USART3_DMA_RX_PERI_ID == 0)
  #define RTE_USART3_DMA_RX_PERI        8
  #define RTE_USART3_DMA_RX_PERI_SEL    1
#elif    (RTE_USART3_DMA_RX_PERI_ID == 1)
  #define RTE_USART3_DMA_RX_PERI        13
  #define RTE_USART3_DMA_RX_PERI_SEL    3
#endif
//   </h> DMA
// </e> USART3 (Universal synchronous asynchronous receiver transmitter) [Driver_USART3]

// <e> SSP0 (Synchronous Serial Port 0) [Driver_SPI0]
// <i> Configuration settings for Driver_SPI0 in component ::Drivers:SPI
#define   RTE_SSP0                      0

//   <h> Pin Configuration
//     <o> SSP0_SSEL <0=>Not used <1=>P1_0 <2=>P3_6 <3=>P3_8 <4=>P9_0 <5=>PF_1
//     <i> Slave Select for SSP0
#define   RTE_SSP0_SSEL_PIN_SEL         1
#if      (RTE_SSP0_SSEL_PIN_SEL == 0)
#define   RTE_SSP0_SSEL_PIN_EN          0
#elif    (RTE_SSP0_SSEL_PIN_SEL == 1)
  #define RTE_SSP0_SSEL_PORT            1
  #define RTE_SSP0_SSEL_BIT             0
  #define RTE_SSP0_SSEL_FUNC            5
  #define RTE_SSP0_SSEL_GPIO_FUNC       0
  #define RTE_SSP0_SSEL_GPIO_PORT       0
  #define RTE_SSP0_SSEL_GPIO_BIT        4
#elif    (RTE_SSP0_SSEL_PIN_SEL == 2)
  #define RTE_SSP0_SSEL_PORT            3
  #define RTE_SSP0_SSEL_BIT             6
  #define RTE_SSP0_SSEL_FUNC            2
  #define RTE_SSP0_SSEL_GPIO_FUNC       0
  #define RTE_SSP0_SSEL_GPIO_PORT       0
  #define RTE_SSP0_SSEL_GPIO_BIT        6
#elif    (RTE_SSP0_SSEL_PIN_SEL == 3)
  #define RTE_SSP0_SSEL_PORT            3
  #define RTE_SSP0_SSEL_BIT             8
  #define RTE_SSP0_SSEL_FUNC            5
  #define RTE_SSP0_SSEL_GPIO_FUNC       4
  #define RTE_SSP0_SSEL_GPIO_PORT       5
  #define RTE_SSP0_SSEL_GPIO_BIT        11
#elif    (RTE_SSP0_SSEL_PIN_SEL == 4)
  #define RTE_SSP0_SSEL_PORT            9
  #define RTE_SSP0_SSEL_BIT             0
  #define RTE_SSP0_SSEL_FUNC            7
  #define RTE_SSP0_SSEL_GPIO_FUNC       0
  #define RTE_SSP0_SSEL_GPIO_PORT       4
  #define RTE_SSP0_SSEL_GPIO_BIT        12
#elif    (RTE_SSP0_SSEL_PIN_SEL == 5)
  #define RTE_SSP0_SSEL_PORT            0xF
  #define RTE_SSP0_SSEL_BIT             1
  #define RTE_SSP0_SSEL_FUNC            2
  #define RTE_SSP0_SSEL_GPIO_FUNC       4
  #define RTE_SSP0_SSEL_GPIO_PORT       7
  #define RTE_SSP0_SSEL_GPIO_BIT        16
#else
  #error "Invalid SSP0 SSP0_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP0_SSEL_PIN_EN
#define   RTE_SSP0_SSEL_PIN_EN          1
#endif
//     <o> SSP0_SCK <0=>P3_0 <1=>P3_3 <2=>PF_0
//     <i> Serial clock for SSP0
#define   RTE_SSP0_SCK_PIN_SEL          0
#if      (RTE_SSP0_SCK_PIN_SEL == 0)
  #define RTE_SSP0_SCK_PORT             3
  #define RTE_SSP0_SCK_BIT              0
  #define RTE_SSP0_SCK_FUNC             4
#elif    (RTE_SSP0_SCK_PIN_SEL == 1)
  #define RTE_SSP0_SCK_PORT             3
  #define RTE_SSP0_SCK_BIT              3
  #define RTE_SSP0_SCK_FUNC             2
#elif    (RTE_SSP0_SCK_PIN_SEL == 2)
  #define RTE_SSP0_SCK_PORT             0xF
  #define RTE_SSP0_SCK_BIT              0
  #define RTE_SSP0_SCK_FUNC             0
#else
  #error "Invalid SSP0 SSP0_SCK Pin Configuration!"
#endif
//     <o> SSP0_MISO <0=>Not used <1=>P1_1 <2=>P3_6 <3=>P3_7 <4=>P9_1 <5=>PF_2
//     <i> Master In Slave Out for SSP0
#define   RTE_SSP0_MISO_PIN_SEL         0
#if      (RTE_SSP0_MISO_PIN_SEL == 0)
  #define RTE_SSP0_MISO_PIN_EN          0
#elif    (RTE_SSP0_MISO_PIN_SEL == 1)
  #define RTE_SSP0_MISO_PORT            1
  #define RTE_SSP0_MISO_BIT             1
  #define RTE_SSP0_MISO_FUNC            5
#elif    (RTE_SSP0_MISO_PIN_SEL == 2)
  #define RTE_SSP0_MISO_PORT            3
  #define RTE_SSP0_MISO_BIT             6
  #define RTE_SSP0_MISO_FUNC            5
#elif    (RTE_SSP0_MISO_PIN_SEL == 3)
  #define RTE_SSP0_MISO_PORT            3
  #define RTE_SSP0_MISO_BIT             7
  #define RTE_SSP0_MISO_FUNC            2
#elif    (RTE_SSP0_MISO_PIN_SEL == 4)
  #define RTE_SSP0_MISO_PORT            9
  #define RTE_SSP0_MISO_BIT             1
  #define RTE_SSP0_MISO_FUNC            7
#elif    (RTE_SSP0_MISO_PIN_SEL == 5)
  #define RTE_SSP0_MISO_PORT            0xF
  #define RTE_SSP0_MISO_BIT             2
  #define RTE_SSP0_MISO_FUNC            2
#else
  #error "Invalid SSP0 SSP0_MISO Pin Configuration!"
#endif
#ifndef   RTE_SSP0_MISO_PIN_EN
  #define RTE_SSP0_MISO_PIN_EN          1
#endif
//     <o> SSP0_MOSI <0=>Not used <1=>P1_2 <2=>P3_7 <3=>P3_8 <4=>P9_2 <5=>PF_3
//     <i> Master Out Slave In for SSP0
#define   RTE_SSP0_MOSI_PIN_SEL         0
#if      (RTE_SSP0_MOSI_PIN_SEL == 0)
  #define RTE_SSP0_MOSI_PIN_EN          0
#elif    (RTE_SSP0_MOSI_PIN_SEL == 1)
  #define RTE_SSP0_MOSI_PORT            1
  #define RTE_SSP0_MOSI_BIT             2
  #define RTE_SSP0_MOSI_FUNC            5
#elif    (RTE_SSP0_MOSI_PIN_SEL == 2)
  #define RTE_SSP0_MOSI_PORT            3
  #define RTE_SSP0_MOSI_BIT             7
  #define RTE_SSP0_MOSI_FUNC            5
#elif    (RTE_SSP0_MOSI_PIN_SEL == 3)
  #define RTE_SSP0_MOSI_PORT            3
  #define RTE_SSP0_MOSI_BIT             8
  #define RTE_SSP0_MOSI_FUNC            2
#elif    (RTE_SSP0_MOSI_PIN_SEL == 4)
  #define RTE_SSP0_MOSI_PORT            9
  #define RTE_SSP0_MOSI_BIT             2
  #define RTE_SSP0_MOSI_FUNC            7
#elif    (RTE_SSP0_MOSI_PIN_SEL == 5)
  #define RTE_SSP0_MOSI_PORT            0xF
  #define RTE_SSP0_MOSI_BIT             3
  #define RTE_SSP0_MOSI_FUNC            2
#else
  #error "Invalid SSP0 SSP0_MOSI Pin Configuration!"
#endif
#ifndef   RTE_SSP0_MOSI_PIN_EN
  #define RTE_SSP0_MOSI_PIN_EN          1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>10 (DMAMUXPER10)
//     </e>
#define   RTE_SSP0_DMA_TX_EN            0
#define   RTE_SSP0_DMA_TX_CH            0
#define   RTE_SSP0_DMA_TX_PERI_ID       0
#if      (RTE_SSP0_DMA_TX_PERI_ID == 0)
  #define RTE_SSP0_DMA_TX_PERI          10
  #define RTE_SSP0_DMA_TX_PERI_SEL      0
#endif
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>9 (DMAMUXPER9)
//     </e>
#define   RTE_SSP0_DMA_RX_EN            0
#define   RTE_SSP0_DMA_RX_CH            1
#define   RTE_SSP0_DMA_RX_PERI_ID       0
#if      (RTE_SSP0_DMA_RX_PERI_ID == 0)
  #define RTE_SSP0_DMA_RX_PERI          9
  #define RTE_SSP0_DMA_RX_PERI_SEL      0
#endif
//   </h> DMA
// </e> SSP0 (Synchronous Serial Port 0) [Driver_SPI0]

// <e> SSP1 (Synchronous Serial Port 1) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::Drivers:SPI
#define   RTE_SSP1                      0

//   <h> Pin Configuration
//     <o> SSP1_SSEL <0=>Not used <1=>P1_5 <2=>P1_20 <3=>PF_5
//     <i> Slave Select for SSP1
#define   RTE_SSP1_SSEL_PIN_SEL         1
#if      (RTE_SSP1_SSEL_PIN_SEL == 0)
  #define RTE_SSP1_SSEL_PIN_EN          0
#elif    (RTE_SSP1_SSEL_PIN_SEL == 1)
  #define RTE_SSP1_SSEL_PORT            1
  #define RTE_SSP1_SSEL_BIT             5
  #define RTE_SSP1_SSEL_FUNC            5
  #define RTE_SSP1_SSEL_GPIO_FUNC       0
  #define RTE_SSP1_SSEL_GPIO_PORT       1
  #define RTE_SSP1_SSEL_GPIO_BIT        8
#elif    (RTE_SSP1_SSEL_PIN_SEL == 2)
  #define RTE_SSP1_SSEL_PORT            1
  #define RTE_SSP1_SSEL_BIT             20
  #define RTE_SSP1_SSEL_FUNC            1
  #define RTE_SSP1_SSEL_GPIO_FUNC       0
  #define RTE_SSP1_SSEL_GPIO_PORT       0
  #define RTE_SSP1_SSEL_GPIO_BIT        15
#elif    (RTE_SSP1_SSEL_PIN_SEL == 3)
  #define RTE_SSP1_SSEL_PORT            0xF
  #define RTE_SSP1_SSEL_BIT             5
  #define RTE_SSP1_SSEL_FUNC            2
  #define RTE_SSP1_SSEL_GPIO_FUNC       4
  #define RTE_SSP1_SSEL_GPIO_PORT       7
  #define RTE_SSP1_SSEL_GPIO_BIT        19
#else
  #error "Invalid SSP1 SSP1_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP1_SSEL_PIN_EN
#define   RTE_SSP1_SSEL_PIN_EN          1
#endif
//     <o> SSP1_SCK <0=>P1_19 <1=>PF_4 <2=>CLK0
//     <i> Serial clock for SSP1
#define   RTE_SSP1_SCK_PIN_SEL          0
#if      (RTE_SSP1_SCK_PIN_SEL == 0)
  #define RTE_SSP1_SCK_PORT             1
  #define RTE_SSP1_SCK_BIT              19
  #define RTE_SSP1_SCK_FUNC             1
#elif    (RTE_SSP1_SCK_PIN_SEL == 1)
  #define RTE_SSP1_SCK_PORT             0xF
  #define RTE_SSP1_SCK_BIT              4
  #define RTE_SSP1_SCK_FUNC             0
#elif    (RTE_SSP1_SCK_PIN_SEL == 2)
  #define RTE_SSP1_SCK_PORT             0x10
  #define RTE_SSP1_SCK_BIT              0
  #define RTE_SSP1_SCK_FUNC             6
#else
  #error "Invalid SSP1 SSP1_SCK Pin Configuration!"
#endif
//     <o> SSP1_MISO <0=>Not used <1=>P0_0 <2=>P1_3 <3=>PF_6
//     <i> Master In Slave Out for SSP1
#define   RTE_SSP1_MISO_PIN_SEL         0
#if      (RTE_SSP1_MISO_PIN_SEL == 0)
  #define RTE_SSP1_MISO_PIN_EN          0
#elif    (RTE_SSP1_MISO_PIN_SEL == 1)
  #define RTE_SSP1_MISO_PORT            0
  #define RTE_SSP1_MISO_BIT             0
  #define RTE_SSP1_MISO_FUNC            1
#elif    (RTE_SSP1_MISO_PIN_SEL == 2)
  #define RTE_SSP1_MISO_PORT            1
  #define RTE_SSP1_MISO_BIT             3
  #define RTE_SSP1_MISO_FUNC            5
#elif    (RTE_SSP1_MISO_PIN_SEL == 3)
  #define RTE_SSP1_MISO_PORT            0xF
  #define RTE_SSP1_MISO_BIT             6
  #define RTE_SSP1_MISO_FUNC            2
#else
  #error "Invalid SSP1 SSP1_MISO Pin Configuration!"
#endif
#ifndef   RTE_SSP1_MISO_PIN_EN
  #define RTE_SSP1_MISO_PIN_EN          1
#endif
//     <o> SSP1_MOSI <0=>Not used <1=>P0_1 <2=>P1_4 <3=>PF_7
//     <i> Master Out Slave In for SSP1
#define   RTE_SSP1_MOSI_PIN_SEL         0
#if      (RTE_SSP1_MOSI_PIN_SEL == 0)
  #define RTE_SSP1_MOSI_PIN_EN          0
#elif    (RTE_SSP1_MOSI_PIN_SEL == 1)
  #define RTE_SSP1_MOSI_PORT            0
  #define RTE_SSP1_MOSI_BIT             1
  #define RTE_SSP1_MOSI_FUNC            1
#elif    (RTE_SSP1_MOSI_PIN_SEL == 2)
  #define RTE_SSP1_MOSI_PORT            1
  #define RTE_SSP1_MOSI_BIT             4
  #define RTE_SSP1_MOSI_FUNC            5
#elif    (RTE_SSP1_MOSI_PIN_SEL == 3)
  #define RTE_SSP1_MOSI_PORT            0xF
  #define RTE_SSP1_MOSI_BIT             7
  #define RTE_SSP1_MOSI_FUNC            2
#else
  #error "Invalid SSP1 SSP1_MOSI Pin Configuration!"
#endif
#ifndef   RTE_SSP1_MOSI_PIN_EN
  #define RTE_SSP1_MOSI_PIN_EN          1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>3 (DMAMUXPER3) <1=>5 (DMAMUXPER5) <2=>12 (DMAMUXPER12) <3=>14 (DMAMUXPER14)
//     </e>
#define   RTE_SSP1_DMA_TX_EN            0
#define   RTE_SSP1_DMA_TX_CH            0
#define   RTE_SSP1_DMA_TX_PERI_ID       0
#if      (RTE_SSP1_DMA_TX_PERI_ID == 0)
  #define RTE_SSP1_DMA_TX_PERI          3
  #define RTE_SSP1_DMA_TX_PERI_SEL      3
#elif    (RTE_SSP1_DMA_TX_PERI_ID == 1)
  #define RTE_SSP1_DMA_TX_PERI          5
  #define RTE_SSP1_DMA_TX_PERI_SEL      2
#elif    (RTE_SSP1_DMA_TX_PERI_ID == 2)
  #define RTE_SSP1_DMA_TX_PERI          12
  #define RTE_SSP1_DMA_TX_PERI_SEL      0
#elif    (RTE_SSP1_DMA_TX_PERI_ID == 3)
  #define RTE_SSP1_DMA_TX_PERI          14
  #define RTE_SSP1_DMA_TX_PERI_SEL      2
#endif
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>4 (DMAMUXPER4) <1=>6 (DMAMUXPER6) <2=>11 (DMAMUXPER11) <3=>13 (DMAMUXPER13)
//     </e>
#define   RTE_SSP1_DMA_RX_EN            0
#define   RTE_SSP1_DMA_RX_CH            1
#define   RTE_SSP1_DMA_RX_PERI_ID       0
#if      (RTE_SSP1_DMA_RX_PERI_ID == 0)
  #define RTE_SSP1_DMA_RX_PERI          4
  #define RTE_SSP1_DMA_RX_PERI_SEL      3
#elif    (RTE_SSP1_DMA_RX_PERI_ID == 1)
  #define RTE_SSP1_DMA_RX_PERI          6
  #define RTE_SSP1_DMA_RX_PERI_SEL      2
#elif    (RTE_SSP1_DMA_RX_PERI_ID == 2)
  #define RTE_SSP1_DMA_RX_PERI          11
  #define RTE_SSP1_DMA_RX_PERI_SEL      0
#elif    (RTE_SSP1_DMA_RX_PERI_ID == 3)
  #define RTE_SSP1_DMA_RX_PERI          13
  #define RTE_SSP1_DMA_RX_PERI_SEL      2
#endif
//   </h> DMA
// </e> SSP1 (Synchronous Serial Port 1) [Driver_SPI1]

// <e> SPI (Serial Peripheral Interface) [Driver_SPI2]
// <i> Configuration settings for Driver_SPI2 in component ::Drivers:SPI
#define   RTE_SPI                       0

//   <h> Pin Configuration
//     <o> SPI_SSEL <0=>Not used <1=>P3_8
//     <i> Slave Select for SPI
#define   RTE_SPI_SSEL_PIN_SEL          0
#if      (RTE_SPI_SSEL_PIN_SEL == 0)
#define   RTE_SPI_SSEL_PIN_EN           0
#elif    (RTE_SPI_SSEL_PIN_SEL == 1)
  #define RTE_SPI_SSEL_PORT             3
  #define RTE_SPI_SSEL_BIT              8
  #define RTE_SPI_SSEL_FUNC             1
  #define RTE_SPI_SSEL_GPIO_FUNC        4
  #define RTE_SPI_SSEL_GPIO_PORT        5
  #define RTE_SPI_SSEL_GPIO_BIT         11
#else
  #error "Invalid SPI SPI_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SPI_SSEL_PIN_EN
#define   RTE_SPI_SSEL_PIN_EN           1
#endif
//     <o> SPI_SCK <0=>P3_3
//     <i> Serial clock for SPI
#define   RTE_SPI_SCK_PIN_SEL           0
#if      (RTE_SPI_SCK_PIN_SEL == 0)
  #define RTE_SPI_SCK_PORT              3
  #define RTE_SPI_SCK_BIT               3
  #define RTE_SPI_SCK_FUNC              1
#else
  #error "Invalid SPI SPI_SCK Pin Configuration!"
#endif
//     <o> SPI_MISO <0=>Not used <1=>P3_6
//     <i> Master In Slave Out for SPI
#define   RTE_SPI_MISO_PIN_SEL          0
#if      (RTE_SPI_MISO_PIN_SEL == 0)
  #define RTE_SPI_MISO_PIN_EN           0
#elif    (RTE_SPI_MISO_PIN_SEL == 1)
  #define RTE_SPI_MISO_PORT             3
  #define RTE_SPI_MISO_BIT              6
  #define RTE_SPI_MISO_FUNC             1
#else
  #error "Invalid SPI SPI_MISO Pin Configuration!"
#endif
#ifndef   RTE_SPI_MISO_PIN_EN
  #define RTE_SPI_MISO_PIN_EN           1
#endif
//     <o> SPI_MOSI <0=>Not used <1=>P3_7
//     <i> Master Out Slave In for SPI
#define   RTE_SPI_MOSI_PIN_SEL          0
#if      (RTE_SPI_MOSI_PIN_SEL == 0)
  #define RTE_SPI_MOSI_PIN_EN           0
#elif    (RTE_SPI_MOSI_PIN_SEL == 1)
  #define RTE_SPI_MOSI_PORT             3
  #define RTE_SPI_MOSI_BIT              7
  #define RTE_SPI_MOSI_FUNC             1
#else
  #error "Invalid SPI SPI_MOSI Pin Configuration!"
#endif
#ifndef   RTE_SPI_MOSI_PIN_EN
  #define RTE_SPI_MOSI_PIN_EN           1
#endif
//   </h> Pin Configuration
// </e> SPI (Serial Peripheral Interface) [Driver_SPI2]

// <e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]
// <i> Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define   RTE_I2S0                      0

//   <h> Pin Configuration
//     <o> I2S0_RX_SCK <0=>Not used <1=>P3_0 <2=>P6_0 <3=>PF_4
//     <i> Receive clock for I2S0
#define   RTE_I2S0_RX_SCK_PIN_SEL       2
#if      (RTE_I2S0_RX_SCK_PIN_SEL == 0)
#define   RTE_I2S0_RX_SCK_PIN_EN        0
#elif    (RTE_I2S0_RX_SCK_PIN_SEL == 1)
  #define RTE_I2S0_RX_SCK_PORT          3
  #define RTE_I2S0_RX_SCK_BIT           0
  #define RTE_I2S0_RX_SCK_FUNC          0
#elif    (RTE_I2S0_RX_SCK_PIN_SEL == 2)
  #define RTE_I2S0_RX_SCK_PORT          6
  #define RTE_I2S0_RX_SCK_BIT           0
  #define RTE_I2S0_RX_SCK_FUNC          4
#elif    (RTE_I2S0_RX_SCK_PIN_SEL == 3)
  #define RTE_I2S0_RX_SCK_PORT          0xF
  #define RTE_I2S0_RX_SCK_BIT           4
  #define RTE_I2S0_RX_SCK_FUNC          7
#else
  #error "Invalid I2S0 I2S0_RX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_SCK_PIN_EN
#define   RTE_I2S0_RX_SCK_PIN_EN        1
#endif
//     <o> I2S0_RX_WS <0=>Not used <1=>P3_1 <2=>P6_1
//     <i> Receive word select for I2S0
#define   RTE_I2S0_RX_WS_PIN_SEL        2
#if      (RTE_I2S0_RX_WS_PIN_SEL == 0)
#define   RTE_I2S0_RX_WS_PIN_EN         0
#elif    (RTE_I2S0_RX_WS_PIN_SEL == 1)
  #define RTE_I2S0_RX_WS_PORT           3
  #define RTE_I2S0_RX_WS_BIT            1
  #define RTE_I2S0_RX_WS_FUNC           1
#elif    (RTE_I2S0_RX_WS_PIN_SEL == 2)
  #define RTE_I2S0_RX_WS_PORT           6
  #define RTE_I2S0_RX_WS_BIT            1
  #define RTE_I2S0_RX_WS_FUNC           3
#else
  #error "Invalid I2S0 I2S0_RX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_WS_PIN_EN
#define   RTE_I2S0_RX_WS_PIN_EN         1
#endif
//     <o> I2S0_RX_SDA <0=>Not used <1=>P3_2 <2=>P6_2
//     <i> Receive master clock for I2S0
#define   RTE_I2S0_RX_SDA_PIN_SEL       2
#if      (RTE_I2S0_RX_SDA_PIN_SEL == 0)
#define   RTE_I2S0_RX_SDA_PIN_EN        0
#elif    (RTE_I2S0_RX_SDA_PIN_SEL == 1)
  #define RTE_I2S0_RX_SDA_PORT          3
  #define RTE_I2S0_RX_SDA_BIT           2
  #define RTE_I2S0_RX_SDA_FUNC          1
#elif    (RTE_I2S0_RX_SDA_PIN_SEL == 2)
  #define RTE_I2S0_RX_SDA_PORT          6
  #define RTE_I2S0_RX_SDA_BIT           2
  #define RTE_I2S0_RX_SDA_FUNC          3
#else
  #error "Invalid I2S0 I2S0_RX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_SDA_PIN_EN
#define   RTE_I2S0_RX_SDA_PIN_EN       1
#endif
//     <o> I2S0_RX_MCLK <0=>Not used <1=>P1_19 <2=>P3_0 <3=>P6_0
//     <i> Receive master clock for I2S0
#define   RTE_I2S0_RX_MCLK_PIN_SEL      0
#if      (RTE_I2S0_RX_MCLK_PIN_SEL == 0)
#define   RTE_I2S0_RX_MCLK_PIN_EN       0
#elif    (RTE_I2S0_RX_MCLK_PIN_SEL == 1)
  #define RTE_I2S0_RX_MCLK_PORT         1
  #define RTE_I2S0_RX_MCLK_BIT          19
  #define RTE_I2S0_RX_MCLK_FUNC         6
#elif    (RTE_I2S0_RX_MCLK_PIN_SEL == 2)
  #define RTE_I2S0_RX_MCLK_PORT         3
  #define RTE_I2S0_RX_MCLK_BIT          0
  #define RTE_I2S0_RX_MCLK_FUNC         1
#elif    (RTE_I2S0_RX_MCLK_PIN_SEL == 3)
  #define RTE_I2S0_RX_MCLK_PORT         6
  #define RTE_I2S0_RX_MCLK_BIT          0
  #define RTE_I2S0_RX_MCLK_FUNC         1
#else
  #error "Invalid I2S0 I2S0_RX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_MCLK_PIN_EN
#define   RTE_I2S0_RX_MCLK_PIN_EN       1
#endif
//     <o> I2S0_TX_SCK <0=>Not used <1=>P3_0 <2=>P4_7
//     <i> Transmit clock for I2S0
#define   RTE_I2S0_TX_SCK_PIN_SEL       1
#if      (RTE_I2S0_TX_SCK_PIN_SEL == 0)
#define   RTE_I2S0_TX_SCK_PIN_EN        0
#elif    (RTE_I2S0_TX_SCK_PIN_SEL == 1)
  #define RTE_I2S0_TX_SCK_PORT          3
  #define RTE_I2S0_TX_SCK_BIT           0
  #define RTE_I2S0_TX_SCK_FUNC          2
#elif    (RTE_I2S0_TX_SCK_PIN_SEL == 2)
  #define RTE_I2S0_TX_SCK_PORT          4
  #define RTE_I2S0_TX_SCK_BIT           7
  #define RTE_I2S0_TX_SCK_FUNC          7
#else
  #error "Invalid I2S0 I2S0_TX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_SCK_PIN_EN
#define   RTE_I2S0_TX_SCK_PIN_EN        1
#endif
//     <o> I2S0_TX_WS <0=>Not used <1=>P0_0 <2=>P3_1 <3=>P3_4 <4=>P7_1 <5=>P9_1 <6=>PC_13
//     <i> Transmit word select for I2S0
#define   RTE_I2S0_TX_WS_PIN_SEL        4
#if      (RTE_I2S0_TX_WS_PIN_SEL == 0)
#define   RTE_I2S0_TX_WS_PIN_EN         0
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 1)
  #define RTE_I2S0_TX_WS_PORT           0
  #define RTE_I2S0_TX_WS_BIT            0
  #define RTE_I2S0_TX_WS_FUNC           6
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 2)
  #define RTE_I2S0_TX_WS_PORT           3
  #define RTE_I2S0_TX_WS_BIT            1
  #define RTE_I2S0_TX_WS_FUNC           0
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 3)
  #define RTE_I2S0_TX_WS_PORT           3
  #define RTE_I2S0_TX_WS_BIT            4
  #define RTE_I2S0_TX_WS_FUNC           5
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 4)
  #define RTE_I2S0_TX_WS_PORT           7
  #define RTE_I2S0_TX_WS_BIT            1
  #define RTE_I2S0_TX_WS_FUNC           2
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 5)
  #define RTE_I2S0_TX_WS_PORT           9
  #define RTE_I2S0_TX_WS_BIT            1
  #define RTE_I2S0_TX_WS_FUNC           4
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 6)
  #define RTE_I2S0_TX_WS_PORT           0xC
  #define RTE_I2S0_TX_WS_BIT            13
  #define RTE_I2S0_TX_WS_FUNC           6
#else
  #error "Invalid I2S0 I2S0_TX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_WS_PIN_EN
#define   RTE_I2S0_TX_WS_PIN_EN         1
#endif
//     <o> I2S0_TX_SDA <0=>Not used <1=>P3_2 <2=>P3_5 <3=>P7_2 <4=>P9_2  <5=>PC_12
//     <i> Transmit data for I2S0
#define   RTE_I2S0_TX_SDA_PIN_SEL       3
#if      (RTE_I2S0_TX_SDA_PIN_SEL == 0)
#define   RTE_I2S0_TX_SDA_PIN_EN        0
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 1)
  #define RTE_I2S0_TX_SDA_PORT          3
  #define RTE_I2S0_TX_SDA_BIT           2
  #define RTE_I2S0_TX_SDA_FUNC          0
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 2)
  #define RTE_I2S0_TX_SDA_PORT          3
  #define RTE_I2S0_TX_SDA_BIT           5
  #define RTE_I2S0_TX_SDA_FUNC          5
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 3)
  #define RTE_I2S0_TX_SDA_PORT          7
  #define RTE_I2S0_TX_SDA_BIT           2
  #define RTE_I2S0_TX_SDA_FUNC          2
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 4)
  #define RTE_I2S0_TX_SDA_PORT          9
  #define RTE_I2S0_TX_SDA_BIT           2
  #define RTE_I2S0_TX_SDA_FUNC          4
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 5)
  #define RTE_I2S0_TX_SDA_PORT          0xC
  #define RTE_I2S0_TX_SDA_BIT           12
  #define RTE_I2S0_TX_SDA_FUNC          6
#else
  #error "Invalid I2S0 I2S0_TX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_SDA_PIN_EN
#define   RTE_I2S0_TX_SDA_PIN_EN        1
#endif
//     <o> I2S0_TX_MCLK <0=>Not used <1=>P3_0 <2=>P3_3 <3=>PF_4 <4=>CLK2
//     <i> Transmit master clock for I2S0
#define   RTE_I2S0_TX_MCLK_PIN_SEL      2
#if      (RTE_I2S0_TX_MCLK_PIN_SEL == 0)
#define   RTE_I2S0_TX_MCLK_PIN_EN       0
#elif    (RTE_I2S0_TX_MCLK_PIN_SEL == 1)
  #define RTE_I2S0_TX_MCLK_PORT         3
  #define RTE_I2S0_TX_MCLK_BIT          0
  #define RTE_I2S0_TX_MCLK_FUNC         3
#elif    (RTE_I2S0_TX_MCLK_PIN_SEL == 2)
  #define RTE_I2S0_TX_MCLK_PORT         3
  #define RTE_I2S0_TX_MCLK_BIT          3
  #define RTE_I2S0_TX_MCLK_FUNC         6
#elif    (RTE_I2S0_TX_MCLK_PIN_SEL == 3)
  #define RTE_I2S0_TX_MCLK_PORT         0xf
  #define RTE_I2S0_TX_MCLK_BIT          4
  #define RTE_I2S0_TX_MCLK_FUNC         6
#elif    (RTE_I2S0_TX_MCLK_PIN_SEL == 4)
  #define RTE_I2S0_TX_MCLK_PORT         0x10
  #define RTE_I2S0_TX_MCLK_BIT          2
  #define RTE_I2S0_TX_MCLK_FUNC         6
#else
  #error "Invalid I2S0 I2S0_TX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_MCLK_PIN_EN
#define   RTE_I2S0_TX_MCLK_PIN_EN       1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>9 (DMAMUXPER9)
//     </e>
#define   RTE_I2S0_DMA_TX_EN            0
#define   RTE_I2S0_DMA_TX_CH            0
#define   RTE_I2S0_DMA_TX_PERI_ID       0
#if      (RTE_I2S0_DMA_TX_PERI_ID == 0)
  #define RTE_I2S0_DMA_TX_PERI          9
  #define RTE_I2S0_DMA_TX_PERI_SEL      1
#endif
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>10 (DMAMUXPER10)
//     </e>
#define   RTE_I2S0_DMA_RX_EN            0
#define   RTE_I2S0_DMA_RX_CH            1
#define   RTE_I2S0_DMA_RX_PERI_ID       0
#if      (RTE_I2S0_DMA_RX_PERI_ID == 0)
  #define RTE_I2S0_DMA_RX_PERI          10
  #define RTE_I2S0_DMA_RX_PERI_SEL      1
#endif
//   </h> DMA
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]

// <e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]
// <i> Configuration settings for Driver_I2S1 in component ::Drivers:SAI
#define   RTE_I2S1                      0

//   <h> Pin Configuration
//     <o> I2S1_RX_SCK <0=>Not used <1=>CLK2 <2=>CLK3
//     <i> Receive clock for I2S1
#define   RTE_I2S1_RX_SCK_PIN_SEL       0
#if      (RTE_I2S1_RX_SCK_PIN_SEL == 0)
#define   RTE_I2S1_RX_SCK_PIN_EN        0
#elif    (RTE_I2S1_RX_SCK_PIN_SEL == 1)
  #define RTE_I2S1_RX_SCK_PORT          0x10
  #define RTE_I2S1_RX_SCK_BIT           2
  #define RTE_I2S1_RX_SCK_FUNC          7
#elif    (RTE_I2S1_RX_SCK_PIN_SEL == 2)
  #define RTE_I2S1_RX_SCK_PORT          0x10
  #define RTE_I2S1_RX_SCK_BIT           3
  #define RTE_I2S1_RX_SCK_FUNC          7
#else
  #error "Invalid I2S1 I2S1_RX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S1_RX_SCK_PIN_EN
#define   RTE_I2S1_RX_SCK_PIN_EN        1
#endif
//     <o> I2S1_RX_WS <0=>Not used <1=>P3_5
//     <i> Receive word select for I2S1
#define   RTE_I2S1_RX_WS_PIN_SEL        0
#if      (RTE_I2S1_RX_WS_PIN_SEL == 0)
#define   RTE_I2S1_RX_WS_PIN_EN         0
#elif    (RTE_I2S1_RX_WS_PIN_SEL == 1)
  #define RTE_I2S1_RX_WS_PORT           3
  #define RTE_I2S1_RX_WS_BIT            5
  #define RTE_I2S1_RX_WS_FUNC           6
#else
  #error "Invalid I2S1 I2S1_RX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S1_RX_WS_PIN_EN
#define   RTE_I2S1_RX_WS_PIN_EN         1
#endif
//     <o> I2S1_RX_SDA <0=>Not used <1=>P3_4
//     <i> Receive master clock for I2S1
#define   RTE_I2S1_RX_SDA_PIN_SEL       0
#if      (RTE_I2S1_RX_SDA_PIN_SEL == 0)
#define   RTE_I2S1_RX_SDA_PIN_EN        0
#elif    (RTE_I2S1_RX_SDA_PIN_SEL == 1)
  #define RTE_I2S1_RX_SDA_PORT          3
  #define RTE_I2S1_RX_SDA_BIT           4
  #define RTE_I2S1_RX_SDA_FUNC          6
#else
  #error "Invalid I2S1 I2S1_RX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S1_RX_SDA_PIN_EN
#define   RTE_I2S1_RX_SDA_PIN_EN       1
#endif
//     <o> I2S1_RX_MCLK <0=>Not used <1=>PA_0
//     <i> Receive master clock for I2S1
#define   RTE_I2S1_RX_MCLK_PIN_SEL      0
#if      (RTE_I2S1_RX_MCLK_PIN_SEL == 0)
#define   RTE_I2S1_RX_MCLK_PIN_EN       0
#elif    (RTE_I2S1_RX_MCLK_PIN_SEL == 1)
  #define RTE_I2S1_RX_MCLK_PORT         0x0A
  #define RTE_I2S1_RX_MCLK_BIT          0
  #define RTE_I2S1_RX_MCLK_FUNC         5
#else
  #error "Invalid I2S1 I2S1_RX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S1_RX_MCLK_PIN_EN
#define   RTE_I2S1_RX_MCLK_PIN_EN       1
#endif
//     <o> I2S1_TX_SCK <0=>Not used <1=>P1_19 <2=>P3_3 <3=>P4_7
//     <i> Transmit clock for I2S1
#define   RTE_I2S1_TX_SCK_PIN_SEL       0
#if      (RTE_I2S1_TX_SCK_PIN_SEL == 0)
#define   RTE_I2S1_TX_SCK_PIN_EN        0
#elif    (RTE_I2S1_TX_SCK_PIN_SEL == 1)
  #define RTE_I2S1_TX_SCK_PORT          1
  #define RTE_I2S1_TX_SCK_BIT           19
  #define RTE_I2S1_TX_SCK_FUNC          7
#elif    (RTE_I2S1_TX_SCK_PIN_SEL == 2)
  #define RTE_I2S1_TX_SCK_PORT          3
  #define RTE_I2S1_TX_SCK_BIT           3
  #define RTE_I2S1_TX_SCK_FUNC          7
#elif    (RTE_I2S1_TX_SCK_PIN_SEL == 3)
  #define RTE_I2S1_TX_SCK_PORT          4
  #define RTE_I2S1_TX_SCK_BIT           7
  #define RTE_I2S1_TX_SCK_FUNC          6
#else
  #error "Invalid I2S1 I2S1_TX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S1_TX_SCK_PIN_EN
#define   RTE_I2S1_TX_SCK_PIN_EN        1
#endif
//     <o> I2S1_TX_WS <0=>Not used <1=>P0_0 <2=>PF_7
//     <i> Transmit word select for I2S1
#define   RTE_I2S1_TX_WS_PIN_SEL        0
#if      (RTE_I2S1_TX_WS_PIN_SEL == 0)
#define   RTE_I2S1_TX_WS_PIN_EN         0
#elif    (RTE_I2S1_TX_WS_PIN_SEL == 1)
  #define RTE_I2S1_TX_WS_PORT           0
  #define RTE_I2S1_TX_WS_BIT            0
  #define RTE_I2S1_TX_WS_FUNC           7
#elif    (RTE_I2S1_TX_WS_PIN_SEL == 2)
  #define RTE_I2S1_TX_WS_PORT           0x0F
  #define RTE_I2S1_TX_WS_BIT            7
  #define RTE_I2S1_TX_WS_FUNC           7
#else
  #error "Invalid I2S1 I2S1_TX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S1_TX_WS_PIN_EN
#define   RTE_I2S1_TX_WS_PIN_EN         1
#endif
//     <o> I2S1_TX_SDA <0=>Not used <1=>P0_1 <2=>PF_6
//     <i> Transmit data for I2S
#define   RTE_I2S1_TX_SDA_PIN_SEL       0
#if      (RTE_I2S1_TX_SDA_PIN_SEL == 0)
#define   RTE_I2S1_TX_SDA_PIN_EN        0
#elif    (RTE_I2S1_TX_SDA_PIN_SEL == 1)
  #define RTE_I2S1_TX_SDA_PORT          0
  #define RTE_I2S1_TX_SDA_BIT           1
  #define RTE_I2S1_TX_SDA_FUNC          7
#elif    (RTE_I2S1_TX_SDA_PIN_SEL == 2)
  #define RTE_I2S1_TX_SDA_PORT          0x0F
  #define RTE_I2S1_TX_SDA_BIT           6
  #define RTE_I2S1_TX_SDA_FUNC          7
#else
  #error "Invalid I2S1 I2S1_TX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S1_TX_SDA_PIN_EN
#define   RTE_I2S1_TX_SDA_PIN_EN        1
#endif
//     <o> I2S1_TX_MCLK <0=>Not used <1=>P8_8 <2=>PF_0 <3=>CLK1
//     <i> Transmit master clock for I2S1
#define   RTE_I2S1_TX_MCLK_PIN_SEL      0
#if      (RTE_I2S1_TX_MCLK_PIN_SEL == 0)
#define   RTE_I2S1_TX_MCLK_PIN_EN       0
#elif    (RTE_I2S1_TX_MCLK_PIN_SEL == 1)
  #define RTE_I2S1_TX_MCLK_PORT         8
  #define RTE_I2S1_TX_MCLK_BIT          8
  #define RTE_I2S1_TX_MCLK_FUNC         7
#elif    (RTE_I2S1_TX_MCLK_PIN_SEL == 2)
  #define RTE_I2S1_TX_MCLK_PORT         0x0F
  #define RTE_I2S1_TX_MCLK_BIT          0
  #define RTE_I2S1_TX_MCLK_FUNC         7
#elif    (RTE_I2S1_TX_MCLK_PIN_SEL == 3)
  #define RTE_I2S1_TX_MCLK_PORT         0x10
  #define RTE_I2S1_TX_MCLK_BIT          1
  #define RTE_I2S1_TX_MCLK_FUNC         7
#else
  #error "Invalid I2S1 I2S1_TX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S1_TX_MCLK_PIN_EN
#define   RTE_I2S1_TX_MCLK_PIN_EN       1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>3 (DMAMUXPER3)
//     </e>
#define   RTE_I2S1_DMA_TX_EN            0
#define   RTE_I2S1_DMA_TX_CH            0
#define   RTE_I2S1_DMA_TX_PERI_ID       0
#if      (RTE_I2S1_DMA_TX_PERI_ID == 0)
  #define RTE_I2S1_DMA_TX_PERI          3
  #define RTE_I2S1_DMA_TX_PERI_SEL      2
#endif
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>4 (DMAMUXPER4)
//     </e>
#define   RTE_I2S1_DMA_RX_EN            0
#define   RTE_I2S1_DMA_RX_CH            1
#define   RTE_I2S1_DMA_RX_PERI_ID       0
#if      (RTE_I2S1_DMA_RX_PERI_ID == 0)
  #define RTE_I2S1_DMA_RX_PERI          4
  #define RTE_I2S1_DMA_RX_PERI_SEL      2
#endif
//   </h> DMA
// </e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]

// <e> CAN0 Controller [Driver_CAN0]
// <i> Configuration settings for Driver_CAN0 in component ::Drivers:CAN
#define   RTE_CAN_CAN0                  0

//   <h> Pin Configuration
//     <o> CAN0_RD <0=>Not used <1=>P3_1 <2=>PE_2
//     <i> CAN0 receiver input.
#define   RTE_CAN0_RD_ID                0
#if      (RTE_CAN0_RD_ID == 0)
  #define RTE_CAN0_RD_PIN_EN            0
#elif    (RTE_CAN0_RD_ID == 1)
  #define RTE_CAN0_RD_PORT              3
  #define RTE_CAN0_RD_BIT               1
  #define RTE_CAN0_RD_FUNC              2
#elif    (RTE_CAN0_RD_ID == 2)
  #define RTE_CAN0_RD_PORT              0xE
  #define RTE_CAN0_RD_BIT               2
  #define RTE_CAN0_RD_FUNC              1
#else
  #error "Invalid RTE_CAN0_RD Pin Configuration!"
#endif
#ifndef   RTE_CAN0_RD_PIN_EN
  #define RTE_CAN0_RD_PIN_EN            1
#endif
//     <o> CAN0_TD <0=>Not used <1=>P3_2 <2=>PE_3
//     <i> CAN0 transmitter output.
#define   RTE_CAN0_TD_ID                0
#if      (RTE_CAN0_TD_ID == 0)
  #define RTE_CAN0_TD_PIN_EN            0
#elif    (RTE_CAN0_TD_ID == 1)
  #define RTE_CAN0_TD_PORT              3
  #define RTE_CAN0_TD_BIT               2
  #define RTE_CAN0_TD_FUNC              2
#elif    (RTE_CAN0_TD_ID == 2)
  #define RTE_CAN0_TD_PORT              0xE
  #define RTE_CAN0_TD_BIT               3
  #define RTE_CAN0_TD_FUNC              1
#else
  #error "Invalid RTE_CAN0_TD Pin Configuration!"
#endif
#ifndef   RTE_CAN0_TD_PIN_EN
  #define RTE_CAN0_TD_PIN_EN            1
#endif
//   </h> Pin Configuration
// </e> CAN0 Controller [Driver_CAN0]

// <e> CAN1 Controller [Driver_CAN1]
// <i> Configuration settings for Driver_CAN1 in component ::Drivers:CAN
#define   RTE_CAN_CAN1                  0

//   <h> Pin Configuration
//     <o> CAN1_RD <0=>Not used <1=>P1_18 <2=>P4_9 <3=>PE_1
//     <i> CAN1 receiver input.
#define   RTE_CAN1_RD_ID                0
#if      (RTE_CAN1_RD_ID == 0)
  #define RTE_CAN1_RD_PIN_EN            0
#elif    (RTE_CAN1_RD_ID == 1)
  #define RTE_CAN1_RD_PORT              1
  #define RTE_CAN1_RD_BIT               18
  #define RTE_CAN1_RD_FUNC              5
#elif    (RTE_CAN1_RD_ID == 2)
  #define RTE_CAN1_RD_PORT              4
  #define RTE_CAN1_RD_BIT               9
  #define RTE_CAN1_RD_FUNC              6
#elif    (RTE_CAN1_RD_ID == 3)
  #define RTE_CAN1_RD_PORT              0xE
  #define RTE_CAN1_RD_BIT               1
  #define RTE_CAN1_RD_FUNC              5
#else
  #error "Invalid RTE_CAN1_RD Pin Configuration!"
#endif
#ifndef   RTE_CAN1_RD_PIN_EN
  #define RTE_CAN1_RD_PIN_EN            1
#endif
//     <o> CAN1_TD <0=>Not used <1=>P1_17 <2=>P4_8 <3=>PE_0
//     <i> CAN1 transmitter output.
#define   RTE_CAN1_TD_ID                0
#if      (RTE_CAN1_TD_ID == 0)
  #define RTE_CAN1_TD_PIN_EN            0
#elif    (RTE_CAN1_TD_ID == 1)
  #define RTE_CAN1_TD_PORT              1
  #define RTE_CAN1_TD_BIT               17
  #define RTE_CAN1_TD_FUNC              5
#elif    (RTE_CAN1_TD_ID == 2)
  #define RTE_CAN1_TD_PORT              4
  #define RTE_CAN1_TD_BIT               8
  #define RTE_CAN1_TD_FUNC              6
#elif    (RTE_CAN1_TD_ID == 3)
  #define RTE_CAN1_TD_PORT              0xE
  #define RTE_CAN1_TD_BIT               0
  #define RTE_CAN1_TD_FUNC              5
#else
  #error "Invalid RTE_CAN1_TD Pin Configuration!"
#endif
#ifndef   RTE_CAN1_TD_PIN_EN
  #define RTE_CAN1_TD_PIN_EN            1
#endif
//   </h> Pin Configuration
// </e> CAN1 Controller [Driver_CAN1]


#endif  /* __RTE_DEVICE_H */
