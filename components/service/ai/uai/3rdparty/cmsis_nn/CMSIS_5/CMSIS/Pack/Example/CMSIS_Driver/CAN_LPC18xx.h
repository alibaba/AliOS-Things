/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V1.0
 *
 * Project:      CAN Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __CAN_LPC18XX_H
#define __CAN_LPC18XX_H

#include <stdint.h>
#include <string.h>

#include "Driver_CAN.h"

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#ifndef RTE_CAN_CAN0
#define RTE_CAN_CAN0                   (0U)
#endif
#ifndef RTE_CAN_CAN1
#define RTE_CAN_CAN1                   (0U)
#endif

#if   ((RTE_CAN_CAN0 == 0U) && (RTE_CAN_CAN1 == 0U))
#error "No CAN is enabled in the RTE_Device.h!"
#endif

#if    (RTE_CAN_CAN1 == 1U)
#define CAN_CTRL_NUM                   (2U)
#else
#define CAN_CTRL_NUM                   (1U)
#endif

// CNTL register bit definitions
#define CNTL_INIT_Msk                  (1U      <<  0)
#define CNTL_IE_Msk                    (1U      <<  1)
#define CNTL_SIE_Msk                   (1U      <<  2)
#define CNTL_EIE_Msk                   (1U      <<  3)
#define CNTL_DAR_Msk                   (1U      <<  5)
#define CNTL_CCE_Msk                   (1U      <<  6)
#define CNTL_TEST_Msk                  (1U      <<  7)

// STAT register bit definitions
#define STAT_LEC_Msk                   (7U      <<  0)
#define STAT_TXOK_Msk                  (1U      <<  3)
#define STAT_RXOK_Msk                  (1U      <<  4)
#define STAT_EPASS_Msk                 (1U      <<  5)
#define STAT_EWARN_Msk                 (1U      <<  6)
#define STAT_BOFF_Msk                  (1U      <<  7)

// EC register bit definitions
#define EC_TEC_7_0_Msk                 (0xFFU   <<  0)
#define EC_REC_6_0_Msk                 (0x7FU   <<  8)
#define EC_RP_Msk                      (1U      << 15)

// BT register bit definitions
#define BT_BRP_Msk                     (0x3FU   <<  0)
#define BT_SJW_Msk                     (3U      <<  6)
#define BT_TSEG1_Msk                   (0x0FU   <<  8)
#define BT_TSEG2_Msk                   (0x07U   << 12)

// TEST register bit definitions
#define TEST_BASIC_Msk                 (1U      <<  2)
#define TEST_SILENT_Msk                (1U      <<  3)
#define TEST_LBACK_Msk                 (1U      <<  4)
#define TEST_TX1_0_Msk                 (3U      <<  5)
#define TEST_RX_Msk                    (1U      <<  7)

// BRPE register bit definitions
#define BRPE_BRPE_Msk                  (0x0FU   <<  0)

// CMDREQ register bit definitions
#define IF_CMDREQ_MESSABE_NUMBER_Msk   (0x3FU   <<  0)
#define IF_CMDREQ_BUSY_Msk             (1U      << 15)

// CMDMSK_W/R register bit definitions
#define IF_CMDMSK_DATA_B_Msk           (1U      <<  0)
#define IF_CMDMSK_DATA_A_Msk           (1U      <<  1)
#define IF_CMDMSK_TXRQST_Msk           (1U      <<  2)
#define IF_CMDMSK_NEWDAT_Msk           (1U      <<  2)
#define IF_CMDMSK_CLRINTPND_Msk        (1U      <<  3)
#define IF_CMDMSK_CTRL_Msk             (1U      <<  4)
#define IF_CMDMSK_ARB_Msk              (1U      <<  5)
#define IF_CMDMSK_MASK_Msk             (1U      <<  6)
#define IF_CMDMSK_WR_RD_Msk            (1U      <<  7)

// MSK1 register bit definitions
#define IF_MSK1_MSK15_0_Msk            (0xFFFFU <<  0)

// MSK2 register bit definitions
#define IF_MSK2_MSK28_16_Msk           (0x1FFFU <<  0)
#define IF_MSK2_MDIR_Msk               (1U      << 14)
#define IF_MSK2_MXTD_Msk               (1U      << 15)

// ARB1 register bit definitions
#define IF_ARB1_ID15_0_Msk             (0xFFFFU <<  0)

// ARB2 register bit definitions
#define IF_ARB2_ID28_16_Msk            (0x1FFFU <<  0)
#define IF_ARB2_DIR_Msk                (1U      << 13)
#define IF_ARB2_XTD_Msk                (1U      << 14)
#define IF_ARB2_MSGVAL_Msk             (1U      << 15)

// MCTRL register bit definitions
#define IF_MCTRL_DLC3_0_Msk            (0x0FU   <<  0)
#define IF_MCTRL_EOB_Msk               (1U      <<  7)
#define IF_MCTRL_TXRQST_Msk            (1U      <<  8)
#define IF_MCTRL_RMTEN_Msk             (1U      <<  9)
#define IF_MCTRL_RXIE_Msk              (1U      << 10)
#define IF_MCTRL_TXIE_Msk              (1U      << 11)
#define IF_MCTRL_UMASK_Msk             (1U      << 12)
#define IF_MCTRL_INTPND_Msk            (1U      << 13)
#define IF_MCTRL_MSGLST_Msk            (1U      << 14)
#define IF_MCTRL_NEWDAT_Msk            (1U      << 15)

#endif // __CAN_LPC18XX_H
