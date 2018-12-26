/**
 * @file zx29_bmu.h
 * @brief Public APIs of PMU drivers
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * @author yangsheng <yang.sheng@sanechips.com.cn>
 * @ingroup si_ap_drv_pmu_id 
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

#ifndef _BMU_SI_H
#define _BMU_SI_H
/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <reg_define_si.h>

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/ 
typedef struct {
	//deviceinfo for aliyun(180bytes)
	unsigned char hostName[36]; 	  //demo 19
	unsigned char productKey[36];	  //demo 8
	unsigned char productSecret[36];  //demo 16
	unsigned char deviceName[36];	  //demo 5
	unsigned char deviceSecret[36];   //demo 16
	unsigned char needStartApp; //not start:0xEE, start: others
	unsigned char select;
    unsigned char reserved[2];
	
}yunos_cfg_t;

typedef struct {
    unsigned char needStartApp; //not start:0xEE, start: others
	unsigned char reserved[3];
}zephyr_cfg_t;

typedef struct {
  unsigned char versionExt[32];
  unsigned char versionInner[32];
  unsigned int wakeupCyc; 			  //seconds for rtc 
  unsigned int psmflag;
  unsigned char exceptResetflag;    //open:1, close:0

  unsigned char reserved[231];
  
  union{
		yunos_cfg_t  	yunosCfg;
		zephyr_cfg_t 	zephyrCfg;
  };
} ap_cfg_t; //

#define SI_PMU_VERSION         		(0x0101)
#define SI_PMU_BASE       			PMU_CTRL_BASEADDR
#define SI_LPM_RST_REG       		(LPM_BASEADDR+0x0)
#define SI_SBY_CRPM_RTCCLKEN_REG  	(SBY_CRPM_BASEADDR+0x1c)
#define SI_LPM_RST_EN_SHIFT			(0)
#define SI_SBY_CRPM_RTC_CLKEN_SHIFT	(2)

#define VBAT_DET_INT_MASK_OFFSET		0x4c
#define SDNCMP_EN_COUNTER               SDNCMP_EN_COUNTER
#define SDNCMP_EN_COUNTER_SHIFT         (16)
#define SDNCMP_EN_COUNTER_MASK          (BIT_MASK(8) << SDNCMP_EN_COUNTER_SHIFT)

#define OUT_SDNCMP_FALL_INT_MASK		OUT_SDNCMP_FALL_INT_MASK
#define OUT_SDNCMP_FALL_INT_MASK_SHIFT	(1)/*bit1*/
#define OUT_SDNCMP_FALL_INT_MASK_MASK	(BIT_MASK(1) << OUT_SDNCMP_FALL_INT_MASK_SHIFT)

#define SDN_EN_FALL_INT_MASK			SDN_EN_FALL_INT_MASK
#define SDN_EN_FALL_INT_MASK_SHIFT		(0)
#define SDN_EN_FALL_INT_MASK_MASK		(BIT_MASK(1) << SDN_EN_FALL_INT_MASK_SHIFT)

#define INT_WAKEUP_CLEAR2_CONFIG        0x28/*bit 8~9*/
#define SDNCMP_EN_FALL_INT_CLEAR        SDNCMP_EN_FALL_INT_CLEAR
#define SDNCMP_EN_FALL_INT_CLEAR_SHIFT  (9)
#define SDNCMP_EN_FALL_INT_CLEAR_MASK   (BIT_MASK(1) << SDNCMP_EN_FALL_INT_CLEAR_SHIFT)

#define OUT_SDNCMP_FALL_INT_CLEAR       OUT_SDNCMP_FALL_INT_CLEAR
#define OUT_SDNCMP_FALL_INT_CLEAR_SHIFT (8)
#define OUT_SDNCMP_FALL_INT_CLEAR_MASK  (BIT_MASK(1) << OUT_SDNCMP_FALL_INT_CLEAR_SHIFT)

#define SDNPVD_INT_CLEAR                SDNPVD_INT_CLEAR
#define SDNPVD_INT_CLEAR_SHIFT          (7)
#define SDNPVD_INT_CLEAR_MASK           (BIT_MASK(1) << SDNPVD_INT_CLEAR_SHIFT)

#define INT_M_READOUT_OFFSET            0x48 /*bit20~21*/
#define SDNCMP_EN_FALL_INT_READ         SDNCMP_EN_FALL_INT_READ
#define SDNCMP_EN_FALL_INT_READ_SHIFT   (20)
#define SDNCMP_EN_FALL_INT_READ_MASK    (BIT_MASK(2) << SDNCMP_EN_FALL_INT_READ_SHIFT)

#if 1
/** 9'h0 REG_RUN_CONFIG_REG1 **/
/* RUN_BUCK_LS0_EN */
#define RUN_BUCK_LS0_EN				RUN_BUCK_LS0_EN
#define RUN_BUCK_LS0_EN_SHIFT		(4)
#define RUN_BUCK_LS0_EN_MASK		(BIT_MASK(1) << RUN_BUCK_LS0_EN_SHIFT)

/* RUN_BUCK_LS1_EN */
#define RUN_BUCK_LS1_EN				RUN_BUCK_LS1_EN
#define RUN_BUCK_LS1_EN_SHIFT		(5)
#define RUN_BUCK_LS1_EN_MASK		(BIT_MASK(1) << RUN_BUCK_LS1_EN_SHIFT)

/*RUN_REG_BUCKDC*/
#define RUN_REG_BUCKDC				RUN_REG_BUCKDC
#define RUN_REG_BUCKDC_SHIFT		(6)
#define RUN_REG_BUCKDC_MASK			(BIT_MASK(4) << RUN_REG_BUCKDC_SHIFT)

/*RUN_REG_BUCKDC_SWFREQ*/
#define RUN_REG_BUCKDC_SWFREQ		RUN_REG_BUCKDC_SWFREQ
#define RUN_REG_BUCKDC_SWFREQ_SHIFT	(10)
#define RUN_REG_BUCKDC_SWFREQ_MASK	(BIT_MASK(2) << RUN_REG_BUCKDC_SWFREQ_SHIFT)

/*RUN_REG_BUCKDC_IOVER*/
#define RUN_REG_BUCKDC_IOVER		RUN_REG_BUCKDC_IOVER
#define RUN_REG_BUCKDC_IOVER_SHIFT	(12)
#define RUN_REG_BUCKDC_IOVER_MASK	(BIT_MASK(1) << RUN_REG_BUCKDC_IOVER_SHIFT)

