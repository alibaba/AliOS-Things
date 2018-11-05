/*******************************************************************************
  Timer Driver Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_variant_mapping.h

  Summary:
    Timer driver feature variant implementations.

  Description:
    This file implements the functions which differ based on different devices
    and various implementations of the same feature.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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


#ifndef _DRV_TMR_VARIANT_MAPPING_H
#define _DRV_TMR_VARIANT_MAPPING_H


#if !defined (DRV_TMR_INTERRUPT_MODE)
    #error "No Task mode chosen at build, interrupt or polling needs to be selected. "
#endif

#if defined __PIC32MX__
            #define CLK_BUS_FOR_TIMER_PERIPHERAL          CLK_BUS_PERIPHERAL_1
#elif defined __PIC32MZ__
            #define CLK_BUS_FOR_TIMER_PERIPHERAL          CLK_BUS_PERIPHERAL_3
#elif defined __PIC32WK__
            #define CLK_BUS_FOR_TIMER_PERIPHERAL          CLK_BUS_PERIPHERAL_1
#else
            #define CLK_BUS_FOR_TIMER_PERIPHERAL          CLK_BUS_PERIPHERAL_1
#endif

/* Clock Source Selection for DA devices */
#if ((__PIC32_FEATURE_SET0 == 68) && (__PIC32_FEATURE_SET1 == 65))
    #define _DRV_TMR_CLOCK_SOURCE_SELECT(timerId, clockSource)          PLIB_TMR_ClockSourceSelect(timerId, (TMR_CLOCK_SOURCE)(clockSource & 0x0F))
/* Clock Source Selection for non-DA devices */
#else
    #define _DRV_TMR_CLOCK_SOURCE_SELECT(timerId, clockSource); \
        PLIB_TMR_ClockSourceSelect(timerId, TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN); \
        if ((clockSource & 0x0F) == 0x02) \
            SYS_CLK_SecondaryOscillatorEnable();
#endif

#endif //_DRV_TMR_VARIANT_MAPPING_H

/*******************************************************************************
 End of File
*/

