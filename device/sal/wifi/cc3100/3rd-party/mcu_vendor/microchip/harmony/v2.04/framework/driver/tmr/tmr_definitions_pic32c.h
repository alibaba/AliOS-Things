/*******************************************************************************
  Timer Device Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    tmr_definitions_pic32c.h
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

#ifndef _TMR_DEFINITIONS_PIC32C_H
#define _TMR_DEFINITIONS_PIC32C_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END
typedef enum {

	TMR_ID_1 = 0,
	TMR_ID_2,
	TMR_ID_3,
	TMR_ID_4,
	TMR_ID_5,
	TMR_ID_6,
	TMR_ID_7,
	TMR_ID_8,
	TMR_ID_9,
	TMR_ID_10,
	TMR_ID_11,
    TMR_ID_12,        
	TMR_NUMBER_OF_MODULES = 12

} TMR_MODULE_ID;
typedef enum {

	TMR_PRESCALE_VALUE_1 = 0x00,
	TMR_PRESCALE_VALUE_2 = 0x01,
	TMR_PRESCALE_VALUE_4 = 0x02,
	TMR_PRESCALE_VALUE_8 = 0x03,
	TMR_PRESCALE_VALUE_16 = 0x04,
	TMR_PRESCALE_VALUE_32 = 0x05,
	TMR_PRESCALE_VALUE_64 = 0x06,
	TMR_PRESCALE_VALUE_256 = 0x07

} TMR_PRESCALE;

typedef enum {

	TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK = 0,
	TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN = 1

} TMR_CLOCK_SOURCE;
typedef enum
{
    /* Clock input to the timer module is internal(Peripheral Clock) */
    DRV_TMR_CLKSOURCE_PCK
            /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,
            
    /* Clock input to the timer module is from T1CK pin (external) 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_MCK_PRESCALE_8
            /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from T1CK pin (external)
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_MCK_PRESCALE_32
            /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,
    
    /* Clock input to the timer module is from secondary oscillator (external) 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_MCK_PRESCALE_128
            /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from secondary oscillator (external)
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_SCLK
            /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/,
            
    /* Clock input to the timer module is from LPRC 
       with clock synchronization enabled */
    DRV_TMR_CLKSOURCE_XC0
            /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/,

    /* Clock input to the timer module is from LPRC
    with clock synchronization disabled */
    DRV_TMR_CLKSOURCE_XC1
            /*DOM-IGNORE-BEGIN*/ = 0x06 /*DOM-IGNORE-END*/,
			
	DRV_TMR_CLKSOURCE_XC2
			/*DOM-IGNORE-BEGIN*/ = 0x07 /*DOM-IGNORE-END*/
            
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