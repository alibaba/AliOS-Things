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

#include <eint.h>
#include "FreeRTOS.h"
#include <stdio.h>
#include <mt_reg_base.h>
#include <mt_gic_v3.h>
#include <mt_printf.h>
#include "task.h"

#if 0
#define EINT_SW_DEBOUNCE
#endif

#define readl(addr)  (*(volatile unsigned int *)(addr))
#define writel(addr, data)  ((*(volatile unsigned int *)(addr)) = (unsigned int)data)
#define WRITE_REG(v, a) writel(a, v)
#define READ_REG(a) readl(a)

#define EINT_IRQ	(118)

/*
 * Define internal data structures.
 */
typedef struct {
	void (*eint_func[EINT_MAX_CHANNEL])(int);
	unsigned int eint_auto_umask[EINT_MAX_CHANNEL];
	unsigned int is_deb_en[EINT_MAX_CHANNEL];
	unsigned int deb_time[EINT_MAX_CHANNEL];
#ifdef EINT_SW_DEBOUNCE
	struct timer_list eint_sw_deb_timer[EINT_MAX_CHANNEL];
#endif
} eint_func;

typedef struct {
	void (*eint_func[EINT_MAX_CHANNEL])(int);
	unsigned int eint_auto_umask[EINT_MAX_CHANNEL];
	unsigned int is_deb_en[EINT_MAX_CHANNEL];
	unsigned int deb_time[EINT_MAX_CHANNEL];
	unsigned int sens[EINT_MAX_CHANNEL];
	unsigned int pol[EINT_MAX_CHANNEL];
} eint_conf;

/*
 * Define global variables.
 */
static eint_func EINT_FUNC;
static eint_conf EINT_CONF;

unsigned int EINT_Mask_Store[EINT_REG_SET_SIZE];
unsigned int EINT_Domain0_Store[EINT_REG_SET_SIZE];


/**********************
 *
 * Function - Mask
 *
 **********************/

/*
 * mt_eint_mask_all: Mask all the specified EINT number.
 */
void mt_eint_mask_all(void)
{
	unsigned int i;
	unsigned int val = 0xFFFFFFFF;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(val, (EINT_MASK_SET_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_unmask_all: Unmask all the specified EINT number.
 */
void mt_eint_unmask_all(void)
{
	unsigned int i;
	unsigned int val = 0xFFFFFFFF;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(val, (EINT_MASK_CLR_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_mask_save_all: Save and Mask the specified EINT number.
 * @eint_num: EINT number to mask
 */
void mt_eint_mask_save_all(void)
{
	unsigned int i;
	unsigned int val = 0xFFFFFFFF;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		EINT_Mask_Store[i] = READ_REG((EINT_MASK_BASE + (i * 4)));
		WRITE_REG(val, (EINT_MASK_SET_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_mask_restore_all: Restore the specified EINT number.
 * @eint_num: EINT number to mask
 */
void mt_eint_mask_restore_all(void)
{
	unsigned int i;
	unsigned int val = 0xFFFFFFFF;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(val, (EINT_MASK_CLR_BASE + (i * 4)));
		WRITE_REG(EINT_Mask_Store[i], (EINT_MASK_SET_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_get_mask: To get the eint mask
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_get_mask(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_MASK_BASE));
		return st;
	} else {
		return 0;
	}
}

/*
 * mt_eint_read_mask: To get the specified eint mask
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_read_mask(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_MASK_BASE));
		return ((st & bit) ? 1 : 0);
	} else {
		return 0;
	}
}

/*
 * mt_eint_mask: Mask the specified EINT number.
 * @eint_num: EINT number to mask
 */
void mt_eint_mask(unsigned int eint_num)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_MASK_SET_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_eint_unmask: Unmask the specified EINT number.
 * @eint_num: EINT number to unmask
 */
void mt_eint_unmask(unsigned int eint_num)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_MASK_CLR_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_eint_mask_save: Save and Mask the specified EINT number.
 * @eint_num: EINT number to mask
 */
unsigned int mt_eint_mask_save(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_MASK_BASE));
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_MASK_SET_BASE));
		return (st & bit);
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
		return 0;
	}
}

/*
 * mt_eint_mask_restore: Restore the specified EINT number.
 * @eint_num: EINT number to mask
 */
void mt_eint_mask_restore(unsigned int eint_num, unsigned int val)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		if(val == 0) {
			WRITE_REG (bit, ((eint_num / 32) * 4 + EINT_MASK_CLR_BASE));
		} else {
			WRITE_REG (bit, ((eint_num / 32) * 4 + EINT_MASK_SET_BASE));
		}
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}


/**********************
 *
 * Function - Sensitivity
 *
 **********************/

/*
 * mt_eint_get_sens: To get the eint sens
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_get_sens(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_SENS_BASE));
		return st;
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
		return 0;
	}
}

/*
 * mt_eint_read_sens: To get the specified eint sens
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_read_sens(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_SENS_BASE));
		return ((st & bit) ? 1 : 0);
	} else {
		return 0;
	}
}

/*
 * mt_eint_set_sens: Set the sensitivity for the specified EINT number.
 * @eint_num: EINT number to set
 * @sens: sensitivity to set
 */
void mt_eint_set_sens(unsigned int eint_num, unsigned int sens)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(sens == EDGE_SENSITIVE) {
		if(eint_num < EINT_MAX_CHANNEL) {
			WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SENS_CLR_BASE));
		} else {
			printf("[EINT] num. exceeds the max num.\n\r");
		}
	} else if(sens == LEVEL_SENSITIVE) {
		if(eint_num < EINT_MAX_CHANNEL) {
			WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SENS_SET_BASE));
		} else {
			printf("[EINT] num. exceeds the max num.\n\r");
		}
	} else {
		printf("[EINT] wrong sensitivity parameter!");
	}

	return;
}


