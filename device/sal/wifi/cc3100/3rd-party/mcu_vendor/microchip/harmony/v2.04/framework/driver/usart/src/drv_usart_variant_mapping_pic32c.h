/*******************************************************************************
  USART Driver Variant Mapping

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_variant_mapping_pic32c.h

  Summary:
    USART Driver Variant Mapping

  Description:
    This file provides feature and build variant mapping macros allowing the
    driver to easily be built with different implementation variations based
    on static build-time configuration selections.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_USART_VARIANT_MAPPING_PIC32C_H
#define _DRV_USART_VARIANT_MAPPING_PIC32C_H


// *****************************************************************************
// *****************************************************************************
// Section: Interrrupt Variations
// *****************************************************************************
// *****************************************************************************
/* Mapping of the interrupt mode variations
*/

#if defined (DRV_USART_INTERRUPT_MODE) && \
            (DRV_USART_INTERRUPT_MODE == true)

    /* Driver is configured for interrupt mode */
    #define _DRV_USART_InterruptIsEnabled(source)               SYS_INT_SourceIsEnabled( source )
    #define _DRV_USART_InterruptEnable(source)                  SYS_INT_SourceEnable( source )
    #define _DRV_USART_InterruptDisable(source)                 SYS_INT_SourceDisable( source )
    #define _DRV_USART_InterruptStatusClear(source)             SYS_INT_SourceStatusClear( source )
    #define _DRV_USART_TxInterruptSourceEnable(module)          ((module->US_IER.w) |= (US_IER_TXEMPTY_Msk))
    #define _DRV_USART_TxInterruptSourceIsEnabled(module)       (( (1) == (module->US_IMR.TXEMPTY)) ? (true) : (false))
    #define _DRV_USART_TxInterruptSourceDisable(module)         ((module->US_IDR.w) |= (US_IDR_TXEMPTY_Msk))
    #define _DRV_USART_RxInterruptSourceEnable(module)          ((module->US_IER.w) |= (US_IER_RXRDY_Msk))
    #define _DRV_USART_RxInterruptSourceIsEnabled(module)       (( (1) == (module->US_IMR.RXRDY)) ? (true) : (false))
    #define _DRV_USART_RxInterruptSourceDisable(module)         ((module->US_IDR.w) |= (US_IDR_RXRDY_Msk))
    #define _DRV_USART_ErrorInterruptSourceEnable(module)       ((module->US_IER.w) |= ((US_IER_OVRE_Msk) | (US_IER_FRAME_Msk) | (US_IER_PARE_Msk)))
    #define _DRV_USART_ErrorInterruptSourceDisable(module)      ((module->US_IDR.w) |= ((US_IDR_OVRE_Msk) | (US_IDR_FRAME_Msk) | (US_IDR_PARE_Msk)))
    #define _DRV_USART_InterruptSourcesDisable(module)          ((module->US_IDR.w) |= ((US_IDR_OVRE_Msk) | (US_IDR_FRAME_Msk) | (US_IDR_PARE_Msk) | (US_IDR_TXEMPTY_Msk) | (US_IDR_RXRDY_Msk)))

    #define _DRV_USART_SEM_POST(x)                              OSAL_SEM_PostISR(x)
    #define _DRV_USART_TAKE_MUTEX(x,y)                          (OSAL_RESULT_TRUE)
    #define _DRV_USART_RELEASE_MUTEX(x)
    #define _DRV_USART_ALWAYS_NON_BLOCKING                      0

    #define _SYS_DMA_ChannelForceStart(channelHandle)

