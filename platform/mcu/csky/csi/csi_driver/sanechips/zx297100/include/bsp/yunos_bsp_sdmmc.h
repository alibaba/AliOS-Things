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

#ifndef YUNOS_BSP_SDMMC_H
#define YUNOS_BSP_SDMMC_H

#include <stdint.h>

/**
 * Send the SDIO command
 * @param[in]   id          device nunber of the SDIO
 * @param[in]   cmd         the command to send (32-bits, encoded)
 * @param[in]   arg         32-bit argument required with some commands
 * @return      OK is success; a negated errno on failure
 */

int yunos_bsp_sdmmc_sendcmd(uint8_t id, uint32_t cmd, uint32_t arg);

/**
 * Poll-wait for the response to the last command to be ready.
 * @param[in]   id          device nunber of the SDIO
 * @param[in]   cmd         the command that was sent.  See 32-bit command definitions above.
 * @return      OK is success; a negated errno on failure
 */

int yunos_bsp_sdmmc_waitresponse(uint8_t id, uint32_t cmd);

/**
 * Enable/disable SDIO interrupts
 * @param[in]   id          device nunber of the SDIO
 * @param[in]   intmask     The set of bits in the SDIO MASK register to set
 * @return      void
 */

void yunos_bsp_sdmmc_configints(uint8_t id, uint32_t intmask);

#endif /* YUNOS_BSP_SDMMC_H */
