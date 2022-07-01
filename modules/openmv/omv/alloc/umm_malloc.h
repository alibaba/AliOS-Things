/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * UMM memory allocator.
 */
#ifndef __UMM_MALLOC_H__
#define __UMM_MALLOC_H__
#include <stdlib.h>

void umm_alloc_fail();

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "umm_malloc.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

//The MIT License (MIT)

//Copyright (c) 2015 Ralph Hempel

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

/* ----------------------------------------------------------------------------
 * umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------ */

void  umm_init_x( size_t size ); // Min of 2.5KB - Max of 640 KB.
void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
void  umm_free( void *ptr );


/* ------------------------------------------------------------------------ */

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* __UMM_MALLOC_H__ */
