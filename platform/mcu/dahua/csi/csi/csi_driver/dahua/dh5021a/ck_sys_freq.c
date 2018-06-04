/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     ck_sys_freq.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2017
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <ck_sys_freq.h>

void ck_set_sys_freq (clk_gen_t source, clk_val_t val)
{
    if (source == IHS_CLK) 
        return;

    uint32_t reg_val;
    uint32_t trc_val = 0;
    uint32_t stable = 0;

    if (source == EHS_CLK) {
        switch (val) {
            case CLK_8M:
                reg_val = CLK_8M_REG_VAL;
                break;
            case CLK_16M:
                reg_val = CLK_16M_REG_VAL;
                break;
            case CLK_24M:
                reg_val = CLK_24M_REG_VAL;
                break;
            case CLK_32M:
                reg_val = CLK_32M_REG_VAL;
                trc_val = TRC_REG_VAL;
                break;
            case CLK_40M:
                reg_val = CLK_40M_REG_VAL;
                trc_val = TRC_REG_VAL;
                break;
            case CLK_48M:
                reg_val = CLK_48M_REG_VAL;
                trc_val = TRC_REG_VAL;
                break;
            default:
                return;
        }

    *((volatile uint32_t *)PMU_PLL_CTRL) = reg_val;

    if (trc_val == TRC_REG_VAL) {
        *((volatile uint32_t *)TRC_ADDR) = trc_val;
    }

    while (1) {
         stable = (*(volatile uint32_t *)PMU_CLK_STABLE);
         if (stable & 0x00000010)
         break;
    }

    *((volatile uint32_t *)PMU_MCLK_SEL) = MCLK_REG_VAL;

    return;
    }

    return;
}
