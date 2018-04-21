/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cmsis.h"
#include "mbed_error.h"
#include "rda_ccfg_api.h"

#if RDA5991H_HW_VER <= 4
extern void sleep_cal_lpo(unsigned int calms);
#endif

/* This function is called after RAM initialization and before main. */
void mbed_sdk_init(void)
{
    /* Update the SystemCoreClock variable. */
    SystemCoreClockUpdate();
    if(rda_ccfg_hwver() <= 4) {
        sleep_cal_lpo(500U);
    }

    if(rda_ccfg_hwver() != RDA5991H_HW_VER) {
        error("\r\nHW=%d/SW=%d not match\r\n", rda_ccfg_hwver(), RDA5991H_HW_VER);
    }
}

