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

#ifndef YUNOS_BSP_INTC_H
#define YUNOS_BSP_INTC_H

#include <stdint.h>

/**
 * Power manager initialize
 * @return      void
 */
void yunos_bsp_pm_initialize(void);

/**
 * Enter wait low power mode.
 * @param[in]   addr    if needed to save context, param is the address to save it.
 * @return      0 on success; N-Zero on failure
 */
int yunos_bsp_pm_wait(unsigned int addr);

/**
 * Enter doze low power mode.
 * @param[in]   addr    if needed to save context, param is the address to save it.
 * @return      0 on success; N-Zero on failure
 */
int yunos_bsp_pm_doze(unsigned int addr);

/**
 * Enter sleep low power mode.
 * @param[in]   addr    if needed to save context, param is the address to save it.
 * @return      0 on success; N-Zero on failure
 */
int yunos_bsp_pm_sleep(unsigned int addr);

/**
 * Enter standby low power mode.
 * @param[in]   addr    if needed to save context, param is the address to save it.
 * @return      0 on success; N-Zero on failure
 */
int yunos_bsp_pm_standby(unsigned int addr);

#endif /* YUNOS_BSP_INTC_H */
