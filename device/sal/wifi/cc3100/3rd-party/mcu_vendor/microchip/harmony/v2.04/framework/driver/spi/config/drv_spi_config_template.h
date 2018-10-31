/*******************************************************************************
  SPI Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_config_template.h

  Summary:
    SPI Driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
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

#ifndef _DRV_SPI_CONFIG_TEMPLATE_H
#define _DRV_SPI_CONFIG_TEMPLATE_H


#error "This is a configuration template file.  Do not include it directly."


// *****************************************************************************
/* SPI hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver
                      .
  Description:
    This definition selects the maximum number of hardware instances that can be 
    supported by the dynamic driver.

  Remarks:
    Mandatory definition
*/

#define DRV_SPI_INSTANCES_NUMBER                        1


/*********************************************************************
  Summary:
    Selects the maximum number of clients.
  Description:
    SPI maximum number of clients
    
    This definition selects the maximum number of clients that the SPI
    driver can support at run time.
  Remarks:
    Mandatory definition
  *********************************************************************/

#define DRV_SPI_CLIENTS_NUMBER                          1


// *****************************************************************************
/* SPI Buffer Queue Depth

  Summary:
    Controls the number of elements that are allocated.

  Description:
    This definition along with DRV_SPI_INSTANCES_NUMBER and DRV_SPI_CLIENT_NUMBER
    controls how many buffer queue elements are created. 

  Remarks:
    Optional definition
*/

#define DRV_SPI_ELEMENTS_PER_QUEUE                      10

// *****************************************************************************
/* SPI Master Mode Enable

  Summary:
    Controls the compilation of master mode

  Description:
    This definition controls whether or not master mode functionality is built
    as part of the driver.  With it set to 1 then master mode will be compiled 
    and spiMode = DRV_SPI_MODE_MASTER will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert

  Remarks:
    Optional definition
*/
#define DRV_SPI_MASTER 1

// *****************************************************************************
/* SPI Slave Mode Enable

  Summary:
    Controls the compilation of slave mode

  Description:
    This definition controls whether or not slave mode functionality is built
    as part of the driver.  With it set to 1 then slave mode will be compiled 
    and spiMode = DRV_SPI_MODE_SLAVE will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert

  Remarks:
    Optional definition
*/

#define DRV_SPI_SLAVE 1

// *****************************************************************************
/* SPI ISR Mode Enable

  Summary:
    Controls the compilation of ISR mode

  Description:
    This definition controls whether or not ISR mode functionality is built
    as part of the driver.  With it set to 1 then ISR mode will be compiled 
    and taskMode = DRV_SPI_TASK_MODE_ISR will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert

  Remarks:
    Optional definition
*/

#define DRV_SPI_ISR 1

// *****************************************************************************
/* SPI Polled Mode Enable

  Summary:
    Controls the compilation of Polled mode

  Description:
    This definition controls whether or not polled mode functionality is built
    as part of the driver.  With it set to 1 then polled mode will be compiled 
    and taskMode = DRV_SPI_TASK_MODE_POLLED will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert

  Remarks:
    Optional definition
*/
#define DRV_SPI_POLLED 1

// *****************************************************************************
/* SPI Standard Buffer Mode Enable

  Summary:
    Controls the compilation of Standard Buffer mode

  Description:
    This definition controls whether or not Standard Buffer mode functionality is built
    as part of the driver.  With it set to 1 then standard buffer mode will be compiled 
    and bufferType = DRV_SPI_BUFFER_TYPE_STANDARD will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert.  This mode is 
    available on all PIC32s

  Remarks:
    Optional definition
*/
#define DRV_SPI_RM 1


// *****************************************************************************
/* SPI Enhanced Buffer Mode Enable  (Hardware FIFO)

  Summary:
    Controls the compilation of Enhanced Buffer Mode mode

  Description:
    This definition controls whether or not Enhanced Buffer mode functionality is built
    as part of the driver.  With it set to 1 then enhanced buffer mode will be compiled 
    and bufferType = DRV_SPI_BUFFER_TYPE_ENHANCED will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert.  This mode is 
    not available on all PIC32s.  Trying to use this mode on PICMX3XX/4XX will cause
    compile time warnings and errors.

  Remarks:
    Optional definition
*/
#define DRV_SPI_EBM 1

// *****************************************************************************
/* SPI 8 Bit Mode Enable

  Summary:
    Controls the compilation of 8 Bit mode

  Description:
    This definition controls whether or not 8 Bit mode functionality is built
    as part of the driver.  With it set to 1 then 8 Bit mode will be compiled 
    and commWidth = SPI_COMMUNICATION_WIDTH_8BITS will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert.

  Remarks:
    Optional definition
*/

#define DRV_SPI_8BIT 1

// *****************************************************************************
/* SPI 16 Bit Mode Enable

  Summary:
    Controls the compilation of 16 Bit mode

  Description:
    This definition controls whether or not 16 Bit mode functionality is built
    as part of the driver.  With it set to 1 then 16 Bit mode will be compiled 
    and commWidth = SPI_COMMUNICATION_WIDTH_16BITS will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert.  With this set the 
    BufferAdd functions will only accept buffer sizes of multiples of 2 (16 bit words)

  Remarks:
    Optional definition
*/

#define DRV_SPI_16BIT 1

// *****************************************************************************
/* SPI 32 Bit Mode Enable

  Summary:
    Controls the compilation of 32 Bit mode

  Description:
    This definition controls whether or not 32 Bit mode functionality is built
    as part of the driver.  With it set to 1 then 32 Bit mode will be compiled 
    and commWidth = SPI_COMMUNICATION_WIDTH_32BITS will be accepted by SPI_DRV_Initialize().  
    With it set to 0 SPI_DRV_Initialize() will cause an assert.  With this set the 
    BufferAdd functions will only accept buffer sizes of multiples of 4 (32 bit words)

  Remarks:
    Optional definition
*/
#define DRV_SPI_32BIT 1


// *****************************************************************************
/* SPI DMA Enable

  Summary:
    Controls the compilation of DMA support

  Description:
    This definition controls whether or not DMA functionality is built
    as part of the driver.  With it set to 1 then DMA will be compiled.  
    

  Remarks:
    Optional definition
*/
#define DRV_SPI_DMA 1

// *****************************************************************************
/* SPI DMA Transfer Size

  Summary:
    Controls the size of DMA transfers

  Description:
    This definition controls the maximum number of bytes to transfer per DMA transfer.
    
  Remarks:
    Optional definition
*/
#define DRV_SPI_DMA_TXFER_SIZE 256

// *****************************************************************************
/* SPI DMA Dummy Buffer Size

  Summary:
    Controls the size of DMA dummy buffer

  Description:
    This controls the size of the buffer the SPI driver uses to give to the DMA
    service when it is to send and receive invalid data on the bus.  This occurs when the 
    number of bytes to be read are different than the number of bytes transmitted.
    
  Remarks:
    Optional definition
*/
#define DRV_SPI_DMA_DUMMY_BUFFER_SIZE 256


#endif // #ifndef _DRV_SPI_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/