/*RUN_REG_BUCKDC_PWM_EN*/
#define RUN_REG_BUCKDC_PWM_EN		RUN_REG_BUCKDC_PWM_EN
#define RUN_REG_BUCKDC_PWM_EN_SHIFT	(13)
#define RUN_REG_BUCKDC_PWM_EN_MASK	(BIT_MASK(1) << RUN_REG_BUCKDC_PWM_EN_SHIFT)

/*RUN_CORELDO*/
#define RUN_CORELDO					RUN_CORELDO
#define RUN_CORELDO_SHIFT			(14)
#define RUN_CORELDO_MASK			(BIT_MASK(4) << RUN_CORELDO_SHIFT)

/* RUN_HVIOLDO_EN */
#define RUN_HVIOLDO_EN				RUN_HVIOLDO_EN
#define RUN_HVIOLDO_EN_SHIFT		(18)
#define RUN_HVIOLDO_EN_MASK			(BIT_MASK(1) << RUN_HVIOLDO_EN_SHIFT)

/*RUN_REG_HVIOLDO*/
#define RUN_REG_HVIOLDO				RUN_REG_HVIOLDO
#define	RUN_REG_HVIOLDO_SHIFT		(19)
#define	RUN_REG_HVIOLDO_MASK		(BIT_MASK(3) << RUN_REG_HVIOLDO_SHIFT)

/* RUN_LVIOLDO_EN */
#define RUN_LVIOLDO_EN				RUN_LVIOLDO_EN
#define RUN_LVIOLDO_EN_SHIFT		(22)
#define RUN_LVIOLDO_EN_MASK			(BIT_MASK(1) << RUN_LVIOLDO_EN_SHIFT)

/*RUN_REG_LVIOLDO*/
#define RUN_REG_LVIOLDO				RUN_REG_LVIOLDO
#define	RUN_REG_LVIOLDO_SHIFT		(23)
#define	RUN_REG_LVIOLDO_MASK		(BIT_MASK(3) << RUN_REG_LVIOLDO_SHIFT)

/*RUN_ABB_LS_EN*/
#define RUN_ABB_LS_EN				RUN_ABB_LS_EN
#define	RUN_ABB_LS_EN_SHIFT			(26)
#define	RUN_ABB_LS_EN_MASK			(BIT_MASK(1) << RUN_ABB_LS_EN_SHIFT)

/*RUN_HVIO_LS_EN*/
#define	RUN_HVIO_LS_EN				RUN_HVIO_LS_EN
#define	RUN_HVIO_LS_EN_SHIFT		(27)
#define	RUN_HVIO_LS_EN_MASK			(BIT_MASK(1) << RUN_HVIO_LS_EN_SHIFT)

/*RUN_ACORE_LDO_EN*/
#define	RUN_ACORE_LDO_EN			RUN_ACORE_LDO_EN
#define	RUN_ACORE_LDO_EN_SHIFT		(28)
#define	RUN_ACORE_LDO_EN_MASK		(BIT_MASK(1) << RUN_ACORE_LDO_EN_SHIFT)


/** 9'h1 REG_RUN_CONFIG_REG2 **/
/* RUN_SIMLDO_EN */
#define	RUN_SIMLDO_EN				RUN_SIMLDO_EN
#define	RUN_SIMLDO_EN_SHIFT			(0)
#define	RUN_SIMLDO_EN_MASK			(BIT_MASK(1) << RUN_SIMLDO_EN_SHIFT)

/*RUN_REG_SIMLDO*/
#define	RUN_REG_SIMLDO				RUN_REG_SIMLDO
#define	RUN_REG_SIMLDO_SHIFT		(1)
#define	RUN_REG_SIMLDO_MASK			(BIT_MASK(3) << RUN_REG_SIMLDO_SHIFT)

/*RUN_ANALDO_EN*/
#define	RUN_ANALDO_EN				RUN_ANALDO_EN
#define	RUN_ANALDO_EN_SHIFT			(4)
#define	RUN_ANALDO_EN_MASK			(BIT_MASK(1) << RUN_REG_SIMLDO_SHIFT)
#endif

#if 1
/** 9'h2 REG_DOR_CONFIG_REG1**/
/*DOR_MAINBGP_EN*/
#define	DOR_MAINBGP_EN				DOR_MAINBGP_EN
#define	DOR_MAINBGP_EN_SHIFT		(0)
#define	DOR_MAINBGP_EN_MASK			(BIT_MASK(1) << DOR_MAINBGP_EN_SHIFT)

/*DOR_BUCKDC_EN*/
#define	DOR_BUCKDC_EN				DOR_BUCKDC_EN
#define	DOR_BUCKDC_EN_SHIFT			(5)
#define	DOR_BUCKDC_EN_MASK			(BIT_MASK(1) << DOR_BUCKDC_EN_SHIFT)

/*DOR_BUCK_LS0_EN*/
#define	DOR_BUCK_LS0_EN				DOR_BUCK_LS0_EN
#define	DOR_BUCK_LS0_EN_SHIFT		(6)
#define	DOR_BUCK_LS0_EN_MASK		(BIT_MASK(1) << DOR_BUCK_LS0_EN_SHIFT)

/*DOR_BUCK_LS1_EN*/
#define	DOR_BUCK_LS1_EN				DOR_BUCK_LS1_EN
#define	DOR_BUCK_LS1_EN_SHIFT		(7)
#define	DOR_BUCK_LS1_EN_MASK		(BIT_MASK(1) << DOR_BUCK_LS1_EN_SHIFT)

/*DOR_REG_BUCKDC*/
#define DOR_REG_BUCKDC				DOR_REG_BUCKDC
#define DOR_REG_BUCKDC_SHIFT		(8)
#define DOR_REG_BUCKDC_MASK			(BIT_MASK(4) << DOR_REG_BUCKDC_SHIFT)

/*DOR_REG_BUCKDC_SWFREQ*/
#define DOR_REG_BUCKDC_SWFREQ		DOR_REG_BUCKDC_SWFREQ
#define DOR_REG_BUCKDC_SWFREQ_SHIFT	(12)
#define DOR_REG_BUCKDC_SWFREQ_MASK	(BIT_MASK(2) << DOR_REG_BUCKDC_SWFREQ_SHIFT)

