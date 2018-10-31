#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system/common/sys_common.h"
#include "system/common/sys_buffer.h"

#define BUFFER_CONFIG_SUPPORT_CIRCULAR false

#define BUFFER_NextIndex(bufferObject, index)               \
    ((index)+1)

#define BUFFER_PrevIndex(bufferObject, index)               \
    ((index)-1)

/* Returns the next index */
#define BUFFER_NextIndex_CIRC(bufferObject, index)               \
    (((bufferObject)->isCircular && (( (index) + 1 )  > ((bufferObject)->size - 1)) )?0:(index)+1)

/* Returns the previous index */
#define BUFFER_PrevIndex_CIRC(bufferObject, index)               \
    (( bufferObject->isCircular && (( (index) - 1 ) < 0) ) ? ((bufferObject)->size - 1) : ((index)-1))

// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_Initialize(BUFFER_OBJECT *bufferObject, const uint8_t *buffer, const uint32_t size,
        const bool isCircular, const DRV_IO_BUFFER_TYPES bufferId)
 *****************************************************************************/

void BUFFER_Initialize(BUFFER_OBJECT *bufferObject, const void *buffer, const uint32_t size,
        const bool isCircular, const DRV_IO_BUFFER_TYPES bufferId)
{
    //SYS_ASSERT((bufferObject != (BUFFER_OBJECT *)NULL), "Invalid Buffer");

    //if(bufferObject != (BUFFER_OBJECT *)NULL)
    {
        bufferObject->buffer = (void *)buffer;
        bufferObject->size   = size;
        bufferObject->readIndex = 0;
        bufferObject->writeIndex = 0;
        bufferObject->count      = 0;
        bufferObject->isCircular = isCircular;
        bufferObject->bufferType = bufferId;
        
        if(bufferId == DRV_IO_BUFFER_TYPE_WRITE)
        {
            bufferObject->writeIndex = size;
            bufferObject->count      = size;
        }
        else if (bufferId == DRV_IO_BUFFER_TYPE_READ)
        {
            bufferObject->readIndex = size;
        }
    }

}
// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_Reset(BUFFER_OBJECT *bufferObject)
 *****************************************************************************/

void BUFFER_Reset(BUFFER_OBJECT *bufferObject)
{
    bufferObject->buffer    = NULL;
    bufferObject->readIndex = 0;
    bufferObject->size      = 0;
    bufferObject->writeIndex = 0;
    bufferObject->overFlowCount = 0;
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_WriteByte(BUFFER_OBJECT *bufferObject, uint8_t byte);
 *****************************************************************************/

void BUFFER_WriteByte(BUFFER_OBJECT *bufferObject, uint8_t byte)
{
    uint8_t *buffer = (uint8_t *)bufferObject->buffer;
    buffer[bufferObject->writeIndex] = byte;
    bufferObject->writeIndex = BUFFER_NextIndex(bufferObject, bufferObject->writeIndex);
    bufferObject->count++;// This needs to be atomic and protected.

/*
    if(BUFFER_FullCount(bufferObject) >= bufferObject->size)
    {
        bufferObject->overFlowCount++;
    }
*/
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_WriteBuffer(BUFFER_OBJECT *bufferObject, uint8_t *buffer, uint32_t numBytes);
 *****************************************************************************/

void BUFFER_WriteDoubleByte(BUFFER_OBJECT *bufferObject, uint16_t byte)
{
    uint16_t *buffer = (uint16_t *)bufferObject->buffer;
    uint8_t writeIndex = bufferObject->writeIndex;
    buffer[writeIndex] = byte;
    bufferObject->writeIndex = BUFFER_NextIndex(bufferObject, writeIndex);
    bufferObject->count++;// This needs to be atomic and protected.

    if(BUFFER_FullCount(bufferObject) >= bufferObject->size)
    {
        bufferObject->overFlowCount++;
    }
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_ReadByte(BUFFER_OBJECT *bufferObject, uint8_t *byte);
 *****************************************************************************/

uint8_t BUFFER_ReadByte(BUFFER_OBJECT *bufferObject)
{
    uint8_t byte;
    uint8_t *buffer = (uint8_t *)bufferObject->buffer;
    uint8_t readIndex = bufferObject->readIndex;
    byte = buffer[readIndex];
    bufferObject->readIndex = BUFFER_NextIndex(bufferObject, readIndex);
    bufferObject->count--; // This needs to be atomic and protected.

    return byte;
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_ReadBuffer(BUFFER_OBJECT *bufferObject, uint8_t *buffer, uint32_t numBytes);
 *****************************************************************************/

uint16_t BUFFER_ReadDoubleByte(BUFFER_OBJECT *bufferObject)
{
    uint16_t doublebyte;
    uint16_t *buffer = (uint16_t *)bufferObject->buffer;
    doublebyte = buffer[bufferObject->readIndex];
    bufferObject->readIndex = BUFFER_NextIndex(bufferObject, bufferObject->readIndex);
    bufferObject->count--; // This needs to be atomic and protected.

    return doublebyte;
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_Flush(BUFFER_OBJECT *bufferObject);
 *****************************************************************************/

void BUFFER_Flush(BUFFER_OBJECT *bufferObject)
{
        bufferObject->readIndex = 0;
        bufferObject->writeIndex = 0;
        bufferObject->count      = 0;
        bufferObject->overFlowCount = 0;
}


// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_FullCount(BUFFER_OBJECT *bufferObject);
 *****************************************************************************/



// *****************************************************************************
/******************************************************************************
  Function:
    void BUFFER_EmptyCount(BUFFER_OBJECT *bufferObject);
 *****************************************************************************/




// *****************************************************************************
/******************************************************************************
  Function:
    bool BUFFER_IsBufferEmpty(BUFFER_OBJECT *buffer)
 *****************************************************************************/



// *****************************************************************************
/******************************************************************************
  Function:
    bool BUFFER_IsBufferFull(BUFFER_OBJECT *buffer)
 *****************************************************************************/


/*******************************************************************************
 End of File
*/
