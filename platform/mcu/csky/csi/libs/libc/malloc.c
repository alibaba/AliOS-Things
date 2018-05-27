/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csi_config.h>
#include <umm_heap.h>
#include <string.h>
#ifndef CONFIG_KERNEL_NONE
#include <csi_kernel.h>
#endif

void *malloc(size_t size)
{
    void *ret;

#ifndef CONFIG_KERNEL_NONE
     csi_kernel_sched_suspend();
#endif

#ifdef CONFIG_KERNEL_RHINO
    ret = csi_kernel_malloc(size, NULL);
#else
    ret = mm_malloc(USR_HEAP, size, __builtin_return_address(0));
#endif

#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif

    return ret;
}

void free(void *ptr)
{
#ifndef CONFIG_KERNEL_NONE
     csi_kernel_sched_suspend();
#endif

#ifdef CONFIG_KERNEL_RHINO
    csi_kernel_free(ptr, NULL);
#else
    mm_free(USR_HEAP, ptr, __builtin_return_address(0));
#endif

#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif
}

void *realloc(void *ptr, size_t size)
{
    void *new_ptr;

#ifndef CONFIG_KERNEL_NONE
     csi_kernel_sched_suspend();
#endif

#ifdef CONFIG_KERNEL_RHINO
    new_ptr = csi_kernel_malloc(size, NULL);
#else
    new_ptr = mm_malloc(USR_HEAP, size, __builtin_return_address(0));
#endif

#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif

    if (ptr) {
        memcpy(new_ptr, ptr, size);

#ifndef CONFIG_KERNEL_NONE
     csi_kernel_sched_suspend();
#endif

#ifdef CONFIG_KERNEL_RHINO
    csi_kernel_free(ptr, NULL);
#else
    mm_free(USR_HEAP, ptr, __builtin_return_address(0));
#endif

#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif
    }

    return new_ptr;
}

void *calloc(size_t nmemb, size_t size)
{
    void *ptr = NULL;

#ifndef CONFIG_KERNEL_NONE
     csi_kernel_sched_suspend();
#endif

#ifdef CONFIG_KERNEL_RHINO
    ptr = csi_kernel_malloc(size * nmemb, NULL);
#else
    ptr = mm_malloc(USR_HEAP, size * nmemb, __builtin_return_address(0));
#endif

#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif

    if (ptr) {
        memset(ptr, 0, size * nmemb);
    }

    return ptr;
}