/*DOR_REG_BUCKDC_IOVER*/
#define DOR_REG_BUCKDC_IOVER		DOR_REG_BUCKDC_IOVER
#define DOR_REG_BUCKDC_IOVER_SHIFT	(14)
#define DOR_REG_BUCKDC_IOVER_MASK	(BIT_MASK(1) << DOR_REG_BUCKDC_IOVER_SHIFT)

/*DOR_REG_BUCKDC_PWM_EN*/
#define DOR_REG_BUCKDC_PWM_EN		DOR_REG_BUCKDC_PWM_EN
#define DOR_REG_BUCKDC_PWM_EN_SHIFT	(15)
#define DOR_REG_BUCKDC_PWM_EN_MASK	(BIT_MASK(1) << DOR_REG_BUCKDC_PWM_EN_SHIFT)

/*DOR_LPCORELDO_EN*/
#define DOR_LPCORELDO_EN			DOR_LPCORELDO_EN
#define	DOR_LPCORELDO_EN_SHIFT		(16)
#define	DOR_LPCORELDO_EN_MASK		(BIT_MASK(1) << DOR_LPCORELDO_EN_SHIFT)

/*DOR_CORELDO*/
#define	DOR_CORELDO					DOR_CORELDO
#define	DOR_CORELDO_SHIFT			(17)
#define	DOR_CORELDO_MASK			(BIT_MASK(4) << DOR_CORELDO_SHIFT)

/*DOR_MCORELDO_EN*/
#define	DOR_MCORELDO_EN				DOR_MCORELDO_EN
#define	DOR_MCORELDO_EN_SHIFT		(21)
#define	DOR_MCORELDO_EN_MASK		(BIT_MASK(1) << DOR_MCORELDO_EN_SHIFT)

/*DOR_HVIOLDO_EN*/
#define	DOR_HVIOLDO_EN				DOR_HVIOLDO_EN
#define	DOR_HVIOLDO_EN_SHIFT		(22)
#define	DOR_HVIOLDO_EN_MASK			(BIT_MASK(1) << DOR_HVIOLDO_EN_SHIFT)

/*DOR_REG_HVIOLDO*/
#define	DOR_REG_HVIOLDO				DOR_REG_HVIOLDO
#define	DOR_REG_HVIOLDO_SHIFT		(23)
#define	DOR_REG_HVIOLDO_MASK		(BIT_MASK(3) << DOR_REG_HVIOLDO_SHIFT)

/*DOR_LVIOLDO_EN*/
#define	DOR_LVIOLDO_EN				DOR_LVIOLDO_EN
#define	DOR_LVIOLDO_EN_SHIFT		(26)
#define	DOR_LVIOLDO_EN_MASK			(BIT_MASK(1) << DOR_LVIOLDO_EN_SHIFT)

/*DOR_REG_LVIOLDO*/
#define	DOR_REG_LVIOLDO				DOR_REG_LVIOLDO
#define	DOR_REG_LVIOLDO_SHIFT		(27)
#define	DOR_REG_LVIOLDO_MASK		(BIT_MASK(3) << DOR_REG_LVIOLDO_SHIFT)

/*DOR_ACORE_LDO_EN*/
#define	DOR_ACORE_LDO_EN			DOR_ACORE_LDO_EN
#define	DOR_ACORE_LDO_EN_SHIFT		(30)
#define	DOR_ACORE_LDO_EN_MASK		(BIT_MASK(1) << DOR_ACORE_LDO_EN_SHIFT)

/** 9'h3 REG_DOR_CONFIG_REG2**/
/*DOR_ABB_LS_EN*/
#define	DOR_ABB_LS_EN				DOR_ABB_LS_EN
#define	DOR_ABB_LS_EN_SHIFT			(0)
#define	DOR_ABB_LS_EN_MASK			(BIT_MASK(1) << DOR_ABB_LS_EN_SHIFT)

/*DOR_HVIO_LS_EN*/
#define	DOR_HVIO_LS_EN				DOR_HVIO_LS_EN
#define	DOR_HVIO_LS_EN_SHIFT		(1)
#define	DOR_HVIO_LS_EN_MASK			(BIT_MASK(1) << DOR_HVIO_LS_EN_SHIFT)

/*DOR_SIMLDO_EN*/
#define	DOR_SIMLDO_EN				DOR_SIMLDO_EN
#define	DOR_SIMLDO_EN_SHIFT			(2)
#define	DOR_SIMLDO_EN_MASK			(BIT_MASK(1) << DOR_SIMLDO_EN_SHIFT)

/*DOR_REG_SIMLDO*/
#define	DOR_REG_SIMLDO				DOR_REG_SIMLDO
#define	DOR_REG_SIMLDO_SHIFT		(3)
#define	DOR_REG_SIMLDO_MASK			(BIT_MASK(3) << DOR_REG_SIMLDO_SHIFT)

/*DOR_ANALDO_EN*/
#define	DOR_ANALDO_EN				DOR_ANALDO_EN
#define	DOR_ANALDO_EN_SHIFT			(6)
#define	DOR_ANALDO_EN_MASK			(BIT_MASK(1) << DOR_ANALDO_EN_SHIFT)
#endif

#if 1
/** 9'h4 REG_SBY_CONFIG_REG1**/
/*SBY_MAINBGP_EN*/
#define	SBY_MAINBGP_EN				SBY_MAINBGP_EN
#define	SBY_MAINBGP_EN_SHIFT		(0)
#define	SBY_MAINBGP_EN_MASK			(BIT_MASK(1) << SBY_MAINBGP_EN_SHIFT)

/*SBY_BUCKDC_EN*/
#define	SBY_BUCKDC_EN				SBY_BUCKDC_EN
#define	SBY_BUCKDC_EN_SHIFT			(5)
#define	SBY_BUCKDC_EN_MASK			(BIT_MASK(1) << SBY_BUCKDC_EN_SHIFT)

/*SBY_BUCK_LS0_EN*/
#define	SBY_BUCK_LS0_EN				SBY_BUCK_LS0_EN
#define	SBY_BUCK_LS0_EN_SHIFT		(6)
#define	SBY_BUCK_LS0_EN_MASK		(BIT_MASK(1) << SBY_BUCK_LS0_EN_SHIFT)

