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

#ifndef YUNOS_BSP_FTL_H
#define YUNOS_BSP_FTL_H

#include <stdint.h>
#include <sys/types.h>
//#include "cffs_flash.h"


#define EFTL_NO_PWR_DOWN_PROT	(0)
#define EFTL_PWR_DOWN_PROT		(1)

/**
 * @brief oss nv configuration structure.
 * @param addr	Nv start addr
 * @param vlen	Valid nv length for users
 * @param tlen	Total nv length for drivers
 * @param attr	Nv attribute, either OSS_NV_POWER_DOWN_SAFE or
				OSS_NV_POWER_DOWN_UNSAFE
 * @note
 * @warning
 */
struct eftl_cfg {
	uint32_t addr;
	uint32_t vlen;
	uint32_t tlen;
	uint32_t attr;
};

/**
 * @brief Read data.
 *	
 * @param addroffset the dest read address
 * @param bytesize   the read length in byte
 * @param buff       the pointer to the store data address
 *
 * @return Standard errno
*/
int yunos_bsp_ftl_read(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);

/**
 * @brief Write data.
 *
 * @param addroffset Write begining addr.
 * @param buff 		 Pointer to the src data.
 * @param bytesize   Write size in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_ftl_write(uint32_t addroffset, const uint8_t *buff, uint32_t bytesize);

/**
 * @brief Write data.
 *
 * @param addroffset Write begining addr.
 * @param buff 		 Pointer to the src data.
 * @param bytesize   Write size in bytes.
 *
 * @return Standard errno
 */
int yunos_bsp_ftl_write_poll(uint32_t addroffset, const uint8_t *buff, uint32_t bytesize);

/** 
 * @brief Init the ftl region.
 *
 * @param cfg Pointer to ftl cfg structure.
 * @param nr  Ftl region num.
 * 
 * @return Standard errno
 */
int yunos_bsp_ftl_init(const struct eftl_cfg *cfg, uint32_t nr);

#endif /* YUNOS_BSP_EFLASH_H */
