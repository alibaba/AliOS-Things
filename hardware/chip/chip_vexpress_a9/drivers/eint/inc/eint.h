/* Copyright Statement:
 *
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
	* any reproduction, modification, use or disclosure of MediaTek Software,
	* and information contained herein, in whole or in part, shall be strictly prohibited.
	*
	* MediaTek Inc. (C) 2018. All rights reserved.
	*
	* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
	* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
	* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
	* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
	* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
	* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
	* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
	* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
	* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
	* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
	* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
	* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
	* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
	* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
	* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
	* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
	* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
	* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
	*
	* The following software/firmware and/or related documentation ("MediaTek Software")
	* have been modified by MediaTek Inc. All revisions are subject to any receiver's
	* applicable license agreements with MediaTek Inc.
	*/

#ifndef __MT_EINT_H__
#define __MT_EINT_H__

	/*
	 * Define hardware registers.
	 */
#define EINT_STA_BASE         ((EINT_BASE + 0x000))
#define EINT_INTACK_BASE      ((EINT_BASE + 0x040))
#define EINT_MASK_BASE        ((EINT_BASE + 0x080))
#define EINT_MASK_SET_BASE    ((EINT_BASE + 0x0C0))
#define EINT_MASK_CLR_BASE    ((EINT_BASE + 0x100))
#define EINT_SENS_BASE        ((EINT_BASE + 0x140))
#define EINT_SENS_SET_BASE    ((EINT_BASE + 0x180))
#define EINT_SENS_CLR_BASE    ((EINT_BASE + 0x1C0))
#define EINT_SOFT_BASE        ((EINT_BASE + 0x200))
#define EINT_SOFT_SET_BASE    ((EINT_BASE + 0x240))
#define EINT_SOFT_CLR_BASE    ((EINT_BASE + 0x280))
#define EINT_POL_BASE         ((EINT_BASE + 0x300))
#define EINT_POL_SET_BASE     ((EINT_BASE + 0x340))
#define EINT_POL_CLR_BASE     ((EINT_BASE + 0x380))
#define EINT_D0_EN_BASE       ((EINT_BASE + 0x400))
#define EINT_D1_EN_BASE       ((EINT_BASE + 0x420))
#define EINT_DBNC_BASE        ((EINT_BASE + 0x500))
#define EINT_DBNC_SET_BASE    ((EINT_BASE + 0x600))
#define EINT_DBNC_CLR_BASE    ((EINT_BASE + 0x700))

	/*
	 * Register Placement
	 */
#define EINT_DBNC_SET_EN_BITS      (0)
#define EINT_DBNC_CLR_EN_BITS      (0)
#define EINT_DBNC_SET_RST_BITS     (1)
#define EINT_DBNC_CLR_RST_BITS     (1)
#define EINT_DBNC_SET_DBNC_BITS    (4)
#define EINT_DBNC_CLR_DBNC_BITS    (4)

	/*
	 * Register Setting
	 */
#define EINT_DBNC_SET_EN           (0x1)
#define EINT_DBNC_CLR_EN           (0x1)
#define EINT_DBNC_RST_EN           (0x1)
#define EINT_DBNC_0_MS             (0x7)

	/*
	 * Define constants.
	 */
#define EINT_MAX_CHANNEL        124
#define MAX_HW_DEBOUNCE_TIME    512000
#define EINT_REG_SET_SIZE       ((EINT_MAX_CHANNEL + 31) / 32)

	/*
	 * Define trigger signal
	 */
#define EDGE_SENSITIVE             (0)
#define LEVEL_SENSITIVE            (1)
#define HIGH_LEVEL_TRIGGER         (1)
#define LOW_LEVEL_TRIGGER          (0)

	/*
	 * Define configs
	 */
#define EINT_INT_MASK              (0)
#define EINT_INT_UNMASK            (1)
#define EINT_INT_AUTO_UNMASK_ON    (1)
#define EINT_INT_AUTO_UNMASK_OFF   (0)

	/*
	 * Define logger level
	 */

	/*
	 * Define EINT Number
	 */
