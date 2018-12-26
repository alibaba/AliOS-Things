/**
 * File: infra_config_7100.h
 * Brief: Public configuration of infra for 7100
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Ningkun Deng <deng.ningkun@sanechips.com.cn>
 * @ingroup si_pub_infra_7100_id
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _INFRA_CONFIG_7100
#define _INFRA_CONFIG_7100

#ifndef _INFRA_CONFIG_H
#error "Don't include infra_config_7100.h directly, include infra_config.h instead!"
#endif

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
/* ram */
#define IRAM_AON_BASE       (0x20110000)
#define IRAM_AON_LEN        (6 * 1024UL)

#define IRAM_VECOTR_BASE    (IRAM_AON_BASE)
#define IRAM_VECOTR_LEN     (512)

#define IRAM_PSM_AP_BASE    (IRAM_VECOTR_BASE + IRAM_VECOTR_LEN)
#define IRAM_PSM_AP_LEN     (3 * 512UL)

/* dormant1模式用户数据预留 */
#if 0
#define IRAM_USER_BASE      (IRAM_PSM_AP_BASE + IRAM_PSM_AP_LEN)
#define IRAM_USER_LEN       (512UL)
#endif

#define IRAM_SPINLOCK_BASE  (IRAM_PSM_AP_BASE + IRAM_PSM_AP_LEN)
#define IRAM_SPINLOCK_LEN   (128UL)

#define IRAM_ICP_PIPE_BASE 	(IRAM_SPINLOCK_BASE + IRAM_SPINLOCK_LEN)
#define IRAM_ICP_PIPE_LEN  	(848UL)

#define IRAM_RAMDUMP_BASE 	(IRAM_ICP_PIPE_BASE + IRAM_ICP_PIPE_LEN)
#define IRAM_RAMDUMP_LEN  	(48UL)

#define IRAM_RESERVE_CP_BASE (IRAM_RAMDUMP_BASE + IRAM_RAMDUMP_LEN)
#define IRAM_RESERVE_CP_LEN  (1024UL)

#define IRAM_PSM_CP_BASE    (IRAM_RESERVE_CP_BASE + IRAM_RESERVE_CP_LEN)
#define IRAM_PSM_CP_LEN     (2 * 1024UL)

#if ((IRAM_VECOTR_LEN + IRAM_PSM_AP_LEN + IRAM_SPINLOCK_LEN + IRAM_ICP_PIPE_LEN + \
	 IRAM_RAMDUMP_LEN + IRAM_RESERVE_CP_LEN + IRAM_PSM_CP_LEN) != IRAM_AON_LEN)
#error "IRAM AON LEN error!"
#endif

#define IRAM_AP_BUS_BASE	(0x10000)
#define IRAM_AP_LEN			(80 * 1024UL)
#define IRAM_AP_PHY_BASE    0x30000000
#define IRAM_AP_TEE_BASE	(IRAM_AP_PHY_BASE)
#define IRAM_AP_TEE_LEN		(8 * 1024UL)
#define IRAM_AP_IMAGE_BASE	(IRAM_AP_TEE_BASE + IRAM_AP_TEE_LEN)
#define IRAM_AP_IMAGE_LEN	(71 * 1024UL)
#define IRAM_AP_LOG_BASE	(IRAM_AP_IMAGE_BASE + IRAM_AP_IMAGE_LEN)
#define IRAM_AP_LOG_LEN		(1020UL)
#define IRAM_AP_AMT_BASE 	(IRAM_AP_LOG_BASE + IRAM_AP_LOG_LEN)
#define IRAM_AP_AMT_LEN		(4UL)
#define IRAM_AP_REE_LEN     (IRAM_AP_IMAGE_LEN + IRAM_AP_LOG_LEN + IRAM_AP_AMT_LEN)

/* dtcm */
#define DTCM_CP_BUS_BASE	(0x81000000)
#define DTCM_CP_LEN    		(128 * 1024UL)

/* eflash */
#define EFLASH1_BASE        (0x0FED0000UL)

#define EFLASH2_BASE        (0x10000000UL)
#define EFLASH2_LEN			(256 * 1024UL)

#define UBOOT_BASE          (EFLASH1_BASE)
#define UBOOT_LEN           (14 * 1024UL)

#define UBOOT_BAK_BASE   	(UBOOT_BASE + UBOOT_LEN)
#define UBOOT_BAK_LEN    	(14 * 1024UL)

#define CP_AMT_BASE      	(UBOOT_BAK_BASE + UBOOT_BAK_LEN)
#define CP_AMT_LEN      	(14 * 1024UL)

#define CP_NV_RO_BASE     	(CP_AMT_BASE + CP_AMT_LEN)
#define CP_NV_RO_LEN     	(16 * 1024UL)

#define CP_NV_RW_BASE    	(EFLASH2_BASE)
#define CP_NV_RW_VLEN      	(3500UL)
#define CP_NV_RW_TLEN     	(4 * 1024UL)

#define AP_NV_RW_SYS_BASE 	(CP_NV_RW_BASE + CP_NV_RW_TLEN)
#define AP_NV_RW_SYS_VLEN  	(500UL)
#define AP_NV_RW_SYS_TLEN  	(1024UL)

#define AP_NV_RW_USER_BASE 	(AP_NV_RW_SYS_BASE + AP_NV_RW_SYS_TLEN)
#define AP_NV_RW_USER_VLEN  (31500UL)
#define AP_NV_RW_USER_TLEN	(32 * 1024UL)

#define FOTA_BASE       	(AP_NV_RW_USER_BASE + AP_NV_RW_USER_TLEN )
#define FOTA_LEN        	(419 * 512UL)

