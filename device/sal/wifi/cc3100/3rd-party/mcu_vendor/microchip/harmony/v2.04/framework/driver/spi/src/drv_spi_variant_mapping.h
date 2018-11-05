/*******************************************************************************
  SPI Driver Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_variant_mapping.h

  Summary:
    SPI driver feature variant implementations.

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
(pursuant to the sub-license terms in the accompanying license agreement).

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


#ifndef _DRV_SPI_VARIANT_MAPPING_H
#define _DRV_SPI_VARIANT_MAPPING_H


    #if defined __PIC32MX__
        /* Interrupt trigger for PIC32MX320 series of devices, which doesn't have persistent interrupt */
        #if ((__PIC32_FEATURE_SET == 320) || (__PIC32_FEATURE_SET == 340) || (__PIC32_FEATURE_SET == 360) || (__PIC32_FEATURE_SET == 420) || (__PIC32_FEATURE_SET == 440) || (__PIC32_FEATURE_SET == 460))

            #define _DRV_SPI_INTERRUPT_TRIGGER(txInterruptSource)           if (qEmpty)\
                                                                            {\
                                                                               SYS_INT_SourceStatusSet(txInterruptSource);\
                                                                            }

            #define _DRV_SPI_QUEUE_STATUS_CHECK(queue)                      volatile bool qEmpty;\
                                                                            if (DRV_SPI_SYS_QUEUE_IsEmpty(queue))\
                                                                            {\
                                                                                qEmpty= true;\
                                                                            }

        /* No interrupt trigger is required for the devices which have persistent interrupts */
        #else
            #define _DRV_SPI_INTERRUPT_TRIGGER(txInterruptSource)
            #define _DRV_SPI_QUEUE_STATUS_CHECK(queue)
        #endif
    #else
        #define _DRV_SPI_INTERRUPT_TRIGGER(txInterruptSource)
        #define _DRV_SPI_QUEUE_STATUS_CHECK(queue)

    #endif

#endif //_DRV_SPI_VARIANT_MAPPING_H

/*******************************************************************************
 End of File
*/

