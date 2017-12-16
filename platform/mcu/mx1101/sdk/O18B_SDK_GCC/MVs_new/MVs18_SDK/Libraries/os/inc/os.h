////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:os.h
//		Description	:
//					as a bridge os.h will export a series of OS API functions
//					to APP,also other macros definition will be available for shortcut.
//            maintainer: Halley
//
//		Changelog	:
//					2012-02-21	Borrow from the DEMO sdk project and do some modification for O18 platform by Robert
//					2013-08-8	Add the task priority set interface by Robert
//					2013-08-14	Add the MMM ioctl interface for HF by Robert
//					2013-12-13	Add the scheduler suspend/resume interface by Robert
//					2015-02-10	Correct comments & beautify code style at last by Robert
///////////////////////////////////////////////////////////////////////////////
#ifndef		__OS_H__
#define		__OS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//compiler standard header file
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

//common header applied to all
#include	"type.h"
#include	"init.h"
#include	"timeout.h"
#include 	"list.h"
#include	"locks.h"
#include	"msgq.h"
#include	"semaphore.h"
#include 	"errno.h"
#include 	"freertoscfg.h"
#include	"timeout.h"
#include	"chip_info.h"
#include	"mm.h"
#include	"task.h"
#ifdef		CFG_SHELL_DEBUG
#include	"shell.h"
#endif		//CFG_SHELL_DEBUG

/*
 * export some important OS API to APP
 */
//[KERNEL API]
/*
 * DESCRIPTION
 *   The OSStartKernel() function initialize software OS kernel and hardware core and peripherals,including but not limit SYSTICK,IRQ,CACHE,DELAY
 * and so on.All the functionality modules will be initialized serially in descending priority order by .machX.init definition.
 * The priorities can be divided into core level,architecture level,driver level and some other peripherals level.see more details to os/init.h
 *
 * RETURN VALUE
 *   On success,OSStartKernel() returns 0;Otherwise a negative counter as function call order will be returned when fails occurs
 * ERRORS
 *	 EXXXX,maybe possible.
 */
extern int start_kernel(void);
#define /*int*/ OSStartKernel(/*void*/)	start_kernel()

/*
 * DESCRIPTION
 *   The OSDOWN() and OSDOWN_INTERRUPTIBLE() function decrements(locks) the semaphore pointed to by
 * Seam.As the operation done will incurs two results as following:
 *	 1>If the semaphore's value is greater than zero,then the decrement proceeds,and the function returns,immediately.
 *   2>If the semaphore's value is equal to or less than zero,then one task requiring the semaphore will be blocked until
 * the semaphore becomes possibly to perform the decrement operation for other task has been released semaphore and woken up suspend task.
 *   Pay a little attention to OSDOWN_INTERRUPTIBLE(),the task will be interrupted or woken up by some events,such a alarm or other message 
 * when the semaphore is not available for the task currently.
 *   OSDOWN_TRYLOCK()is the same as OS_DOWN(),except that if the decrement cannot be immediately performed,then call returns an error 
 * (errno set to EAGAIN) instead of blocked.
 *
 * RETURN VALUE
 *   OSDOWN() functions return nothing on success, on the contrary, OSDOWN_INTERRUPTIBLE() &
 * OSDOWN_TRYLOCK() return 0 on success; on error, the value of the semaphore's value is left unchanged,
 * -1 or others is returned, and errno is set to indicate the error that has occurs.
 *
 * ERRORS
 *   EINTR  The call was interrupted by other task message notification or timeout occurrence
 *   EINVAL The Seam is a invalid semaphore
 *   EAGAIN The operation could not be performed without blocked(i.e.,the semaphore currently has the value zero),for OSDOWN_TRYLOCK() only.
 */
#define	OSDOWN(/*SEMAPHORE*/ Seam)		__OSDOWN(/*SEMAPHORE**/ &Seam)
#define	OSDOWN_INTERRUPTIBLE(/*SEMAPHORE*/ Seam)	__OSDOWN_INTERRUPTIBLE(/*SEMAPHORE**/ &Seam)
#define	OSDOWN_TRYLOCK(/*SEMAPHORE*/ Seam)	__OSDOWN_TRYLOCK(/*SEMAPHORE**/ &Seam)

