/*******************************************************************************
  Clock System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_clk.h

  Summary:
    Clock system service interface header.

  Description:
    This file contains the interface definition for the Clock System
    Service.  It provides a way to interact with the Clock subsystem to
    manage the timing requests supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2016 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _SYS_CLK_H
#define _SYS_CLK_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system/common/sys_module.h"

#if defined(__PIC32MX__) || defined(__PIC32MZ__) || \
    defined(__PIC32MK__) || defined(__PIC32WK__)
    #include "peripheral/osc/plib_osc.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: SYS Clock Data Types - Clock Buses
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Peripherals Clock Buses enumeration

  Summary:
    Lists the available peripheral clock buses.

  Description:
    This enumeration lists all of the available peripheral clock buses. This is
    used by the SYS_CLK_PeripheralFrequencyGet and
    SYS_CLK_PeripheralFrequencySet functions.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Select peripheral bus 1. This should be the selection for the
    devices which don't have multiple peripheral buses */
    CLK_BUS_PERIPHERAL_1 = 0x00,

    /* Select peripheral bus 2 */
    CLK_BUS_PERIPHERAL_2 = 0x01,

    /* Select peripheral bus 3 */
    CLK_BUS_PERIPHERAL_3 = 0x02,

    /* Select peripheral bus 4 */
    CLK_BUS_PERIPHERAL_4 = 0x03,

    /* Select peripheral bus 5 */
    CLK_BUS_PERIPHERAL_5 = 0x04,

    /* Select peripheral bus 6 */
    CLK_BUS_PERIPHERAL_6 = 0x05,

    /* Select peripheral bus 7 */
    CLK_BUS_PERIPHERAL_7 = 0x06,

    /* Select peripheral bus 8 */
    CLK_BUS_PERIPHERAL_8 = 0x07,

} CLK_BUSES_PERIPHERAL;


// *****************************************************************************
/*  Reference Clock Buses enumeration

  Summary:
    Lists the available reference clock buses.

  Description:
    This enumeration lists all of the available Reference clock buses. This is
    used by the SYS_CLK_ReferenceFrequencyGet and SYS_CLK_ReferenceFrequencySet
    functions.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for the your device.
*/

typedef enum
{
    /* Select Reference bus 1. This should be the selection for the
    devices which don't have multiple Reference buses */
    CLK_BUS_REFERENCE_1 = 0x00,

    /* Select Reference bus 2 */
    CLK_BUS_REFERENCE_2 = 0x01,

    /* Select Reference bus 3 */
    CLK_BUS_REFERENCE_3 = 0x02,

    /* Select Reference bus 4 */
    CLK_BUS_REFERENCE_4 = 0x03,

    /* Select Reference bus 5 */
    CLK_BUS_REFERENCE_5 = 0x04,

} CLK_BUSES_REFERENCE;


// *****************************************************************************
// *****************************************************************************
// Section: SYS Clock Data Types - Clock sources
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  System Clock sources enumeration

  Summary:
    Lists the available clock sources for the system clock.

  Description:
    This enumeration lists all the available clock sources for the system clock.
    This is used by the SYS_CLK_INIT structure.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M
*/

typedef enum
{
    /* Source of clock is internal fast RC */
    SYS_CLK_SOURCE_FRC,

    /* Source of clock is internal fast RC multiplied by system PLL */
    SYS_CLK_SOURCE_FRC_SYSPLL,

    /* Source of clock is primary oscillator */
    SYS_CLK_SOURCE_PRIMARY,

    /* Source of clock is primary oscillator multiplied by
    the System PLL value and divided by the divisor configured by software */
    SYS_CLK_SOURCE_PRIMARY_SYSPLL,

    /* Source of clock is secondary oscillator */
    SYS_CLK_SOURCE_SECONDARY,

    /* Source of clock is internal low power RC */
    SYS_CLK_SOURCE_LPRC,

    /* Source of clock is internal fast RC divided by the divisor
    configured in software */
    SYS_CLK_SOURCE_FRC_BY_16,

    /* Source of clock is internal fast RC divided by the divisor
    configured in software */
    SYS_CLK_SOURCE_FRC_BY_DIV,
            
    /* Source of clock is backup fast RC */
    SYS_CLK_SOURCE_BKP_FRC,

    /* Source of clock is USB PLL output configured in software */
    SYS_CLK_SOURCE_UPLL,

    /* Source of clock is none*/
    SYS_CLK_SOURCE_NONE,

} CLK_SOURCES_SYSTEM;


// *****************************************************************************
/*  Peripheral clock sources enumeration

  Summary:
    Lists the available clock sources for the peripheral clock.

  Description:
    This enumeration lists all the available peripheral clock sources. This is
    used by the SYS_CLK_PeripheralFrequencySet function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M
*/

typedef enum
{
    /* Source of clock is system clock */
    CLK_SOURCE_PERIPHERAL_SYSTEMCLK

} CLK_SOURCES_PERIPHERAL;


// *****************************************************************************
/*  Reference clock sources enumeration

  Summary:
    Lists the available clock sources for the Reference clock.

  Description:
    This enumeration lists all the available peripheral clock sources. This is
    used by the SYS_CLK_ReferenceFrequencySet function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M
*/

typedef enum
{
    /* Source of Reference clock is System clock */
    CLK_SOURCE_REF_SYSTEMCLK,

    /* Source of Reference clock is Peripheral clock */
    CLK_SOURCE_REF_PBCLK_BUS1,

    /* Source of Reference clock is Primary oscillator */
    CLK_SOURCE_REF_PRIMARY,

    /* Source of Reference clock is Fast RC oscillator */
    CLK_SOURCE_REF_FRC,

    /* Source of Reference clock is Low power RC oscillator */
    CLK_SOURCE_REF_LPRC,

    /* Source of Reference clock is Secondary oscillator */
    CLK_SOURCE_REF_SECONDARY,

    /* Source of clock is output of USB PLL
    This is used only for the Reference clock. */
    CLK_SOURCE_REF_USBPLL_OUT ,

    /* Source of clock is the output of System PLL. Input to the
    system PLL may be Primary or FRC */
    CLK_SOURCE_REF_SYSPLL_OUT ,

    /* Source of clock is external(from the pin) */
    CLK_SOURCE_REF_EXTERNAL,

    /* Source of Reference clock is backup Fast RC oscillator */
    CLK_SOURCE_REF_BFRC,

} CLK_SOURCES_REFERENCE;


