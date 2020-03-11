/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
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
 *
 * ----------------------------------------------------------------------
 *
 * Project:      ARMv8-M System Recovery demo
 * Title:        SysTick_s.h   SysTick handler & timeout based watchdog 
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#ifndef _SYSTICK_S_H
#define _SYSTICK_S_H

#include <stdint.h>

// Initialize Secure SysTick
// \param StartSeconds   setup seconds value
extern void InitWatchdog (uint32_t StartSeconds);

// Restart Watchdog
// \param Food    token obtained by StartWatchdog_s
extern void FeedWatchdog_s (uint32_t Food);

// Start Watchdog
// \return Food token expected by RestartWatchdog
extern uint32_t StartWatchdog_s (void);


#endif  /* !_SYSTICK_S_H */