/*SBY_BUCK_LS1_EN*/
#define	SBY_BUCK_LS1_EN				SBY_BUCK_LS1_EN
#define	SBY_BUCK_LS1_EN_SHIFT		(7)
#define	SBY_BUCK_LS1_EN_MASK		(BIT_MASK(1) << SBY_BUCK_LS1_EN_SHIFT)

/*SBY_REG_BUCKDC*/
#define SBY_REG_BUCKDC				SBY_REG_BUCKDC
#define SBY_REG_BUCKDC_SHIFT		(8)
#define SBY_REG_BUCKDC_MASK			(BIT_MASK(4) << SBY_REG_BUCKDC_SHIFT)

/*SBY_REG_BUCKDC_SWFREQ*/
#define SBY_REG_BUCKDC_SWFREQ		SBY_REG_BUCKDC_SWFREQ
#define SBY_REG_BUCKDC_SWFREQ_SHIFT	(12)
#define SBY_REG_BUCKDC_SWFREQ_MASK	(BIT_MASK(2) << SBY_REG_BUCKDC_SWFREQ_SHIFT)

/*SBY_REG_BUCKDC_IOVER*/
#define SBY_REG_BUCKDC_IOVER		SBY_REG_BUCKDC_IOVER
#define SBY_REG_BUCKDC_IOVER_SHIFT	(14)
#define SBY_REG_BUCKDC_IOVER_MASK	(BIT_MASK(1) << SBY_REG_BUCKDC_IOVER_SHIFT)

/*SBY_REG_BUCKDC_PWM_EN*/
#define SBY_REG_BUCKDC_PWM_EN		SBY_REG_BUCKDC_PWM_EN
#define SBY_REG_BUCKDC_PWM_EN_SHIFT	(15)
#define SBY_REG_BUCKDC_PWM_EN_MASK	(BIT_MASK(1) << SBY_REG_BUCKDC_PWM_EN_SHIFT)

/*SBY_LPCORELDO_EN*/
#define SBY_LPCORELDO_EN			SBY_LPCORELDO_EN
#define	SBY_LPCORELDO_EN_SHIFT		(16)
#define	SBY_LPCORELDO_EN_MASK		(BIT_MASK(1) << SBY_LPCORELDO_EN_SHIFT)

/*SBY_CORELDO*/
#define	SBY_CORELDO					SBY_CORELDO
#define	SBY_CORELDO_SHIFT			(17)
#define	SBY_CORELDO_MASK			(BIT_MASK(4) << SBY_CORELDO_SHIFT)

/*SBY_MCORELDO_EN*/
#define	SBY_MCORELDO_EN				SBY_MCORELDO_EN
#define	SBY_MCORELDO_EN_SHIFT		(21)
#define	SBY_MCORELDO_EN_MASK		(BIT_MASK(1) << SBY_MCORELDO_EN_SHIFT)

/*SBY_HVIOLDO_EN*/
#define	SBY_HVIOLDO_EN				SBY_HVIOLDO_EN
#define	SBY_HVIOLDO_EN_SHIFT		(22)
#define	SBY_HVIOLDO_EN_MASK			(BIT_MASK(1) << SBY_HVIOLDO_EN_SHIFT)

/*SBY_REG_HVIOLDO*/
#define	SBY_REG_HVIOLDO				SBY_REG_HVIOLDO
#define	SBY_REG_HVIOLDO_SHIFT		(23)
#define	SBY_REG_HVIOLDO_MASK		(BIT_MASK(3) << SBY_REG_HVIOLDO_SHIFT)

/*SBY_LVIOLDO_EN*/
#define	SBY_LVIOLDO_EN				SBY_LVIOLDO_EN
#define	SBY_LVIOLDO_EN_SHIFT		(26)
#define	SBY_LVIOLDO_EN_MASK			(BIT_MASK(1) << SBY_LVIOLDO_EN_SHIFT)

/*SBY_REG_LVIOLDO*/
#define	SBY_REG_LVIOLDO				SBY_REG_LVIOLDO
#define	SBY_REG_LVIOLDO_SHIFT		(27)
#define	SBY_REG_LVIOLDO_MASK		(BIT_MASK(3) << SBY_REG_LVIOLDO_SHIFT)

/*SBY_ACORE_LDO_EN*/
#define	SBY_ACORE_LDO_EN			SBY_ACORE_LDO_EN
#define	SBY_ACORE_LDO_EN_SHIFT		(30)
#define	SBY_ACORE_LDO_EN_MASK		(BIT_MASK(1) << SBY_ACORE_LDO_EN_SHIFT)


/** 9'h5 REG_SBY_CONFIG_REG2**/
/*SBY_ABB_LS_EN*/
#define	SBY_ABB_LS_EN				SBY_ABB_LS_EN
#define	SBY_ABB_LS_EN_SHIFT			(0)
#define	SBY_ABB_LS_EN_MASK			(BIT_MASK(1) << SBY_ABB_LS_EN_SHIFT)

/*SBY_HVIO_LS_EN*/
#define	SBY_HVIO_LS_EN				SBY_HVIO_LS_EN
#define	SBY_HVIO_LS_EN_SHIFT		(1)
#define	SBY_HVIO_LS_EN_MASK			(BIT_MASK(1) << SBY_HVIO_LS_EN_SHIFT)

/*SBY_SIMLDO_EN*/
#define	SBY_SIMLDO_EN				SBY_SIMLDO_EN
#define	SBY_SIMLDO_EN_SHIFT			(2)
#define	SBY_SIMLDO_EN_MASK			(BIT_MASK(1) << SBY_SIMLDO_EN_SHIFT)

/*SBY_REG_SIMLDO*/
#define	SBY_REG_SIMLDO				SBY_REG_SIMLDO
#define	SBY_REG_SIMLDO_SHIFT		(3)
#define	SBY_REG_SIMLDO_MASK			(BIT_MASK(3) << SBY_REG_SIMLDO_SHIFT)

/*SBY_ANALDO_EN*/
#define	SBY_ANALDO_EN				SBY_ANALDO_EN
#define	SBY_ANALDO_EN_SHIFT			(6)
#define	SBY_ANALDO_EN_MASK			(BIT_MASK(1) << SBY_ANALDO_EN_SHIFT)
#endif

#if 1
/** 9'h6 PMU CTRL ENABLE CONFIG REG1**/
/*MAINBGP_POWEROK_EN*/
#define	MAINBGP_POWEROK_EN			MAINBGP_POWEROK_EN
#define	MAINBGP_POWEROK_EN_SHIFT	(0)
#define	MAINBGP_POWEROK_EN_MASK		(BIT_MASK(1) << MAINBGP_POWEROK_EN_SHIFT)