// *****************************************************************************
/*  System clock operation status

  Summary:
    Lists all the possible status of a system clock operation.

  Description:
    This enumeration lists all the possible status of a system clock operation.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32M
*/

typedef enum
{
    /* The last operation was successful */
    SYS_CLK_OPERATION_COMPLETE /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,

    /* Operation started, may be PLL is not yet locked */
    SYS_CLK_OPERATION_IN_PROGRESS /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,

    /* The last operation was not successful, Clock switching is not enabled
    through configuration bits */
    SYS_CLK_OPERATION_FAIL_SWITCHING_DISABLED /*DOM-IGNORE-BEGIN*/ = -1 /*DOM-IGNORE-END*/,

    /* The last operation was not successful, requested frequency
    is not achievable */
    SYS_CLK_OPERATION_FAIL_NOT_ACHIEVABLE /*DOM-IGNORE-BEGIN*/ = -2 /*DOM-IGNORE-END*/,

    /* The last operation was not successful, register is not ready to
    take the new value  */
    SYS_CLK_OPERATION_FAIL_REGISTER_NOT_READY /*DOM-IGNORE-BEGIN*/ = -3 /*DOM-IGNORE-END*/,

    /* The last operation was not successful, USB PLL is not enabled in
    through the 'configuration bits'. The same must be update in system_config.h  */
    SYS_CLK_OPERATION_FAIL_USBPLL_NOT_ENABLED /*DOM-IGNORE-BEGIN*/ = -4 /*DOM-IGNORE-END*/,

    /* PLL Lock time out. A restart is the only option  */
    SYS_CLK_OPERATION_FAIL_PLL_LOCK_TIMEOUT /*DOM-IGNORE-BEGIN*/ = -5 /*DOM-IGNORE-END*/,

    /* PLL cannot work with the current input clock frequency, adjust the
    clock input or the input divisor. Check the device data sheet to know the PLL input
    requirement */
    SYS_CLK_PLL_INVALID_INP_FREQUENCY /*DOM-IGNORE-BEGIN*/ = -6 /*DOM-IGNORE-END*/,

} SYS_CLK_STATUS;


// *****************************************************************************
/*  Internal Clock Outputs enumeration

  Summary:
    Lists the available internal clocks.

  Description:
    This enumeration lists all of the available internal clocks. This is
    used by the SYS_CLK_FrequencyGet function to get an actual frequency
    of the corresponding internal clock source.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Core Clock (HCLK) */
    SYS_CLK_PROCESSOR,

    /* System Clock (MCK) */
    SYS_CLK_MASTER,

    /* SysTick Clock */
    SYS_CLK_SYSTICK,

    /* Generic Clock 0 (I2S 0) */
    SYS_CLK_GENERIC_0,

    /* Generic Clock 1 (I2S 1) */
    SYS_CLK_GENERIC_1,

    /* Peripheral Clock 0 */
    SYS_CLK_PERIPHERAL_0,

    /* Peripheral Clock 1 */
    SYS_CLK_PERIPHERAL_1,

    /* Programmable Clock 0 */
    SYS_CLK_PROGRAMMABLE_0,

    /* Programmable Clock 1 */
    SYS_CLK_PROGRAMMABLE_1,

    /* Programmable Clock 2 */
    SYS_CLK_PROGRAMMABLE_2,

    /* Programmable Clock 3 (ETM) */
    SYS_CLK_PROGRAMMABLE_3,

    /* Programmable Clock 4 (UARTx / USARTx) */
    SYS_CLK_PROGRAMMABLE_4,

    /* Programmable Clock 5 (CANx) */
    SYS_CLK_PROGRAMMABLE_5,

    /* Programmable Clock 6 (TCx) */
    SYS_CLK_PROGRAMMABLE_6,

    /* Programmable Clock 7 (TC0) */
    SYS_CLK_PROGRAMMABLE_7,

    /* USB Full-Speed Clock (USB_48M) */
    SYS_CLK_USB_FS,

    /* USB High-Speed Clock (USB_480M) */
    SYS_CLK_USB_HS,

} SYS_CLK_CLOCK;


// *****************************************************************************
/*  Programmable Clock Outputs enumeration

  Summary:
    Lists the available Programmable Clocks.

  Description:
    This enumeration lists all of the available Programmable Clocks. This is
    used by the SYS_CLK_ProgrammableClockSetup to setup corresponding clock
    source. The resulting frequency can be obtained via SYS_CLK_FrequencyGet
    function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Programmable Clock 0. */
    SYS_CLK_PCK_0,

    /* Programmable Clock 1. */
    SYS_CLK_PCK_1,

    /* Programmable Clock 2. */
    SYS_CLK_PCK_2,

    /* Programmable Clock 3. */
    SYS_CLK_PCK_3,

    /* Programmable Clock 4. */
    SYS_CLK_PCK_4,

    /* Programmable Clock 5. */
    SYS_CLK_PCK_5,

    /* Programmable Clock 6. */
    SYS_CLK_PCK_6,

    /* Programmable Clock 7. */
    SYS_CLK_PCK_7,

} SYS_CLK_PCK;


// *****************************************************************************
/*  Generic Clock Outputs enumeration

  Summary:
    Lists the available Generic Clocks.

  Description:
    This enumeration lists all of the available Generic Clocks. This is
    used by the SYS_CLK_GenericClockSetup to setup corresponding clock
    source. The resulting frequency can be obtained via SYS_CLK_FrequencyGet
    function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Generic clock 0. */
    SYS_CLK_GCLK_0,

    /* Generic clock 1. */
    SYS_CLK_GCLK_1,

} SYS_CLK_GCLK;


// *****************************************************************************
/*  Clock sources enumeration

  Summary:
    Lists the available clock sources for the internal clocks.

  Description:
    This enumeration lists all of the available clock sources.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
    Some peripherals can only use limited subset of listed clock sources.
*/