#define EINT_NUMBER_0     0
#define EINT_NUMBER_1     1
#define EINT_NUMBER_2     2
#define EINT_NUMBER_3     3
#define EINT_NUMBER_4     4
#define EINT_NUMBER_5     5
#define EINT_NUMBER_6     6
#define EINT_NUMBER_7     7
#define EINT_NUMBER_8     8
#define EINT_NUMBER_9     9
#define EINT_NUMBER_10    10
#define EINT_NUMBER_11    11
#define EINT_NUMBER_12    12
#define EINT_NUMBER_13    13
#define EINT_NUMBER_14    14
#define EINT_NUMBER_15    15
#define EINT_NUMBER_16    16
#define EINT_NUMBER_17    17
#define EINT_NUMBER_18    18
#define EINT_NUMBER_19    19

	/*
	 * Define function prototypes.
	 */
	extern unsigned int mt_eint_get_mask(unsigned int eint_num);
	extern unsigned int mt_eint_read_mask(unsigned int eint_num);
	extern void mt_eint_mask(unsigned int eint_num);
	extern void mt_eint_unmask(unsigned int eint_num);
	extern unsigned int mt_eint_mask_save(unsigned int eint_num);
	extern void mt_eint_mask_restore(unsigned int eint_num, unsigned int val);
	extern unsigned int mt_eint_get_sens(unsigned int eint_num);
	extern unsigned int mt_eint_read_sens(unsigned int eint_num);
	extern void mt_eint_set_sens(unsigned int eint_num, unsigned int sens);
	extern unsigned int mt_eint_get_polarity(unsigned int eint_num);
	extern unsigned int mt_eint_read_polarity(unsigned int eint_num);
	extern void mt_eint_set_polarity(unsigned int eint_num, unsigned int pol);
	extern unsigned int mt_eint_get_soft(unsigned int eint_num);
	extern unsigned int mt_eint_read_soft(unsigned int eint_num);
	extern void mt_eint_soft_set(unsigned int eint_num);
	extern void mt_eint_soft_clr(unsigned int eint_num);
	extern void mt_eint_send_pulse(unsigned int eint_num);
	extern unsigned int mt_can_en_debounce(unsigned int eint_num);
	extern void mt_eint_dis_hw_debounce(unsigned int eint_num);
	extern void mt_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	extern unsigned int mt_eint_get_status(unsigned int eint_num);
	extern unsigned int mt_eint_read_status(unsigned int eint_num);
	extern void mt_eint_ack(unsigned int eint_num);
	extern void mt_eint_registration(unsigned int eint_num, unsigned int sens, unsigned int pol, void (EINT_FUNC_PTR)(int), unsigned int unmask, unsigned int is_auto_umask);
	extern void mt_eint_unregistration(unsigned int eint_num);
	extern int mt_eint_init(void);
	extern unsigned int mt_eint_get_def_pol(unsigned int eint_num);
	extern void mt_eint_dump_config(unsigned int eint_num);
	extern void mt_eint_dump_all_config(void);

	/*
	 * Define function
	 */
#define eint_mask_all               mt_eint_mask_all
#define eint_unmask_all             mt_eint_unmask_all
#define eint_mask_save_all          mt_eint_mask_save_all
#define eint_mask_restore_all       mt_eint_mask_restore_all
#define eint_get_mask               mt_eint_get_mask
#define eint_read_mask              mt_eint_read_mask
#define eint_mask                   mt_eint_mask
#define eint_unmask                 mt_eint_unmask
#define eint_mask_save              mt_eint_mask_save
#define eint_mask_restore           mt_eint_mask_restore
#define eint_get_sens               mt_eint_get_sens
#define eint_read_sens              mt_eint_read_sens
#define eint_set_sens               mt_eint_set_sens
#define eint_get_polarity           mt_eint_get_polarity
#define eint_read_polarity          mt_eint_read_polarity
#define eint_set_polarity           mt_eint_set_polarity
#define eint_get_soft               mt_eint_get_soft
#define eint_read_soft              mt_eint_read_soft
#define eint_soft_set               mt_eint_soft_set
#define eint_soft_clr               mt_eint_soft_clr
#define eint_send_pulse             mt_eint_send_pulse
#define eint_dis_hw_debounce        mt_eint_dis_hw_debounce
#define eint_set_hw_debounce        mt_eint_set_hw_debounce
#define eint_get_status             mt_eint_get_status
#define eint_read_status            mt_eint_read_status
#define eint_ack                    mt_eint_ack
#define eint_registration           mt_eint_registration
#define eint_unregistration         mt_eint_unregistration
#define eint_init                   mt_eint_init

#endif  /*MT_EINT_H_*/

