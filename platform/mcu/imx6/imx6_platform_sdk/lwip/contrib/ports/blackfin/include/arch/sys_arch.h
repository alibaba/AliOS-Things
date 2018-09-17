/************************************************************************/
/**	\file	sys_arch.h
 *	\brief	LwIP system functions for VDK on blackfin
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

/* Includes */
#include <blackfin.h>
#include "sys_protect.h"

/* Macros */

/* Types */
typedef int sys_sem_t;
typedef int sys_mutex_t;
typedef u32_t sys_thread_t;
typedef struct sys_mbox* sys_mbox_t;

/* Variables */

/* Functions */
#define sys_sem_valid(s) 			((s != NULL) && (*s != -1))
#define sys_sem_set_invalid(s)		*s = -1

#define sys_mutex_valid(m)			((m != NULL) && (*m != -1))
#define sys_mutex_set_invalid(m)	*m = -1

#define sys_mbox_valid(m)			((m != NULL) && (*m != NULL))
#define sys_mbox_set_invalid(m)		*m = NULL

#endif // _SYS_ARCH_H_