typedef enum
{
    /* Slow clock source. */
    SYS_CLK_SOURCE_SLOW,

    /* Main clock source. */
    SYS_CLK_SOURCE_MAIN,

    /* Phase Locked Loop 'A' clock source. */
    SYS_CLK_SOURCE_PLLA,

    /* USB Phase Locked Loop clock source. */
    SYS_CLK_SOURCE_USB_PLL,

    /* Master clock source. */
    SYS_CLK_SOURCE_MASTER,

} SYS_CLK_SOURCE;


// *****************************************************************************
/*  Master Clock Prescaler enumeration

  Summary:
    Lists the available Master Clock Prescaler options.

  Description:
    This enumeration lists all of the available Master Clock Prescaler
    options. This is used by the SYS_CLK_MasterClockSetup function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Master clock prescaler:  Divide by 1. */
    SYS_CLK_MASTER_PRESCALER_1,

    /* Master clock prescaler:  Divide by 2. */
    SYS_CLK_MASTER_PRESCALER_2,

    /* Master clock prescaler:  Divide by 3. */
    SYS_CLK_MASTER_PRESCALER_3,

    /* Master clock prescaler:  Divide by 4. */
    SYS_CLK_MASTER_PRESCALER_4,

    /* Master clock prescaler:  Divide by 8. */
    SYS_CLK_MASTER_PRESCALER_8,

    /* Master clock prescaler:  Divide by 16. */
    SYS_CLK_MASTER_PRESCALER_16,

    /* Master clock prescaler:  Divide by 32. */
    SYS_CLK_MASTER_PRESCALER_32,

    /* Master clock prescaler:  Divide by 64. */
    SYS_CLK_MASTER_PRESCALER_64,

} SYS_CLK_MASTER_PRESCALER;


// *****************************************************************************
/*  Master Clock Divider enumeration

  Summary:
    Lists the available Master Clock Divider options.

  Description:
    This enumeration lists all of the available Master Clock postcaler divider
    options. This is used by the SYS_CLK_MasterClockSetup function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Master clock postcale divider:  Divide by 1. */
    SYS_CLK_MASTER_DIVIDER_1,

    /* Master clock postcale divider:  Divide by 2. */
    SYS_CLK_MASTER_DIVIDER_2,

    /* Master clock postcale divider:  Divide by 3. */
    SYS_CLK_MASTER_DIVIDER_3,

    /* Master clock postcale divider:  Divide by 4. */
    SYS_CLK_MASTER_DIVIDER_4,

} SYS_CLK_MASTER_DIVIDER;


// *****************************************************************************
/*  Frequency of the On-Chip RC Oscillator enumeration

  Summary:
    Lists the available frequencies for the On-Chip RC Oscillator.

  Description:
    This enumeration lists all of the available frequencies for
    the On-Chip RC Oscillato. This is used by the SYS_CLK_MainClockSetup
    function.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
*/

typedef enum
{
    /* Frequency of the On-Chip RC Oscillator: 4 MHz */
    SYS_CLK_RC_FREQUENCY_4_MHZ,

    /* Frequency of the On-Chip RC Oscillator: 8 MHz */
    SYS_CLK_RC_FREQUENCY_8_MHZ,

    /* Frequency of the On-Chip RC Oscillator: 12 MHz */
    SYS_CLK_RC_FREQUENCY_12_MHZ,

} SYS_CLK_RC_FREQUENCY;


// *****************************************************************************
/*  Main Clock sources enumeration

  Summary:
    Lists the available clock sources for the Main Clock.

  Description:
    This enumeration lists all of the available clock sources
    for the Main Clock.

  Remarks:
    This enumeration is supported on the following Microchip device families:
        - PIC32C

    The system service covers a wide range of devices. Not all the elements
    listed in here will be relevant for your device.
    Some peripherals can only use limited subset of listed clock sources.
*/

typedef enum
{
    /* Use On-Chip RC Oscillator as a clock source */
    SYS_CLK_MAIN_SOURCE_RC,

    /* Use External Crystal Oscillator as a clock source */
    SYS_CLK_MAIN_SOURCE_XTAL,

} SYS_CLK_MAIN_SOURCE;

// *****************************************************************************
/*  Main Clock Setup request structure

  Summary:
    Defines the data required to setup the Main Clock source.

  Description:
    This structure defines the data required to setup the Main
    Clock source.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

    This structure only includes the variable settings. Some of the settings
    depend on the hardware and are static for a given platform, so they
    are configured via static definitions.
*/

typedef struct
{
    /* Enable an External Crystal Oscillator */
    bool                 enableXtal;

    /* Enable Crystal Oscillator Bypass */
    bool                 bypassXtal;

    /* Keep On-Chip RC Oscillator enabled */
    bool                 enableRcOsc;

    /* Frequency of the On-Chip RC Oscillator */
    SYS_CLK_RC_FREQUENCY rcOscFrequency;

    /* Clock source for the Main Clock */
    SYS_CLK_MAIN_SOURCE  source;

} SYS_CLK_MAIN_SETUP;


// *****************************************************************************
/*  Slow Clock Setup request structure

  Summary:
    Defines the data required to setup the Slow Clock source.

  Description:
    This structure defines the data setup to initialize the Slow
    Clock source.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

    This structure only includes the variable settings. Some of the settings
    depend on the hardware and are static for a given platform, so they
    are configured via static definitions.

*/

typedef struct
{
    /* Use a crystal oscillator as a Slow Clock source */
    bool   enableXtal;

    /* Enable crystal oscillator bypass */
    bool   bypassXtal;

} SYS_CLK_SLOW_SETUP;


// *****************************************************************************
/*  PLLA Setup request structure

  Summary:
    Defines the data required to initialize the PLLA.

  Description:
    This structure defines the data required to initialize the PLLA.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

    This structure only includes the variable settings. Some of the settings
    depend on the hardware and are static for a given platform, so they
    are configured via static definitions.

*/

