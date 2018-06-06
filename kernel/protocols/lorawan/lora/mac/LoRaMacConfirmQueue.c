/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
 ___ _____ _   ___ _  _____ ___  ___  ___ ___
/ __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
\__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
|___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
embedded.connectivity.solutions===============

Description: LoRa MAC confirm queue implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Gregory Cristian ( Semtech ) and Daniel Jaeckle ( STACKFORCE )
*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "timer.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "LoRaMacConfirmQueue.h"


static LoRaMacPrimitives_t* Primitives;

/*!
 * MlmeConfirm queue data structure
 */
static MlmeConfirmQueue_t MlmeConfirmQueue[LORA_MAC_MLME_CONFIRM_QUEUE_LEN];

/*!
 * Counts the number of MlmeConfirms to process
 */
static uint8_t MlmeConfirmQueueCnt;

/*!
 * Pointer to the first element of the ring buffer
 */
MlmeConfirmQueue_t* BufferStart;

/*!
 * Pointer to the last element of the ring buffer
 */
MlmeConfirmQueue_t* BufferEnd;

/*!
 * Variable which holds a common status
 */
LoRaMacEventInfoStatus_t CommonStatus;


static MlmeConfirmQueue_t* IncreaseBufferPointer( MlmeConfirmQueue_t* bufferPointer )
{
    if( bufferPointer == &MlmeConfirmQueue[LORA_MAC_MLME_CONFIRM_QUEUE_LEN - 1] )
    {
        // Reset to the first element
        bufferPointer = MlmeConfirmQueue;
    }
    else
    {
        // Increase
        bufferPointer++;
    }
    return bufferPointer;
}

static MlmeConfirmQueue_t* DecreaseBufferPointer( MlmeConfirmQueue_t* bufferPointer )
{
    if( bufferPointer == MlmeConfirmQueue )
    {
        // Reset to the last element
        bufferPointer = &MlmeConfirmQueue[LORA_MAC_MLME_CONFIRM_QUEUE_LEN - 1];
    }
    else
    {
        bufferPointer--;
    }
    return bufferPointer;
}

static MlmeConfirmQueue_t* GetElement( Mlme_t request, MlmeConfirmQueue_t* bufferStart, MlmeConfirmQueue_t* bufferEnd )
{
    MlmeConfirmQueue_t* element = bufferStart;

    do
    {
        if( element->Request == request )
        {
            // We have found the element
            return element;
        }
        else
        {
            element = IncreaseBufferPointer( element );
        }
    }while( element != bufferEnd );

    return NULL;
}


void LoRaMacConfirmQueueInit( LoRaMacPrimitives_t* primitives )
{
    Primitives = primitives;

    // Init counter
    MlmeConfirmQueueCnt = 0;

    // Init buffer
    BufferStart = MlmeConfirmQueue;
    BufferEnd = MlmeConfirmQueue;

    memset1( (uint8_t*) MlmeConfirmQueue, 0xFF, sizeof( MlmeConfirmQueue ) );

    // Common status
    CommonStatus = LORAMAC_EVENT_INFO_STATUS_ERROR;
}

bool LoRaMacConfirmQueueAdd( MlmeConfirmQueue_t* mlmeConfirm )
{
    if( MlmeConfirmQueueCnt >= LORA_MAC_MLME_CONFIRM_QUEUE_LEN )
    {
        // Protect the buffer against overwrites
        return false;
    }

    // Add the element to the ring buffer
    BufferEnd->Request = mlmeConfirm->Request;
    BufferEnd->Status = mlmeConfirm->Status;
    BufferEnd->RestrictCommonReadyToHandle = mlmeConfirm->RestrictCommonReadyToHandle;
    BufferEnd->ReadyToHandle = false;
    // Increase counter
    MlmeConfirmQueueCnt++;
    // Update end pointer
    BufferEnd = IncreaseBufferPointer( BufferEnd );

    return true;
}

