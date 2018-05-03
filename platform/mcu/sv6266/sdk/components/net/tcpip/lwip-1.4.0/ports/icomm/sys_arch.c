/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

//*****************************************************************************
//
// Include OS functionality.
//
//*****************************************************************************

/* ------------------------ System architecture includes ----------------------------- */
#include "soc_types.h"
#include "arch/sys_arch.h"
#include "arch/cc.h"
#include "osal.h"

/* ------------------------ lwIP includes --------------------------------- */
#include <lwip/opt.h>

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

/* ------------------------ lwIP includes --------------------------------- */

#define LWIP_SEM_MAX 1


/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates a new mailbox
 * Inputs:
 *      int size                -- Size of elements in the mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
err_t sys_mbox_new( sys_mbox_t *pxMailBox, int iSize )
{
	err_t xReturn = ERR_MEM;
	int OS_Return;

	//*pxMailBox = xQueueCreate( iSize, sizeof( void * ) );
	//change to OS_independent
    OS_Return=OS_MsgQCreate(pxMailBox, iSize); 
	

	if(OS_Return==0)
	{
		xReturn = ERR_OK;
		SYS_STATS_INC_USED( mbox );
	}
	return xReturn;
}


/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a mailbox. If there are messages still present in the
 *      mailbox when the mailbox is deallocated, it is an indication of a
 *      programming error in lwIP and the developer should be notified.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