/*
 * DESCRIPTION
 *   The OSUP() increments (unlocks) the semaphore pointed to by Seam.If the semaphore's value
 * consequently becomes greater than zero, then  another process  blocked in  a OSDOWN() or
 * OSDOWN_INTERRUPTIBLE() call will be woken up and proceed to lock the semaphore.
 * RETURN VALUE
 * 	NONE
 */
#define OSUP(/*SEMAPHORE*/ Seam)	__OSUP(/*SEMAPHORE**/ &Seam)

/*
 * DESCRIPTION
 *	 FIXME ???
 * RETURN VALUE
 *   FIXME ???
 * ERRORS
 *   FIXME ???
 */
#define OSATOMIC_SET(/*ATOMIC*/V,/*int*/I)	__ATOMIC_SET(/*ATOMIC**/&V,/*int*/I)
#define OSATOMIC_ADD_RETURN(/*int*/I,/*ATOMIC*/V)  __ATOMIC_ADD_RETURN(/*int*/I,/*ATOMIC**/&V)
#define OSATOMIC_SUB_RETURN(/*int*/I,/*ATOMIC*/V)  __ATOMIC_SUB_RETURN(/*int*/I,/*ATOMIC**/&V)
#define OSATOMIC_CLEAR_MASK(/*unsigned int*/Mask,/*unsigned int*/Addr) __ATOMIC_CLEAR_MASK(/*unsigned int*/Mask,/*unsigned int*/Addr)

/*
 * DESCRIPTION
 *	 FIXME ???
 * RETURN VALUE
 *   FIXME ???
 * ERRORS
 *   FIXME ???
 */
#define	OSSPIN_LOCK(/*SPINLOCK*/Lock)      __SPIN_LOCK(/*SPINLOCK**/&Lock)
#define	OSSPIN_TRYLOCK(/*SPINLOCK*/Lock)   __SPIN_TRYLOCK(/*SPINLOCK**/&Lock)
#define	OSSPIN_UNLOCK(/*SPINLOCK*/Lock)    __SPIN_UNLOCK(/*SPINLOCK**/&Lock)

#define	OSSPIN_LOCK_IRQ(/*SPINLOCK*/Lock)                __SPIN_LOCK_IRQ(/*SPINLOCK**/&Lock)
#define	OSSPIN_UNLOCK_IRQ(/*SPINLOCK*/Lock)              __SPIN_UNLOCK_IRQ(/*SPINLOCK**/&Lock)

#define	OSSPIN_LOCK_IRQSAVE(/*SPINLOCK*/Lock,/*unsigned int*/Flags)      __SPIN_LOCK_IRQSAVE(/*SPINLOCK**/&Lock,/*unsigned int*/Flags)
#define	OSSPIN_UNLOCK_IRQRESTORE(/*SPINLOCK*/Lock,/*unsigned int*/Flags) __SPIN_UNLOCK_IRQRESTORE(/*SPINLOCK**/&Lock,/*unsigned int*/Flags)

/*
 * DESCRIPTION
 *	 FIXME ???
 * RETURN VALUE
 *   FIXME ???
 * ERRORS
 *   FIXME ???
 */
#define	OSREAD_LOCK(/*RWLOCK*/Lock)      __READ_LOCK(/*RWLOCK**/&Lock)
#define	OSREAD_TRYLOCK(/*RWLOCK*/Lock)   __READ_TRYLOCK(/*RWLOCK**/&Lock)
#define	OSREAD_UNLOCK(/*RWLOCK*/Lock)    __READ_UNLOCK(/*RWLOCK**/&Lock)

#define	OSREAD_LOCK_IRQ(/*RWLOCK*/Lock)    __READ_LOCK_IRQ(/*RWLOCK**/&Lock)
#define	OSREAD_UNLOCK_IRQ(/*RWLOCK*/Lock)  __READ_UNLOCK_IRQ(/*RWLOCK**/&Lock)

