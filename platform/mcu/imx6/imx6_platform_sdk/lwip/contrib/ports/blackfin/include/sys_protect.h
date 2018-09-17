/************************************************************************/
/**	\file	sys_protect.h
 *	\brief	LwIP protection functions for VDK on blackfin
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _SYS_PROTECT_H_
#define _SYS_PROTECT_H_

/* Includes */
#include <blackfin.h>

/* Macros */
#define EVT_DRIVER	(EVT_IVG7|EVT_IVG11)
#define EVT_PROTECT	(EVT_DRIVER|EVT_IVG14|EVT_IVTMR)

/* Types */
typedef u32_t sys_prot_t;

/* Variables */

/* Functions */
inline sys_prot_t sys_arch_protect(void)		{	sys_prot_t old=cli(); sti(old&(~EVT_PROTECT)); return old;	}
inline void sys_arch_unprotect(sys_prot_t val)	{	sti(val);		}

#endif // _SYS_PROTECT_H_

