/*!
 * \file      timer.c
 *
 * \brief     Timer objects and scheduling management implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "utilities.h"
#include "board.h"
#include "rtc-board.h"
#include "system/timer.h"

/*!
 * Safely execute call back
 */
#define ExecuteCallBack( _callback_ ) \
    do                                \
    {                                 \
        if( _callback_ == NULL )      \
        {                             \
            while( 1 );               \
        }                             \
        else                          \
        {                             \
            _callback_( );            \
        }                             \
    }while( 0 );

/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer( TimerEvent_t *obj );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer( TimerEvent_t *obj );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
static bool TimerExists( TimerEvent_t *obj );

void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
    obj->Timestamp = 0;
    obj->ReloadValue = 0;
    obj->IsRunning = false;
    obj->Callback = callback;
    obj->Next = NULL;
}

void TimerStart( TimerEvent_t *obj )
{
    uint32_t elapsedTime = 0;

    CRITICAL_SECTION_BEGIN( );

    if( ( obj == NULL ) || ( TimerExists( obj ) == true ) )
    {
        CRITICAL_SECTION_END( );
        return;
    }

    obj->Timestamp = obj->ReloadValue;
    obj->IsRunning = false;

    if( TimerListHead == NULL )
    {
        RtcSetTimerContext( );
        // Inserts a timer at time now + obj->Timestamp
        TimerInsertNewHeadTimer( obj );
    }
    else
    {
        elapsedTime = RtcGetTimerElapsedTime( );
        obj->Timestamp += elapsedTime;

        if( obj->Timestamp < TimerListHead->Timestamp )
        {
            TimerInsertNewHeadTimer( obj );
        }
        else
        {
            TimerInsertTimer( obj );
        }
    }
    CRITICAL_SECTION_END( );
}

static void TimerInsertTimer( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;
    TimerEvent_t* next = TimerListHead->Next;

    while( cur->Next != NULL )
    {
        if( obj->Timestamp > next->Timestamp )
        {
            cur = next;
            next = next->Next;
        }
        else
        {
            cur->Next = obj;
            obj->Next = next;
            return;
        }
    }
    cur->Next = obj;
    obj->Next = NULL;
}

static void TimerInsertNewHeadTimer( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    if( cur != NULL )
    {
        cur->IsRunning = false;
    }

    obj->Next = cur;
    TimerListHead = obj;
    TimerSetTimeout( TimerListHead );
}

void TimerIrqHandler( void )
{
    TimerEvent_t* cur;
    TimerEvent_t* next;

    uint32_t old =  RtcGetTimerContext( );
    uint32_t now =  RtcSetTimerContext( );
    uint32_t deltaContext = now - old; // intentional wrap around

    // Update timeStamp based upon new Time Reference
    // because delta context should never exceed 2^32
    if( TimerListHead != NULL )
    {
        for( cur = TimerListHead; cur->Next != NULL; cur = cur->Next )
        {
            next = cur->Next;
            if( next->Timestamp > deltaContext )
            {
                next->Timestamp -= deltaContext;
            }
            else
            {
                next->Timestamp = 0;
            }
        }
    }

    // Execute immediately the alarm callback
    if ( TimerListHead != NULL )
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        ExecuteCallBack( cur->Callback );
    }

    // Remove all the expired object from the list
    while( ( TimerListHead != NULL ) && ( TimerListHead->Timestamp < RtcGetTimerElapsedTime( ) ) )
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        ExecuteCallBack( cur->Callback );
    }

    // Start the next TimerListHead if it exists AND NOT running
    if( ( TimerListHead != NULL ) && ( TimerListHead->IsRunning == false ) )
    {
        TimerSetTimeout( TimerListHead );
    }
}

void TimerStop( TimerEvent_t *obj )
{
    CRITICAL_SECTION_BEGIN( );

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

    // List is empty or the obj to stop does not exist
    if( ( TimerListHead == NULL ) || ( obj == NULL ) )
    {
        if (obj != NULL) {
            obj->IsRunning = false;
        }

        CRITICAL_SECTION_END( );
        return;
    }

    if( TimerListHead == obj ) // Stop the Head
    {
        if( TimerListHead->IsRunning == true ) // The head is already running
        {
            TimerListHead->IsRunning = false;
            if( TimerListHead->Next != NULL )
            {
                TimerListHead = TimerListHead->Next;
                TimerSetTimeout( TimerListHead );
            }
            else
            {
                RtcStopAlarm( );
                TimerListHead = NULL;
            }
        }
        else // Stop the head before it is started
        {
            if( TimerListHead->Next != NULL )
            {
                TimerListHead = TimerListHead->Next;
            }
            else
            {
                TimerListHead = NULL;
            }
        }
    }
    else // Stop an object within the list
    {
        obj->IsRunning = false;
        while( cur != NULL )
        {
            if( cur == obj )
            {
                if( cur->Next != NULL )
                {
                    cur = cur->Next;
                    prev->Next = cur;
                }
                else
                {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            }
            else
            {
                prev = cur;
                cur = cur->Next;
            }
        }
    }
    CRITICAL_SECTION_END( );
}

static bool TimerExists( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    while( cur != NULL )
    {
        if( cur == obj )
        {
            return true;
        }
        cur = cur->Next;
    }
    return false;
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop( obj );
    TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    uint32_t minValue = 0;
    uint32_t ticks = RtcMs2Tick( value );

    TimerStop( obj );

    minValue = RtcGetMinimumTimeout( );

    if( ticks < minValue )
    {
        ticks = minValue;
    }

    obj->Timestamp = ticks;
    obj->ReloadValue = ticks;
}

TimerTime_t TimerGetCurrentTime( void )
{
    uint32_t now = RtcGetTimerValue( );
    return  RtcTick2Ms( now );
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )
{
    uint32_t nowInTicks = RtcGetTimerValue( );
    uint32_t pastInTicks = RtcMs2Tick( past );

    // Intentional wrap around. Works Ok if tick duration below 1ms
    return RtcTick2Ms( nowInTicks - pastInTicks );
}

static void TimerSetTimeout( TimerEvent_t *obj )
{
    int32_t minTicks= RtcGetMinimumTimeout( );
    obj->IsRunning = true;

    //in case deadline too soon
    if(obj->Timestamp  < ( RtcGetTimerElapsedTime( ) + minTicks) )
    {
        obj->Timestamp = RtcGetTimerElapsedTime( ) + minTicks;
    }
    RtcSetAlarm( obj->Timestamp );
}

TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature )
{
    return RtcTempCompensation( period, temperature );
}

void TimerProcess( void )
{
    RtcProcess( );
}
