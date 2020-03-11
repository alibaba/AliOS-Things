/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMORY.C
 *      Purpose: Interface functions for Dynamic Memory Management System
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
#include "rt_Memory.h"


/* Functions */

// Initialize Dynamic Memory pool
//   Parameters:
//     pool:    Pointer to memory pool
//     size:    Size of memory pool in bytes
//   Return:    0 - OK, 1 - Error

U32 rt_init_mem (void *pool, U32 size) {
  MEMP *ptr;

  if ((pool == NULL) || (size < sizeof(MEMP))) { return (1U); }

  ptr = (MEMP *)pool;
  ptr->next = (MEMP *)((U32)pool + size - sizeof(MEMP *));
  ptr->next->next = NULL;
  ptr->len = 0U; 

  return (0U);
}

// Allocate Memory from Memory pool
//   Parameters:
//     pool:    Pointer to memory pool
//     size:    Size of memory in bytes to allocate
//   Return:    Pointer to allocated memory

void *rt_alloc_mem (void *pool, U32 size) {
  MEMP *p, *p_search, *p_new;
  U32   hole_size;

  if ((pool == NULL) || (size == 0U)) { return NULL; }

  /* Add header offset to 'size' */
  size += sizeof(MEMP);
  /* Make sure that block is 4-byte aligned  */
  size = (size + 3U) & ~(U32)3U;

  p_search = (MEMP *)pool;
  while (1) {
    hole_size  = (U32)p_search->next - (U32)p_search;
    hole_size -= p_search->len;
    /* Check if hole size is big enough */
    if (hole_size >= size) { break; }
    p_search = p_search->next;
    if (p_search->next == NULL) {
      /* Failed, we are at the end of the list */
      return NULL;
    }
  }

  if (p_search->len == 0U) {
    /* No block is allocated, set the Length of the first element */
    p_search->len = size;
    p = (MEMP *)(((U32)p_search) + sizeof(MEMP));
  } else {
    /* Insert new list element into the memory list */
    p_new       = (MEMP *)((U32)p_search + p_search->len);
    p_new->next = p_search->next;
    p_new->len  = size;
    p_search->next = p_new;
    p = (MEMP *)(((U32)p_new) + sizeof(MEMP));
  }

  return (p);
}

// Free Memory and return it to Memory pool
//   Parameters:
//     pool:    Pointer to memory pool
//     mem:     Pointer to memory to free
//   Return:    0 - OK, 1 - Error

U32 rt_free_mem (void *pool, void *mem) {
  MEMP *p_search, *p_prev, *p_return;

  if ((pool == NULL) || (mem == NULL)) { return (1U); }

  p_return = (MEMP *)((U32)mem - sizeof(MEMP));
  
  /* Set list header */
  p_prev = NULL;
  p_search = (MEMP *)pool;
  while (p_search != p_return) {
    p_prev   = p_search;
    p_search = p_search->next;
    if (p_search == NULL) {
      /* Valid Memory block not found */
      return (1U);
    }
  }

  if (p_prev == NULL) {
    /* First block to be released, only set length to 0 */
    p_search->len = 0U;
  } else {
    /* Discard block from chain list */
    p_prev->next = p_search->next;
  }

  return (0U);
}
