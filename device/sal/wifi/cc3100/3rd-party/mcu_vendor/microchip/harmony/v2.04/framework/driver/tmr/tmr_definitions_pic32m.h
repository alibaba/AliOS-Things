/*******************************************************************************
  Timer Device Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    tmr_definitions_pic32m.h
  Summary:
    Timer device driver definitions header file.

  Description:
    This header file contains the definitions of the
    data types and constants that make up the interface to the Timer device
    driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _TMR_DEFINITIONS_PIC32M_H
#define _TMR_DEFINITIONS_PIC32M_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END
typedef enum
{
    /* Clock input to the timer module is internal(Peripheral Clock) */
    DRV_TMR_CLKSOURCE_INTERNAL
            /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,
            
    /* Clock input to the timer module is from T1CK pin (external) 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_EXTERNAL_SYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from T1CK pin (external)
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_EXTERNAL_ASYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x11 /*DOM-IGNORE-END*/,
    
    /* Clock input to the timer module is from secondary oscillator (external) 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_SECONDARY_OSCILLATOR_SYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from secondary oscillator (external)
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_SECONDARY_OSCILLATOR_ASYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x12 /*DOM-IGNORE-END*/,
            
    /* Clock input to the timer module is from LPRC 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_LPRC_OSCILLATOR_SYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from LPRC
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_LPRC_OSCILLATOR_ASYNCHRONOUS
            /*DOM-IGNORE-BEGIN*/ = 0x13 /*DOM-IGNORE-END*/
            
} DRV_TMR_CLK_SOURCES;  
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_TMR_H

/*******************************************************************************
 End of File
*/