/*BUCKDC_POWEROK_EN*/
#define	BUCKDC_POWEROK_EN			BUCKDC_POWEROK_EN
#define	BUCKDC_POWEROK_EN_SHIFT		(1)
#define	BUCKDC_POWEROK_EN_MASK		(BIT_MASK(1) << BUCKDC_POWEROK_EN_SHIFT)

/*LLP_EN*/
#define	LLP_EN						LLP_EN
#define	LLP_EN_SHIFT				(2)
#define	LLP_EN_MASK					(BIT_MASK(1) << LLP_EN_SHIFT)

/*LP_EN*/
#define	LP_EN						LP_EN
#define	LP_EN_SHIFT					(3)
#define	LP_EN_MASK					(BIT_MASK(1) << LP_EN_SHIFT)

/*LLP_TO_DO*/
#define	LLP_TO_DO					LLP_TO_DO
#define	LLP_TO_DO_SHIFT				(4)
#define	LLP_TO_DO_MASK				(BIT_MASK(1) << LLP_TO_DO_SHIFT)

/*POWEROFF_PROTECT_EN*/
#define	POWEROFF_PROTECT_EN			POWEROFF_PROTECT_EN
#define	POWEROFF_PROTECT_EN_SHIFT	(5)
#define	POWEROFF_PROTECT_EN_MASK	(BIT_MASK(1) << POWEROFF_PROTECT_EN_SHIFT)

/*KEYON_COUNTER_EN*/
#define	KEYON_COUNTER_EN			KEYON_COUNTER_EN
#define	KEYON_COUNTER_EN_SHIFT		(6)
#define	KEYON_COUNTER_EN_MASK		(BIT_MASK(1) << KEYON_COUNTER_EN_SHIFT)
#endif

#if 1
/** 9'h7 PMU CTRL POWERUP DELAY TIME CONFIG REG2**/
/*SWSIM_POWERUP_DELAY_TIME*/
#define	SWSIM_POWERUP_DELAY_TIME		SWSIM_POWERUP_DELAY_TIME
#define	SWSIM_POWERUP_DELAY_TIME_SHIFT	(0)
#define	SWSIM_POWERUP_DELAY_TIME_MASK	(BIT_MASK(9) << SWSIM_POWERUP_DELAY_TIME_SHIFT)
#endif

#if 1
/** 9'h8 PS HOLD CONFIG REG2**/
/*PS_HOLD*/
#define	PS_HOLD						PS_HOLD
#define	PS_HOLD_SHIFT				(0)
#define	PS_HOLD_MASK				(BIT_MASK(1) << PS_HOLD_SHIFT)
#endif

#if 1
/** 9'he SW SIM POWEROFF INT DEAL TIME CONFIG REG**/
/*SWSIM_POWEROFF_INTDEAL_TIME*/
#define	SWSIM_POWEROFF_INTDEAL_TIME			SWSIM_POWEROFF_INTDEAL_TIME
#define	SWSIM_POWEROFF_INTDEAL_TIME_SHIFT	(0)
#define	SWSIM_POWEROFF_INTDEAL_TIME_MASK	(BIT_MASK(18) << SWSIM_POWEROFF_INTDEAL_TIME_SHIFT)
#endif

#if 1
/** 9'h12 ACTT_PMU_STATUS_READ_ADDR**/
/*BUCKDC_OC_5V*/
#define	BUCKDC_OC_5V				BUCKDC_OC_5V	
#define	BUCKDC_OC_5V_SHIFT			(0)
#define	BUCKDC_OC_5V_MASK			(BIT_MASK(1) << BUCKDC_OC_5V_SHIFT)

/*ACORELDO_OC_18*/
#define	ACORELDO_OC_18				ACORELDO_OC_18	
#define	ACORELDO_OC_18_SHIFT		(1)
#define	ACORELDO_OC_18_MASK			(BIT_MASK(1) << ACORELDO_OC_18_SHIFT)

/*HVIOLDO_OC_5V*/
#define	HVIOLDO_OC_5V				HVIOLDO_OC_5V	
#define	HVIOLDO_OC_5V_SHIFT			(2)
#define	HVIOLDO_OC_5V_MASK			(BIT_MASK(1) << HVIOLDO_OC_5V_SHIFT)

/*SIMLDO_OC_5V*/
#define	SIMLDO_OC_5V				SIMLDO_OC_5V	
#define	SIMLDO_OC_5V_SHIFT			(3)
#define	SIMLDO_OC_5V_MASK			(BIT_MASK(1) << SIMLDO_OC_5V_SHIFT)

/*MCORELDO_OC_18*/
#define	MCORELDO_OC_18				MCORELDO_OC_18	
#define	MCORELDO_OC_18_SHIFT		(4)
#define	MCORELDO_OC_18_MASK			(BIT_MASK(1) << MCORELDO_OC_18_SHIFT)

/*LVIOLDO_OC_5V*/
#define	LVIOLDO_OC_5V				LVIOLDO_OC_5V	
#define	LVIOLDO_OC_5V_SHIFT			(5)
#define	LVIOLDO_OC_5V_MASK			(BIT_MASK(1) << LVIOLDO_OC_5V_SHIFT)

/*ACORELDO_OK_18*/
#define	ACORELDO_OK_18				ACORELDO_OK_18	
#define	ACORELDO_OK_18_SHIFT		(6)
#define	ACORELDO_OK_18_MASK			(BIT_MASK(1) << ACORELDO_OK_18_SHIFT)

/*ANALDO_OK_5V*/
#define	ANALDO_OK_5V				ANALDO_OK_5V	
#define	ANALDO_OK_5V_SHIFT			(7)
#define	ANALDO_OK_5V_MASK			(BIT_MASK(1) << ANALDO_OK_5V_SHIFT)

/*BUCKDC_OK_5V*/
#define	BUCKDC_OK_5V				BUCKDC_OK_5V	
#define	BUCKDC_OK_5V_SHIFT			(8)
#define	BUCKDC_OK_5V_MASK			(BIT_MASK(1) << BUCKDC_OK_5V_SHIFT)

/*HVIOLDO_OK_5V*/
#define	HVIOLDO_OK_5V				HVIOLDO_OK_5V	
#define	HVIOLDO_OK_5V_SHIFT			(9)
#define	HVIOLDO_OK_5V_MASK			(BIT_MASK(1) << HVIOLDO_OK_5V_SHIFT)

