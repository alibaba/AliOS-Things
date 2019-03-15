/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
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

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/interpt.h"
#include "board.h"

/**
 * Interrupt vector init
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_init(void)
{
	return HAL_OK;
}

/**
 * Mask specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_mask(int32_t vec)
{
	return HAL_OK;
}

/**
 * Unmask specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_umask(int32_t vec)
{
	return HAL_OK;
}

/**
 * Install specified interrupt vector
 *
 *
 * @param[in] vec specified interrupt vector
 *
 * @return  0 : on success, -1 : if an error occurred with any step
 */
int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, 
                                     void *para,  char *name)
{
	return HAL_OK;
}
