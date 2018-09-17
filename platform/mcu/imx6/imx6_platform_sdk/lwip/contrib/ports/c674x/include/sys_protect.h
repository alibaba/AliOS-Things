/************************************************************************/
/**	\file	sys_protect.h
 *	\brief	LwIP protection functions for SYSBIOS on C674x
 *
 *	\date	2011/10/07
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/
#ifndef _SYS_PROTECT_H_
#define _SYS_PROTECT_H_

/* Includes */
#include <ti/pspiom/cslr/cslr_chip.h>
#include <c6x.h>
#include "arch/cc.h"

/* Macros */
#define IER_DRIVER		(CSL_CHIP_IER_IE09_MASK | CSL_CHIP_IER_IE10_MASK | CSL_CHIP_IER_IE11_MASK)
#define IER_BIOSTIMER	(CSL_CHIP_IER_IE14_MASK)
#define IER_PROTECT		(IER_DRIVER|IER_BIOSTIMER)

/* Types */
typedef u32_t sys_prot_t;

/* Variables */

/* Functions */
inline sys_prot_t sys_arch_protect(void) {
											sys_prot_t old = IER;
											IER &= ~IER_PROTECT;
											return old;
										 }

inline void sys_arch_unprotect(sys_prot_t val)	{	IER = val;	}

#endif // _SYS_PROTECT_H_