#define	OSWRITE_LOCK(/*RWLOCK*/Lock)       __WRITE_LOCK(/*RWLOCK**/&Lock)
#define	OSWRITE_TRYLOCK(/*RWLOCK*/Lock)    __WRITE_TRYLOCK(/*RWLOCK**/&Lock)
#define	OSWRITE_UNLOCK(/*RWLOCK*/Lock)     __WRITE_UNLOCK(/*RWLOCK**/&Lock)

#define	OSWRITE_LOCK_IRQ(/*RWLOCK*/Lock)      __WRITE_LOCK_IRQ(/*RWLOCK**/&Lock)
#define	OSWRITE_UNLOCK_IRQ(/*RWLOCK*/Lock)    __WRITE_UNLOCK_IRQ(/*RWLOCK**/&Lock)

#define	OSREAD_LOCK_IRQSAVE(/*RWLOCK*/Lock,/*unsigned int*/Flags)    \
		  					__READ_LOCK_IRQSAVE(/*RWLOCK**/&Lock,/*unsigned int*/Flags)
#define	OSREAD_UNLOCK_IRQRESTORE(/*RWLOCK*/Lock,/*unsigned int*/Flags)  \
		  					__READ_UNLOCK_IRQRESTORE(/*RWLOCK**/&Lock,/*unsigned int*/Flags)

#define	OSWRITE_LOCK_IRQSAVE(/*RWLOCK*/Lock,/*unsigned int*/Flags) \
							__WRITE_LOCK_IRQSAVE(/*RWLOCK**/&Lock,/*unsigned int*/Flags)
#define	OSWRITE_UNLOCK_IRQRESTORE(/*RWLOCK*/Lock,/*unsigned int*/Flags)  \
							__WRITE_UNLOCK_IRQRESTORE(/*RWLOCK**/&Lock,/*unsigned int*/Flags)

//[QUEUE API]
/*
 * DESCRIPTION
 *   The OSQueueMsgSend() function shall add the message which type is specified by Msg and which body
 * is pointed to the argument MsgBody into the message queue in system.The MsgLen argument specifies
 * the length  of  the  message, in bytes, pointed to by MsgBody.If send a message without message body,
 * then the MsgBody can be set to NULL and MsgLen is set to zero.
 *   And also the MsgBody pointer can be pointed to a block of memory derived from user own memory and the MsgLen is set to 4 .
 * this can be used as memory share between tasks,and user will control memory operation by her or him self,such as memory allocation or free.
 *   If the memory resource is sufficient,OSQueueMsgSend() shall behave as if the message allocated dynamically from memory is inserted
 * into the message queue at the position indicated by the Prio argument.A message with a larger
 * numeric value of Prio shall be inserted before messages with lower values of Prio by priority.Otherwise,a
 * message shall be inserted after other messages in the queue, if any,with equivalent to or less than Prio.The value of
 * Prio shall be less than MQ_PRIO_MAX macro.
 *   Note:OSQueueMsgSend() can be applied to process context and interrupt context to notify one or more tasks that message is coming.
 *
 * RETURN VALUE
 *   Upon successful completion,OSQueueMsgSend() function shall return a value of zero.
 *   Otherwise no message shall be enqueued, the functions shall return a negative value to indicate of cause of error.
 *
 * ERRORS
 *   ESCHR  Nobody require the message and OS discard this message forcedly
 *   ENOMEM Message too big to allocate from insufficient memory
 */
#define /*int*/OSQueueMsgSend(/*unsigned short*/Msg, /*void**/MsgBody, /*int*/MsgLen, /*char*/Prio, /*cha*/Sync)\
				  (!__get_IPSR() ? msgq_msg_send(Msg, MsgBody, MsgLen, Prio, Sync) \
								 : msgq_msg_send_fromisr(Msg, MsgBody, MsgLen, Prio))
