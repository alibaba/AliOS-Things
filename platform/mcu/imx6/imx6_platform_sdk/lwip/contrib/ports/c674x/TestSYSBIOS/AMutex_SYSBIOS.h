/************************************************************************/
/**	\file	AMutex_SYSBIOS.h
 *	\brief	ACCESS Mutex for SYS/BIOS
 *
 *	\date	2011/11/17
 *	\author	Stephane Lesage - ATEIS
 *
 */
/************************************************************************/
#ifndef _AMUTEX_SYSBIOS_H_
#define _AMUTEX_SYSBIOS_H_

/* Includes */
#include <xdc/std.h>
#include <ti/sysbios/gates/GateMutex.h>

/* Types */
typedef GateMutex_Handle	AMutexID;

/* Macros */
#define	AMUTEX_KEY(key)		IArg key

#define AMutex_Create(lock)			lock = GateMutex_create(NULL, NULL)
#define AMutex_Lock(lock, key)		key = GateMutex_enter(lock)
#define AMutex_Release(lock, key)	GateMutex_leave(lock, key)
#define AMutex_Destroy(lock)		GateMutex_delete(&lock)

/* Externals */

/* Functions */

/* Include Guard */
#endif
