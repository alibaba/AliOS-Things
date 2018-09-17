/************************************************************************/
/**	\file	sys_arch.c
 *	\brief	LwIP system functions for VDK on blackfin
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include <string.h>
#include "VDK.h"

/* Macros */

/* Types */
struct sys_mbox
{
	sys_sem_t	sem;
	s16_t		size;		// max number of messages in the mailbox
	s16_t		count;		// number of messages in queue
	s16_t		read;		// index of next message to read
	s16_t		write;		// index of next message to write
	void*		msgs[];		// the message queue
};

struct sys_thread_params
{
	lwip_thread_fn	fn;
	void*			arg;
};

/* Externals */
extern struct VDK_ThreadTemplate LwIP_ThreadTemplate;

/* Variables */
u32_t	ticks_per_ms;
u32_t	ms_per_ticks;

/* Functions */

//--------------------------------------------------------------------------
void sys_init(void)
{
	ms_per_ticks = VDK_GetTickPeriod() * 256;
	ticks_per_ms = 256 / VDK_GetTickPeriod();
}

//--------------------------------------------------------------------------
u32_t sys_now(void)
{
	return VDK_GetUptime();
}

//--------------------------------------------------------------------------
err_t sys_sem_new(sys_sem_t* sem, u8_t count)
{
	*sem = VDK_CreateSemaphore(count, UINT_MAX, 0, 0);

	if (*sem != UINT_MAX)
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

	u32_t ipend = *pIPEND;
	if (ipend & EVT_DRIVER)	VDK_C_ISR_PostSemaphore((VDK_SemaphoreID)*sem);
	else					VDK_PostSemaphore((VDK_SemaphoreID)*sem);
}

//--------------------------------------------------------------------------
u32_t sys_arch_sem_wait(sys_sem_t* sem, u32_t timeout)
{
	u32_t		time;
	VDK_Ticks	before, after, ticks;
	
	LWIP_ASSERT("sys_arch_sem_wait: invalid semaphore", sys_sem_valid(sem));

	if (!timeout)	ticks = 0;
	else
	{
		ticks = (timeout*ticks_per_ms)>>8;
		if (ticks)	ticks |= VDK_kNoTimeoutError;
		else		ticks = VDK_kDoNotWait;
	}
	before = VDK_GetUptime();
	if (!VDK_PendSemaphore((VDK_SemaphoreID)*sem, ticks))	return SYS_ARCH_TIMEOUT;
	after = VDK_GetUptime();

	time = after - before;
	time = (time * ms_per_ticks) >> 8;
  	return time;
}

//--------------------------------------------------------------------------
void sys_sem_free(sys_sem_t* sem)
{
	LWIP_ASSERT("sys_sem_free: invalid semaphore", sys_sem_valid(sem));

	VDK_ClearThreadError();
	VDK_DestroySemaphore((VDK_SemaphoreID)*sem);
	if (VDK_GetLastThreadError() != VDK_kNoError)	return;

	SYS_STATS_DEC(sem.used);
}

//--------------------------------------------------------------------------
err_t sys_mutex_new(sys_mutex_t *mutex)
{
	*mutex = VDK_CreateMutex();

	if (*mutex != UINT_MAX)
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

	VDK_AcquireMutex((VDK_MutexID)*mutex);
}

//--------------------------------------------------------------------------
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	LWIP_ASSERT("sys_mutex_unlock: invalid mutex", sys_mutex_valid(mutex));

	VDK_ReleaseMutex((VDK_MutexID)*mutex);
}

//--------------------------------------------------------------------------
void sys_mutex_free(sys_mutex_t *mutex)
{
	LWIP_ASSERT("sys_mutex_free: invalid mutex", sys_mutex_valid(mutex));

	VDK_ClearThreadError();
	VDK_DestroyMutex((VDK_MutexID)*mutex);
	if (VDK_GetLastThreadError() != VDK_kNoError)	return;

	SYS_STATS_DEC(mutex.used);
}

//--------------------------------------------------------------------------
err_t sys_mbox_new(sys_mbox_t* mbox, int size)
{
	LWIP_ASSERT("sys_mbox_new: size is 0", size != 0);

	struct sys_mbox* m = (struct sys_mbox*) malloc(sizeof(struct sys_mbox) + size*sizeof(void*));

	LWIP_ASSERT("sys_mbox_new: new mbox not allocated", m != NULL);

	if (m)
	{
		if (sys_sem_new(&m->sem, 0) == ERR_OK)
		{
			m->size = size;
			m->count = m->read = m->write = 0;
		}
		else
		{
			free(m);
			m = NULL;
		}
	}

	if (!m)
	{
		SYS_STATS_INC(mbox.err);
		return ERR_MEM;
	}

	SYS_STATS_INC_USED(mbox);

	*mbox = m;
	return ERR_OK;
}

