/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMORY.H
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

/* Types */
typedef struct mem {              /* << Memory Pool management struct >>     */
  struct mem *next;               /* Next Memory Block in the list           */
  U32         len;                /* Length of data block                    */
} MEMP;

/* Functions */
extern U32   rt_init_mem  (void *pool, U32  size);
extern void *rt_alloc_mem (void *pool, U32  size);
extern U32   rt_free_mem  (void *pool, void *mem);
