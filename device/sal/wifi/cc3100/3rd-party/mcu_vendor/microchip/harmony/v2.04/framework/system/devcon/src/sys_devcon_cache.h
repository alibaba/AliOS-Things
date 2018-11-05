/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/

#ifndef __PIC32_CACHE_H
#define __PIC32_CACHE_H

#ifndef _CP0_ERRCTL
#define _CP0_ERRCTL                         $26, 0
#endif

#ifndef _CP0_TAGLO
#define _CP0_TAGLO                          $28, 0
#endif

/*
 * Cache operations
 */

#define Index_Store_Tag_I                0x08        /* 2       0 */
#define Index_Store_Tag_D                0x09        /* 2       1 */

#define Hit_Writeback_Inv_D              0x15        /* 5       1 */
#define Hit_Invalidate_I                 0x10        /* 4       0 */
#define Hit_Writeback_Inv_S              0x17        /* 5       3 */
#define Hit_Invalidate_D                 0x11        /* 4       1 */
#define Index_Invalidate_I               0x00        /* 0       0 */
#define Index_Writeback_Inv_D            0x01        /* 0       1 */
#define Fetch_Lock_I                 	 0x1C        /* 7       0 */
#define Fetch_Lock_D                 	 0x1D        /* 7       1 */

#define tmp t0
#define cfg t1
#define icachesize t2
#define ilinesize t3
#define iways t4
#define dcachesize t5
#define dlinesize t6
#define dways t7
#define coherency v0
#define save_ra v1

#endif