#define PSMDATA_BASE        (FOTA_BASE + FOTA_LEN)
#define PSMDATA_LEN         (6 * 1024UL)

#define TBS_BASE            (0x1003F200)
#define TBS_LEN             (3 * 1024UL)

#define BOOT_FLAG_BASE   	(0x1003FE00)
#define BOOTFLAG_LEN		(512UL)

#if ((CP_NV_RW_TLEN + AP_NV_RW_SYS_TLEN + AP_NV_RW_USER_TLEN + FOTA_LEN +	\
	 PSMDATA_LEN + TBS_LEN + BOOTFLAG_LEN) != EFLASH2_LEN)
#error "EFLASH2 LEN error!"
#endif

/* cpu id */
#define CPU_ID_MIN	(0)
#define CPU_CK_ID   (0)
#define CPU_M0_ID   (1)
#define CPU_CEVA_ID (2)
#define CPU_ID_NR  	(3)

/* oss icp pipe */
#define OSS_ICP_PIPE_CK2CEVA_THREAD_BASE	(IRAM_ICP_PIPE_BASE)
#define OSS_ICP_PIPE_CK2CEVA_THREAD_SIZE	(192)
#define OSS_ICP_PIPE_CK2CEVA_IRQ_BASE		(OSS_ICP_PIPE_CK2CEVA_THREAD_BASE + OSS_ICP_PIPE_CK2CEVA_THREAD_SIZE)
#define OSS_ICP_PIPE_CK2CEVA_IRQ_SIZE 		(64)

#define OSS_ICP_PIPE_CEVA2CK_THREAD_BASE 	(OSS_ICP_PIPE_CK2CEVA_IRQ_BASE + OSS_ICP_PIPE_CK2CEVA_IRQ_SIZE)
#define OSS_ICP_PIPE_CEVA2CK_THREAD_SIZE	(192)
#define OSS_ICP_PIPE_CEVA2CK_IRQ_BASE		(OSS_ICP_PIPE_CEVA2CK_THREAD_BASE + OSS_ICP_PIPE_CEVA2CK_THREAD_SIZE)
#define OSS_ICP_PIPE_CEVA2CK_IRQ_SIZE		(64)

#define OSS_ICP_PIPE_M02CEVA_THREAD_BASE 	(IRAM_ICP_PIPE_BASE)
#define OSS_ICP_PIPE_M02CEVA_THREAD_SIZE	(192)
#define OSS_ICP_PIPE_M02CEVA_IRQ_BASE		(OSS_ICP_PIPE_M02CEVA_THREAD_BASE + OSS_ICP_PIPE_M02CEVA_THREAD_SIZE)
#define OSS_ICP_PIPE_M02CEVA_IRQ_SIZE 		(64)

#define OSS_ICP_PIPE_CEVA2M0_THREAD_BASE	(OSS_ICP_PIPE_M02CEVA_IRQ_BASE + OSS_ICP_PIPE_M02CEVA_IRQ_SIZE)
#define OSS_ICP_PIPE_CEVA2M0_THREAD_SIZE	(192)
#define OSS_ICP_PIPE_CEVA2M0_IRQ_BASE		(OSS_ICP_PIPE_CEVA2M0_THREAD_BASE + OSS_ICP_PIPE_CEVA2M0_THREAD_SIZE)
#define OSS_ICP_PIPE_CEVA2M0_IRQ_SIZE		(64)

/* oss icp channel */
#define OSS_ICP_CHANNEL_MIN		(0)
#define OSS_ICP_CHANNEL_AT_SEND	(0)
#define OSS_ICP_CHANNEL_AT_FREE	(1)
#define OSS_ICP_CHANNEL_AT_EMG	(2)
#define OSS_ICP_CHANNEL_PS		(3)
#define OSS_ICP_CHANNEL_RAMDUMP	(4)
#define OSS_ICP_CHANNEL_PSM		(5)
#define OSS_ICP_CHANNEL_ZCAT	(6)
#define OSS_ICP_CHANNEL_NR		(7)

#define OSS_ICP_CB_CTX_THREAD	(0)
#define OSS_ICP_CB_CTX_IRQ		(1)
#define OSS_ICP_CB_CTX_NR		(2)

#define OSS_ICP_CHANNEL_MSG_SIZE_MAX	(32)

/* oss event */
#define OSS_EVENT_ICP			(0)
#define OSS_EVENT_UART_0		(1)
#define OSS_EVENT_UART_1		(2)
#define OSS_EVENT_UART_2		(3)
#define OSS_EVENT_UART_3		(4)
#define OSS_EVENT_CPU_STATISTIC	(5)
#define OSS_EVENT_FREE      	(6)
#define OSS_EVENT_NR			(7)

/* oss nv */
#define OSS_NV_CFG_NR_MAX	(2)

#ifndef __ASSEMBLER__
/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
//bootrom flag nv
typedef struct {
  unsigned char run_mode; //download or boot
  unsigned char uart_timeout;
  unsigned char poweron_flag; 
  unsigned char uart_print_en;//0:disable;1:enable
  unsigned char ap_run_core; //0:m0;1;ck802;2:both
  unsigned char ap_icp_core; //CPU_CK_ID:ck802;CPU_M0_ID:m0
} boot_flag_cfg_t; //store based on BOOTFLAG_BASE,no longer than BOOTFLAG_LEN

typedef struct {
    void *data;
    unsigned int size;
} at_icp_msg_t;

/*******************************************************************************
 *                       Global variable declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                       Global function declarations                          *
 ******************************************************************************/
 
/*******************************************************************************
 *                      Inline function implementations                        *
 ******************************************************************************/
#endif
#endif  // #ifndef _INFRA_CONFIG_7100

