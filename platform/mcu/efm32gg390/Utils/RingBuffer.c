/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 */

#include "BCDS_Utils.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_UTILS_MODULE_ID_RINGBUFFER

#include "BCDS_RingBuffer.h"

#if BCDS_FEATURE_RINGBUFFER

#include "BCDS_Basics.h"
#include "BCDS_Assert.h"

static BCDS_INLINE void Add(RingBuffer_T * ringBuffer, uint8_t element)
{
    /* load the writeIndex to avoid false intermediate values on the real write index */
    register uint32_t index = ringBuffer->WriteIndex;

    /* write the element and increment the index */
    ringBuffer->Base[index++] = element;

    /* update the index */
    ringBuffer->WriteIndex = (ringBuffer->Size != index) ? index : 0UL;
    /* now it is safe to read from the "old" index and write to the new index */
}

static BCDS_INLINE uint8_t Get(RingBuffer_T * ringBuffer)
{
    /* use a copy of the ReadIndex to avoid false intermediate values on the real read index
     * since it is also used in the write process (add) */
    register uint32_t index = ringBuffer->ReadIndex;

    /* read the data from the buffer and increment the index */
    uint8_t element = ringBuffer->Base[index++];

    /*update the index */
    ringBuffer->ReadIndex = (ringBuffer->Size != index) ? index : 0UL;
    /* now it is safe to write to this index again */
    return element;
}

static BCDS_INLINE bool RingBuffer_IsFull(RingBuffer_T *ringBuffer)
{
    /* incrementing the WriteIndex would hit the ReadIndex => full */
    return ((ringBuffer->WriteIndex + 1UL) % (ringBuffer->Size)) != ringBuffer->ReadIndex ? false : true;
}

static BCDS_INLINE bool RingBuffer_IsEmpty(RingBuffer_T *ringBuffer)
{
    /* the ReadIndex equals the WriteIndex => empty */
    return ringBuffer->ReadIndex == ringBuffer->WriteIndex ? true : false;
}

void RingBuffer_Initialize(RingBuffer_T * ringBuffer, uint8_t * bufferSpace, uint32_t size)
{
    if ((NULL == ringBuffer) || (NULL == bufferSpace) || (0UL == size))
    {
        Retcode_RaiseError (RETCODE( RETCODE_SEVERITY_FATAL, RETCODE_INVALID_PARAM));
    }
    else
    {
        /* initialize the ring-buffer structure:buffer, index r/w, buffer size */
        ringBuffer->Base = bufferSpace;
        ringBuffer->ReadIndex = 0;
        ringBuffer->WriteIndex = 0;
        ringBuffer->Size = size;
    }
}

uint32_t RingBuffer_Write(RingBuffer_T * ringBuffer, uint8_t * data, uint32_t length)
{
    uint32_t actualLength = 0UL;

    while (!RingBuffer_IsFull(ringBuffer) && length--)
    {
        /* Now it is safe to write to the buffer when it is not empty, since no one else is writing to it.
         * It could only get even "more empty" since some read processes can have consumed (read) data from the buffer
         */
        Add(ringBuffer, data[actualLength++]);
    }
    return actualLength;
}

uint32_t RingBuffer_Read(RingBuffer_T * ringBuffer, uint8_t * data, uint32_t length)
{
    uint32_t actualLength = 0UL;
    if ((NULL != ringBuffer) && (NULL != data))
    {
        /* read data while the buffer is not empty for the amount of requested data */
        while (!RingBuffer_IsEmpty(ringBuffer) && length--)
        {
            data[actualLength++] = Get(ringBuffer);
        }
    }
    /* report the number of bytes read */
    return actualLength;
}

void RingBuffer_Reset(RingBuffer_T *ringBuffer)
{
    if (NULL != ringBuffer)
    {
        /* reset the ring-buffer structure:buffer, index r/w */
        ringBuffer->ReadIndex = 0;
        ringBuffer->WriteIndex = 0;
        (void) memset(ringBuffer->Base, 0x00, ringBuffer->Size);
    }
    else
    {
        Retcode_RaiseError (RETCODE( RETCODE_SEVERITY_FATAL, RETCODE_INVALID_PARAM));
    }
}

#endif /* if BCDS_FEATURE_RINGBUFFER */
