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

#ifndef _WATCHPOINT_INIT_H
#define _WATCHPOINT_INIT_H

#include <stdint.h>
#include <watchpoint.h>
#include <stdlib.h>

extern size_t cpu_intrpt_save(void);
extern void   cpu_intrpt_restore(size_t psr);

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define WP_CRITICAL_ENTER()     cpu_intrpt_save()
#define WP_CRITICAL_EXIT(flags) cpu_intrpt_restore(flags)
typedef unsigned int irqstate_t;

/* HAD BANK */
#define HAD_B0_MBCA 0xE0011010
#define HAD_B0_BABA 0xE001101C
#define HAD_B0_BAMA 0xE0011024

#define HAD_B0_MBCB 0xE0011014
#define HAD_B0_BABB 0xE0011020
#define HAD_B0_BAMB 0xE0011028

#define HAD_B0_HCR  0xE0011034
#define HAD_B0_CSR  0xE0011054

#define HAD_B1_MBIR 0xE00110EC

/*MBEE in HAD_B0_CSR to enable a watchpiont fall into exception,7th exception especially */
#define CSR_MBEE 0x00C0

/*put the value into HAD_B0_BAMA or HAD_B1_BAMB,
 * and disable memory breakpoint*/
#define WATCHPOINT_DISABLE 0xFFFFFFFF

#define HCR_RCA            0xffffffdf
#define HCR_RCB            0xffffefff

#define A_BIT_SAFE         0xffffffe0
#define A_BIT_WATCH        0x00000005
#define A_BIT_RWATCH       0x00000006
#define A_BIT_AWATCH       0x00000003

#define B_BIT_SAFE         0xfffff83f
#define B_BIT_WATCH        0x00000140
#define B_BIT_RWATCH       0x00000180
#define B_BIT_AWATCH       0x000000c0

/* disable memory breakpoint */
#define DISAB 0xFFFFFFFF

/* max address len can watch */
#define MAX_WATCH_LEN 4

void wp_watchtype_A_set(watch_type watch_s);

void wp_watchtype_B_set(watch_type watch_s);

void wp_bitor_putreg16(uint16_t val, uint32_t *reg);

void wp_bitor_putreg32(uint32_t val, uint32_t *reg);

void wp_bitand_putreg32(uint32_t val, uint32_t *reg);

inline void wp_putreg32(uint32_t val, uint32_t *reg);

inline void wp_putreg16(uint16_t val, uint32_t *reg);

inline void wp_putreg8(uint8_t val, uint32_t *reg);

#endif /* _WATCHPOINT_INIT_H */
