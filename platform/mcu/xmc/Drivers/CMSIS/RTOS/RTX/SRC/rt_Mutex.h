/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MUTEX.H
 *      Purpose: Implements mutex synchronization objects
 *      Rev.:    V4.70
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

/* Functions */
extern void      rt_mut_init    (OS_ID mutex);
extern OS_RESULT rt_mut_delete  (OS_ID mutex);
extern OS_RESULT rt_mut_release (OS_ID mutex);
extern OS_RESULT rt_mut_wait    (OS_ID mutex, U16 timeout);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
