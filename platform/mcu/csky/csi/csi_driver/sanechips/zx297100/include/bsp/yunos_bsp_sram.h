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

#ifndef YUNOS_BSP_SRAM_H
#define YUNOS_BSP_SRAM_H

#include <stdint.h>
#include <sys/types.h>

/**
 * SRAM size
 */
#define SRAM_SIZE                       16384

/**
 * SRAM BSP error number
 */
#define YUNOS_BSP_API_SRAM_CONFIG       0x0100
#define YUNOS_BSP_API_SRAM_CONTROL      0x0200
#define YUNOS_BSP_API_SRAM_ADDRCFG      0x0300
#define YUNOS_BSP_API_SRAM_CDCFG        0x0400
#define YUNOS_BSP_API_SRAM_APCFG        0x0500
#define YUNOS_BSP_API_SRAM_READ         0x0600
#define YUNOS_BSP_API_SRAM_WRITE        0x0700

/**
 * Config the region SRAM to be secure or not secure
 * @param[in]   id      SRAM region index
 * @param[in]   secure  Secure or not secure
 * @return      0, success; error code, fail
 */
int yunos_bsp_sram_region_config(uint8_t id, bool secure);

/**
 * Set the SRAM region enable or disable
 * @param[in]   id      SRAM region index
 * @param[in]   valid   enable or disable
 * @return      0, success; error code, fail
 */
int yunos_bsp_sram_region_control(uint8_t id, bool valid);

/**
 * Set SRAM addr and size
 * @param[in]   id          SRAM region index
 * @param[in]   baseaddr    SRAM region base address
 * @param[in]   size        SRAM region size
 * @return      0, success; error code, fail
 */
int yunos_bsp_sram_region_addr_config(uint8_t id, uint32_t baseaddr, uint32_t size);

/**
 * Set SRAM region CD
 * @param[in]   id          SRAM region index
 * @param[in]   val         CD value
 * @return      0, success;error code, fail
 */
int yunos_bsp_sram_region_cd_config(uint8_t id, uint8_t val);

/**
 * Set SRAM region AP
 * @param[in]   id          SRAM region index
 * @param[in]   val         AP value
 * @param[in]   user        Supervisor user or normal user
 * @return      0, success;error code, fail
 */
int yunos_bsp_sram_region_ap_config(uint8_t id, uint8_t val, uint8_t user);

/**
 * SRAM region read data
 * @param[in]   id          SRAM region index
 * @param[in]   offset      the offset addr of SRAM
 * @param[out]  buff        the data to read
 * @param[in]   cnt         the read byte
 * @return      0, success; error code, fail
 */
int yunos_bsp_sram_region_read(uint8_t id, uint32_t offset, void *buff, size_t cnt);

/**
 * SRAM region write data
 * @param[in]   id          SRAM region index
 * @param[in]   offset      the offset addr of SRAM
 * @param[in]   buff        the data to write
 * @param[in]   cnt         the read byte
 * @return      0, success; error code, fail
 */
int yunos_bsp_sram_region_write(uint8_t id, uint32_t offset, void *buff, size_t cnt);

#endif /* YUNOS_BSP_SRAM_H */