/*
 * DESCRIPTION
 *   The OSQueueMsgRecv() function shall receive one the oldest and the highest priority message belong to current task from
 * the message queue.If the size of the buffer in bytes,referred by the MsgLen argument,is less
 * than the length of message in queue,the function shall fail and return an error,Otherwise,the
 * selected message shall be removed from the queue and copied message datum (if any) to the buffer pointed to the MsgBody
 * argument if incoming message contains message body and store the message body length to MsgLen point variable.
 *	 If the message belonged to current task is absent of message queue and the WaitTime is set to 0,function return
 * directly with error code.On the contrary,if WaitTime is set to some a value and the requested
 * message(s) is not available,OSQueueMsgRecv() shall block until a message is enqueued onto the message queue or timeout occurs.
 *   Note:In interrupt context,calling the OSQueueMsgRecv will be ignored and returned a ENOSYS error code.
 *        The timeout value WaitTime can be accurate in one millisecond.
 *
 * RETURN VALUE
 *   Upon successful completion,OSQueueMsgRecv() function shall return a value of message type.
 * Otherwise,no message shall be enqueued, the functions shall return with an error code.
 *
 * ERRORS
 *   ETIME  No message for this task is available.
 */
#define /*int*/OSQueueMsgRecv(/*void**/MsgBody, /*int**/MsgLen, /*int*/WaitTime)\
						(!__get_IPSR() ? msgq_msg_recv(MsgBody, MsgLen, WaitTime) : -ENOSYS)
/*
 * DESCRIPTION
 *   The OSQueueMsgIOCtl() function shall process user command referred to Cmd with command's option
 * specified by Opt.The command's name is like MSGQ_IOCTL_XXX_YYYY which are defined as below.If the
 * command is not realized currently,OSQueueMsgIOCtl function will return a error code ENOSYS.
 *
 * RETURN VALUE
 *   Upon successful completion,OSQueueMsgIOCtl() function shall return zero or command result mainly depended on command type.
 * Otherwise,the command can not be executed and return associated error number.
 *
 * ERRORS
 *   ENOSYS Function functionality has not been realized
 */
enum
{
    MSGQ_IOCTL_FILL_MSGSET = 1,	/* register all the message class to current task */
    MSGQ_IOCTL_PEEK_TASKMSG,	/* peek current task message count */
    MSGQ_IOCTL_GET_MSGSET,		/* get current task message class set */
    MSGQ_IOCTL_GET_ALTMSGSET,	/* get current alternative message class set */
    MSGQ_IOCTL_SET_MSGSET,		/* set current message class set */
    MSGQ_IOCTL_SET_ALTMSGSET,	/* set current alternative message class set */
    MSGQ_IOCTL_START_MQTMR,		/* depricated??? */
    MSGQ_IOCTL_STOP_MQTMR,		/* depricated??? */
    MSGQ_IOCTL_DEL_PENDMSG,		/* delete a class pending message */
    MSGQ_IOCTL_DEL_SPECMSG,		/* delete a special pending message */
    MSGQ_IOCTL_SET_MSGTMO,		/* set all the message timeout value */
};
#define /*int*/OSQueueMsgIOCtl(/*int*/Cmd, /*unsigned int*/Opt)	msgq_msg_ioctl(Cmd, Opt)


//[TASK API]
/*
 * Type by which tasks are referred.For example,a call to OSTaskCreate() returns (via a pointer
 * parameter) an xTaskHandle variable that can then be used as a parameter to vTaskDelete to
 * delete the task.
 */
//typedef void* xTaskHandle;

/*
 * DESCRIPTION
 *   OSschedYield() causes the calling task to relinquish the CPU.The task is moved to the end
 * of the queue for its static priority and a new task gets to run if necessary.
 *   NOTE:If the calling task is the only task in the highest priority list at that time, it
 * will continue to run after a call to OSscheduleYield().
 *
 * RETURN VALUE
 *   OSscheduleYield() returns 0 for ever after OS re-schedule.
 */
#define /*int*/OSScheduleYield(/*void*/)		schedule_yield()