/**********************
 *
 * Function - Polarity
 *
 **********************/

/*
 * mt_eint_get_polarity: To get the polarity status
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_get_polarity(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_POL_BASE));
		return st;
	} else {
		return 0;
	}
}

/*
 * mt_eint_read_polarity: To get the specified polarity status
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_read_polarity(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_POL_BASE));
		return ((st & bit) ? 1 : 0);
	} else {
		return 0;
	}
}

/*
 * mt_eint_set_polarity: Set the polarity for the specified EINT number.
 * @eint_num: EINT number to set
 * @pol: polarity to set
 */
void mt_eint_set_polarity(unsigned int eint_num, unsigned int pol)
{
	unsigned int mask_flag;
	unsigned int bit = 1 << (eint_num % 32);

	mask_flag = mt_eint_mask_save(eint_num);

	if(pol == LOW_LEVEL_TRIGGER) {
		if (eint_num < EINT_MAX_CHANNEL) {
			WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_POL_CLR_BASE));
		} else {
			printf("[EINT] num. exceeds the max num.\n\r");
		}
	} else if(pol == HIGH_LEVEL_TRIGGER) {
		if (eint_num < EINT_MAX_CHANNEL) {
			WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_POL_SET_BASE));
		} else {
			printf("[EINT] num. exceeds the max num.\n\r");
		}
	} else {
		printf("[EINT] wrong polarity parameter!");
	}

	mt_eint_mask_restore(eint_num, mask_flag);

	return;
}


/**********************
 *
 * Function - Soft Interrupt
 *
 **********************/

/*
 * mt_eint_get_soft: To get the eint soft interrupt status
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_get_soft(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_SOFT_BASE));
		return st;
	} else {
		return 0;
	}
}

/*
 * mt_eint_read_soft: To get the specified eint soft interrupt status
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_read_soft(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_SOFT_BASE));
		return ((st & bit) ? 1 : 0);
	} else {
		return 0;
	}
}

/*
 * mt_eint_soft_set: Assert the specified EINT number.
 * @eint_num: EINT number to set
 */
void mt_eint_soft_set(unsigned int eint_num)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SOFT_SET_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_eint_soft_clr: Deassert the specified EINT number.
 * @eint_num: EINT number to clear
 */
