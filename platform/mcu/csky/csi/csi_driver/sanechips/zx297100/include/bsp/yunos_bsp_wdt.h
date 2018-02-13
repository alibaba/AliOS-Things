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

#ifndef YUNOS_BSP_WDT_H
#define YUNOS_BSP_WDT_H

#include <stdint.h>

/**
 * enable watchdog.
 *
 * @param[in]   id              watchdog id
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wdt_enable(uint8_t id);

/**
 * disable watchdog.
 *
 * @param[in]   id              watchdog id
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wdt_disable(uint8_t id);

/**
 * configure watchdog.
 * @param[in]   id                 watchdog id
 * @param[in]   clk_div          watchdog clk div
 * @param[in]   int_val          watchdog int value
 * @param[in]   load_val        watchdog load value
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wdt_config(uint8_t id, uint32_t clk_div, uint32_t int_val, uint32_t load_val);


/**
 * restart watchdog.
 *
 * @param[in]   id              watchdog id
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wdt_restart(uint8_t id);



#endif /* YUNOS_BSP_WDT_H */
