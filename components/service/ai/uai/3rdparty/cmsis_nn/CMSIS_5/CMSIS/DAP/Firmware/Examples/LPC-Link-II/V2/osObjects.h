/*
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
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
 * $Date:        1. December 2017
 * $Revision:    V2.0.0
 *
 * Project:      CMSIS-DAP Examples LPC-Link-II
 * Title:        osObjects.h CMSIS-DAP RTOS2 Objects for LPC-Link-II
 *
 *---------------------------------------------------------------------------*/

#ifndef __osObjects_h__
#define __osObjects_h__

#include "cmsis_os2.h"

#ifdef osObjectsExternal
extern osThreadId_t DAP_ThreadId;
extern osThreadId_t SWO_ThreadId;
#else
const osThreadAttr_t DAP_ThreadAttr = {
  .priority = osPriorityNormal
};
const osThreadAttr_t SWO_ThreadAttr = {
  .priority = osPriorityAboveNormal
};
extern osThreadId_t DAP_ThreadId;
       osThreadId_t DAP_ThreadId;
extern osThreadId_t SWO_ThreadId;
       osThreadId_t SWO_ThreadId;
#endif

extern void DAP_Thread (void *argument);
extern void SWO_Thread (void *argument);

extern void app_main (void *argument);

#endif  /* __osObjects_h__ */