/*
 * DESCRIPTION
 *	 The OSTaskCreate() function shall create a new task,with task name specified by TaskName,
 * stack size specified by TaskStackSize,target task's parameter specified by TaskPara,task
 * priority specified by TaskPri.
 *   Upon successful completion, OSTaskCreate() shall store the ID of  the  created task in the
 * location referred by TaskHandler.
 *   The task is created executing TaskEntry with TaskPara as its sole argument.If the TaskEntry
 * returns,the effect shall be as if there was an implicit call to OSTaskExit() using the return 
 * value of OSTaskExit as the exit status.
 *
 * RETURN VALUE
 *   If successful, the OSTaskCreate() function shall return TRUE; otherwise, an error number -1
 *   shall be returned to indicate the error.
*/
#define /*int*/OSTaskCreate(/*void(*)(void*)*/TaskEntry, /*const unsigned char**/TaskName,\
					/*unsigned short*/TaskStackSize, /*void**/TaskPara,\
							/*unsigned int*/TaskPri, /*xTaskHandle**/TaskHandler) \
						xTaskCreate(TaskEntry, TaskName, TaskStackSize, TaskPara, TaskPri, TaskHandler)
/*
 * DESCRIPTION
 *   The OSTaskExit() function shall terminate the calling task.Any resource cleaning action or
 * handler are executed in the reverse order.Task termination does not release any application
 * visible task resources,including,but not limited to,stack,mutex and so on clean up actions.
 *   If task handle TaskHandler is set to NULL,OS will terminate calling task,otherwise OS will
 * terminate the specific task referred by task handler TaskHandler.
 *
 * RETURN VALUE
 *   The OSTaskExit() function cannot return to its caller.
 * ERRORS
 *   No errors are defined.
 */
#define /*int*/OSTaskExit(/*xTaskHandle*/TaskHandler)	vTaskDelete(TaskHandler)

/*
 * DESCRIPTION
 *   The OSTaskSleep() function shall cause the current task to be suspended from executing until
 * the time interval specified by the ExpireTime argument has elapsed.
 *   The suspension time may be longer than requested because argument ExpireTime is rounded up to
 * an integer multiple of the sleep resolution(1000/HZ) or because of the scheduling of other activity by the system.
 *
 * RETURN VALUE
 *   If the OSTaskSleep() function returns 0 for ever.
 */
#define /*int*/OSTaskSleep(/*int*/ExpireTime) vTaskSleep(ExpireTime)

/*
 * DESCRIPTION
 *   The OSSchedulerSuspend() function will suspend scheduler to stop OS task schedule,all tasks but
 * except itself will be suspend until the suspend action is cancelled.During the suspend time,the
 * ticks elapsed of other tasks will be supplemented before resuming again.
 *
 * RETURN VALUE
 *   None
 * ERRORS
 *   No errors are defined.
 */
#define /*void*/OSSchedulerSuspend(/*void*/)	vTaskSuspendAll()

/*
 * DESCRIPTION
 *   The OSSchedulerResume() function will put all the suspended tasks back to ready queue from
 * pending queue and restore their status.If the ticks elapse during the suspend time,the lost ticks
 * will be added to their exhausting time.
 *
 ** RETURN VALUE
 *   None
 * ERRORS
 *   No errors are defined.
 */
#define /*void*/OSSchedulerResume(/*void*/)	(void)xTaskResumeAll()

/*
 * DESCRIPTION
 *   The OSTaskSuspend() function shall cause the task specified by TaskHandler to be suspended
 * until the task is resumed calling OSTaskResume() by another task or handler.
 * If argument TaskHandler equals to NULL,the current task itself will be suspended.
 *
 * RETURN VALUE
 *   None
 * ERRORS
 *   No errors are defined.
 */
#define /*void*/OSTaskSuspend(/*xTaskHandle*/TaskHandler)	vTaskSuspend(TaskHandler)

/*
 * DESCRIPTION
 *   The OSTaskResume() function resume the task referred by TaskHandler which has be put onto
 * suspend list by previous calling OSTaskSuspend() to be resumed if possible.If success,the OS
 * will put the task onto ready list preparing to be scheduled by scheduler if permitted.
 *   The OSTaskResumeFromISR() function perform the same functionality as the OSTaskResume do
 * except for calling from interrupt context.
 *
 * RETURN VALUE
 *   It makes no sense to resume caller itself if parameter TaskHandler equals to NULL or
 * current task handler;At the same time,it is no use to resuming unsuspended task too.
 * ERRORS
 *	 EINVAL The argument TaskHandler is invalid,maybe equals to NULL or itself
 *	 EBUSY  The task referred by TaskHandler is not in suspend state or indefinite waiting state.
 */