void mt_eint_soft_clr(unsigned int eint_num)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SOFT_CLR_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * eint_send_pulse: Send a pulse on the specified EINT number.
 * @eint_num: EINT number to send
 */
void mt_eint_send_pulse(unsigned int eint_num)
{
	unsigned int mask_flag;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		mask_flag = mt_eint_mask_save(eint_num);

		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SOFT_SET_BASE));

		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_SOFT_CLR_BASE));

		mt_eint_mask_restore(eint_num, mask_flag);
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}


/**********************
 *
 * Function - Debounce
 *
 **********************/

/*
 * mt_can_en_debounce: Check the EINT number is able to enable debounce or not
 * @eint_num: the EINT number to set
 */
unsigned int mt_can_en_debounce(unsigned int eint_num)
{
	unsigned int sens = mt_eint_read_sens(eint_num);

	/* debounce: debounce time is not 0 && it is not edge sensitive */
	if(eint_num < EINT_MAX_CHANNEL && sens != 0) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * mt_eint_dis_hw_debounce: To disable hw debounce
 * @eint_num: the EINT number to set
 */
void mt_eint_dis_hw_debounce(unsigned int eint_num)
{
	unsigned int bit;

	EINT_FUNC.is_deb_en[eint_num] = 0;
	EINT_FUNC.deb_time[eint_num] = 0;

	bit = (EINT_DBNC_CLR_EN << EINT_DBNC_CLR_EN_BITS) << ((eint_num % 4) * 8);
	WRITE_REG(bit, ((eint_num / 4) * 4 + EINT_DBNC_CLR_BASE));

	return;
}

/*
 * mt_eint_set_hw_debounce: Set the hardware de-bounce time for the specified EINT number.
 * @eint_num: EINT number to acknowledge
 * @us: the de-bounce time to set (in microseconds)
 */
void mt_eint_set_hw_debounce(unsigned int eint_num, unsigned int us)
{
	unsigned int dbnc, bit, clr_bit, rst;
	unsigned int mask_flag;

	if(!mt_can_en_debounce(eint_num)) {
		printf("Can't enable debounce of eint_num:%d in %s\n\r", eint_num, "mt_eint_set_hw_debounce");
		return;
	}

	if(us >= MAX_HW_DEBOUNCE_TIME) {
		printf("Can't enable debounce time of %d eint_num:%d, HW maximum debounce time is %d\n\r", us, eint_num, MAX_HW_DEBOUNCE_TIME);
		return;
	}

	if (us == 0) {
		dbnc = 0;
	} else if (us <= 250) {
		dbnc = 1;
	} else if (us <= 500) {
		dbnc = 2;
	} else if (us <= 1000) {
		dbnc = 3;
	} else if (us <= 16000) {
		dbnc = 4;
	} else if (us <= 32000) {
		dbnc = 5;
	} else if (us <= 64000) {
		dbnc = 6;
	} else if (us <= 128000) {
		dbnc = 7;
	} else if (us <= 256000) {
		dbnc = 8;
	//} else if (us <= 512000) {
	//	dbnc = 9;
	} else {
		dbnc = 9;
	}

	/* setp 1: mask the EINT */
	mask_flag = mt_eint_mask_save(eint_num);

	/* step 2: Check hw debouce number to decide which type should be used */

	/* step 2.1: disable debounce */
	mt_eint_dis_hw_debounce(eint_num);

	EINT_FUNC.is_deb_en[eint_num] = 1;
	EINT_FUNC.deb_time[eint_num] = us;

	/* step 2.2: clear register */
	clr_bit = 0xFF << ((eint_num % 4) * 8);
	WRITE_REG(clr_bit, ((eint_num / 4) * 4 + EINT_DBNC_CLR_BASE));

	/* step 2.3: reset counter and dsb */
	rst = (EINT_DBNC_RST_EN << EINT_DBNC_SET_RST_BITS) << ((eint_num % 4) * 8);
	WRITE_REG(rst, ((eint_num / 4) * 4 + EINT_DBNC_SET_BASE));

	/* step 2.4: set new debounce value & enable function */
	bit = ((dbnc << EINT_DBNC_SET_DBNC_BITS) | (EINT_DBNC_SET_EN << EINT_DBNC_SET_EN_BITS)) << ((eint_num % 4) * 8);
	WRITE_REG(bit, ((eint_num / 4) * 4 + EINT_DBNC_SET_BASE));

	/* step 3: unmask the EINT */
	mt_eint_mask_restore(eint_num, mask_flag);

	return;
}

#ifdef EINT_SW_DEBOUNCE
/*
 * mt_eint_en_sw_debounce: To set EINT_FUNC.is_deb_en[eint_num] enable
 * @eint_num: the EINT number to set
 */
void mt_eint_en_sw_debounce(unsigned int eint_num)
{
	EINT_FUNC.is_deb_en[eint_num] = 1;
}
#endif


/**********************
 *
 * Function - Domain
 *
 **********************/

/*
 * mt_set_domain0_all: Enable all EINT on domain0.
 */
void mt_eint_domain0_set_all(void)
{
	unsigned int val = 0xFFFFFFFF, i;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(val, (EINT_D0_EN_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_set_domain0_all: Disable all EINT on domain0.
 */
void mt_eint_domain0_clr_all(void)
{
	unsigned int val = 0x0, i;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(val, (EINT_D0_EN_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_domain0_save_all: Save and disable all EINT on domain0.
 * @eint_num: the EINT number to set
 */
void mt_eint_domain0_save_all(void)
{
	unsigned int val = 0x0, i;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		EINT_Domain0_Store[i] = READ_REG((EINT_D0_EN_BASE + (i * 4)));
		WRITE_REG(val, (EINT_D0_EN_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_eint_domain0_restore: Restore all EINT on domain0.
 * @eint_num: the EINT number to clear
 */
void mt_eint_domain0_restore_all(void)
{
	unsigned int i;

	for(i = 0; i < EINT_REG_SET_SIZE; i++) {
		WRITE_REG(EINT_Domain0_Store[i], (EINT_D0_EN_BASE + (i * 4)));
	}

	return;
}

/*
 * mt_set_domain0_all: Enable specific EINT on domain0.
 * @eint_num: the EINT number to set
 */
void mt_eint_domain0_set(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		st |= (1 << (eint_num % 32));
		WRITE_REG(st, ((eint_num / 32) * 4 + EINT_D0_EN_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_domain0_clr: Disable specific EINT on domain0.
 * @eint_num: the EINT number to clear
 */
void mt_eint_domain0_clr(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		st &= ~(1 << (eint_num % 32));
		WRITE_REG(st, ((eint_num / 32) * 4 + EINT_D0_EN_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_eint_domain0_save: Save and diable specific EINT on domain0.
 * @eint_num: the EINT number to set
 */
unsigned int mt_eint_domain0_save(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		WRITE_REG((st & (~bit)), ((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		return (st & bit);
	} else {
		return 0;
	}
}

/*
 * mt_eint_domain0_restore: Restore specific EINT on domain0.
 * @eint_num: the EINT number to clear
 */
void mt_eint_domain0_restore(unsigned int eint_num, unsigned int val)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		if(val == 0) {
			st = READ_REG(((eint_num / 32) * 4 + EINT_D0_EN_BASE));
			WRITE_REG((st & (~bit)), ((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		} else {
			st = READ_REG(((eint_num / 32) * 4 + EINT_D0_EN_BASE));
			WRITE_REG((st | bit), ((eint_num / 32) * 4 + EINT_D0_EN_BASE));
		}
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}


/**********************
 *
 * Function - Status
 *
 **********************/

/*
 * mt_eint_get_status: To get the interrupt status
 * @eint_num: the EINT number to get
 */
unsigned int mt_eint_get_status(unsigned int eint_num)
{
	unsigned int st;

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_STA_BASE));
		return st;
	} else {
		return 0;
	}
}

/*
 * mt_eint_read_status: To read the specific interrupt status
 * @eint_num: the EINT number to set
 */
unsigned int mt_eint_read_status(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		st = READ_REG(((eint_num / 32) * 4 + EINT_STA_BASE));
		return ((st & bit) ? 1 : 0);
	} else {
		return 0;
	}
}

/*
 * mt_eint_ack: To ack the interrupt
 * @eint_num: the EINT number to ack
 */
void mt_eint_ack(unsigned int eint_num)
{
	unsigned int bit = 1 << (eint_num % 32);

	if(eint_num < EINT_MAX_CHANNEL) {
		WRITE_REG(bit, ((eint_num / 32) * 4 + EINT_INTACK_BASE));
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}


/**********************
 *
 * Function - Init and Registration
 *
 **********************/

/*
 * eint_lisr: EINT interrupt service routine.
 * *
 */
static void mt_eint_lisr(void)
{
	unsigned int index;
	unsigned int status = 0;
	unsigned int status_check;


	for(index = 0; index < EINT_MAX_CHANNEL; index++) {
		/* read & print status register every 32 interrupts */
		if((index % 32) == 0) {
			status = mt_eint_get_status(index);
		}

		status_check = status & (1 << (index % 32));

		/* execute EINT callback function */
		if(status_check) {
			mt_eint_mask(index);

			mt_eint_ack(index);

			if(EINT_FUNC.eint_func[index]) {
				EINT_FUNC.eint_func[index](index);
			}

			if(EINT_FUNC.eint_auto_umask[index]) {
				mt_eint_unmask(index);
			}
		}
	}

	return ;
}
/*
 * eint_registration: register a EINT.
 * @eint_num: the EINT number to register
 * @sens: sensitivity value,1: level trigger,  0:edge trigger
 * @pol: polarity value, 1: high level trigger, 0: low level trigger
 * @EINT_FUNC_PTR: the ISR callback function
 * @unmask: unmask interrupt, 1: unmask,  0: do nothing
 * @is_auto_unmask: the indication flag of auto unmasking after ISR callback is processed,1: autounmask  0:do nothing

 */
void mt_eint_registration(unsigned int eint_num, unsigned int sens, unsigned int pol, void (EINT_FUNC_PTR)(int), unsigned int unmask, unsigned int is_auto_umask)
{
	if (eint_num < EINT_MAX_CHANNEL) {

		EINT_FUNC.eint_func[eint_num] = EINT_FUNC_PTR;

		EINT_FUNC.eint_auto_umask[eint_num] = is_auto_umask;

		mt_eint_domain0_set(eint_num);

		mt_eint_set_sens(eint_num,sens);

		mt_eint_set_polarity(eint_num, pol);

		mt_eint_ack(eint_num);

		if(unmask) {
			mt_eint_unmask(eint_num);
		}

	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
	}

	return;
}

/*
 * mt_eint_get_def_pol: get default polarity of specified EINT number.
 * @eint_num: EINT number to get
 */
unsigned int mt_eint_get_def_pol (unsigned int eint_num)
{
	int mask, sens, pol, domain;
	unsigned int def_pol;

	/* Save Config */
	domain = mt_eint_domain0_save(eint_num);

	mask = mt_eint_mask_save(eint_num);
	sens = mt_eint_read_sens(eint_num);
	pol = mt_eint_read_polarity(eint_num);

	/* Get default polarity */
	mt_eint_set_sens(eint_num, LEVEL_SENSITIVE);
	mt_eint_set_polarity(eint_num, LOW_LEVEL_TRIGGER);
	mt_eint_ack(eint_num);
	mt_eint_unmask(eint_num);

	if(mt_eint_read_status(eint_num)) {
		def_pol = LOW_LEVEL_TRIGGER;
	} else {
		def_pol = HIGH_LEVEL_TRIGGER;
	}

	/* Restore Config */
	mt_eint_set_polarity(eint_num, (pol ? HIGH_LEVEL_TRIGGER : LOW_LEVEL_TRIGGER));
	mt_eint_set_sens(eint_num, (sens ? LEVEL_SENSITIVE: EDGE_SENSITIVE));
	mt_eint_mask_restore(eint_num, mask);

	mt_eint_ack(eint_num);
	mt_eint_domain0_restore(eint_num, domain);

	return def_pol;
}

/*
 * mt_eint_save_all_config: save all eint configs.
 */
void mt_eint_save_all_config(void)
{
	int i;

	mt_eint_domain0_save_all();

	mt_eint_mask_save_all();

	for (i = 0; i < EINT_MAX_CHANNEL; i++) {
		EINT_CONF.eint_func[i] = EINT_FUNC.eint_func[i];
		EINT_CONF.eint_auto_umask[i] = EINT_FUNC.eint_auto_umask[i];
		EINT_CONF.is_deb_en[i] = EINT_FUNC.is_deb_en[i];
		EINT_CONF.deb_time[i] = EINT_FUNC.deb_time[i];
		EINT_CONF.sens[i] = mt_eint_read_sens(i);
		EINT_CONF.pol[i] = mt_eint_read_polarity(i);
	}

	return;
}

/*
 * mt_eint_store_all_config: restore all eint configs.
 */
void mt_eint_store_all_config(void)
{
	int i;

	for (i = 0; i < EINT_MAX_CHANNEL; i++) {
		EINT_FUNC.eint_func[i] = EINT_CONF.eint_func[i];
		EINT_FUNC.eint_auto_umask[i] = EINT_CONF.eint_auto_umask[i];
		EINT_FUNC.is_deb_en[i] = EINT_CONF.is_deb_en[i];
		EINT_FUNC.deb_time[i] = EINT_CONF.deb_time[i];
		mt_eint_set_sens(i, (EINT_CONF.sens[i] ? LEVEL_SENSITIVE: EDGE_SENSITIVE));
		mt_eint_set_polarity(i, (EINT_CONF.pol[i] ? HIGH_LEVEL_TRIGGER : LOW_LEVEL_TRIGGER));
		mt_eint_ack(i);
	}

	mt_eint_mask_restore_all();

	return;
}

/*
 * eint_init: initialize EINT driver.
 * Always return 0.
 */
int mt_eint_init(void)
{
	static int init = 0;
	unsigned int i;

	if (!init) {
		init = 1;
	} else {
		return 0;
	}


	printf("[EINT]driver init\n\r");

	for(i = 0; i < EINT_MAX_CHANNEL; i++) {
		EINT_FUNC.eint_func[i] = NULL;
		EINT_FUNC.eint_auto_umask[i] = 0;
		EINT_FUNC.is_deb_en[i] = 0;
		EINT_FUNC.deb_time[i] = 0;
#ifdef EINT_SW_DEBOUNCE
		EINT_FUNC.eint_sw_deb_timer[i].expires = 0;
		EINT_FUNC.eint_sw_deb_timer[i].data = 0;
		EINT_FUNC.eint_sw_deb_timer[i].function = NULL;
#endif
	}

	for(i = 0; i < EINT_MAX_CHANNEL; i++) {
		EINT_CONF.eint_func[i] = NULL;
		EINT_CONF.eint_auto_umask[i] = 0;
		EINT_CONF.is_deb_en[i] = 0;
		EINT_CONF.deb_time[i] = 0;
		EINT_CONF.sens[i] = 0;
		EINT_CONF.pol[i] = 0;

	}

	/* init EINT interrupt ack  */
	for(i = 0; i < EINT_MAX_CHANNEL; i++) {
		mt_eint_ack(i);
	}

	/* request SCP IRQ  */
	request_irq(EINT_IRQ, mt_eint_lisr, IRQ_TYPE_LEVEL_HIGH, "EINT", NULL);
	#if 0
	for( i=0; i<128; i++)
	{
		printf("eint %d mask %d\n", i, mt_eint_read_mask(i));
	}
	#endif
	return 0;
}

/*
 * mt_eint_dump_config: To get the specified eint config
 * @eint_num: the EINT number to dump hw configuration
 */
void mt_eint_dump_config(unsigned int eint_num)
{
	unsigned int st;
	unsigned int bit;
	unsigned int eint_mask;
	unsigned int eint_sens;
	unsigned int eint_pol;
	unsigned int is_deb_en;
	unsigned int deb_time;
	unsigned int deb_reg_value;
	unsigned int int_status;
	void (*eint_func)(int);

	if(eint_num < EINT_MAX_CHANNEL) {
		bit = 1 << (eint_num % 32);
		st = READ_REG(((eint_num / 32) * 4 + EINT_MASK_BASE));
		eint_mask = bit & st;
		if( eint_mask > 0)
			eint_mask = 1;
		st = READ_REG(((eint_num / 32) * 4 + EINT_SENS_BASE));
		eint_sens = bit & st;
		if( eint_sens > 0)
			eint_sens = 1;
		st = READ_REG(((eint_num / 32) * 4 + EINT_POL_BASE));
		eint_pol = bit & st;
		if( eint_pol > 0)
			eint_pol = 1;
		deb_time = EINT_FUNC.deb_time[eint_num];

		deb_reg_value = READ_REG(((eint_num / 4) * 4 + EINT_DBNC_BASE));
		deb_reg_value = deb_reg_value >> ((eint_num % 4) * 8);
		is_deb_en = deb_reg_value & 0x01;
		deb_reg_value = deb_reg_value & 0x70;
		deb_reg_value = deb_reg_value >> 4;
		eint_func = EINT_FUNC.eint_func[eint_num];
		int_status = mt_eint_get_status(eint_num);

		printf("[EINT] number   = %d\n\r",eint_num);
		printf("[EINT] mask     = %d\n\r",eint_mask);
		printf("[EINT] sens     = %d\n\r",eint_sens);
		printf("[EINT] pol      = %d\n\r",eint_pol);
		printf("[EINT] deb_en   = %d\n\r",is_deb_en);
		printf("[EINT] deb_time = %d\n\r",deb_time);
		printf("[EINT] deb_reg_value = %d\n\r",deb_reg_value);
		printf("[EINT] eint_func = %p\n\r",eint_func);
		printf("[EINT] int_status = %d\n\r",int_status);

		return;
	} else {
		printf("[EINT] num. exceeds the max num.\n\r");
		return;
	}
}

/*
 * mt_eint_dump_all_config: dump all eint configs.
 */
void mt_eint_dump_all_config(void)
{
	unsigned int st;
	unsigned int bit;
	unsigned int eint_mask;
	unsigned int eint_sens;
	unsigned int eint_pol;
	unsigned int is_deb_en;
	unsigned int deb_time;
	unsigned int deb_reg_value;
	unsigned int int_status;
	void (*eint_func)(int);
	int eint_num;
	printf("[EINT]\n\r");
	printf("eint_id\tmask\tsens\tpol\tdeb_en\tdeb_ms\tdeb_reg\tfunc\tint_sta\n\r");
	for (eint_num = 0; eint_num < EINT_MAX_CHANNEL; eint_num++) {
		bit = 1 << (eint_num % 32);
		st = READ_REG(((eint_num / 32) * 4 + EINT_MASK_BASE));
		eint_mask = bit & st;
		if( eint_mask > 0)
			eint_mask = 1;
		st = READ_REG(((eint_num / 32) * 4 + EINT_SENS_BASE));
		eint_sens = bit & st;
		if( eint_sens > 0)
			eint_sens = 1;
		st = READ_REG(((eint_num / 32) * 4 + EINT_POL_BASE));
		eint_pol = bit & st;
		if( eint_pol > 0)
			eint_pol = 1;
		deb_time = EINT_FUNC.deb_time[eint_num];

		deb_reg_value = READ_REG(((eint_num / 4) * 4 + EINT_DBNC_BASE));
		deb_reg_value = deb_reg_value >> ((eint_num % 4) * 8);
		is_deb_en = deb_reg_value & 0x01;
		deb_reg_value = deb_reg_value & 0x70;
		deb_reg_value = deb_reg_value >> 4;
		eint_func = EINT_FUNC.eint_func[eint_num];
		int_status = mt_eint_get_status(eint_num);

		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%p\t%d\n\r",eint_num,eint_mask,eint_sens,eint_pol,is_deb_en,deb_time,deb_reg_value,eint_func,int_status);
	}
	return;
}