typedef struct
{
    /* Enable PLLA output */
    bool   enable;

    /* Multiplier for the PLLA output (valid range 2-63) */
    int    multiplier;

    /* Divider for the PLLA output (valid range 1-255) */
    int    divider;

} SYS_CLK_PLLA_SETUP;


// *****************************************************************************
/*  USB PLL Setup request structure

  Summary:
    Defines the data required to setup the USB PLL.

  Description:
    This structure defines the data required to setup the USB PLL.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

    This structure only includes the variable settings. Some of the settings
    depend on the hardware and are static for a given platform, so they
    are configured via static definitions.

*/

typedef struct
{
    /* Enable UPLL output */
    bool   enable;

    /* Multiplier for the USB PLL output (valid values are 30 and 40 for
       crystal frequencies of 16 MHz and 12 MHz) */
    int    multiplier;

    /* Divider for the USB PLL output (valid values are 1 and 2) */
    int    divider;

} SYS_CLK_UPLL_SETUP;


// *****************************************************************************
/*  Master Clock Setup request structure

  Summary:
    Defines the data required to setup the Master Clock.

  Description:
    This structure defines the data required to setup the Master Clock.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C
*/

typedef struct
{
    /* Source clock for the Master Clock unit */
    SYS_CLK_SOURCE             source;

    /* Prescaler common to all outputs */
    SYS_CLK_MASTER_PRESCALER   prescaler;

    /* Divider for the Master Clock */
    SYS_CLK_MASTER_DIVIDER     divider;

} SYS_CLK_MASTER_SETUP;


// *****************************************************************************
/*  Programmble Clock Setup request structure

  Summary:
    Defines the data required to initialize the Programmble Clock.

  Description:
    This structure defines the data required to initialize the Programmble Clock.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

*/

typedef struct
{
    /* Enable Programmble Clock output */
    bool             enable;

    /* Clock source for the Programmable Clock controller */
    SYS_CLK_SOURCE   source;

    /* Output frequency divider (valid range 1-256) */
    int              divider;

} SYS_CLK_PROGRAMMABLE_SETUP;


// *****************************************************************************
/*  Generic and Peripheral Clock Setup request structure

  Summary:
    Defines the data required to setup the Generic and Peripheral Clocks.

  Description:
    This structure defines the data required to setup the Generic and
    Peripheral Clocks.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

*/

typedef struct
{
    /* Enable Peripheral Clock output (Master Clock pass though) */
    bool             enableClockPeripheral;

    /* Enable Generic Clock output */
    bool             enableClockGeneric;

    /* Clock source for the Generic Clock */
    SYS_CLK_SOURCE   source;

    /* Output frequency divider (valid range 1-256) */
    int              divider;

} SYS_CLK_GENERIC_SETUP;


// *****************************************************************************
/*  Full-Speed USB Clock Setup request structure

  Summary:
    Defines the data required to setup the Full-Speed USB Clock.

  Description:
    This structure defines the data required to setup the Full-Speed
    USB Clock.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32C

*/

typedef struct
{
    /* Enable Full-Speed USB clock output */
    bool   enable;

    /* Clock source for the Generic Clock (valid values are SYS_CLK_SOURCE_PLLA
       and SYS_CLK_SOURCE_USB_PLL) */
    SYS_CLK_SOURCE   source;

    /* Output frequency divider (valid range 1-16) */
    int divider;

} SYS_CLK_USB_CLOCK_SETUP;


// *****************************************************************************
// *****************************************************************************
// Section: SYS Clock Data Types - Structures
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* CLK System Service Error Handler Function Pointer

   Summary:
    Pointer to a CLK System service SYS_CLK_ClockFailureCallbackRegister
    function.

   Description:
    This data type defines the type of the CLK System Service Error Handler
    callback function. The parameters are described here and a partial example
    implementation is provided. On a clock failure, the device will switch its
    clock source to the FRC clock. By registering the callback function user
    will get the current System clock source and the clock frequency. User can
    use this information to try switching back.

  Parameters:
    systemSource      - System clock source.
    systemFrequencyHz - System clock frequency in hertz.

  Returns:
    None.

  Example:
    <code>
    void SYS_CLK_ClockFailureCallbackRegister(  errorHandler );

    errorHandler ( CLK_SOURCES_SYSTEM systemSource, uint32_t systemFrequencyHz )
    {
        //Log a warning
        //Try switching back to the old source
    }
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
*/

typedef void ( *SYS_CLK_ERROR_HANDLER )  ( CLK_SOURCES_SYSTEM systemSource, uint32_t systemFrequencyHz );


// *****************************************************************************
/* Clock System Service Reference Oscillator initialization data

  Summary:
    Defines the data required to initialize the Oscillator for the Clock System
    Service.

  Description:
    This structure defines the data required to initialize the Oscillator
    for the Clock System Service.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32M
        - PIC32C

    This structure only includes the necessary settings for the clock module.
    Other features like USB clock and reference clock are considered to be
    optional and not every system will use it. There are dedicated APIs for
    configuring those.
*/

typedef    struct
{
#if defined(__PIC32MX__) || defined(__PIC32MZ__) || \
    defined(__PIC32MK__) || defined(__PIC32WK__)

    /* System module initialization */
    SYS_MODULE_INIT          moduleInit;

    /* Initializations for System clock  */
    CLK_SOURCES_SYSTEM       systemClockSource;

    /* Set the System clock frequency */
    uint32_t                 systemClockFrequencyHz;

    /* Wait until the clock switch operation is completed */
    bool                     waitTillComplete;

    /* Even though the secondary oscillator is not used, keeping the
    oscillator running, allows a fast switch to the lower system
    clock for low-power operation */
    bool                    secondaryOscKeepEnabled;

    /* System action on a 'Wait' instruction */
    OSC_OPERATION_ON_WAIT   onWaitInstruction;

#else

    /* System module initialization */
    SYS_MODULE_INIT         moduleInit;

#endif

} SYS_CLK_INIT;


