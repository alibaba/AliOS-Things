/******************************************************************************
 *
 *  Copyright (C) 2016 Realtek Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/******************************************************************************
*
*	Module Name:
*	    bt_skbuff.c
*
*	Abstract:
*	    Data buffer managerment through whole bluetooth stack.
*
*	Major Change History:
*	      When             Who       What
*	    --------------------------------------------------------------
*	    2010-06-11       W.Bi    Created.
*
*	Notes:
*		  To reduce memory copy when pass data buffer to other layers,
*      	RTK_BUFFER is designed referring to linux socket buffer.
*       But I still wonder its effect, since RTK_BUFFER is much bigger
*       than original data buffer.RTK_BUFFER will reduce its member if
*       it would not reach what i had expected.
*
******************************************************************************/


#define LOG_TAG "bt_h5"
#undef NDEBUG
//#include <utils/Log.h>
#include <stdlib.h>
//#include <fcntl.h>

//#include <termios.h>
#include <errno.h>
//#include <pthread.h>


#include "bt_list.h"
#include "bt_skbuff.h"
#include "stack/bt_types.h"
#include "string.h"
#include <osi/mutex.h>
#define IN
#define OUT

//****************************************************************************
// CONSTANT DEFINITION
//****************************************************************************
///default header size
///l2cap header(8)+hci acl(4)
#define DEFAULT_HEADER_SIZE    (8+4)

//RTK_BUFFER data buffer alignment
#define RTB_ALIGN   4

//do alignment with RTB_ALIGN
#define RTB_DATA_ALIGN(_Length)     ((_Length + (RTB_ALIGN - 1)) & (~(RTB_ALIGN - 1)))

//****************************************************************************
// STRUCTURE DEFINITION
//****************************************************************************
typedef struct _RTB_QUEUE_HEAD{
    RT_LIST_HEAD List;
    uint32_t  QueueLen;
    osi_mutex_t Lock;
    uint8_t   Id[RTB_QUEUE_ID_LENGTH];
}RTB_QUEUE_HEAD, *PRTB_QUEUE_HEAD;

//****************************************************************************
// FUNCTION
//****************************************************************************
/**
    check whether queue is empty
    \return :   FALSE   Queue is not empty
        TRU Queue is empty
*/
unsigned char
RtbQueueIsEmpty(
   IN RTB_QUEUE_HEAD* RtkQueueHead
)
{
    //return ListIsEmpty(&RtkQueueHead->List);
    return  RtkQueueHead->QueueLen > 0 ? FALSE : TRUE;
}

/**
    Allocate a RTK_BUFFER with specified data length and reserved headroom.
    If caller does not know actual headroom to reserve for further usage, specify it to zero to use default value.
    \param [IN]     Length            <uint32_t>        : current data buffer length to allcated
    \param [IN]     HeadRoom     <uint32_t>         : if caller knows reserved head space, set it; otherwise set 0 to use default value
    \return pointer to RTK_BUFFER if succeed, null otherwise
*/
RTK_BUFFER*
RtbAllocate(
    uint32_t Length,
    uint32_t HeadRoom
    )
{
    RTK_BUFFER* Rtb = NULL;
    ///Rtb buffer length:
    ///     RTK_BUFFER   48
    ///     HeadRoom      HeadRomm or 12
    ///     Length
    ///memory size: 48 + Length + 12(default) + 8*2(header for each memory) ---> a multiple of 8
    ///example:       (48 + 8)+ (300 + 12 + 8) = 372
    Rtb = malloc( sizeof(RTK_BUFFER) );
    if(Rtb)
    {
        uint32_t BufferLen = HeadRoom ? (Length + HeadRoom) : (Length + DEFAULT_HEADER_SIZE);
        BufferLen = RTB_DATA_ALIGN(BufferLen);
        Rtb->Head = malloc(BufferLen);
        if(Rtb->Head)
        {
            Rtb->HeadRoom = HeadRoom ? HeadRoom : DEFAULT_HEADER_SIZE;
            Rtb->Data = Rtb->Head + Rtb->HeadRoom;
            Rtb->End = Rtb->Data;
            Rtb->Tail = Rtb->End + Length;
            Rtb->Length = 0;
            ListInitializeHeader(&Rtb->List);
            Rtb->RefCount = 1;
            return Rtb;
        }
    }

    if (Rtb)
    {
        if (Rtb->Head)
        {
            free(Rtb->Head);
        }

        free(Rtb);
    }
    return NULL;
}