/*SIMLDO_OK_5V*/
#define	SIMLDO_OK_5V				SIMLDO_OK_5V	
#define	SIMLDO_OK_5V_SHIFT			(10)
#define	SIMLDO_OK_5V_MASK			(BIT_MASK(1) << SIMLDO_OK_5V_SHIFT)

/*MCORELDO_OK_18*/
#define	MCORELDO_OK_18				MCORELDO_OK_18	
#define	MCORELDO_OK_18_SHIFT		(11)
#define	MCORELDO_OK_18_MASK			(BIT_MASK(1) << MCORELDO_OK_18_SHIFT)

/*LCORELDO_OK_18*/
#define	LCORELDO_OK_18				LCORELDO_OK_18
#define	LCORELDO_OK_18_SHIFT		(12)
#define	LCORELDO_OK_18_MASK			(BIT_MASK(1) << LCORELDO_OK_18_SHIFT)

/*ABB_OK_18*/
#define	ABB_OK_18					ABB_OK_18	
#define	ABB_OK_18_SHIFT				(13)
#define	ABB_OK_18_MASK				(BIT_MASK(1) << ABB_OK_18_SHIFT)

/*LVIOLDO_OK_5V*/
#define	LVIOLDO_OK_5V				LVIOLDO_OK_5V	
#define	LVIOLDO_OK_5V_SHIFT			(14)
#define	LVIOLDO_OK_5V_MASK			(BIT_MASK(1) << LVIOLDO_OK_5V_SHIFT)

/*SDNPVD_INTN_5V*/
#define	SDNPVD_INTN_5V				SDNPVD_INTN_5V	
#define	SDNPVD_INTN_5V_SHIFT		(15)
#define	SDNPVD_INTN_5V_MASK			(BIT_MASK(1) << SDNPVD_INTN_5V_SHIFT)

/*OUT_SDNCMP*/
#define	OUT_SDNCMP					OUT_SDNCMP	
#define	OUT_SDNCMP_SHIFT			(16)
#define	OUT_SDNCMP_MASK				(BIT_MASK(1) << OUT_SDNCMP_SHIFT)

/*SBYLDO_OK_5V*/
#define	SBYLDO_OK_5V				SBYLDO_OK_5V	
#define	SBYLDO_OK_5V_SHIFT			(17)
#define	SBYLDO_OK_5V_MASK			(BIT_MASK(1) << SBYLDO_OK_5V_SHIFT)

/*SDNBOR_DET_5V*/
#define	SDNBOR_DET_5V				SDNBOR_DET_5V	
#define	SDNBOR_DET_5V_SHIFT			(18)
#define	SDNBOR_DET_5V_MASK			(BIT_MASK(1) << SDNBOR_DET_5V_SHIFT)

/*SDNBGP_OK_5V*/
#define	SDNBGP_OK_5V				SDNBGP_OK_5V	
#define	SDNBGP_OK_5V_SHIFT			(19)
#define	SDNBGP_OK_5V_MASK			(BIT_MASK(1) << SDNBGP_OK_5V_SHIFT)

/*V28RF_OK_5V*/
#define	V28RF_OK_5V					V28RF_OK_5V	
#define	V28RF_OK_5V_SHIFT			(20)
#define	V28RF_OK_5V_MASK			(BIT_MASK(1) << V28RF_OK_5V_SHIFT)

/*BUCKLS0_OK_18*/
#define	BUCKLS0_OK_18				BUCKLS0_OK_18	
#define	BUCKLS0_OK_18_SHIFT			(21)
#define	BUCKLS0_OK_18_MASK			(BIT_MASK(1) << BUCKLS0_OK_18_SHIFT)

/*BUCKLS1_OK_18*/
#define	BUCKLS1_OK_18				BUCKLS1_OK_18
#define	BUCKLS1_OK_18_SHIFT			(22)
#define	BUCKLS1_OK_18_MASK			(BIT_MASK(1) << BUCKLS1_OK_18_SHIFT)

/*MAINBGP_OK_5V*/
#define	MAINBGP_OK_5V				MAINBGP_OK_5V	
#define	MAINBGP_OK_5V_SHIFT			(23)
#define	MAINBGP_OK_5V_MASK			(BIT_MASK(1) << MAINBGP_OK_5V_SHIFT)
#endif

#if 1
/** 9'h13 ACTT_PMU_STATUS_WRITE_ADDR**/
/*REG_SDNBOR_5V*/
#define	REG_SDNBOR_5V				REG_SDNBOR_5V	
#define	REG_SDNBOR_5V_SHIFT			(4)
#define	REG_SDNBOR_5V_MASK			(BIT_MASK(3) << REG_SDNBOR_5V_SHIFT)

/*REG_SBYLDO_5V*/
#define	REG_SBYLDO_5V				REG_SBYLDO_5V	
#define	REG_SBYLDO_5V_SHIFT			(7)
#define	REG_SBYLDO_5V_MASK			(BIT_MASK(3) << REG_SBYLDO_5V_SHIFT)

/*SDNCMP_EN_18*/
#define	SDNCMP_EN_18				SDNCMP_EN_18
#define	SDNCMP_EN_18_SHIFT			(10)
#define	SDNCMP_EN_18_MASK			(BIT_MASK(1) << SDNCMP_EN_18_SHIFT)

/*REG_SDNCMP_18*/
#define	REG_SDNCMP_18				REG_SDNCMP_18	
#define	REG_SDNCMP_18_SHIFT			(11)
#define	REG_SDNCMP_18_MASK			(BIT_MASK(2) << REG_SDNCMP_18_SHIFT)

/*SDNPVD_EN_5V*/
#define	SDNPVD_EN_5V				SDNPVD_EN_5V	
#define	SDNPVD_EN_5V_SHIFT			(13)
#define	SDNPVD_EN_5V_MASK			(BIT_MASK(1) << SDNPVD_EN_5V_SHIFT)

/*REG_SDNPVD_5V*/
#define	REG_SDNPVD_5V				REG_SDNPVD_5V
#define	REG_SDNPVD_5V_SHIFT			(14)
#define	REG_SDNPVD_5V_MASK			(BIT_MASK(3) << REG_SDNPVD_5V_SHIFT)