//--------------------------------------------------------------------------
void sys_mbox_free(sys_mbox_t* mbox)
{
	struct sys_mbox* m = *mbox;

	LWIP_ASSERT("sys_mbox_free: invalid mbox", sys_mbox_valid(mbox));

	sys_sem_free(&m->sem);
    free(m);

	SYS_STATS_DEC(mbox.used);
}

//--------------------------------------------------------------------------
err_t sys_mbox_trypost(sys_mbox_t* mbox, void *msg)
{
	sys_prot_t old_level;
	struct sys_mbox* m = *mbox;

	LWIP_ASSERT("sys_mbox_trypost: invalid mbox", sys_mbox_valid(mbox));

	old_level = sys_arch_protect();

	LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox %p msg %p\n",
                          (void *)mbox, (void *)msg));  

	if (m->count >= m->size)
	{
		sys_arch_unprotect(old_level);
		SYS_STATS_INC(mbox.err);
		LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox is full\n"));
		return ERR_MEM;
	}

	m->msgs[m->write++] = msg;
	if (m->write == m->size)	m->write = 0;
	m->count++;
	sys_sem_signal(&m->sem);

	sys_arch_unprotect(old_level);

	return ERR_OK;
}

//--------------------------------------------------------------------------
void sys_mbox_post(sys_mbox_t* mbox, void *msg)
{
	sys_prot_t old_level;
	struct sys_mbox* m = *mbox;

	LWIP_ASSERT("sys_mbox_post: invalid mbox", sys_mbox_valid(mbox));

	old_level = sys_arch_protect();

	if (m->count >= m->size)
	{
		LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox is full\n"));
		do
		{
			sys_arch_unprotect(old_level);
			VDK_Yield();
			old_level = sys_arch_protect();
		}
		while (m->count >= m->size);
	}

	m->msgs[m->write++] = msg;
	if (m->write == m->size)	m->write = 0;
	m->count++;
	sys_sem_signal(&m->sem);

	sys_arch_unprotect(old_level);
}

//--------------------------------------------------------------------------
u32_t sys_arch_mbox_fetch(sys_mbox_t* mbox, void **msg, u32_t timeout)
{
	u32_t time_taken;
	sys_prot_t old_level;
	struct sys_mbox* m = *mbox;

	LWIP_ASSERT("sys_arch_mbox_fetch: invalid mbox", sys_mbox_valid(mbox));

	time_taken = sys_arch_sem_wait(&m->sem, timeout);
	if (time_taken == SYS_ARCH_TIMEOUT)		return SYS_ARCH_TIMEOUT;

	old_level = sys_arch_protect();
	if (msg != NULL)			*msg = m->msgs[m->read];
	if (++m->read == m->size)	m->read = 0;
	m->count--;
	sys_arch_unprotect(old_level);

	return time_taken;
}

//--------------------------------------------------------------------------
u32_t sys_arch_mbox_tryfetch(sys_mbox_t* mbox, void **msg)
{
	u32_t ret;
	sys_prot_t old_level;
	struct sys_mbox* m = *mbox;

	LWIP_ASSERT("sys_arch_mbox_tryfetch: invalid mbox", sys_mbox_valid(mbox));

	old_level = sys_arch_protect();

	if (m->count > 0) 
	{
		if (msg != NULL)			*msg = m->msgs[m->read];
		if (++m->read == m->size)	m->read = 0;
		m->count--;
		ret = 0;
		sys_arch_sem_wait(&m->sem, 0);
	}
	else
	{
		ret = SYS_MBOX_EMPTY;
	}

	sys_arch_unprotect(old_level);
	return ret;
}

//--------------------------------------------------------------------------
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	struct sys_thread_params p;
	VDK_ThreadCreationBlock tcb;
	
	p.fn = thread;
	p.arg = arg;

	tcb.template_id = kDynamicThreadType;
	tcb.thread_stack_size = stacksize;
	tcb.thread_priority = (VDK_Priority) (32 - prio);
	tcb.user_data_ptr = &p;
	tcb.pTemplate = &LwIP_ThreadTemplate;
	
	return VDK_CreateThreadEx(&tcb);
}
