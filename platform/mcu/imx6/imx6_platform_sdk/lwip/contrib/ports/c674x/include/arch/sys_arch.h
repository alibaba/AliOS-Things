/************************************************************************/
/**	\file	sys_arch.h
 *	\brief	LwIP system functions for SYSBIOS on C674x
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/
#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

/* Includes */
#include <xdc/std.h>
#include "sys_protect.h"
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/knl/semaphore.h>
#include <ti/sysbios/knl/clock.h>
#include <ti/sysbios/bios.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h>

/* Macros */

/* Types */
typedef Semaphore_Handle sys_sem_t;
typedef Mailbox_Handle sys_mbox_t;
typedef u32_t sys_thread_t;
typedef struct
{
	GateMutex_Handle	Handle;
	IArg				Key;
} sys_mutex_t;


/* Variables */

/* Functions */
#define sys_sem_valid(s) 			((s != NULL) && (*s != NULL))
#define sys_sem_set_invalid(s)		*s = NULL

#define sys_mutex_valid(m)			((m != NULL) && (m->Handle != NULL))
#define sys_mutex_set_invalid(m)	*m = NULL

#define sys_mbox_valid(m)			((m != NULL) && (*m != NULL))
#define sys_mbox_set_invalid(m)		*m = NULL

#endif // _SYS_ARCH_H_