#define /*int*/OSTaskResume(/*xTaskHandle*/TaskHandler)	vTaskResume(TaskHandler)
#define /*int*/OSTaskResumeFromISR(/*xTaskHandle*/TaskHandler)	xTaskResumeFromISR(TaskHandler)

/*
 * DESCRIPTION
 *   The OSStartSchedule() function take some steps to make preparation for system schedule,
 * including,but not limit to start system tick timer,enable interrupt(especial systick),re-spawn
 * idle task and evolve the init-task and so on.
 *
 * RETURN VALUE
 *   None
 * ERRORS
 *   No errors are defined.
 */
#define /*void*/OSStartSchedule(/*void*/)	vTaskStartScheduler()

/*
 * DESCRIPTION
 *   The OSRescheduleTimeout() function yield the CPU to other tasks for about TimeOut time in
 * millisecond friendly if no message wake up the task,in other hand,it maybe waken up early
 * before timeout occurs because of message interrupt.
 *
 * RETURN VALUE
 *	 return 0 forever
 * ERRORS
 * 	 None
 */

extern int schedule_timeout(unsigned int timeout);
#define /*int*/OSRescheduleTimeout(/*int*/TimeOut)	schedule_timeout(TimeOut)

/*
 * DESCRIPTION
 *   The OSTaskPrioSet() function set the task referred by TaskHandler as its newly priority referred by
 * NewPrio argument.The task maximum priority range from CFG_MIN_PRIORITIES	to [CFG_MAX_PRIORITIES - 1],
 * and if the task sets itself priority,just replace the TaskHandler with NULL only.
 *
 * RETURN VALUE
 *	 None
 */
#define /*void*/OSTaskPrioSet(/*xTaskHandle*/TaskHandler, /*unsigned int*/NewPrio) vTaskPrioritySet(TaskHandler, NewPrio)

/*
 * 	 System tick counter,4 bytes size capacity can afford as long time as about 500 days if HZ is set to
 * 100.The OS or APP get system tick by referring this wrapper function indirectly.If the timer wrap
 * occurs,the jiffies will be reset,to prevent erroneous comparability from timer wrap during this
 * period, I strongly recommend you to use TIME_BEFORE(), TIME_AFTER() function alternately, e.g.
 * please refer to time.h.
 *   OSSysTickGet() return system tick timer counter which has elapsed from reset(SYSTERMRESET,POR or OTHERS)
 *   OSSysAuxtmrGet() return system tick timer 1 millisecond counter which has elapsed from reset(SYSTERMRESET,POR or OTHERS)
 */
#define /*unsigned int*/OSSysTickGet(/*void*/)		xTaskGetTickCount()
#define /*unsigned int*/OSSysAuxtmrGet(/*void*/)	auxtmr_count_get()


//[MMM API]
/*
 * DESCRIPTION:
 *	 The OSMalloc() function shall allocates memory by Size in byte with option Opt as below and returns a pointer to the allocated memory.
 * The allocated memory is not guaranteed that a clean memory will be allocated for user.
 *	 If Size is zeor or negative, then OSMalloc() returns NULL with errno set to -EINVAL
 *	 If insufficient memory for request to allocate,OSMalloc() returns NULL with errno set to -ENOMEM
 *   In addition to argument Size,there is also another argument Opt which is defined as:
 */
