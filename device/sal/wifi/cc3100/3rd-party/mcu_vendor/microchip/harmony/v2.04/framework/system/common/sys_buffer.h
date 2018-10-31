/*******************************************************************************
  Buffer Interface

  Company:
    Microchip Technology Inc.
    
  File Name:
    sys_buffer.h

  Summary:
    Buffer interface.

  Description:
    These flags set up the driver for the default mode of operation. Based on the
    manner in which the USART is used in the system change the configuration as 
    per the requirements of the system.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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


#ifndef _BUFFER_H
#define _BUFFER_H

#include "driver/driver_common.h"

//*****************************************************************************
/* Buffer Object

   Summary:
    Structure required for maintaining the buffer.

   Description:
    Defines the structure required for maintaining the buffer.

   Remarks:
    None.
*/

typedef struct 
{
    // Write pointer for the buffer ; Writer only modifies this .
    uint32_t                 writeIndex;

    // Read pointer for the buffer ; Reader only modifies this
    uint32_t                 readIndex;
            
    // Buffer Count ; Keeps a count of the number of chars in the buffer
    // Atomic and Protected instructions should modify this, since both the
    // reader and writer modify this.
    uint32_t                 count;
    
    // Size of the buffer 
    uint32_t                size;
    
    // Buffer 
    void                 *buffer;

    // Overflow Counter
    uint8_t                 overFlowCount;

    // Flag to indicate the buffer is circular
    bool                    isCircular;

    // Buffer Type
    DRV_IO_BUFFER_TYPES     bufferType;
    
}BUFFER_OBJECT;

// *****************************************************************************
/*
  Function:
    void BUFFER_Initialize(BUFFER_OBJECT *bufferObject, const uint8_t *buffer, 
	                       const uint32_t size, const bool isCircular, 
						   const DRV_IO_BUFFER_TYPES bufferId)

  Summary:
    Creates a buffer object.

  Description:
    This function initializes and creates a buffer object. The buffer passed in
    used as the buffer pointer for buffer.
 
  Precondition:
    None.

  Parameters:
    buffer      - The buffer which is to be used as the buffer
    size        - The size of the buffer provided
    isCircular  - The flag to indicate that the buffer is circular

  Returns:
    None.

  Example:
    <code>
    BUFFER_OBJECT *buffer = BUFFER_Initialize(buffer, size, false);
    </code>

  Remarks:
    None.
 
*/

void BUFFER_Initialize(BUFFER_OBJECT *bufferObject, const void *buffer, const uint32_t size,
        const bool isCircular, const DRV_IO_BUFFER_TYPES bufferId);


// *****************************************************************************
/*
  Function:
    void BUFFER_Reset(BUFFER_OBJECT *bufferObject);

  Summary:
    Returns the pointer back to the queue.

  Description:
    This function returns the pointer back to the queue.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5];
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    BUFFER_Delete(bufferObject);
    </code>

  Remarks:
    None.

*/

void BUFFER_Reset(BUFFER_OBJECT *bufferObject);

// *****************************************************************************
/*
  Function:
    void BUFFER_WriteByte(BUFFER_OBJECT *BUFFER, uint8_t byte);

  Summary:
    Writes a byte to the buffer.

  Description:
    This function writes a byte to the buffer.

  Precondition:
    None.

  Parameters:
    BUFFER      - The buffer object
    byte        - The byte to be written

  Returns:
    None.

  Example:
    <code>
    BUFFER_OBJECT *BUFFER = BUFFER_Create(buffer, size);

    BUFFER_WriteByte(bufferObject, 0x55);
    </code>

  Remarks:
    None.

*/

void BUFFER_WriteByte(BUFFER_OBJECT *bufferObject, uint8_t byte);


// *****************************************************************************
/*
  Function:
    void BUFFER_WriteDoubleByte(BUFFER_OBJECT *bufferObject, uint16_t doubleByte);

  Summary:
    Writes a array of bytes to the buffer.

  Description:
    This function writes a array of bytes to the buffer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object
    buffer           - The array of bytes to be written
    numBytes         - The number of bytes to be written

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5] = "12345";
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    BUFFER_WriteBuffer(bufferObject, 0x5566);
    </code>

  Remarks:
    None.

*/

void BUFFER_WriteDoubleByte(BUFFER_OBJECT *bufferObject, uint16_t doubleByte);


// *****************************************************************************
/*
  Function:
    void BUFFER_ReadByte(BUFFER_OBJECT *bufferObject, uint8_t *byte);

  Summary:
    Reads a byte to the buffer.

  Description:
    This function reads a byte to the buffer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object
    byte             - The byte to be read

  Returns:
    None.

  Example:
    <code>
    uint8_t byte;
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    BUFFER_WriteByte(bufferObject, &byte);
    </code>

  Remarks:
    None.

*/

uint8_t BUFFER_ReadByte(BUFFER_OBJECT *bufferObject);


// *****************************************************************************
/*
  Function:
    uint16_t BUFFER_ReadDoubleByte(BUFFER_OBJECT *bufferObject);
 
  Summary:
    Reads a array of bytes to the buffer.

  Description:
    This function reads a array of bytes to the buffer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object
    buffer           - The array of bytes to be read
    numBytes         - The number of bytes to be read

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5];
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    BUFFER_ReadBuffer(bufferObject, buffer, 5);
    </code>

  Remarks:
    None.

*/

uint16_t BUFFER_ReadDoubleByte(BUFFER_OBJECT *bufferObject);


// *****************************************************************************
/*
  Function:
    void BUFFER_Flush(BUFFER_OBJECT *bufferObject);

  Summary:
    Resets the pointers for the reader and the writer.

  Description:
    This function resets the pointers for the reader and the writer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5];
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    BUFFER_Flush(bufferObject);
    </code>

  Remarks:
    None.

*/

void BUFFER_Flush(BUFFER_OBJECT *bufferObject);


// *****************************************************************************
/*
  Function:
    void BUFFER_FullCount(BUFFER_OBJECT *bufferObject);

  Summary:
    Gets the number of bytes in the buffer

  Description:
    This function gets the number of bytes in the buffer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5];
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    uint32_t fullCount = BUFFER_FullCount(bufferObject);
    </code>

  Remarks:
    None.

*/

#define BUFFER_FullCount(bufferObject)  ((bufferObject)->count)


// *****************************************************************************
/*
  Function:
    void BUFFER_EmptyCount(BUFFER_OBJECT *bufferObject);

  Summary:
    Gets the number of empty positions in the buffer.

  Description:
    This function gets the number of empty positions in the buffer.

  Precondition:
    None.

  Parameters:
    bufferObject     - The buffer object

  Returns:
    None.

  Example:
    <code>
    uint8_t buffer[5];
    BUFFER_OBJECT *bufferObject = BUFFER_Create(buffer, size);

    uint32_t emptyCount = BUFFER_EmptyCount(bufferObject);
    </code>

  Remarks:
    None.

*/

#define BUFFER_EmptyCount(bufferObject)                          \
(                                                                \
    ((bufferObject)->size - (bufferObject)->count)               \
)

#define BUFFER_IsBufferEmpty(bufferObject)                       \
(                                                                \
    ((bufferObject)->count == 0)                                 \
)

#define BUFFER_IsBufferFull(bufferObject)                        \
(                                                                \
    ((bufferObject)->count >= (bufferObject)->size)              \
)

#endif //_BUFFER_H

/*******************************************************************************
 End of File
*/