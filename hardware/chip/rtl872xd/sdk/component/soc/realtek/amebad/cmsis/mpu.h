/**************************************************************************//**
 * @file     mpu.h
 * @brief    Defines macros for the MPU registers seting.
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

#ifndef _MPU_H_
#define _MPU_H_

/**
  \brief  The data structure for a MPU region configuration
*/
typedef struct {
	uint32_t region_base;      /*!< MPU region base, 32 bytes aligned. */
	uint32_t region_size;      /*!< MPU region size, 32 bytes aligned. */
	uint8_t  xn;        /*!< eXecute Never attribute. This parameter can be a value of @ref mpu_region_config_xn_define. */
	uint8_t  ap;        /*!< Access permissions. This parameter can be a value of @ref mpu_region_config_ap_define. */
	uint8_t  sh;        /*!< Shareability for Normal memory. This parameter can be a value of @ref mpu_region_config_sh_define. */
	uint8_t  attr_idx;  /*!< memory attribute indirect index. This parameter can be a value of 0~7. */
} mpu_region_config;

/** @defgroup mpu_region_config_xn_define
  * @{
  *    note: eXecute Never attribute(MPU_RBAR[0]):
  *            0: Allow program execution in this region.
  *            1: Does not allow program execution in this region.
  */
#define MPU_EXEC_ALLOW				0
#define MPU_EXEC_NEVER				1
/**
  * @}
  */

/** @defgroup mpu_region_config_ap_define
  * @{
  *    note: Access permissions (MPU_RBAR[2:1]):
  *            00: Read/write by privileged code only.
  *            01: Read/write by any privilege level.
  *            10: Read only by privileged code only.
  *            11: Read only by any privilege level.
  */
#define MPU_PRIV_RW				(0 << 1)
#define MPU_UN_PRIV_RW				(1 << 1)
#define MPU_PRIV_R					(2 << 1)
#define MPU_PRIV_W					(3 << 1)
/**
  * @}
  */

/** @defgroup mpu_region_config_sh_define
  * @{
  *    note: Shareability for Normal memory(MPU_RBAR[4:3]):
  *            00: Non-shareable.
  *            01: Reserved.
  *            10: Outer shareable.
  *            11: Inner shareable.
  */
#define MPU_NON_SHAREABLE				(0 << 3)
#define MPU_OUT_SHAREABLE				(2 << 3)
#define MPU_INR_SHAREABLE				(3 << 3)
/**
  * @}
  */
#if defined (ARM_CORE_CM4)
#define MPU_MAX_REGION				8
#else
#define MPU_MAX_REGION				4
#endif
/** @defgroup mpu_region_memory_attribute_define
  * @{
  *    note: Outer, bits [7:4]:
  *            0b0000 Device memory.
  *            0b00RW Normal memory, Outer write-through transient (RW!='00').
  *            0b0100 Normal memory, Outer non-cacheable.
  *            0b01RW Normal memory, Outer write-back transient (RW!='00').
  *            0b10RW Normal memory, Outer write-through non-transient.
  *            0b11RW Normal memory, Outer write-back non-transient.
  *    note: The transient attribute indicates that the benefit of caching is for a relatively short period,
  *            and that therefore it might be better to restrict allocation, to avoid possibly casting-out other, 
  *            less transient, entries.
  *    note: Inner, bits [3:0], when Outer != '0000'
  *            0b00RW Normal memory, Inner write-through transient (RW!='00').
  *            0b0100 Normal memory, Inner non-cacheable.
  *            0b01RW Normal memory, Inner write-back transient (RW!='00').
  *            0b10RW Normal memory, Inner write-through non-transient.
  *            0b11RW Normal memory, Inner write-back non-transient
  */
// define memory attribute of Normal memory with  write-through transient,  write allocation
#define NORMAL_WT_T_WA            (0x01)

// define memory attribute of Normal memory with  write-through transient,  read allocation
#define NORMAL_WT_T_RA            (0x02)

// define memory attribute of Normal memory with  write-through transient,  read & write allocation
#define NORMAL_WT_T_RWA           (0x03)

// define memory attribute of Normal memory with  non-cacheable
#define NORMAL_NC                 (0x04)

// define memory attribute of Normal memory with  write-back transient,  write allocation
#define NORMAL_WB_T_WA            (0x05)

// define memory attribute of Normal memory with  write-back transient,  read allocation
#define NORMAL_WB_T_RA            (0x06)

// define memory attribute of Normal memory with  write-back transient,  read and write allocation
#define NORMAL_WB_T_RWA           (0x07)

// define memory attribute of Normal memory with  write-through non-transient, no  allocation
#define NORMAL_WT_NT              (0x08)

// define memory attribute of Normal memory with  write-through non-transient,  write allocation
#define NORMAL_WT_NT_WA           (0x09)

// define memory attribute of Normal memory with  write-through non-transient,  read allocation
#define NORMAL_WT_NT_RA           (0x0A)

// define memory attribute of Normal memory with  write-through non-transient,  read and write allocation
#define NORMAL_WT_NT_RWA          (0x0B)

// define memory attribute of Normal memory with  write-back non-transient, no  allocation
#define NORMAL_WB_NT              (0x0C)

// define memory attribute of Normal memory with  write-back non-transient,  write allocation
#define NORMAL_WB_NT_WA           (0x0D)

// define memory attribute of Normal memory with  write-back non-transient,  read allocation
#define NORMAL_WB_NT_RA           (0x0E)

// define memory attribute of Normal memory with  write-back non-transient,  read and write allocation
#define NORMAL_WB_NT_RWA          (0x0F)
/**
  * @}
  */

/** @defgroup mpu_region_device_attribute_define
  * @{
  *    note: Device, bits [3:2], when Outer == '0000':
  *            0b00 Device-nGnRnE.
  *            0b01 Device-nGnRE.
  *            0b10 Device-nGRE.
  *            0b11 Device-GRE.
  */
// define memory attribute of Device memory with non-gathering, non-reording, non-Early Write Acknowledge
#define DEVICE_NG_NR_NE             ((0<<4)|(0<<2))

// define memory attribute of Device memory with non-gathering, non-reording, Early Write Acknowledge
#define DEVICE_NG_NR_E              ((0<<4)|(1<<2))

// define memory attribute of Device memory with non-gathering, reording, Early Write Acknowledge
#define DEVICE_NG_R_E               ((0<<4)|(2<<2))

// define memory attribute of Device memory with gathering, reording, Early Write Acknowledge
#define DEVICE_G_R_E                ((0<<4)|(3<<2))
/**
  * @}
  */

void mpu_init (void);
void mpu_set_mem_attr(uint8_t attr_idx, uint8_t mem_attr);
void mpu_region_cfg(uint8_t region_num, mpu_region_config *pmpu_cfg);
void mpu_enable(void);
void mpu_disable(void);
void mpu_entry_free(u32 entry_index);
char mpu_entry_alloc(void);

#endif //_MPU_H_