// *****************************************************************************
/* Clock System Service Reference Oscillator initialization data

  Summary:
    Defines the data required to initialize the Oscillator for the Clock System
    Service.

  Description:
    This structure defines the data required to initialize the Oscillator
    for the Clock System Service.

  Remarks:
    This structure is supported on the following Microchip device families:
        - PIC32M

    This structure only includes the necessary settings for the clock module.
    Other features like USB clock and reference clock are considered to be
    optional and not every system will use it. There are dedicated APIs for
    configuring those.
*/

typedef    struct
{
    /* Reference clock module should stop its operation in 'Sleep' mode */
    bool suspendInSleep;

    /* Reference clock module should stop its operation in 'Idle' mode */
    bool stopInIdle;

} SYS_CLK_REFERENCE_SETUP;


// *****************************************************************************
/* Clock module FRC tuning type

  Summary:
    Defines the data type for tuning the Fast RC Oscillator.

  Description:
    This structure defines the data type to tune the Fast RC Oscillator.

  Remarks:
   None.
*/

typedef    unsigned int SYS_CLK_FRC_TUNING_TYPE;


// *****************************************************************************
// *****************************************************************************
// Section: SYS CLK Module System Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_CLK_Initialize ( const SYS_CLK_INIT * clkInit )

  Summary:
    Initializes hardware and internal data structure of the System Clock.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Precondition:
    None.

  Parameters:
    clkInit      - Pointer to a data structure containing any data necessary
                   to initialize the System Clock. This pointer can be
                   NULL if no data is required as static overrides have
                   been provided.

  Returns:
    None.

  Example:
    <code>
    //Example 1: Do not alter the configuration bit settings
    SYS_CLK_Initialize ( NULL );

    //Example 2: Want to change the configuration bit settings
    SYS_CLK_INIT generalInit;

    // Populate the oscillator initialization structure
    generalInit.systemClockSource = SYS_CLK_SOURCE_FRC_SYSPLL;
    generalInit.systemClockFrequencyHz = 30000000;
    generalInit.onWaitInstruction = OSC_ON_WAIT_SLEEP;
    generalInit.secondaryOscKeepEnabled = true;
    SYS_CLK_Initialize (&generalInit);

    //Wait until complete
    while ( SYS_CLK_SystemOperationStatus ( ) !=  SYS_CLK_OPERATION_COMPLETE );

    //Configuration success
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
        - PIC32C

    This is API must be be called at the time of system initialization to
    initialize the oscillator. Call this again to change any of the
    initialization settings.

    If the frequency settings are already done through the 'configuration bits',
    call this API by passing 'NULL' to it. In any case calling this API is
    necessary get the internal data structures updated.

    To change the clock source clock switching must be enabled through
    'configuration bits'.
*/

void SYS_CLK_Initialize ( const SYS_CLK_INIT * clkInit );


//******************************************************************************
/* Function:
    void SYS_CLK_TaskError ( void )

  Summary:
    Informs the user on a clock failure by invoking the registered call back
    function.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function informs the user on a clock failure by invoking the registered
    call back function. This must be called from the Fail Safe Clock Monitor (FSCM)
    interrupt service routine.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired System Clock

  Returns:
    None.

  Example:
    <code>
    void clockMonitorISR ( void )
    {
        SYS_CLK_TaskError ( void );
    }
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
*/

void SYS_CLK_TaskError ( void );


// *****************************************************************************
// *****************************************************************************
// Section: System clock APIs
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t SYS_CLK_SystemFrequencySet ( CLK_SOURCES_SYSTEM systemSource,
                                uint32_t systemClockHz, bool waitUntilComplete )

  Summary:
    Configures the System clock of the device to the value specified.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function configures the clock multipliers and divisors to achieve requested
    System clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API. To
    change the clock source, clock switching must be enabled through 'configuration bits'.


  Parameters:
    systemSource       - Clock source means the path of the clock signal. One of the
                        possible value from CLK_SOURCES_SYSTEM enum. See the block
                        diagram in the data sheet to get an idea.
    systemClockHz      - Required System Clock frequency in Hertz.
    waitUntilComplete - Passing this parameter as 'true' will make until the actual
                        hardware operation is complete.

  Returns:
    'Zero' on an unsuccessful operation. Configured system clock frequency on successful
    operation.

  Example:
    <code>
    uint32_t achievedFrequencyHz;

    // ******** Blocking call******
    achievedFrequencyHz = SYS_CLK_SystemFrequencySet (
        SYS_CLK_SOURCE_PRIMARY, 8000000, true );

    if ( achievedFrequencyHz != 0 )
    {
        //Clock setting complete
    }

    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    Getting the new frequency doesn't mean that the operation is complete if the
    API is not called to 'wait until' the operation is complete. The API will
    return with the possible value immediately. The actual hardware switching
    may take longer. Use SYS_CLK_SystemClockStatus function to get the status
    of completion.

    Calling this API is only necessary to change the system clock. Use
    SYS_CLK_Initialize function to set up the system clock initially.
*/

uint32_t SYS_CLK_SystemFrequencySet ( CLK_SOURCES_SYSTEM systemSource,
                                uint32_t systemClockHz, bool waitUntilComplete );


//******************************************************************************
/* Function:
    uint32_t SYS_CLK_SystemFrequencyGet ( void )

  Summary:
    Gets the system clock frequency in Hertz.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function gets the System clock frequency in Hertz.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    None.

  Returns:
    System clock frequency in Hertz.

  Example:
    <code>
    uint32_t sysClockHz;

    sysClockHz = SYS_CLK_SystemFrequencyGet ( );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
        - PIC32C
 */

uint32_t SYS_CLK_SystemFrequencyGet ( void );