void sys_mbox_free( sys_mbox_t *pxMailBox )
{


	#if SYS_STATS
    unsigned long ulMessagesWaiting = (unsigned long)OS_MsgQWaitingSize((void*)pxMailBox);

	{
		if( ulMessagesWaiting != 0UL )
		{
			SYS_STATS_INC( mbox.err );
		}

		SYS_STATS_DEC( mbox.used );
	}
    #else
    OS_MsgQWaitingSize(pxMailBox);
	#endif /* SYS_STATS */

	OS_MsgQDelete(*pxMailBox);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_post
 *---------------------------------------------------------------------------*
 * Description:
 *      Post the "msg" to the mailbox.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *data              -- Pointer to data to post
 *---------------------------------------------------------------------------*/
void sys_mbox_post( sys_mbox_t *pxMailBox, void *pxMessageToPost )
{
	/******************************/
	OsMsgQEntry OsMsg;
    int res;
    OsMsg.MsgCmd=0;
	OsMsg.MsgData=pxMessageToPost;

	/******************************/
	//while( xQueueSendToBack( *pxMailBox, &pxMessageToPost, portMAX_DELAY ) != pdTRUE );
	//change to OS_independent
	/*res=uxQueueMessagesWaiting(*pxMailBox);
	LOG_PRINTF("Before queue size=%d\r\n",res);*/
    do
    {   
        res = OS_MsgQEnqueue( *pxMailBox, &OsMsg );
        if(OS_SUCCESS == res)
            break;

        OS_TickDelay(1);
        //LOG_PRINTF("tcp mbox_post fail\r\n");        

    }while(1);
	/*res=uxQueueMessagesWaiting(*pxMailBox);
	LOG_PRINTF("After queue size=%d\r\n",res);*/

}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_trypost
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to post the "msg" to the mailbox.  Returns immediately with
 *      error if cannot.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *msg               -- Pointer to data to post
 * Outputs:
 *      err_t                   -- ERR_OK if message posted, else ERR_MEM
 *                                  if not.
 *---------------------------------------------------------------------------*/
err_t sys_mbox_trypost( sys_mbox_t *pxMailBox, void *pxMessageToPost )
{
err_t xReturn;
/******************************/
	OsMsgQEntry OsMsg;
	OsMsg.MsgCmd=0;
	OsMsg.MsgData=pxMessageToPost;
	/******************************/
	//if( xQueueSend( *pxMailBox, &pxMessageToPost, 0UL ) == pdPASS )
	//if( xQueueSend( *pxMailBox, &OsMsg, 0UL ) == pdPASS )
	//if(OS_MsgQEnqueue( *pxMailBox, &OsMsg,false ) == OS_SUCCESS )
  if(OS_MsgQEnqueue( *pxMailBox, &OsMsg ) == OS_SUCCESS )
	{
		xReturn = ERR_OK;
	}
	else
	{		
		/* The queue was already full. */
		xReturn = ERR_MEM;
		SYS_STATS_INC( mbox.err );
	}

	return xReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_fetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread until a message arrives in the mailbox, but does
 *      not block the thread longer than "timeout" milliseconds (similar to
 *      the sys_arch_sem_wait() function). The "msg" argument is a result
 *      parameter that is set by the function (i.e., by doing "*msg =
 *      ptr"). The "msg" parameter maybe NULL to indicate that the message
 *      should be dropped.
 *
 *      The return values are the same as for the sys_arch_sem_wait() function:
 *      Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
 *      timeout.
 *
 *      Note that a function with a similar name, sys_mbox_fetch(), is
 *      implemented by lwIP.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- SYS_ARCH_TIMEOUT if timeout, else number
 *                                  of milliseconds until received.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_fetch( sys_mbox_t *pxMailBox, void **ppvBuffer, u32_t ulTimeOut )
{

    OsMsgQEntry OsMsg;

    void *pvDummy;
    OS_TICKTYPE xStartTime = 0, xEndTime = 0, xElapsed = 0;
    unsigned long ulReturn;
    OsMsg.MsgCmd=0;
    OsMsg.MsgData=*ppvBuffer;
    xStartTime = OS_GetSysTick();
    //ulTimeOut=ulTimeOut>>2;
    if( NULL == ppvBuffer )
    {
        ppvBuffer = &pvDummy;
    }
    if( ulTimeOut != 0 )
    {
        //if( pdTRUE == xQueueReceive( *pxMailBox, &( OsMsg ), ulTimeOut/ portTICK_RATE_MS ) )
        //change to OS_independent
        if( OS_SUCCESS == OS_MsgQDequeue( *pxMailBox, &OsMsg,(ulTimeOut/OS_TICK_RATE_MS) ))
        {
            *ppvBuffer=OsMsg.MsgData;
            xEndTime = OS_GetSysTick();
            xElapsed = ( xEndTime - xStartTime ) * OS_TICK_RATE_MS;
            ulReturn = xElapsed;
        }
        else 
        {
            /* Timed out. */
            *ppvBuffer = NULL;
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        //while( pdTRUE != xQueueReceive( *pxMailBox, &( *ppvBuffer ), portMAX_DELAY ) );
        //change to OS_independent
        if (OS_SUCCESS == OS_MsgQDequeue( *pxMailBox, &OsMsg,portMAX_DELAY ) )
        {
            *ppvBuffer=OsMsg.MsgData;
            xEndTime = OS_GetSysTick();
            xElapsed = ( xEndTime - xStartTime ) * OS_TICK_RATE_MS;
            ulReturn = xElapsed;
        }
        else
        {
            // aMH TODO: check this return is reight??
            /* Timed out. */
            *ppvBuffer = NULL;
            ulReturn = SYS_ARCH_TIMEOUT;
        }
	}
    return ulReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_tryfetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Similar to sys_arch_mbox_fetch, but if message is not ready
 *      immediately, we'll return with SYS_MBOX_EMPTY.  On success, 0 is
 *      returned.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 * Outputs:
 *      u32_t                   -- SYS_MBOX_EMPTY if no messages.  Otherwise,
 *                                  return ERR_OK.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_tryfetch( sys_mbox_t *pxMailBox, void **ppvBuffer )
{

    OsMsgQEntry OsMsg;
    void *pvDummy;
    unsigned long ulReturn;
	if( ppvBuffer== NULL )
	{
		ppvBuffer = &pvDummy;
	}

	//if( pdTRUE == xQueueReceive( *pxMailBox, &( *ppvBuffer ), 0UL ) )
	if( OS_SUCCESS == OS_MsgQDequeue( *pxMailBox, &OsMsg,1 ))
	{
		*ppvBuffer=OsMsg.MsgData;
		ulReturn = ERR_OK;
	}
	else
	{
		ulReturn = SYS_MBOX_EMPTY;
	}

	return ulReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates and returns a new semaphore. The "ucCount" argument specifies
 *      the initial state of the semaphore.
 *      NOTE: Currently this routine only creates counts of 1 or 0
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      u8_t ucCount              -- Initial ucCount of semaphore (1 or 0)
 * Outputs:
 *      sys_sem_t               -- Created semaphore or 0 if could not create.
 *---------------------------------------------------------------------------*/
err_t sys_sem_new( sys_sem_t *pxSemaphore, u8_t ucCount )
{
    err_t xReturn = ERR_MEM;

    OS_SemInit( pxSemaphore , LWIP_SEM_MAX, ucCount);

    if( *pxSemaphore != NULL )
    {
        xReturn = ERR_OK;
        SYS_STATS_INC_USED( sem );
    }
    else
    {
        SYS_STATS_INC( sem.err );
    }

    return xReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_sem_wait
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread while waiting for the semaphore to be
 *      signaled. If the "timeout" argument is non-zero, the thread should
 *      only be blocked for the specified time (measured in
 *      milliseconds).
 *
 *      If the timeout argument is non-zero, the return value is the number of
 *      milliseconds spent waiting for the semaphore to be signaled. If the
 *      semaphore wasn't signaled within the specified time, the return value is
 *      SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 *      (i.e., it was already signaled), the function may return zero.
 *
 *      Notice that lwIP implements a function with a similar name,
 *      sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to wait on
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- Time elapsed or SYS_ARCH_TIMEOUT.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_sem_wait( sys_sem_t *pxSemaphore, u32_t ulTimeout )
{
    OS_TICKTYPE xStartTime, xEndTime, xElapsed;
    u32_t ulReturn;
    
    xStartTime = OS_GetSysTick();

    if( ulTimeout != 0UL )
    {
        //if ulTimeout less than OS_TICK_RATE_MS, it will cause sleep forever.
        if(ulTimeout<OS_TICK_RATE_MS)
            ulTimeout = OS_TICK_RATE_MS;
        
        if( OS_SemWait( *pxSemaphore , (ulTimeout/(OS_TICK_RATE_MS))) == OS_SUCCESS )
        {
            xEndTime = OS_GetSysTick();
            xElapsed = (xEndTime - xStartTime) * OS_TICK_RATE_MS;
            ulReturn = xElapsed;
        }
        else
        {
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        OS_SemWait( *pxSemaphore , portMAX_DELAY);
        xEndTime = OS_GetSysTick();
        xElapsed = ( xEndTime - xStartTime ) * OS_TICK_RATE_MS;

        if( xElapsed == 0UL )
        {
            xElapsed = 1UL;
        }

        ulReturn = xElapsed;
    }

    return ulReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_signal
 *---------------------------------------------------------------------------*
 * Description:
 *      Signals (releases) a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to signal
 *---------------------------------------------------------------------------*/
void sys_sem_signal( sys_sem_t *pxSemaphore )
{
    OS_SemSignal( *pxSemaphore);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 *---------------------------------------------------------------------------*/
void sys_sem_free( sys_sem_t *pxSemaphore )
{
	//SYS_STATS_DEC(sem.used);	
    OS_SemDelete(*pxSemaphore);
	//vQueueDelete( *pxSemaphore );
}

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new( sys_mutex_t *pxMutex ) 
{
err_t xReturn = ERR_MEM;

    OS_MutexInit( pxMutex );

	if( *pxMutex != NULL ) 
	{
		xReturn = ERR_OK;
		SYS_STATS_INC_USED( mutex );
	} 
	else 
	{
		SYS_STATS_INC( mutex.err );
	}
	
	return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock( sys_mutex_t *pxMutex )
{
    OS_MutexLock( *pxMutex );
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *pxMutex )
{
    OS_MutexUnLock( *pxMutex);
	//xSemaphoreGive( *pxMutex );
}


/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free( sys_mutex_t *pxMutex )
{
    OS_MutexDelete( *pxMutex );
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize sys arch
 *---------------------------------------------------------------------------*/
void sys_init(void)
{

}

u32_t sys_now(void)
{
	return os_tick2ms(OS_GetSysTick());
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_thread_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts a new thread with priority "prio" that will begin its
 *      execution in the function "thread()". The "arg" argument will be
 *      passed as an argument to the thread() function. The id of the new
 *      thread is returned. Both the id and the priority are system
 *      dependent.
 * Inputs:
 *      char *name              -- Name of thread
 *      void (* thread)(void *arg) -- Pointer to function to run.
 *      void *arg               -- Argument passed into function
 *      int stacksize           -- Required stack amount in bytes
 *      int prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
sys_thread_t sys_thread_new( const char *pcName, void( *pxThread )( void *pvParameters ), void *pvArg, int iStackSize, int iPriority )
{
    OsTaskHandle xCreatedTask = NULL;

    s32_t xResult;
    xResult = OS_TaskCreate( pxThread, (void *)pcName, iStackSize, pvArg, iPriority, &xCreatedTask );

	return xCreatedTask;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect( void )
{
    return ( sys_prot_t ) OS_IntSave();
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect( sys_prot_t xValue )
{    
    OS_IntRestore(xValue);
}

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert( const char *pcMessage )
{
	(void) pcMessage;

	for (;;)
	{
	}
}
/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/

