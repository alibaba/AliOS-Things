/**
 *	@file    psmalloc.h
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	Header for psMalloc functions.
 */
/*
 *	Copyright (c) 2013-2015 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_MALLOC
#define _h_PS_MALLOC

/******************************************************************************/
/*
*/
#ifdef PS_UNSUPPORTED_OS
	#include "matrixos.h"
#else
/******************************************************************************/
/*
*/

#include <string.h> /* memset, memcpy */

#define MATRIX_NO_POOL		(void *)0x0

/******************************************************************************/
/*
	Native memory routines
*/
#include <stdlib.h> 		/* malloc, free, etc... */

#define MAX_MEMORY_USAGE	0
#define psOpenMalloc()		0
#define psCloseMalloc()
#define psDefineHeap(A, B)
#define psAddPoolCache(A, B)
#define psMalloc(A, B)		malloc(B)
#define psCalloc(A, B, C)	calloc(B, C)
#define psMallocNoPool		malloc
#define psRealloc(A, B, C)	realloc(A, B)
#define psFree(A, B)		free(A)
#define psMemset			memset
#define psMemcpy			memcpy

typedef int32 psPool_t;


/******************************************************************************/

#endif /* !PS_UNSUPPORTED_OS */
#endif /* _h_PS_MALLOC */
/******************************************************************************/

