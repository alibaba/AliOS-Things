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

#include <stdio.h>
#include <stdint.h>
#include "watchpoint_init.h"
#include <watchpoint.h>

/* count the number of watchpoint that already has been set. */
uint8_t watchpoint_count = 0;
watchpoint_t wp[WP_MAX];

/**********************************************************************
 * Function:        wp_get_callback
 * Description:     get the user's calback function, disopen to user
 * Parameters:      void
 * Returns:         void
 **********************************************************************/

void wp_get_callback(void)
{
    irqstate_t flags;

    flags = WP_CRITICAL_ENTER();

    if ((*((volatile unsigned int *)HAD_B1_MBIR)) == 1) {
        if (wp[0].alive) {
            wp[0].get_callfuct();
        }
    }

    if ((*((volatile unsigned int *)HAD_B1_MBIR)) == 2) {
        if (wp[1].alive) {
            wp[1].get_callfuct();
        }
    }

    WP_CRITICAL_EXIT(flags);
}

/**********************************************************************
 * Function:        wp_register
 * Description:     get the watchpoint information
 * Parameters:      base_addr is where the watchpiont start; callfuct is a callback
                    function which user can define; watch_type can one of the three
                    WATCH, AWATCH, RWATCH.
 * Returns:         return the number in sequence 1 and 2; return -1 when failed.
 **********************************************************************/

int wp_register(uint32_t base_addr, watch_type watch_s, void (*callfuct)(void))
{
    irqstate_t flags;

    flags = WP_CRITICAL_ENTER();

    if (watchpoint_count <= (WP_MAX - 1)) {
        if (!wp[0].alive) {
            wp[0].get_addr     = base_addr;
            wp[0].get_len      = MAX_WATCH_LEN;
            wp[0].get_callfuct = callfuct;
            wp[0].alive        = 1;
            /* (*((volatile unsigned int *) HAD_B0_MBCA)) = 0; */
            wp_watchtype_A_set(watch_s);
            watchpoint_count += 1;
            WP_CRITICAL_EXIT(flags);
            return 1;
        } else {
            if (!wp[1].alive) {
                wp[1].get_addr     = base_addr;
                wp[1].get_len      = MAX_WATCH_LEN;
                wp[1].get_callfuct = callfuct;
                wp[1].alive        = 1;
                /* (*((volatile unsigned int *) HAD_B0_MBCB)) = 0; */
                wp_watchtype_B_set(watch_s);
                watchpoint_count += 1;
                WP_CRITICAL_EXIT(flags);
                return 2;
            } else {
                printf("There is already has two(only) watchpoints, please unregister one of them.\n");
                WP_CRITICAL_EXIT(flags);
                return -1;
            }
        }
    } else {
        WP_CRITICAL_EXIT(flags);
        return -1;
    }
}

/**********************************************************************
 * Function:        wp_unregister
 * Description:     a pair of function with register; free the watchpoint in used
 * Parameters:      number of watchpoint in sequence when registered, 1 or 2.
 * Returns:         0 when successed, -1 when failed.
 **********************************************************************/

int wp_unregister(uint8_t number)
{
    irqstate_t flags;

    flags = WP_CRITICAL_ENTER();

    if (number == 1 || number == 2) {
        if (number == 1 && wp[0].alive == 1) {
            wp_putreg32(0xabcd, (uint32_t *)HAD_B0_BABA);
            wp[0].alive = 0;
            watchpoint_count -= 1;
        }

        if (number == 2 && wp[1].alive == 1) {
            wp_putreg32(0xabcd, (uint32_t *)HAD_B0_BABB);
            wp[1].alive = 0;
            watchpoint_count -= 1;
        }

        WP_CRITICAL_EXIT(flags);
        return 0;
    } else {
        WP_CRITICAL_EXIT(flags);
        return -1;
    }
}

/**********************************************************************
 * Function:        wp_enable
 * Description:     enable the watchpoint which already has been registered
 * Parameters:      number of watchpoint in sequence when registered, 1 or 2.
 * Returns:         0 when successed, -1 when failed.
 **********************************************************************/

int wp_enable(uint8_t number)
{
    irqstate_t flags;

    flags = WP_CRITICAL_ENTER();

    if (watchpoint_count <= 2) {
        wp_bitor_putreg16(CSR_MBEE, (uint32_t *)HAD_B0_CSR);

        if (number == 1 || number == 2) {
            if (number == 1) {
                uint32_t mask = ~(wp[0].get_len - 1);
                uint32_t base = wp[0].get_addr & mask;
                wp_bitand_putreg32(HCR_RCA, (uint32_t *)HAD_B0_HCR);
                wp_putreg32(mask, (uint32_t *)HAD_B0_BAMA);
                wp_putreg32(base, (uint32_t *)HAD_B0_BABA);
            } else {
                uint32_t mask = ~(wp[1].get_len - 1);
                uint32_t base = wp[1].get_addr & mask;
                wp_bitand_putreg32(HCR_RCB, (uint32_t *)HAD_B0_HCR);
                wp_putreg32(mask, (uint32_t *)HAD_B0_BAMB);
                wp_putreg32(base, (uint32_t *)HAD_B0_BABB);
            }

            WP_CRITICAL_EXIT(flags);
            return 0;
        } else {
            WP_CRITICAL_EXIT(flags);
            return -1;
        }
    } else {
        WP_CRITICAL_EXIT(flags);
        return -1;
    }
}

/**********************************************************************
 * Function:        wp_disable
 * Description:     disenable the watchpoint which already has been enabled
 * Parameters:      number of watchpoint in sequence when registered, 1 or 2.
 * Returns:         0 when successed, -1 when failed.
 **********************************************************************/

int wp_disable(uint8_t number)
{
    irqstate_t flags;

    flags = WP_CRITICAL_ENTER();

    if (number == 1 || number == 2) {
        if (number == 1) {
            wp_putreg32(WATCHPOINT_DISABLE, (uint32_t *)HAD_B0_BABA);
        } else {
            wp_putreg32(WATCHPOINT_DISABLE, (uint32_t *)HAD_B0_BABB);
        }

        WP_CRITICAL_EXIT(flags);
        return 0;
    } else {
        WP_CRITICAL_EXIT(flags);
        return -1;
    }
}