bool LoRaMacConfirmQueueRemoveLast( void )
{
    if( MlmeConfirmQueueCnt == 0 )
    {
        return false;
    }

    // Increase counter
    MlmeConfirmQueueCnt--;
    // Update start pointer
    BufferEnd = DecreaseBufferPointer( BufferEnd );

    return true;
}

bool LoRaMacConfirmQueueRemoveFirst( void )
{
    if( MlmeConfirmQueueCnt == 0 )
    {
        return false;
    }

    // Increase counter
    MlmeConfirmQueueCnt--;
    // Update start pointer
    BufferStart = IncreaseBufferPointer( BufferStart );

    return true;
}

void LoRaMacConfirmQueueSetStatus( LoRaMacEventInfoStatus_t status, Mlme_t request )
{
    MlmeConfirmQueue_t* element = NULL;

    if( MlmeConfirmQueueCnt > 0 )
    {
        element = GetElement( request, BufferStart, BufferEnd );
        if( element != NULL )
        {
            element->Status = status;
            element->ReadyToHandle = true;
        }
    }
}

LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatus( Mlme_t request )
{
    MlmeConfirmQueue_t* element = NULL;

    if( MlmeConfirmQueueCnt > 0 )
    {
        element = GetElement( request, BufferStart, BufferEnd );
        if( element != NULL )
        {
            return element->Status;
        }
    }
    return LORAMAC_EVENT_INFO_STATUS_ERROR;
}

void LoRaMacConfirmQueueSetStatusCmn( LoRaMacEventInfoStatus_t status )
{
    MlmeConfirmQueue_t* element = BufferStart;

    CommonStatus = status;

    if( MlmeConfirmQueueCnt > 0 )
    {
        do
        {
            element->Status = status;
            // Set the status if it is allowed to set it with a call to
            // LoRaMacConfirmQueueSetStatusCmn.
            if( element->RestrictCommonReadyToHandle == false )
            {
                element->ReadyToHandle = true;
            }
            element = IncreaseBufferPointer( element );
        }while( element != BufferEnd );
    }
}

LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatusCmn( void )
{
    return CommonStatus;
}

bool LoRaMacConfirmQueueIsCmdActive( Mlme_t request )
{
    if( GetElement( request, BufferStart, BufferEnd ) != NULL )
    {
        return true;
    }
    return false;
}

void LoRaMacConfirmQueueHandleCb( MlmeConfirm_t* mlmeConfirm )
{
    uint8_t nbElements = MlmeConfirmQueueCnt;
    bool readyToHandle = false;
    MlmeConfirmQueue_t mlmeConfirmToStore;

    for( uint8_t i = 0; i < nbElements; i++ )
    {
        mlmeConfirm->MlmeRequest = BufferStart->Request;
        mlmeConfirm->Status = BufferStart->Status;
        readyToHandle = BufferStart->ReadyToHandle;

        if( readyToHandle == true )
        {
            Primitives->MacMlmeConfirm( mlmeConfirm );
        }
        else
        {
            // The request is not processed yet. Store the state.
            mlmeConfirmToStore.Request = BufferStart->Request;
            mlmeConfirmToStore.Status = BufferStart->Status;
            mlmeConfirmToStore.RestrictCommonReadyToHandle = BufferStart->RestrictCommonReadyToHandle;
        }

        // Increase the pointer afterwards to prevent overwrites
        LoRaMacConfirmQueueRemoveFirst( );

        if( readyToHandle == false )
        {
            // Add a request which has not been finished again to the queue
            LoRaMacConfirmQueueAdd( &mlmeConfirmToStore );
        }
    }
}

uint8_t LoRaMacConfirmQueueGetCnt( void )
{
    return MlmeConfirmQueueCnt;
}

bool LoRaMacConfirmQueueIsFull( void )
{
    if( MlmeConfirmQueueCnt >= LORA_MAC_MLME_CONFIRM_QUEUE_LEN )
    {
        return true;
    }
    else
    {
        return false;
    }
}