#elif defined (DRV_USART_INTERRUPT_MODE) && \
            (DRV_USART_INTERRUPT_MODE == false)

    /* Driver is configured for polled mode */
    #define _DRV_USART_InterruptIsEnabled(source)               false
    #define _DRV_USART_InterruptEnable(source)
    #define _DRV_USART_InterruptDisable(source)                 false
    #define _DRV_USART_InterruptStatusClear(source)             SYS_INT_SourceStatusClear( source )
    #define _DRV_USART_TxInterruptSourceEnable(module)
    #define _DRV_USART_TxInterruptSourceIsEnabled(module)       false  
    #define _DRV_USART_TxInterruptSourceDisable(module)         
    #define _DRV_USART_RxInterruptSourceEnable(module)
    #define _DRV_USART_RxInterruptSourceIsEnabled(module)       false  
    #define _DRV_USART_RxInterruptSourceDisable(module)        
    #define _DRV_USART_ErrorInterruptSourceEnable(module)
    #define _DRV_USART_ErrorInterruptSourceIsEnabled(module)    false  
    #define _DRV_USART_ErrorInterruptSourceDisable(module)      
    #define _DRV_USART_InterruptSourcesDisable(source)          false
    
    #define _DRV_USART_SEM_POST(x)                             OSAL_SEM_Post(x)
    #define _DRV_USART_TAKE_MUTEX(x,y)                         OSAL_MUTEX_Lock(x,y)
    #define _DRV_USART_RELEASE_MUTEX(x)                        OSAL_MUTEX_Unlock(x)

    #define _SYS_DMA_ChannelForceStart(channelHandle)   	   SYS_DMA_ChannelForceStart(channelHandle)

    #ifndef OSAL_USE_RTOS

        /* This means the driver is being built in a baremetal application.
           We cannot let a client open the driver in blocking mode */

           #define _DRV_USART_ALWAYS_NON_BLOCKING             (DRV_IO_INTENT_NONBLOCKING)

    #else

        /* This means the driver is being built with RTOS support. We can
           support blocking operation */

            #define _DRV_USART_ALWAYS_NON_BLOCKING            0

    #endif

#else
    #error "No Task mode chosen at build, interrupt or polling needs to be selected."
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Buffer Queue support
// *****************************************************************************
// *****************************************************************************
/* Mapping of macros for buffer queue support
*/

#if defined(DRV_USART_BYTE_MODEL_SUPPORT) && (DRV_USART_BYTE_MODEL_SUPPORT == true)

    /* We dont need buffer queues or byte model support */

    #define _DRV_USART_TRANSMIT_BUFFER_QUEUE_TASKS(x)           _DRV_USART_ByteTransmitTasks(x)
    #define _DRV_USART_RECEIVE_BUFFER_QUEUE_TASKS(x)            _DRV_USART_ByteReceiveTasks(x)
    #define _DRV_USART_ERROR_TASKS(x)                           _DRV_USART_ByteErrorTasks(x)
    #define _DRV_USART_CLIENT_BUFFER_QUEUE_OBJECTS_REMOVE(x)    true

#if defined (DRV_USART_INTERRUPT_MODE) && \
            (DRV_USART_INTERRUPT_MODE == true)
    #define _DRV_USART_ByteModelRXInterruptSourceEnable(module) ((module->US_IER.w) |= (US_IER_RXRDY_Msk))
#else
    #define _DRV_USART_ByteModelRXInterruptSourceEnable(module)         
#endif
#else

    /* Buffer Queues are needed for read write and buffer queue model */
    #define _DRV_USART_TRANSMIT_BUFFER_QUEUE_TASKS(x)           _DRV_USART_BufferQueueTxTasks(x)
    #define _DRV_USART_RECEIVE_BUFFER_QUEUE_TASKS(x)            _DRV_USART_BufferQueueRxTasks(x)
    #define _DRV_USART_ERROR_TASKS(x)                           _DRV_USART_BufferQueueErrorTasks(x)
    #define _DRV_USART_CLIENT_BUFFER_QUEUE_OBJECTS_REMOVE(x)    _DRV_USART_ClientBufferQueueObjectsRemove(x)

    #define _DRV_USART_ByteModelRXInterruptSourceEnable(module)         

#endif

// *****************************************************************************
// *****************************************************************************
// Initializtion Parameter Static Overrides
// *****************************************************************************
// *****************************************************************************



#endif //_DRV_USART_VARIANT_MAPPING_PIC32C_H

/*******************************************************************************
 End of File
*/