/*PVDSEL_5V*/
#define	PVDSEL_5V					PVDSEL_5V		
#define	PVDSEL_5V_SHIFT				(17)
#define	PVDSEL_5V_MASK				(BIT_MASK(1) << PVDSEL_5V_SHIFT)

/*MAINBGP_PGEN_5V*/
#define	MAINBGP_PGEN_5V				MAINBGP_PGEN_5V	
#define	MAINBGP_PGEN_5V_SHIFT		(18)
#define	MAINBGP_PGEN_5V_MASK		(BIT_MASK(1) << MAINBGP_PGEN_5V_SHIFT)

/*TEST_EN_5V*/
#define	TEST_EN_5V					TEST_EN_5V	
#define	TEST_EN_5V_SHIFT			(21)
#define	TEST_EN_5V_MASK				(BIT_MASK(1) << TEST_EN_5V_SHIFT)

/*TEST_SEL_5V*/
#define	TEST_SEL_5V					TEST_SEL_5V	
#define	TEST_SEL_5V_SHIFT			(22)
#define	TEST_SEL_5V_MASK			(BIT_MASK(1) << TEST_SEL_5V_SHIFT)

/*REG_ANALDO_5V*/
#define	REG_ANALDO_5V				REG_ANALDO_5V	
#define	REG_ANALDO_5V_SHIFT			(23)
#define	REG_ANALDO_5V_MASK			(BIT_MASK(3) << REG_ANALDO_5V_SHIFT)

/*LDOOC_EN_5V*/
#define	LDOOC_EN_5V					LDOOC_EN_5V	
#define	LDOOC_EN_5V_SHIFT			(26)
#define	LDOOC_EN_5V_MASK			(BIT_MASK(1) << LDOOC_EN_5V_SHIFT)

/*KEYON_WAKEUP1_LLP_VALID*/
#define	KEYON_WAKEUP1_LLP_VALID			KEYON_WAKEUP1_LLP_VALID	
#define	KEYON_WAKEUP1_LLP_VALID_SHIFT	(27)
#define	KEYON_WAKEUP1_LLP_VALID_MASK	(BIT_MASK(1) << KEYON_WAKEUP1_LLP_VALID_SHIFT)

/*KEYON_WAKEUP1_LP_VALID*/
#define	KEYON_WAKEUP1_LP_VALID			KEYON_WAKEUP1_LP_VALID	
#define	KEYON_WAKEUP1_LP_VALID_SHIFT	(28)
#define	KEYON_WAKEUP1_LP_VALID_MASK		(BIT_MASK(1) << KEYON_WAKEUP1_LP_VALID_SHIFT)

/*KEYON_WAKEUP1_SP_VALID*/
#define	KEYON_WAKEUP1_SP_VALID			KEYON_WAKEUP1_SP_VALID	
#define	KEYON_WAKEUP1_SP_VALID_SHIFT	(29)
#define	KEYON_WAKEUP1_SP_VALID_MASK		(BIT_MASK(1) << KEYON_WAKEUP1_SP_VALID_SHIFT)
#endif

#if 1
/** 9'h1d BUTTON_SPLPLLP_TIME_CONFIG_ADDR**/
/*REG_SP_TIME*/
#define	REG_SP_TIME				REG_SP_TIME
#define	REG_SP_TIME_SHIFT		(0)
#define	REG_SP_TIME_MASK		(BIT_MASK(8) << REG_SP_TIME_SHIFT)

/*REG_LP_TIME*/
#define	REG_LP_TIME				REG_LP_TIME
#define	REG_LP_TIME_SHIFT		(8)
#define	REG_LP_TIME_MASK		(BIT_MASK(8) << REG_LP_TIME_SHIFT)

/*REG_LLP_TIME*/
#define	REG_LLP_TIME			REG_LLP_TIME
#define	REG_LLP_TIME_SHIFT		(16)
#define	REG_LLP_TIME_MASK		(BIT_MASK(8) << REG_LLP_TIME_SHIFT)
#endif

#if 1
/** 9'h1e VERSION_CONFIG_ADDR**/
/*VERSION*/
#define	VERSION					VERSION
#define	VERSION_SHIFT			(0)
#define	VERSION_MASK			(BIT_MASK(8) << VERSION_SHIFT)
#endif

#if 1
/** 9'h1f STARTUP_STATUS_ADDR**/
/*KEYON_WAKEUP1_STATE*/
#define	KEYON_WAKEUP1_STATE				KEYON_WAKEUP1_STATE
#define	KEYON_WAKEUP1_STATE_SHIFT		(0)
#define	KEYON_WAKEUP1_STATE_MASK		(BIT_MASK(1) << KEYON_WAKEUP1_STATE_SHIFT)

/*DIP_SWITCH_WAKEUP0_STATE*/
#define	DIP_SWITCH_WAKEUP0_STATE		DIP_SWITCH_WAKEUP0_STATE
#define	DIP_SWITCH_WAKEUP0_STATE_SHIFT	(1)
#define	DIP_SWITCH_WAKEUP0_STATE_MASK	(BIT_MASK(1) << DIP_SWITCH_WAKEUP0_STATE_SHIFT)

/*TLLP_STATUS*/
#define	TLLP_STATUS				TLLP_STATUS
#define	TLLP_STATUS_SHIFT		(2)
#define	TLLP_STATUS_MASK		(BIT_MASK(1) << TLLP_STATUS_SHIFT)
#endif

#if 1
/** 9'h24 REG_BGP_CONFIG_REG**/
/*RUN_REG_MAINBGP*/
#define	RUN_REG_MAINBGP			RUN_REG_MAINBGP
#define	RUN_REG_MAINBGP_SHIFT	(0)
#define	RUN_REG_MAINBGP_MASK	(BIT_MASK(4) << RUN_REG_MAINBGP_SHIFT)

/*REG_SDNBGP*/
#define	REG_SDNBGP				REG_SDNBGP
#define	REG_SDNBGP_SHIFT		(4)
#define	REG_SDNBGP_MASK			(BIT_MASK(3) << REG_SDNBGP_SHIFT)

/*REG_SDNBGP*/
#define	REG_SDNBGP_MODE			REG_SDNBGP_MODE
#define	REG_SDNBGP_MODE_SHIFT	(7)
#define	REG_SDNBGP_MODE_MASK	(BIT_MASK(1) << REG_SDNBGP_MODE_SHIFT)
#endif