#define		MMM_ALLOC_NORMAL		(1 << 0)  	/* if request memory use as a normal,the OS will allocate it from low memory */
#define		MMM_ALLOC_FAST			(1 << 1)  	/* if request memory will be free quickly,the OS do it as MMM_ALLOC_NORMAL do */
#define		MMM_ALLOC_TASK			(1 << 2)	/* if request memory will use as long time as task life and won't be free for a while,the OS will allocate the high memory for it */
#define		MMM_ALLOC_BOOTUP		(1 << 3)	/* if request memory used as bootup memory,it won't be free unless reboot */
#define		MMM_ALLOC_MERGE			(1 << 4) 	/* internal use only */
#define		MMM_ALLOC_PREALLOC		(1 << 5) 	/* internal use only */
 /*
 * RETURN VALUE:
 *    For OSMalloc(),return a pointer to be allocated memory,which is suitably aligned for any kind
 *  of variable. On error, this function return NULL and the errno is set as ENOMEM.
 * ERRORS
 * 	 ENOMEM Lack of memory 
 */
#define	/*void**/ OSMalloc(/*unsigned int*/ Size, /*unsigned char*/ Opt)	mmm_malloc(Size, Opt)

/*
 * DESCRIPTION:
 *   OSRealloc() changes the size of the memory block pointed to by Ptr to size in byte.The
 * contents will be unchanged to the minimum of the old and new sizes;newly allocated memory will be uninitialized.
 * 1>If Ptr is NULL, then the call is equivalent to  OSMalloc(Size), for all values of size.
 * 2>If Size is equal to zero, and Ptr is not NULL, then the call is equivalent to free(Ptr).
 *   Unless Ptr is NULL, it must have been returned by an earlier call to OSMalloc().
 * 3>If the area pointed to was moved, a free(ptr) is done before the newly memory allocate.
 *
 * RETURN VALUE:
 *   see OSMalloc()
 */
#define	/*void**/ OSRealloc(/*void**/Ptr, /*unsigned int*/ Size, /*unsigned char*/ Opt)	mmm_realloc(Ptr, Size, Opt)


/*
 * DESCRIPTION:
 *   The OSFree() function free the memory space pointed to Ptr which must have been returned by a previous  call to OSMalloc() or OSRealloc(). 
 * Otherwise, or if OSFree(ptr) has already been called before,undefined behavior will occurs.
 *   If Ptr is NULL or invalid, no operation is performed as expect.
 *   If Ptr multi-free for more than one time,OS will throw a warning "Multi-free"
 *
 * RETURN VALUE:
 *	 None
 */
#define	/*void*/ OSFree(/*void**/Ptr)	mmm_free(Ptr)

/*
 * DESCRIPTION:
 *   The OSMMMIoCtl() function issue command to control system memory,if the command referred by Cmd require
 * the associated argument or option,pass the argument indicated of Arg to the second parameter.
 *   Upon successful completion,OSMMMIoCtl() function shall return the output of the command result value or zero.
 * Otherwise,the command can not be executed and return associated error number for user.
 */
#define	MMM_IOCTL_GETMAX_BLKSZ		(1)	/* command with argument MMM_ALLOC_NORMAL or MMM_ALLOC_BOOTUP or both,return a maximum sole free block */
#define	MMM_IOCTL_GETSZ_INALL		(2) /* command without argument return current free size in all blocks */
#define	MMM_IOCTL_GETMAXSZ_FREE		(3) /* command without any argument return maximum free size in history*/
#define	MMM_IOCTL_GETMINSZ_FREE		(4) /* command without any argument return minimum free size in history*/
/*
 * RETURN VALUE:
 *   On successful return,a command result will be returned,otherwise,a error number will be returned.
 * ERRORS
 *   ENOSYS Function command has not been realized yet.
 */

int mmm_ioctl(unsigned long Cmd, unsigned long Arg);
#define	OSMMMIoCtl(/*unsigned int*/Cmd, /*unsigned int*/Arg)	mmm_ioctl(Cmd, Arg)