/**
    Free specified Rtk_buffer
    \param [IN]     RtkBuffer            <RTK_BUFFER*>        : buffer to free
*/
void
RtbFree(
    RTK_BUFFER* RtkBuffer
)
{
    if(RtkBuffer)
    {
        free(RtkBuffer->Head);
        free(RtkBuffer);
    }
    return;
}

/**
    Add a specified length protocal header to the start of data buffer hold by specified rtk_buffer.
    This function extends used data area of the buffer at the buffer start.
    \param [IN OUT]     RtkBuffer            <RTK_BUFFER*>        : data buffer to add
    \param [IN]            Length                <uint32_t>                 : header length
    \return  Pointer to the first byte of the extra data is returned
*/
uint8_t*
RtbAddHead(
    RTK_BUFFER* RtkBuffer,
    uint32_t                 Length
    )
{

    if ((uint32_t)(RtkBuffer->Data - RtkBuffer->Head) >= Length)
    {
        RtkBuffer->Data -= Length;
        RtkBuffer->Length += Length;
        RtkBuffer->HeadRoom -= Length;
        return RtkBuffer->Data;
    }

    return NULL;
}
/**
    Remove a specified length data from the start of data buffer hold by specified rtk_buffer.
    This function returns the memory to the headroom.
    \param [IN OUT]     RtkBuffer            <RTK_BUFFER*>        : data buffer to remove
    \param [IN]            Length                <uint32_t>                 : header length
    \return  Pointer to the next data in the buffer is returned, usually useless
*/
unsigned char
RtbRemoveHead(
    RTK_BUFFER* RtkBuffer,
    uint32_t                 Length
    )
{

    if (RtkBuffer->Length >= Length)
    {
        RtkBuffer->Data += Length;
        RtkBuffer->Length -= Length;
        RtkBuffer->HeadRoom += Length;
        return  TRUE;
    }

    return FALSE;
}

/**
    Add a specified length protocal header to the end of data buffer hold by specified rtk_buffer.
    This function extends used data area of the buffer at the buffer end.
    \param [IN OUT]     RtkBuffer            <RTK_BUFFER*>        : data buffer to add
    \param [IN]            Length                <uint32_t>                 : header length
    \return  Pointer to the first byte of the extra data is returned
*/
uint8_t*
RtbAddTail(
    RTK_BUFFER* RtkBuffer,
    uint32_t                 Length
    )
{

    if ((uint32_t)(RtkBuffer->Tail - RtkBuffer->End) >= Length)
    {
        uint8_t* Tmp = RtkBuffer->End;
        RtkBuffer->End += Length;
        RtkBuffer->Length += Length;
        return Tmp;
    }

    return NULL;
}

unsigned char
RtbRemoveTail(
    IN OUT RTK_BUFFER * RtkBuffer,
    IN     uint32_t       Length
)
{

    if ((uint32_t)(RtkBuffer->End - RtkBuffer->Data) >= Length)
    {
        RtkBuffer->End -= Length;
        RtkBuffer->Length -= Length;
        return TRUE;
    }

    return FALSE;
}
//****************************************************************************
// RTB list manipulation
//****************************************************************************
/**
    Initialize a rtb queue.
    \return  Initilized rtb queue if succeed, otherwise NULL
*/
RTB_QUEUE_HEAD*
RtbQueueInit(
)
{
    RTB_QUEUE_HEAD* RtbQueue = NULL;

    RtbQueue = malloc(sizeof(RTB_QUEUE_HEAD));
    if(RtbQueue)
    {
        osi_mutex_new(&RtbQueue->Lock);
        ListInitializeHeader(&RtbQueue->List);
        RtbQueue->QueueLen = 0;
        return RtbQueue;
    }

    //error code comes here
    if (RtbQueue)
    {
        free(RtbQueue);
    }
    return NULL;

}

