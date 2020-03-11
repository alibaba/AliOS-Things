/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX.C
 *      Purpose: Interface functions for fixed memory block management system
 *      Rev.:    V4.79
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2017 ARM Germany GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_MemBox.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- _init_box -------------------------------------*/

U32 _init_box  (void *box_mem, U32 box_size, U32 blk_size) {
  /* Initialize memory block system, returns 0 if OK, 1 if fails. */
  void *end;
  void *blk;
  void *next;
  U32  sizeof_bm;

  /* Create memory structure. */
  if (blk_size & BOX_ALIGN_8) {
    /* Memory blocks 8-byte aligned. */ 
    blk_size = ((blk_size & ~BOX_ALIGN_8) + 7U) & ~(U32)7U;
    sizeof_bm = (sizeof (struct OS_BM) + 7U) & ~(U32)7U;
  }
  else {
    /* Memory blocks 4-byte aligned. */
    blk_size = (blk_size + 3U) & ~(U32)3U;
    sizeof_bm = sizeof (struct OS_BM);
  }
  if (blk_size == 0U) {
    return (1U);
  }
  if ((blk_size + sizeof_bm) > box_size) {
    return (1U);
  }
  /* Create a Memory structure. */
  blk = ((U8 *) box_mem) + sizeof_bm;
  ((P_BM) box_mem)->free = blk;
  end = ((U8 *) box_mem) + box_size;
  ((P_BM) box_mem)->end      = end;
  ((P_BM) box_mem)->blk_size = blk_size;

  /* Link all free blocks using offsets. */
  end = ((U8 *) end) - blk_size;
  while (1)  {
    next = ((U8 *) blk) + blk_size;
    if (next > end) { break; }
    *((void **)blk) = next;
    blk = next;
  }
  /* end marker */
  *((void **)blk) = 0U;
  return (0U);
}

/*--------------------------- rt_alloc_box ----------------------------------*/

void *rt_alloc_box (void *box_mem) {
  /* Allocate a memory block and return start address. */
  void **free;
#ifndef __USE_EXCLUSIVE_ACCESS
  U32  irq_mask;

  irq_mask = (U32)__disable_irq ();
  free = ((P_BM) box_mem)->free;
  if (free) {
    ((P_BM) box_mem)->free = *free;
  }
  if (irq_mask == 0U) { __enable_irq (); }
#else
  do {
    if ((free = (void **)__ldrex(&((P_BM) box_mem)->free)) == 0U) {
      __clrex();
      break;
    }
  } while (__strex((U32)*free, &((P_BM) box_mem)->free));
#endif
  return (free);
}


/*--------------------------- _calloc_box -----------------------------------*/

void *_calloc_box (void *box_mem)  {
  /* Allocate a 0-initialized memory block and return start address. */
  void *free;
  U32 *p;
  U32 i;

  free = _alloc_box (box_mem);
  if (free)  {
    p = free;
    for (i = ((P_BM) box_mem)->blk_size; i; i -= 4U)  {
      *p = 0U;
      p++;
    }
  }
  return (free);
}


/*--------------------------- rt_free_box -----------------------------------*/

U32 rt_free_box (void *box_mem, void *box) {
  /* Free a memory block, returns 0 if OK, 1 if box does not belong to box_mem */
#ifndef __USE_EXCLUSIVE_ACCESS
  U32 irq_mask;
#endif

  if ((box < box_mem) || (box >= ((P_BM) box_mem)->end)) {
    return (1U);
  }

#ifndef __USE_EXCLUSIVE_ACCESS
  irq_mask = (U32)__disable_irq ();
  *((void **)box) = ((P_BM) box_mem)->free;
  ((P_BM) box_mem)->free = box;
  if (irq_mask == 0U) { __enable_irq (); }
#else
  do {
    do {
      *((void **)box) = ((P_BM) box_mem)->free;
      __DMB();
    } while (*(void**)box != (void *)__ldrex(&((P_BM) box_mem)->free));
  } while (__strex ((U32)box, &((P_BM) box_mem)->free));
#endif
  return (0U);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
