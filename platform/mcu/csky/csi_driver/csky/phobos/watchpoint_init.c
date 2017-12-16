/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include "watchpoint_init.h"

void wp_watchtype_A_set(watch_type watch_s)
{
    uint32_t tmp = *((volatile unsigned int *) HAD_B0_HCR);

    if (watch_s == WATCH) {
        tmp = (tmp & A_BIT_SAFE) | A_BIT_WATCH;
    }

    if (watch_s == RWATCH) {
        tmp = (tmp & A_BIT_SAFE) | A_BIT_RWATCH;
    }

    if (watch_s == AWATCH) {
        tmp = (tmp & A_BIT_SAFE) | A_BIT_AWATCH;
    }

    *((volatile unsigned int *) HAD_B0_HCR) = tmp;
}

void wp_watchtype_B_set(watch_type watch_s)
{
    uint32_t tmp = *((volatile unsigned int *) HAD_B0_HCR);

    if (watch_s == WATCH) {
        tmp = (tmp & B_BIT_SAFE) | B_BIT_WATCH;
    }

    if (watch_s == RWATCH) {
        tmp = (tmp & B_BIT_SAFE) | B_BIT_RWATCH;
    }

    if (watch_s == AWATCH) {
        tmp = (tmp & B_BIT_SAFE) | B_BIT_AWATCH;
    }

    *((volatile unsigned int *) HAD_B0_HCR) = tmp;
}

void wp_bitor_putreg16(uint16_t val, uint32_t *reg)
{
    uint16_t tmp = *((volatile unsigned int *) reg);
    tmp |= val;
    *((volatile unsigned int *) reg) = tmp;
}

void wp_bitor_putreg32(uint32_t val, uint32_t *reg)
{
    uint32_t tmp = *((volatile unsigned int *) reg);
    tmp = val | tmp;
    *((volatile unsigned int *) reg) = tmp;
}

void wp_bitand_putreg32(uint32_t val, uint32_t *reg)
{
    uint32_t tmp = *((volatile unsigned int *) reg);
    tmp &= val;
    *((volatile unsigned int *) reg) = tmp;
}

inline void wp_putreg32(uint32_t val, uint32_t *reg)
{
    *((volatile unsigned int *) reg) = val;
}

inline void wp_putreg16(uint16_t val, uint32_t *reg)
{
    *((volatile unsigned int *) reg) = val;
}

inline void wp_putreg8(uint8_t val, uint32_t *reg)
{
    *((volatile unsigned int *) reg) = val;
}

