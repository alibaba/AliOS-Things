/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * ======== ffosal.c ========
 *
 * OSAL Support for FatFS using ti/dpl and RTS malloc/free
 */

#include <stddef.h>

#include <ti/drivers/dpl/SemaphoreP.h>

#include "ff.h"

typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern void *malloc(size_t size);
extern void free(void *ptr);

void *ff_memalloc(unsigned int size)
{
    return ( malloc(size) );
}

void ff_memfree(void *p)
{
    free(p);
}

#if _FS_REENTRANT

/*------------------------------------------------------------------------
 * Create a Synchronization Object for a Volume

 * This function is called in f_mount function to create a new
   synchronization object, such as semaphore and mutex. When a FALSE is
  returned, the f_mount function fails with FR_INT_ERR.

  returns: TRUE: Function succeeded, FALSE:Could not create due to any error
*/

BOOL ff_cre_syncobj (
        BYTE vol,        /* Corresponding logical drive being processed */
        _SYNC_t *sobj    /* Pointer to return the created sync object */
)
{
    BOOL ret;

    *sobj = SemaphoreP_create(1, NULL);
    ret = (*sobj != NULL) ? TRUE : FALSE;

    return ret;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a synchronization
/  object that created with ff_cre_syncobj function. When a FALSE is
/  returned, the f_mount function fails with FR_INT_ERR.
/  returns: TRUE: Function succeeded, FALSE: Could not delete due to any error
*/

BOOL ff_del_syncobj (
        _SYNC_t sobj  /* Sync object tied to the logical drive to be deleted */
)
{
    SemaphoreP_delete((SemaphoreP_Handle)sobj);

    return TRUE;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a FALSE is returned, the file function fails with FR_TIMEOUT.
/
/  returns:  TRUE: Got a grant to access the volume, FALSE: Could not get a
/            grant
*/

BOOL ff_req_grant (
        _SYNC_t sobj        /* Sync object to wait */
)
{
    if (SemaphoreP_pend((SemaphoreP_Handle)sobj, _FS_TIMEOUT) == SemaphoreP_OK) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
        _SYNC_t sobj        /* Sync object to be signaled */
)
{
    SemaphoreP_post((SemaphoreP_Handle)sobj);
}

#endif