#if 1
/** 9'h26 REG_PMU_CTRL_SIGNAL**/
/*PMU_STATE*/
#define	PMU_STATE				PMU_STATE
#define	PMU_STATE_SHIFT			(0)
#define	PMU_STATE_MASK			(BIT_MASK(5) << PMU_STATE_SHIFT)

/*DIP_SWITCH_WAKEUP0*/
#define	DIP_SWITCH_WAKEUP0			DIP_SWITCH_WAKEUP0
#define	DIP_SWITCH_WAKEUP0_SHIFT	(5)
#define	DIP_SWITCH_WAKEUP0_MASK		(BIT_MASK(1) << DIP_SWITCH_WAKEUP0_SHIFT)
#endif

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
/** PMU_CTRL REG */
struct bmu_si_regs {
	uint32_t run_config_reg1;								/**<9'h0 offset:0x00    RW     	REG_RUN_CONFIG_REG1 */
	uint32_t run_config_reg2;								/**<9'h1 offset:0x04    RW     	REG_RUN_CONFIG_REG2 */
	uint32_t dor_config_reg1;								/**<9'h2 offset:0x08    RW     	REG_DOR_CONFIG_REG1 */
	uint32_t dor_config_reg2;								/**<9'h3 offset:0x0c    RW     	REG_DOR_CONFIG_REG2 */
	uint32_t sby_config_reg1;								/**<9'h4 offset:0x10    RW     	REG_SBY_CONFIG_REG1 */
	uint32_t sby_config_reg2;								/**<9'h5 offset:0x14    RW     	REG_SBY_CONFIG_REG2 */
	uint32_t enable_config_reg1;							/**<9'h6 offset:0x18    RW     	PMU_CTRL_ENABLE_CONFIG_REG1 */
	uint32_t powerup_delay_time_reg2;						/**<9'h7 offset:0x1c    RW     	PMU_CTRL_POWERUP_DELAY_TIME_CONFIG_REG2 */
	uint32_t pshold_config_reg2;							/**<9'h8 offset:0x20    RW     	PS_HOLD_CONFIG_REG2 */
	uint32_t backup_config_reg0;							/**<9'h9 offset:0x24    RW     	BACKUP_CONFIG_REG0 */
	uint32_t backup_config_reg1;							/**<9'ha offset:0x28    RW     	BACKUP_CONFIG_REG1 */
	uint32_t backup_config_reg2;							/**<9'hb offset:0x2c    RW     	BACKUP_CONFIG_REG2 */
	uint32_t backup_config_reg3;							/**<9'hc offset:0x30    RW     	BACKUP_CONFIG_REG3 */
	uint32_t flag_config_reg;								/**<9'hd offset:0x34    RW     	FLAG_CONFIG_REG */
	uint32_t sw_sim_poweroff_int_deal_time_config_reg;		/**<9'he offset:0x38    RW     	SW_SIM_POWEROFF_INT_DEAL_TIME_CONFIG_REG */
	uint32_t normal_sw_control_key_reg1;					/**<9'hf offset:0x3c    RW      NORMAL_SW_CONTROL_KEY_REG1 */
	uint32_t normal_sw_control_key_reg2;					/**<9'h10 offset:0x40   RW     	NORMAL_SW_CONTROL_KEY_REG2 */
	uint32_t sw_sim_rst5_config_reg2;						/**<9'h11 offset:0x44   RW     	SW_SIM_RST5_CONFIG_REG2 */
	uint32_t actt_bmu_status_read_addr;						/**<9'h12 offset:0x48   R      	ACTT_PMU_STATUS_READ_ADDR */
	uint32_t actt_bmu_status_write_addr;					/**<9'h13 offset:0x4c   RW     	ACTT_PMU_STATUS_WRITE_ADDR */
	uint32_t common_io_pin_status_write_addr;				/**<9'h14 offset:0x50   RW     	COMMON_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t rf_io_pin_status_write_addr;					/**<9'h15 offset:0x54   RW      RF_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t gp1_io_pin_status_write_addr;					/**<9'h16 offset:0x58	RW	 	GP1_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t gp2_io_pin_status_write_addr;					/**<9'h17 offset:0x5c	RW	 	GP2_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t gp3_io_pin_status_write_addr;					/**<9'h18 offset:0x60   RW     	GP3_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t sim_io_pin_status_write_addr;					/**<9'h19 offset:0x64   RW     	SIM_IO_PIN_STATUS_WRITE_ADDR  */
	uint32_t rfctrl_io_pin_status_write_addr;				/**<9'h1a offset:0x68   RW     	RFCTRL_IO_PIN_STATUS_WRITE_ADDR */
	uint32_t io_pin_status_write_addr;						/**<9'h1b offset:0x6c   RW     	IO_PIN_STATUS_WRITE_ADDR */
	uint32_t sysrst_powerup_delay_time_config_addr;			/**<9'h1c offset:0x70   RW     	SYSRST_POWERUP_DELAY_TIME_CONFIG_ADDR */
	uint32_t button_splpllp_time_config_addr;				/**<9'h1d offset:0x74   RW     	BUTTON_SPLPLLP_TIME_CONFIG_ADDR  */
	uint32_t version_config_addr;                     		/**<9'h1e offset:0x78   R      	VERSION_CONFIG_ADDR */
	uint32_t startup_status_addr;							/**<9'h1f offset:0x7c   RC    	STARTUP_STATUS_ADDR */	
	uint32_t intr_mask_config_addr;                      	/**<9'h20 offset:0x80   RW     	INTR_MASK_CONFIG_ADDR */
	uint32_t intr_addr;                      				/**<9'h21 offset:0x84   RW    	INTR_ADDR */
	uint32_t reset_state_config_addr;						/**<9'h22 offset:0x88   RW     	RESET_STATE_CONFIG_ADDR */
	uint32_t pd_eflash_config_reg;							/**<9'h23 offset:0x8c   RW     	PD_EFLASH_CONFIG_REG */
	uint32_t reg_bgp_config_reg;                      		/**<9'h24 offset:0x90   RW     	REG_BGP_CONFIG_REG */
	uint32_t reg_enn_config_reg;                      	    /**<9'h25 offset:0x94   RW    	REG_ENN_CONFIG_REG */
	uint32_t reg_bmu_ctrl_signal;							/**<9'h26 offset:0x98   R     	REG_PMU_CTRL_SIGNAL */
};

#endif

