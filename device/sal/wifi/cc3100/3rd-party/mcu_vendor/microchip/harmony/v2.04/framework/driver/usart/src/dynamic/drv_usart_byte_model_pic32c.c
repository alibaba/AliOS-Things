/********************************************************************************
  USART Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_byte_model_pic32c.c

  Summary:
    Source code for the USART driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver's byte model functionality.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#include "driver/usart/src/drv_usart_local_pic32c.h"

// *****************************************************************************
/* Function:
    void DRV_USART_WriteByte( const DRV_HANDLE handle, const uint8_t byte)

  Summary:
    Dynamic implementation of DRV_USART_WriteByte client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_WriteByte client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_WriteByte( const DRV_HANDLE handle, const uint8_t byte)
{
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_OBJ * hDriver;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    hDriver = client->hDriver;

    /* This function needs to be thread safe */

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
	    /* Send one byte */    
        hDriver->moduleId->US_THR.w |= US_THR_TXCHR( byte );

	    /* Enable module TX interrupt source */
        _DRV_USART_TxInterruptSourceEnable(hDriver->moduleId);
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    else
    {
        SYS_DEBUG(0, "Hardware Instance Mutex Time out in DRV_USART_WriteByte() function");
    }
}

// *****************************************************************************
/* Function:
    void DRV_USART_ReadByte( const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_ReadByte client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_ReadByte client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

uint8_t DRV_USART_ReadByte( const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_OBJ * hDriver;
    uint8_t readValue;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return 0;
    }

    hDriver = client->hDriver;

    /* This function needs to be thread safe */
    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
	    /* Read one byte */
	    readValue = hDriver->moduleId->US_RHR.RXCHR;
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    else
    {
        SYS_DEBUG(0, "Hardware Instance Mutex Time out in DRV_USART_ReadByte() function");
        return 0;
    }
	return readValue;
}

// *****************************************************************************
/* Function:
    void DRV_USART_ReceiverBufferSizeGet( const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_ReceiverBufferSizeGet client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_ReceiverBufferSizeGet client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

unsigned int DRV_USART_ReceiverBufferSizeGet (const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ * client;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return 0;
    }

    return 1;

}

// *****************************************************************************
/* Function:
    void DRV_USART_TransmitBufferSizeGet( const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_TransmitBufferSizeGet client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_TransmitBufferSizeGet client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

unsigned int DRV_USART_TransmitBufferSizeGet( const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ * client;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return 0;
    }

    return 1;

}

// *****************************************************************************
/* Function:
    void DRV_USART_ReceiverBufferIsEmpty( const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_ReceiverBufferIsEmpty client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_ReceiverBufferIsEmpty client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

bool DRV_USART_ReceiverBufferIsEmpty( const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_OBJ * hDriver;
    bool rxIsEmpty = false;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return false;
    }

    hDriver = client->hDriver;

    /* If there is data available in the receiver, then the
       buffer is not empty and this function returns false. */
    if(0 == hDriver->moduleId->US_CSR.RXRDY)
    {
        rxIsEmpty = true;
    }
    
    return rxIsEmpty;
}

// *****************************************************************************
/* Function:
    void DRV_USART_TransmitBufferIsFull( const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_TransmitBufferIsFull client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_TransmitBufferIsFull client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

bool DRV_USART_TransmitBufferIsFull( const DRV_HANDLE handle )
{
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_OBJ * hDriver;
    bool txIsFull = false;

    /* Validate the client handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver Handle");
        return false;
    }

    hDriver = client->hDriver;

    /* If there is data still in TX buffer, then this 
     * function returns True. */
    if(0 == hDriver->moduleId->US_CSR.TXEMPTY)
    {
        txIsFull = true;
    }
    
    return txIsFull;
}

