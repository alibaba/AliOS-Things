/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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
 *  ======== errno.h ========
 */

#ifndef os_freertos_posix_errno__include
#define os_freertos_posix_errno__include

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EPERM
#define EPERM        1  /* Do not have permission to do the operation  */
#endif

#ifndef EAGAIN
#define EAGAIN       11  /* Semaphore not available, MQueue Full */
#endif

#ifndef ENOMEM
#define ENOMEM       12 /* Cannot allocate memory */
#endif

#ifndef EBUSY
#define EBUSY        16  /* Device busy */
#endif

#ifndef EINVAL
#define EINVAL       22  /* Invalid argument */
#endif

#ifndef ENOSPC
#define ENOSPC       28  /* No space left on device */
#endif

#ifndef EDEADLK
#define EDEADLK      45 /* Deadlock condition  */
#endif

#ifndef ENOSYS
#define ENOSYS       88 /* Function not implemented */
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT    116 /* Timed out waiting for resource */
#endif

#ifndef EFREERTOS
#define EFREERTOS    2001 /* FreeRTOS function failure */
#endif

#ifdef __cplusplus
}
#endif

#endif  /* os_freertos_posix_pthread__include */