/**
    Free a rtb queue.
    \param [IN]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
*/
void
RtbQueueFree(
    RTB_QUEUE_HEAD* RtkQueueHead
    )
{
    if (RtkQueueHead)
    {


        RtbEmptyQueue(RtkQueueHead);
        osi_mutex_free(&RtkQueueHead->Lock);
        free(RtkQueueHead);
    }
}

/**
    Queue specified RtkBuffer into a RtkQueue at list tail.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \param [IN]            RtkBuffer                <RTK_BUFFER*>                 : Rtk buffer to add
*/
void
RtbQueueTail(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*                 RtkBuffer
    )
{
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);
    ListAddToTail(&RtkBuffer->List, &RtkQueueHead->List);
    RtkQueueHead->QueueLen++;
    osi_mutex_unlock(&RtkQueueHead->Lock);
}

/**
    Queue specified RtkBuffer into a RtkQueue at list Head.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \param [IN]            RtkBuffer                <RTK_BUFFER*>                 : Rtk buffer to add
*/
void
RtbQueueHead(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*                 RtkBuffer
    )
{
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);
    ListAddToHead(&RtkBuffer->List, &RtkQueueHead->List);
    RtkQueueHead->QueueLen++;
    osi_mutex_unlock(&RtkQueueHead->Lock);
}


/**
    Insert new Rtkbuffer in the old buffer
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \param [IN]            OldRtkBuffer                <RTK_BUFFER*>                 : old rtk buffer
    \param [IN]            NewRtkBuffer                <RTK_BUFFER*>                 : Rtk buffer to add
*/
void
RtbInsertBefore(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*  pOldRtkBuffer,
    IN RTK_BUFFER*  pNewRtkBuffer
)
{
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);
    ListAdd(&pNewRtkBuffer->List, pOldRtkBuffer->List.Prev, &pOldRtkBuffer->List);
    RtkQueueHead->QueueLen++;
    osi_mutex_unlock(&RtkQueueHead->Lock);
}

/**
    check whether the buffer is the last node in the queue
*/
unsigned char
RtbNodeIsLast(
    IN RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*                 pRtkBuffer
)
{
    RTK_BUFFER* pBuf;
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);

    pBuf = (RTK_BUFFER*)RtkQueueHead->List.Prev;
    if(pBuf == pRtkBuffer)
    {
        osi_mutex_unlock(&RtkQueueHead->Lock);
        return TRUE;
    }
    osi_mutex_unlock(&RtkQueueHead->Lock);
    return FALSE;
}

/**
    get the next buffer node after the specified buffer in the queue
    if the specified buffer is the last node in the queue , return NULL
    \param [IN]     RtkBuffer        <RTK_BUFFER*>        : Rtk Queue
    \param [IN]     RtkBuffer        <RTK_BUFFER*>        : Rtk buffer
    \return node after the specified buffer
*/
RTK_BUFFER*
RtbQueueNextNode(
    IN RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*                 pRtkBuffer
)
{
    RTK_BUFFER* pBuf;
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);
    pBuf = (RTK_BUFFER*)RtkQueueHead->List.Prev;
    if(pBuf == pRtkBuffer)
    {
        osi_mutex_unlock(&RtkQueueHead->Lock);
        return NULL;    ///< if it is already the last node in the queue , return NULL
    }
    pBuf = (RTK_BUFFER*)pRtkBuffer->List.Next;
    osi_mutex_unlock(&RtkQueueHead->Lock);
    return pBuf;    ///< return next node after this node
}

/**
    Delete specified RtkBuffer from a RtkQueue.
    It don't hold spinlock itself, so caller must hold it at someplace.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \param [IN]            RtkBuffer                <RTK_BUFFER*>                 : Rtk buffer to Remove
*/
void
RtbRemoveNode(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead,
    IN RTK_BUFFER*                 RtkBuffer
)
{
    RtkQueueHead->QueueLen--;
    ListDeleteNode(&RtkBuffer->List);
}