// *****************************************************************************
// *****************************************************************************
// Section: Peripheral clock APIs
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t SYS_CLK_PeripheralFrequencySet ( CLK_BUSES_PERIPHERAL peripheralBus,
                CLK_SOURCES_PERIPHERAL peripheralSource, uint32_t peripheralClockHz,
                bool waitUntilComplete )

  Summary:
    Configures the peripheral clock of the device to the value specified.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function configures the clock multipliers and divisors to achieve requested
    Peripheral clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.
    If the operation is a failure, SYS_CLK_PeripheralClosestFrequencyGet function will
    give the closest possible frequency. If the closest possible value is acceptable,
    user can reconfigure with that value.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    peripheralBus     - Peripheral bus selection. One of the possible value from
                        CLK_BUSES_PERIPHERAL enum. For devices that do not have multiple
                        clock channels for Peripheral clock, CLK_BUS_PERIPHERAL_1 should
                        be the selection.
    peripheralSource  - Clock source means the path of the clock signal. One of the
                        possible value from CLK_SOURCES_PERIPHERAL enum. See the block
                        diagram in the data sheet to get an idea.
    peripheralClockHz - Clock frequency in Hertz.
    waitUntilComplete - Passing this parameter as 'true' will make until the actual
                        hardware operation is complete.

  Returns:
    None.

  Example:
    <code>
    uint32_t achievedFrequencyHz;

    //Example for MX, single peripheral bus
    achievedFrequencyHz = SYS_CLK_PeripheralFrequencySet ( CLK_BUS_PERIPHERAL_1,
        CLK_SOURCE_PERIPHERAL_SYSTEMCLK, 8000000, true );

    if ( achievedFrequencyHz !=  0 )
    {
        //Frequency successfully set
    }

    //Example for MZ, multiple peripheral buses
    achievedFrequencyHz = SYS_CLK_PeripheralFrequencySet ( CLK_BUS_PERIPHERAL_4,
        CLK_SOURCE_PERIPHERAL_SYSTEMCLK, 8000000, true );

    if ( achievedFrequencyHz !=  0 )
    {
        //Frequency successfully set
    }
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    Most devices do not have multiple Peripheral clock buses. In those instances,
    pass CLK_BUS_PERIPHERAL_1 as the bus number.
*/

uint32_t SYS_CLK_PeripheralFrequencySet ( CLK_BUSES_PERIPHERAL peripheralBus,
                CLK_SOURCES_PERIPHERAL peripheralSource, uint32_t peripheralClockHz,
                bool waitUntilComplete );


//******************************************************************************
/* Function:
    uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus )

  Summary:
    Gets the selected clock peripheral bus frequency in Hertz.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function gets the selected peripheral bus clock frequency in Hertz.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    peripheralBus - Reference clock bus selection. One of the possible value from
                    CLK_BUSES_PERIPHERAL enum. For devices that do not have multiple
                    clock channels for Reference clock, CLK_BUS_PERIPHERAL_1 should be
                    the selection.

  Returns:
    Clock frequency in Hertz.

  Example:
    <code>
    unsigned long peripheralClockHz;

    peripheralClockHz = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_PERIPHERAL_5 );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    Most devices do not have multiple peripheral clock buses. In those instances,
    pass CLK_USB_PERIPHERAL_1 as the bus number.
 */

uint32_t SYS_CLK_PeripheralFrequencyGet ( CLK_BUSES_PERIPHERAL peripheralBus );


