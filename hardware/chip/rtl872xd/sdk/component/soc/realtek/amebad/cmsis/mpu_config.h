/**************************************************************************//**
 * @file     mpu_config.c
 * @brief    Defines macros for the MPU configuration for the Secure region.
 * 
 * @version  V1.00
 * @date     2017-03-21
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef _MPU_CONFIG_S_H_
#define _MPU_CONFIG_S_H_

#include "mpu.h"

/** @defgroup mpu_config_define
  * @{
  *    note: MPU_INIT_CTRL_ENABLE
  *            <q> Enable MPU
  *            <i> Value for MPU->CTRL register bit ENABLE
  *            <0=> MPU is disabled.
  *            <1=> MPU is enabled.
  *    note: MPU_INIT_CTRL_PRIVDEFENA
  *            <i> Value for MPU->CTRL register bit PRIVDEFENA
  *            Privileged background region enable:
  *            <0=> All accesses to unmapped addresses result in faults.
  *            <1=> Enables the default memory map for privilege code when the address accessed
  *            does not map into any MPU region. Unprivileged accesses to unmapped addresses
  *            result in faults.
  *    note: MPU_INIT_CTRL_HFNMIENA
  *            <i> Value for MPU->CTRL register bit HFNMIENA MPU Enable for HardFault and NMI (Non-Maskable Interrupt):
  *            <0=> HardFault and NMI handlers bypass MPU configuration as if MPU is disabled.
  *            <1=> MPU access rules apply to HardFault and NMI handlers.
  */
#define MPU_INIT_CTRL_ENABLE			1
#define MPU_INIT_CTRL_PRIVDEFENA		1
#define MPU_INIT_CTRL_HFNMIENA		0
/**
  * @}
  */

/** @defgroup mpu_mem_attri_typical_define
  * @{
  *    note: MPU_MEM_ATTR0~MPU_MEM_ATTR7
  *            no cache
  *            write-through + read allocation
  *            write-back + read allocation + write allocation

  *            <1=> MPU is enabled.
  */
#define MPU_MEM_ATTR0                   ((NORMAL_NC << 4) | NORMAL_NC) // The memory attribute configuration of the MAIR[Attr0]
#define MPU_MEM_ATTR1                   ((NORMAL_WT_T_RA << 4)  | NORMAL_WT_T_RA) // The memory attribute configuration of the MAIR[Attr1]
#define MPU_MEM_ATTR2                   ((NORMAL_WB_T_RWA << 4)  | NORMAL_WB_T_RWA) // The memory attribute configuration of the MAIR[Attr2]
#define MPU_MEM_ATTR3                   (DEVICE_NG_NR_NE) // The memory attribute configuration of the MAIR[Attr3]
#define MPU_MEM_ATTR4                   (DEVICE_NG_NR_NE) // The memory attribute configuration of the MAIR[Attr4]
#define MPU_MEM_ATTR5                   (DEVICE_NG_NR_NE) // The memory attribute configuration of the MAIR[Attr5]
#define MPU_MEM_ATTR6                   (DEVICE_NG_NR_NE) // The memory attribute configuration of the MAIR[Attr6]
#define MPU_MEM_ATTR7                   (DEVICE_NG_NR_NE) // The memory attribute configuration of the MAIR[Attr7]

#define MPU_MEM_ATTR_IDX_NC			0
#define MPU_MEM_ATTR_IDX_WT_T_RA	1
#define MPU_MEM_ATTR_IDX_WB_T_RWA	2
#define MPU_MEM_ATTR_IDX_DEVICE		3
/**
  * @}
  */

#endif //_MPU_CONFIG_S_H_