//[DRIVER API]
//[[SERIAL]]
/*
 * DESCRIPTION
 *   The OSDBGUartInit() function initialize the serial communication port specified by Port at the
 * speed of BaudRate with port attribution Opt.In default,the serial port will be configurated  as
 * 115200,8-N-1.
 *   There is a default serial port numbered 0 to be used as system debug serial port,Also it
 * configured with 115200,8-N-1 setting.So do not configure port 0 again if you do not intend to, in
 * order to avoid no debug message output.
 *
 * RETURN VALUE
 *   If the OSDBGUartInit() function initialize the requested serial port successfully,it will return
 * 0,otherwise it will return -1 and set the errno to indicate the error.
 * ERRORS
 *   The OSDBGUartInit() function shall fail if:
 *   ENODEV  The requested serial port is not available.
 *   EINVAL	 The parameters including BaudRate & Opt are not set correctly.
 *   ENOSYS	 Serial port function have not realized at present.
 */
#define /*int*/OSDBGUartInit(/*unsigned int*/BaudRate, /*unsigned char*/DatumBits, /*unsigned char*/Parity, /*unsigned char*/StopBits)\
	FuartInit(BaudRate, DatumBits, Parity, StopBits)

//[MISC]
/*
 * DESCRIPTION
 *   The OSTaskMSDelay() and OSTaskUSDelay function,both of all are used to realize busy delay in a
 * short time specified by MS or US.During the short busy waiting time,the task or handler does not
 * want to yield CPU,but there is nothing to do except waiting.Pay a attention to their arguments,
 * Calling function OSTaskMSDelay() with argument value must be in the range from 0 to 999 in
 * millisecond,similarly,the argument of OSTaskUSDelay() must satisfy this range but in microsecond.
 * So if the delay time in microsecond is greater than 999,you can call OSTaskMSDelay alternately,
 * similarly,if the delay time in millisecond is greater than 1000/HZ,you had better call function
 * OSRescheduleTimeout() alternately for better system performance.
 *   NOTE:It is no guarantee that there must isn't task switch by scheduler if the length of delay
 *
 * RETURN VALUE
 *	 In most of cases,calling of them will not fall into fail except the range of argument value mentioned above.
 *   If error occurs in case,it return not zero and set the errno to ERANGE.
 * ERRORS
 *   ERANGE	the argument MS or US is out of the range which it can handle
 */
#define OSTaskMSDelay(/*unsigned int*/ms)		DelayMs(/*unsigned int*/ms)
#define OSTaskUSDelay(/*unsigned int*/us)		DelayUs(/*unsigned int*/us)

/*
 * DESCRIPTION
 *   These functions in the printf() family produce output according to a format.The functions
 * OSprintf() write output to stdout, the standard  output stream; OSsprintf(), OSsnprintf() write to
 * the character string str.
 *   The functions OSsnprintf() write at most size bytes (including the trailing null byte ('\0'))
 * to str.
 *   These functions write the output under the control of a format string that specifies how
 * subsequent arguments (or arguments accessed via the variable-length  argument facilities of
 * stdarg(3)) are converted for output.
 *   C99 and POSIX.1-2001 specify that the results are undefined if a call to sprintf(), snprintf(),
 * would cause copying to take place between objects that overlap(e.g.,if the target string array
 * and one of the supplied input arguments refer to the same buffer).
 *
 * RETURN VALUE
 *   Upon successful return, these functions return the number of characters printed(not including
 * the trailing '\0' used to end output to strings).
 *   The functions snprintf() do not write more than size bytes (including the trailing '\0').If
 * the output was truncated due to this limit then the  return value is the number of characters
 * (not including the trailing '\0') which would have been written to the final string if enough
 * space had been available.Thus,a return value of size or more means that the output was
 * truncated.
 *   If an output error is encountered,a negative value is returned.
 */
#define /*int*/ OSPrintf(/*const char**/ Format, ...) mvprintf(Format, ##__VA_ARGS__)
#define /*int*/ OSSprintf(/*char**/ Buf, /*const unsigned char**/ Format, ...) sprintf(Buf, Format, ##__VA_ARGS__)
#define /*int*/ OSSnprintf(/*char**/ Buf, /*unsigned int*/ Cnt, /*const char**/ Format, ...)	snprintf(Buf, Cnt, Format, ##__VA_ARGS__)

const unsigned char *GetLibVersionFreertos(void);
extern void OsSetDebugFlag(unsigned char Flag);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//_OS_H_