// *****************************************************************************
// *****************************************************************************
// Section: Reference clock APIs
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_CLK_ReferenceClockSetup ( CLK_BUSES_REFERENCE referenceBus,
                                    SYS_CLK_REFERENCE_SETUP *refSetup )

  Summary:
    Sets up the reference clock of the device to the value specified.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets up the clock multipliers and divisors to achieve requested
    Reference clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.
    If the operation is a failure, SYS_CLK_ReferenceClosestFrequencyGet function will
    give the closest possible frequency. If the closest possible value is acceptable,
    user can reconfigure with that value.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    referenceBus  - Reference clock bus selection. One of the possible value from
                    CLK_BUSES_REFERENCE enum. For devices that do not have multiple
                    clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
                    the selection.
    refSetup      - A structure which holds the reference oscillator configuration.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_REFERENCE_SETUP refSetup;

    refSetup.suspendInSleep = true;
    refSetup.stopInIdle = true;

    SYS_CLK_ReferenceClockSetup ( CLK_BUS_REFERENCE_3, &refSetup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    Most devices do not have multiple reference clock buses. In those instances,
    pass CLK_BUS_REFERENCE_1 as the bus number.
*/

void SYS_CLK_ReferenceClockSetup ( CLK_BUSES_REFERENCE referenceBus,
                                   SYS_CLK_REFERENCE_SETUP *refSetup );


// *****************************************************************************
/* Function:
    uint32_t SYS_CLK_ReferenceFrequencySet ( CLK_BUSES_REFERENCE referenceBus,
                CLK_SOURCES_REFERENCE referenceSource, uint32_t referenceClockHz,
                bool waitUntilComplete )

  Summary:
    Sets the reference clock of the device to the value specified.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the clock multipliers and divisors to achieve requested
    Reference clock frequency. Initially it checks the difference between the requested
    value and possible value. If it is not within 'SYS_CLK_CONFIG_FREQ_ERROR_LIMIT',
    the registers values will not be changed and a value '0' will be returned to let
    user know that the operation was unsuccessful. If the value requested is acceptable,
    then it writes to the oscillator registers and return with the newly set frequency.
    If the operation is a failure, SYS_CLK_ReferenceClosestFrequencyGet function will
    give the closest possible frequency. If the closest possible value is acceptable,
    user can reconfigure with that value.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API. To
    change the clock source clock switching must be enabled through 'configuration
    bits'.

  Parameters:
    referenceBus  - Reference clock bus selection. One of the possible value from
                    CLK_BUSES_REFERENCE enum. For devices that do not have multiple
                    clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
                    the selection.

    referenceSource     - Clock source means the path of the clock signal. One of the
                          possible value from CLK_SOURCES_REFERENCE enum. See the block
                          diagram in the data sheet.

    referenceClockHz    - Clock frequency in Hertz.

    waitUntilComplete   - Passing this parameter as 'true' will make until the actual
                          hardware operation is complete.


  Returns:
    The actual frequency set or zero on failure.

  Example:
    <code>
    uint32_t achievedFrequencyHz;

    //I want 200 KHz as output
    achievedFrequencyHz = SYS_CLK_ReferenceFrequencySet ( CLK_BUS_REFERENCE_3,
        CLK_SOURCE_REF_FRC, 200000, true );

    if ( achievedFrequencyHz != 0 )
    {
        //Frequency successfully set
    }
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    Most devices do not have multiple reference clock buses. In those instances,
    pass CLK_BUS_REFERENCE_1 as the bus number.
*/

uint32_t SYS_CLK_ReferenceFrequencySet ( CLK_BUSES_REFERENCE referenceBus,
                CLK_SOURCES_REFERENCE referenceSource, uint32_t referenceClockHz,
                bool waitUntilComplete );


//******************************************************************************
/* Function:
    uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus )

  Summary:
    Gets the selected Reference clock bus frequency in Hertz.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function gets frequency of the selected Reference clock bus in Hertz.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    peripheralBus - Reference clock bus selection. One of the possible value from
                    CLK_BUSES_REFERENCE enum. For devices that do not have multiple
                    clock channels for Reference clock, CLK_BUS_REFERENCE_1 should be
                    the selection.

  Returns:
    Clock frequency in Hertz.

  Example:
    <code>
    unsigned long sysClockOutputHz;

    sysClockOutputHz = SYS_CLK_ReferenceFrequencyGet ( CLK_BUS_REFERENCE_3 );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
 */

uint32_t SYS_CLK_ReferenceFrequencyGet ( CLK_BUSES_REFERENCE referenceBus );


// *****************************************************************************
// *****************************************************************************
// Section: Error checking Routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void SYS_CLK_ClockFailureCallbackRegister ( SYS_CLK_ERROR_HANDLER callback )

  Summary:
    Allows registration of a call back function that will be triggered on a
    clock failure.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function allows registration of a callback function that will be
    triggered on a clock failure.

  Precondition:
    None.

  Parameters:
    callback     - Address of a callback function of type 'SYS_CLK_ERROR_HANDLER'.

  Returns:
    None.

  Example:
    <code>
    void ErrorHandle (void)
    {
        //Handle the error.
    }

    SYS_CLK_ClockFailureCallbackRegister ( ErrorHandle );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    This function is not available on all devices. Please refer to the specific
    device data sheet for availability.
*/

void SYS_CLK_ClockFailureCallbackRegister ( SYS_CLK_ERROR_HANDLER callback );


// *****************************************************************************
// *****************************************************************************
// Section: SYS Clock Optional features
// *****************************************************************************
// *****************************************************************************
/* These APIs may not be applicable for all the devices */

//******************************************************************************
/* Function:
    void SYS_CLK_FRCTune ( SYS_CLK_FRC_TUNING_TYPE tuningData )

  Summary:
    This function is used for direct value based FRC oscillator tuning.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function tunes the FRC as per the given value. FRC tuning functionality
    has been provided to help customers compensate for temperature effects on
    the FRC frequency over a wide range of temperatures.

  Precondition:
    The device selected must support the oscillator tuning feature.

  Parameters:
    tuningData - One of the possible value of Tuning. Refer to the specific device
                 data sheet for possible values.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_FRCTune( ox30 );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M

    The tuning step size is an approximation, and is neither characterized, nor
    tested.

    This API can only be used with devices that support direct value
    based FRC tuning. Refer to the specific device data sheet to determine
    whether this feature exists for your device.
 */

void SYS_CLK_FRCTune ( SYS_CLK_FRC_TUNING_TYPE tuningData );


// *****************************************************************************
/* Function:
    void SYS_CLK_SecondaryOscillatorEnable ( void )

  Summary:
    Enables the secondary oscillator.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function enables secondary oscillator which can be used as a clock
    source for peripherals like RTCC, Timer etc.. The SOSC requires a warm-up
    period of 1024 before it can be used as a clock source.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_SecondaryOscillatorEnable ( );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
*/

void SYS_CLK_SecondaryOscillatorEnable ( void );


// *****************************************************************************
/* Function:
    void SYS_CLK_SecondaryOscillatorDisable ( void )

  Summary:
    Disables the secondary oscillator.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function disables the secondary oscillator.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_SecondaryOscillatorDisable ( );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
*/

void SYS_CLK_SecondaryOscillatorDisable ( void );


// *****************************************************************************
/* Function:
    bool SYS_CLK_SecondaryOscillatorIsEnabled ( void )

  Summary:
    Identifies whether secondary oscillator is enabled or disabled.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function identifies whether the secondary oscillator is enabled or
    disabled.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    None.

  Returns:
    true    - If Secondary oscillator is enabled.
    false   - If Secondary oscillator is disabled.

  Example:
    <code>
    bool status;

    status = SYS_CLK_SecondaryOscillatorIsEnabled ( );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32M
*/

bool SYS_CLK_SecondaryOscillatorIsEnabled ( void );


//******************************************************************************
/* Function:
    uint32_t SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock )

  Summary:
    Gets the frequency of the specified clock source in Hertz.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function gets the frequency of the specified clock source in Hertz.

  Precondition:
    The SYS_CLK_Initialize function should be called before calling this API.

  Parameters:
    clock - Clock source selection. One of the possible value from
            SYS_CLK_CLOCK enum.

  Returns:
    Specified clock frequency in Hertz.

  Example:
    <code>
    uint32_t sysTickClockHz;

    sysTickClockHz = SYS_CLK_FrequencyGet ( SYS_CLK_SYSTICK );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

uint32_t SYS_CLK_FrequencyGet ( SYS_CLK_CLOCK clock );


//******************************************************************************
/* Function:
    void SYS_CLK_MainClockSetup ( const SYS_CLK_MAIN_SETUP * setup )

  Summary:
    Sets up the source for the Main Clock .
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the source for the Main Clock.

  Precondition:
    No other units should use Main Clock as a clock source.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_MAIN_SETUP setup;

    // Configure Main Clock to run from external crystal oscillator
    setup.enableXtal = true;
    setup.bypassXtal = false;

    SYS_CLK_MainClockSetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_MainClockSetup ( const SYS_CLK_MAIN_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_SlowClockSetup ( const SYS_CLK_SLOW_SETUP * setup )

  Summary:
    Sets up the source for the Slow Clock .
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the source for the Slow Clock.

  Precondition:
    No other units should use Slow Clock as a clock source.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_SLOW_SETUP setup;

    // Configure Slow Clock to run from external crystal oscillator
    setup.enableXtal = true;
    setup.bypassXtal = false;

    SYS_CLK_SlowClockSetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_SlowClockSetup ( const SYS_CLK_SLOW_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_PLLASetup ( const SYS_CLK_PLLA_SETUP * setup )

  Summary:
    Sets up the PLLA frequency.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the PLLA frequency.

  Precondition:
    No other units should use PLLA as a clock source.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_PLLA_SETUP setup;

    // Configure PLLA to generate 300 MHz, provided that
    // Main Clock frequency is 12 MHz
    setup.enable = true;
    setup.multiplier = 25;
    setup.divider = 1;

    SYS_CLK_PLLASetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_PLLASetup ( const SYS_CLK_PLLA_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_UPLLSetup ( const SYS_CLK_UPLL_SETUP * setup )

  Summary:
    Sets up the USB PLL frequency.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the USB PLL frequency.

  Precondition:
    No other units should use USB PLL as a clock source and Main
    Crystal Oscillator should output 12 MHz or 16 MHz.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_PLLA_SETUP setup;

    // Configure USB PLL to generate 480 MHz, provided that
    // Main Crystal oscillator frequency is 12 MHz
    setup.enable = true;
    setup.multiplier = 40;
    setup.divider = 1;

    SYS_CLK_UPLLSetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_UPLLSetup ( const SYS_CLK_UPLL_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_MasterClockSetup ( const SYS_CLK_MASTER_SETUP * setup )

  Summary:
    Sets up the Master Clock Controller.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the Master Clock Controller. This includes
    configuring the source clock for the Master Clock, Processor Clock and
    SysTick timer.

  Precondition:
    Selected clock source must be set up and enabled.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_MASTER_SETUP setup;

    // Configure Processor Clock to be 300 MHz and Master Clock
    // to be 150 MHz, provided that PLLA is set up to output 300 MHz.
    setup.source = SYS_CLK_SOURCE_PLLA;
    setup.prescaler = SYS_CLK_MCK_PRESCALER_1;
    setup.divider = SYS_CLK_MCK_DIVIDER_2;

    SYS_CLK_MasterClockSetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_MasterClockSetup ( const SYS_CLK_MASTER_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_ProgrammableClockSetup ( const SYS_CLK_PCK controller,
            const SYS_CLK_PROGRAMMABLE_SETUP * setup )

  Summary:
    Sets up the specified Programmable Clock.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the specified Programmable Clock.

  Precondition:
    Selected clock source must be set up and enabled.

  Parameters:
    controller - index of the Programmable Clock controller
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_PROGRAMMABLE_SETUP setup;

    // Configure TC0 (PCK 7) clock to be 1 kHz.
    setup.enable = true;
    setup.source = SYS_CLK_SOURCE_SLOW_CLOCK;
    setup.divider = 32;

    SYS_CLK_ProgrammableClockSetup ( SYS_CLK_PCK_7, &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_ProgrammableClockSetup ( const SYS_CLK_PCK controller,
        const SYS_CLK_PROGRAMMABLE_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_GenericClockSetup ( const SYS_CLK_GCLK controller,
            const SYS_CLK_GENERIC_SETUP * setup )

  Summary:
    Sets up the specified Generic and Peripheral Clock.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the specified Generic and Peripheral Clock.

  Precondition:
    Selected clock source must be set up and enabled.

  Parameters:
    controller - index of the Generic Clock controller
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_GENERIC_SETUP setup;

    // Configure Generic Clock 0 for 50 MHz operation, provided that
    // Master Clock is 150 MHz.
    setup.enablePeripheralClock = false;
    setup.enableGenericClock = true;
    setup.source = SYS_CLK_SOURCE_MASTER_CLOCK;
    setup.divider = 3;

    SYS_CLK_GenericClockSetup ( SYS_CLK_GCLK_0, &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_GenericClockSetup ( const SYS_CLK_GCLK controller,
    const SYS_CLK_GENERIC_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_USBClockSetup ( const SYS_CLK_USB_CLOCK_SETUP * setup )

  Summary:
    Sets up the Full-Spee USB Clock.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function sets up the Full-Speed USB Clock.

  Precondition:
    Selected clock source must be set up and enabled.

  Parameters:
    setup - Setup parameters.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_USB_CLOCK_SETUP setup;

    // Configure Full-Speed USB Clock to be 48 MHz, provided
    // that USB PLL is set up to output 480 MHz.
    setup.enable = true;
    setup.source = SYS_CLK_SOURCE_USB_PLL;
    setup.divider = 10;

    SYS_CLK_USBClockSetup ( &setup );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_USBClockSetup ( const SYS_CLK_USB_CLOCK_SETUP * setup );


//******************************************************************************
/* Function:
    void SYS_CLK_PeripheralEnable ( int peripheralId )

  Summary:
    Enables clock to the specified peripheral.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function enables clock to the specified peripheral.

  Precondition:
    None.

  Parameters:
    peripheralId - ID of the peripherl.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_PeripheralEnable ( ID_USART0 );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_PeripheralEnable ( int peripheralId );


//******************************************************************************
/* Function:
    void SYS_CLK_PeripheralDisable ( int peripheralId )

  Summary:
    Disables clock to the specified peripheral.
    <p><b>Implementation:</b> Static</p>

  Description:
    This function disables clock to the specified peripheral.

  Precondition:
    None.

  Parameters:
    peripheralId - ID of the peripherl.

  Returns:
    None.

  Example:
    <code>
    SYS_CLK_PeripheralEnable ( ID_QSPI );
    </code>

  Remarks:
    This function is supported on the following Microchip device families:
        - PIC32C
 */

void SYS_CLK_PeripheralDisable ( int peripheralId );


// *****************************************************************************
// *****************************************************************************
// Section: File includes for the APIs which are required for backward
//          compatibility.
// *****************************************************************************
// *****************************************************************************

#include "system/clk/sys_clk_compatibility.h"


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //SYS_CLK_H

/*******************************************************************************
 End of File
*/