/**
    Get the RtkBuffer which is the head of a RtkQueue
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \return head of the RtkQueue , otherwise NULL
*/
RTK_BUFFER*
RtbTopQueue(
    IN RTB_QUEUE_HEAD* RtkQueueHead
)
{
    RTK_BUFFER* Rtb = NULL;
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);

    if (RtbQueueIsEmpty(RtkQueueHead))
    {
        osi_mutex_unlock(&RtkQueueHead->Lock);
        return NULL;
    }

    Rtb = (RTK_BUFFER*)RtkQueueHead->List.Next;
    osi_mutex_unlock(&RtkQueueHead->Lock);

    return Rtb;
}

/**
    Remove a RtkBuffer from specified rtkqueue at list tail.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \return    removed rtkbuffer if succeed, otherwise NULL
*/
RTK_BUFFER*
RtbDequeueTail(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead
)
{
    RTK_BUFFER* Rtb = NULL;

    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);
    if (RtbQueueIsEmpty(RtkQueueHead))
    {
         osi_mutex_unlock(&RtkQueueHead->Lock);
         return NULL;
    }
    Rtb = (RTK_BUFFER*)RtkQueueHead->List.Prev;
    RtbRemoveNode(RtkQueueHead, Rtb);
    osi_mutex_unlock(&RtkQueueHead->Lock);

    return Rtb;
}

/**
    Remove a RtkBuffer from specified rtkqueue at list head.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \return    removed rtkbuffer if succeed, otherwise NULL
*/
RTK_BUFFER*
RtbDequeueHead(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead
    )
{
    RTK_BUFFER* Rtb = NULL;
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);

     if (RtbQueueIsEmpty(RtkQueueHead))
     {
         osi_mutex_unlock(&RtkQueueHead->Lock);
         return NULL;
     }
    Rtb = (RTK_BUFFER*)RtkQueueHead->List.Next;
    RtbRemoveNode(RtkQueueHead, Rtb);
    osi_mutex_unlock(&RtkQueueHead->Lock);
    return Rtb;
}

/**
    Get current rtb queue's length.
    \param [IN]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
    \return    current queue's length
*/
signed long RtbGetQueueLen(
    IN RTB_QUEUE_HEAD* RtkQueueHead
    )
{
    return RtkQueueHead->QueueLen;
}

/**
    Empty the rtkqueue.
    \param [IN OUT]     RtkQueueHead        <RTB_QUEUE_HEAD*>        : Rtk Queue
*/
void
RtbEmptyQueue(
    IN OUT RTB_QUEUE_HEAD* RtkQueueHead
    )
{
    RTK_BUFFER* Rtb = NULL;
    osi_mutex_lock(&RtkQueueHead->Lock, OSI_MUTEX_MAX_TIMEOUT);

    while( !RtbQueueIsEmpty(RtkQueueHead))
    {
        Rtb = (RTK_BUFFER*)RtkQueueHead->List.Next;
        RtbRemoveNode(RtkQueueHead, Rtb);
        RtbFree(Rtb);
    }

    osi_mutex_unlock(&RtkQueueHead->Lock);
    return;
}


///Annie_tmp
unsigned char
RtbCheckQueueLen(IN RTB_QUEUE_HEAD* RtkQueueHead, IN uint8_t Len)
{
    return RtkQueueHead->QueueLen < Len ? TRUE : FALSE;
}

/**
    clone buffer for upper or lower layer, because original buffer should be stored in l2cap
    \param <RTK_BUFFER* pDataBuffer: original buffer
    \return cloned buffer
*/
RTK_BUFFER*
RtbCloneBuffer(
    IN RTK_BUFFER* pDataBuffer
)
{
    RTK_BUFFER* pNewBuffer = NULL;
    if(pDataBuffer)
    {
        pNewBuffer = RtbAllocate(pDataBuffer->Length,0);
        if(!pNewBuffer)
        {
            return NULL;
        }
        if(pDataBuffer && pDataBuffer->Data)
            memcpy(pNewBuffer->Data, pDataBuffer->Data, pDataBuffer->Length);
        else
        {
            RtbFree(pNewBuffer);
            return NULL;
        }

        pNewBuffer->Length = pDataBuffer->Length;
    }
    return pNewBuffer;
}
