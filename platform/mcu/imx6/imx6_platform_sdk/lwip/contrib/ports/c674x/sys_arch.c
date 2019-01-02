/************************************************************************/
/**	\file	sys_arch.c
 *	\brief	LwIP system functions for SYSBIOS on C674x
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include <string.h>

/* Macros */

/* Types */

/* Externals */

/* Variables */

/* Functions */

//--------------------------------------------------------------------------
void sys_init(void)
{

}

//--------------------------------------------------------------------------
err_t sys_sem_new(sys_sem_t* sem, u8_t count)
{
	*sem = Semaphore_create(count, NULL, NULL);

	if (*sem != NULL)
	{
		SYS_STATS_INC_USED(sem);
		return ERR_OK;
	}
	else
	{
		SYS_STATS_INC(sem.err);
		return ERR_MEM;
	}
}

//--------------------------------------------------------------------------
void sys_sem_signal(sys_sem_t* sem)
{
	LWIP_ASSERT("sys_sem_signal: invalid semaphore", sys_sem_valid(sem));

	Semaphore_post(*sem);
}

//--------------------------------------------------------------------------
u32_t sys_arch_sem_wait(sys_sem_t* sem, u32_t timeout)
{
	u32_t		time;
	u32_t		before, after;
	UInt		ticks;
	
	LWIP_ASSERT("sys_arch_sem_wait: invalid semaphore", sys_sem_valid(sem));

	if (!timeout)	ticks = BIOS_WAIT_FOREVER;
	else			ticks = timeout;
	before = Clock_getTicks();
	if (!Semaphore_pend(*sem, ticks))	return SYS_ARCH_TIMEOUT;
	after = Clock_getTicks();

	time = after - before;
  	return time;
}

//--------------------------------------------------------------------------
void sys_sem_free(sys_sem_t* sem)
{
	LWIP_ASSERT("sys_sem_free: invalid semaphore", sys_sem_valid(sem));

	Semaphore_delete(sem);

	SYS_STATS_DEC(sem.used);
}


//--------------------------------------------------------------------------
err_t sys_mutex_new(sys_mutex_t *mutex)
{
	mutex->Handle = GateMutex_create(NULL, NULL);

	if (mutex->Handle != NULL)
	{
		SYS_STATS_INC_USED(mutex);
		return ERR_OK;
	}
	else
	{
		SYS_STATS_INC(mutex.err);
		return ERR_MEM;
	}
}

//--------------------------------------------------------------------------
void sys_mutex_lock(sys_mutex_t *mutex)
{
	LWIP_ASSERT("sys_mutex_lock: invalid mutex", sys_mutex_valid(mutex));

	mutex->Key = GateMutex_enter(mutex->Handle);
}

//--------------------------------------------------------------------------
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	LWIP_ASSERT("sys_mutex_unlock: invalid mutex", sys_mutex_valid(mutex));

	GateMutex_leave(mutex->Handle, mutex->Key);
}

//--------------------------------------------------------------------------
void sys_mutex_free(sys_mutex_t *mutex)
{
	LWIP_ASSERT("sys_mutex_free: invalid mutex", sys_mutex_valid(mutex));

	GateMutex_delete(&mutex->Handle);

	SYS_STATS_DEC(mutex.used);
}

//--------------------------------------------------------------------------
err_t sys_mbox_new(sys_mbox_t* mbox, int size)
{
	LWIP_ASSERT("sys_mbox_new: size is 0", size != 0);

	Mailbox_Handle m = Mailbox_create(sizeof(void *), size, NULL, NULL);

	LWIP_ASSERT("sys_mbox_new: new mbox not allocated", m != NULL);

	if (m)
	{
		SYS_STATS_INC_USED(mbox);
		*mbox = m;
		return ERR_OK;
	}
	else
	{
		SYS_STATS_INC(mbox.err);
		return ERR_MEM;
	}
}

//--------------------------------------------------------------------------
void sys_mbox_free(sys_mbox_t* mbox)
{
	LWIP_ASSERT("sys_mbox_free: invalid mbox", sys_mbox_valid(mbox));

	Mailbox_delete(mbox);

	SYS_STATS_DEC(mbox.used);
}

//--------------------------------------------------------------------------
err_t sys_mbox_trypost(sys_mbox_t* mbox, void* msg)
{

	LWIP_ASSERT("sys_mbox_trypost: invalid mbox", sys_mbox_valid(mbox));

	LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox %p msg %p\n",
                          (void *)mbox, (void *)msg));  

	Bool bval = Mailbox_post( (Mailbox_Handle) *mbox, &msg, BIOS_NO_WAIT );

	if (!bval)
	{
		LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox is full\n"));
		return ERR_MEM;
	}
	return ERR_OK;
}

//--------------------------------------------------------------------------
void sys_mbox_post(sys_mbox_t* mbox, void *msg)
{
	LWIP_ASSERT("sys_mbox_post: invalid mbox", sys_mbox_valid(mbox));

	Mailbox_post((Mailbox_Handle) *mbox, &msg, BIOS_WAIT_FOREVER);
}

//--------------------------------------------------------------------------
u32_t sys_arch_mbox_fetch(sys_mbox_t* mbox, void **msg, u32_t timeout)
{
	u32_t before, after;
	u32_t ticks;
	LWIP_ASSERT("sys_arch_mbox_fetch: invalid mbox", sys_mbox_valid(mbox));

	if (!timeout)	ticks = BIOS_WAIT_FOREVER;
	else			ticks = timeout;
	before = Clock_getTicks();

	Bool bval = Mailbox_pend( (Mailbox_Handle) *mbox, msg, ticks);
	if (!bval) return SYS_ARCH_TIMEOUT;
	after = Clock_getTicks();
	return (after-before);
}

//--------------------------------------------------------------------------
u32_t sys_arch_mbox_tryfetch(sys_mbox_t* mbox, void **msg)
{
	LWIP_ASSERT("sys_arch_mbox_tryfetch: invalid mbox", sys_mbox_valid(mbox));

	Bool bval = Mailbox_pend((Mailbox_Handle) *mbox, msg, BIOS_NO_WAIT);
	return bval ? 0 : SYS_MBOX_EMPTY;
}

//--------------------------------------------------------------------------
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	Task_Params tpm;
	Task_Params_init(&tpm);
	
	tpm.priority = prio;
	tpm.stackSize = stacksize;
	tpm.arg0 = (UArg) arg;

	Task_Handle h = Task_create( (Task_FuncPtr) thread, &tpm, NULL );
	return (sys_thread_t) h;
}
