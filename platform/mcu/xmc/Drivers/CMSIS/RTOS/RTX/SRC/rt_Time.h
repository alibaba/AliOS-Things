/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_TIME.H
 *      Purpose: Delay and interval wait functions definitions
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

/* Variables */
extern U32 os_time;

/* Functions */
extern U32  rt_time_get (void);
extern void rt_dly_wait (U16 delay_time);
extern void rt_itv_set  (U16 interval_time);
extern void rt_itv_wait (void);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
