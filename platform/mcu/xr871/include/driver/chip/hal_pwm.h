/**
  * @file  hal_pwm.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_PWM_H_
#define _DRIVER_CHIP_HAL_PWM_H_

#include "driver/chip/hal_def.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_clock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief PWM group id.
  */
typedef enum {
	PWM_GROUP_0, /*!<PWM GROUP 0, include channel_0 and channel_1*/
	PWM_GROUP_1, /*!<PWM GROUP 1, include channel_2 and channel_3*/
	PWM_GROUP_2, /*!<PWM GROUP 2, include channel_4 and channel_5*/
	PWM_GROUP_3, /*!<PWM GROUP 3, include channel_6 and channel_7*/
	PWM_GROUP_NUM,
	PWM_GROUP_NULL = PWM_GROUP_NUM,
} PWM_GROUP_ID;

/**
  * @brief PWM channel id.
  */
typedef enum {
	PWM_GROUP0_CH0 = 0U,		/*!< bit 0	PORTA_PIN8 or PORTA_PIN19 */
	PWM_GROUP0_CH1 = 1U,		/*!< bit 1	PORTA_PIN9 or PORTA_PIN20 */
	PWM_GROUP1_CH2 = 2U,		/*!< bit 2	PORTA_PIN10 or PORTA_PIN21 */
	PWM_GROUP1_CH3 = 3U,		/*!< bit 3	PORTA_PIN11 or PORTA_PIN22 */
	PWM_GROUP2_CH4 = 4U,		/*!< bit 4	PORTA_PIN12 or PORTB_PIN0 */
	PWM_GROUP2_CH5 = 5U,		/*!< bit 5	PORTA_PIN13 or PORTB_PIN1 */
	PWM_GROUP3_CH6 = 6U,		/*!< bit 6	PORTA_PIN14 or PORTB_PIN2 */
	PWM_GROUP3_CH7 = 7U,		/*!< bit 7	PORTA_PIN15 or PORTB_PIN3 */
	PWM_CH_NUM = 8U,
	PWM_CH_NULL = PWM_CH_NUM,
}PWM_CH_ID;

/**
  * @brief The register for PWM.
  */
typedef struct {
	__IO uint32_t PCR;        		/*!< PWM Control Register,							Address offset: 0x60+0x0+N*0X20(N=0~7) */
	__IO uint32_t PPR;        		/*!< PWM Period Register,							Address offset: 0x60+0x4+N*0X20(N=0~7) */
	__I  uint32_t PCNTR;        	/*!< PWM Count Register,							Address offset: 0x60+0x8+N*0X20(N=0~7) */
	__IO uint32_t CCR;        		/*!< Capture Control Register,						Address offset: 0x60+0xC+N*0X20(N=0~7) */
	__IO uint32_t CRLR;	        	/*!< Capture Rise Lock Register,						Address offset: 0x60+0x10+N*0X20(N=0~7) */
	__IO uint32_t CFLR;        		/*!< Capture Fall Lock Register,						Address offset: 0x60+0x14+N*0X20(N=0~7) */
	 uint32_t RESERVED0[2];
} PWM_CHANNEL_REG;

/**
  * @brief The register for PWM.
  */
typedef struct {
	__IO uint32_t PIER;        		/*!< PWM IRQ Enable Register,						Address offset: 0x00 */
	__IO uint32_t PISR;        		/*!< PWM IRQ Status Register,						Address offset: 0x04 */
		 uint32_t RESERVED0[2];		/*!< NULL*/
	__IO uint32_t CIER;        		/*!< Capture IRQ Enable Register,					Address offset: 0x10 */
	__IO uint32_t CISR;        		/*!< Capture IRQ Status Register,					Address offset: 0x14 */
		 uint32_t RESERVED1[2];		/*!< NULL*/
	__IO uint32_t PCCR[4];        	/*!< PWM01 Clock Configuration Register				Address offset: 0x20 */
	__IO uint32_t PDZCR[4];        	/*!< PWM01 Dead Zone Control Register,				Address offset: 0x30 */
	__IO uint32_t PER;        		/*!< PWM Enable Register,							Address offset: 0x40 */
	__IO uint32_t CER;        		/*!< PWM Capture Enable Register,					Address offset: 0x44 */
		 uint32_t RESERVED2[6];
	PWM_CHANNEL_REG CH_REG[PWM_CH_NUM];

} PWM_T;

#define PWM	((PWM_T *)PWM_BASE)

/**
  * @brief The clock division for PWM group.
  */
typedef enum {
	PWM_SRC_CLK_DIV_1	=	0U, 	/*!< Src Clk Frequency division 1 */
	PWM_SRC_CLK_DIV_2	=	1U, 	/*!< Src Clk Frequency division 2 */
	PWM_SRC_CLK_DIV_4	=	2U,  	/*!< Src Clk Frequency division 4 */
	PWM_SRC_CLK_DIV_8	=	3U,  	/*!< Src Clk Frequency division 8 */
	PWM_SRC_CLK_DIV_16	=	4U,  	/*!< Src Clk Frequency division 16 */
	PWM_SRC_CLK_DIV_32	=	5U,  	/*!< Src Clk Frequency division 32 */
	PWM_SRC_CLK_DIV_64	=	6U,  	/*!< Src Clk Frequency division 64 */
	PWM_SRC_CLK_DIV_128	=	7U,  	/*!< Src Clk Frequency division 128 */
	PWM_SRC_CLK_DIV_256	=	8U,  	/*!< Src Clk Frequency division 256 */
} PWM_SrcClkDiv;

#define PWM_SRC_CLK_DIV_V			((uint32_t)0x0000000F)	//bit 0~3
#define PWM_CH_CLK_GATING			HAL_BIT(4) 				//bit 4
#define PWM_LOW_CH_CLKBYPASS		HAL_BIT(5)				//bit 5
#define PWM_HIGH_CH_CLKBYPASS		HAL_BIT(6)				//bit 6
#define PWM_SRC_CLK_SELECT		((uint32_t)0x00000180)//bit 7~8, bit8 ignore , 01:APB1 00:24M

/*  Bit definition for PWMxx(G01,G23,G45,G67) Dead Zone Control register  **************/

#define PWM_CH_DZ_EN		HAL_BIT(0) 				// bit 0
#define PWM_CH_DZ_INV		((uint32_t)0x0000FF00)	// bit 8~15 PWM Dead Zone interval value

/********************  Bit definition for PWM  Control register  **********************/

#define PWM_PCR_PRESCAL			((uint32_t)0x000000FF)	// bit 0~7
#define PWM_PCR_ACT_STA			HAL_BIT(8)	// bit 8
#define PWM_PCR_MODE			HAL_BIT(9)	// bit 9
#define PWM_PCR_PLUSE_START		HAL_BIT(10)	// bit 10
#define PWM_PCR_PERIODRDY		HAL_BIT(11)	// bit 11

/***************  Bit definition for PWM  Period register  ****************************/

#define PWM_PPR_ACT_CYCLE		((uint32_t)0x0000FFFF)	// bit0~15
#define PWM_PPR_ENTIER_CYCLE	((uint32_t)0xFFFF0000)	// bit16~31

/***************  Bit definition for PWM  Counter register  ***************************/

#define PWM_TIME_COUNTER						((uint32_t)0x0000FFFF)  // bit 0~15

/***************  Bit definition for Capture  Control register  ***********************/

#define PWM_CCR_CAPINV		HAL_BIT(0) 	// bit 0
#define PWM_CCR_CFLF		HAL_BIT(1) 	// bit 1
#define PWM_CCR_CRLF		HAL_BIT(2) 	// bit 2

/***************  Bit definition for Capture  Rise Lock register  *********************/

#define PWM_CRLR		((uint32_t)0x0000FFFF)  // bit 0~15

/***************  Bit definition for Capture Fall Lock register  **********************/

#define PWM_CFLR		((uint32_t)0x0000FFFF)  // bit 0~15

/***************  Bit definition for PWM Enable register  *****************************/

/**
  * @brief The sorce clock for PWM group.
  */
typedef enum {
	PWM_CLK_HOSC,
	PWM_CLK_APB1,
} PWM_GroupSrcClk;

/**
  * @brief The run mode for PWM channel.
  */
typedef enum {
	PWM_CYCLE_MODE,		/*!< The ouput signal is cycle */
	PWM_PLUSE_MODE,		/*!< Run in this mode, caller HAL_PWM_PluseStart(PWM_CH_ID ch_id) produce one pluse */
	PWM_CAPTURE_MODE,   /*!< Capture the input signal */
} PWM_Mode;

/**
  * @brief The Polarity for PWM channel.
  */
typedef enum {
	PWM_LOWLEVE,
	PWM_HIGHLEVE,
} PWM_Polarity;

/**
  * @brief The PWM interrupt event.
  */
typedef enum {
	PWM_IRQ_OUTPUT,
	PWM_IRQ_RISEEDGE,
	PWM_IRQ_FALLEDGE,
	PWM_IRQ_BOTHEDGE,
	PWM_IRQ_NULL,
} PWM_IrqEvent;

/**
  * @brief The PWM interrupt callback.
  * @param arg: The param for callback.
  * @param event: The interrupt event.
  */
typedef void (*PWM_IrqCallback) (void *arg,  PWM_IrqEvent event);

/**
  * @brief Config PWM interrupt.
  */
typedef struct {
	PWM_IrqEvent event;			/*!< The interrupt event */
	PWM_IrqCallback	callback;	/*!< The interrupt callback */
	void *arg;					/*!< The callback  param */
} PWM_IrqParam;

/**
  * @brief Config PWM group clock.
  */
typedef struct {
	PWM_GroupSrcClk clk;	/*!< The source clock for PWM group */
	PWM_SrcClkDiv div;		/*!< The division for clock*/
} PWM_ClkParam;

/**
  * @brief Config PWM channel.
  */
typedef struct {
	PWM_Mode mode;			/*!< The channel's run mode */
	PWM_Polarity polarity;	/*!< The channel's polarity */
	uint32_t hz;			/*!< The channel's output requncy ; run in capture mode , it's intput signal's requency*/
} PWM_ChInitParam;

/**
  * @brief Config PWM Complementary.
  */
typedef struct {
	PWM_Polarity polarity;	/*!< The channel's polarity, used for number of small channels, another
								  channel's polarity is opposite*/
	uint32_t hz;			/*!< The channel's output equncy */
}PWM_CompInitParam;

/**
  * @brief Save capture result.
  */
typedef struct {
	uint32_t highLevelTime;
	uint32_t lowLevelTime;
	uint32_t periodTime;
}PWM_CapResult;

/**
  * @brief Capture mode.
  */
typedef enum {
	PWM_CAP_PLUSE,	/*!< Used for capture pluse */
	PWM_CAP_CYCLE,	/*!< Used for capture continuous signal  */
}PWM_CaptureMode;

HAL_Status HAL_PWM_GroupClkCfg(PWM_GROUP_ID group_id, PWM_ClkParam *param);
int HAL_PWM_ChInit(PWM_CH_ID ch_id, PWM_ChInitParam *param);
HAL_Status HAL_PWM_ChDeinit(PWM_CH_ID ch_id);
int HAL_PWM_ComplementaryInit(PWM_GROUP_ID group_id, PWM_CompInitParam *param);
HAL_Status HAL_PWM_ComplementaryDeInit(PWM_GROUP_ID group_id);

HAL_Status HAL_PWM_EnableCh(PWM_CH_ID ch_id, PWM_Mode mode, uint8_t en);
HAL_Status HAL_PWM_OutputPluse(PWM_CH_ID ch_id);
HAL_Status HAL_PWM_EnableComplementary(PWM_GROUP_ID group_id, uint8_t en);
HAL_Status HAL_PWM_EnableIRQ(PWM_CH_ID ch_id, const PWM_IrqParam *param);
void HAL_PWM_DisableIRQ(PWM_CH_ID ch_id);

HAL_Status HAL_PWM_ChSetDutyRatio(PWM_CH_ID ch_id, uint16_t value);
HAL_Status HAL_PWM_ComplementarySetDutyRatio(PWM_GROUP_ID group_id, uint16_t value);
HAL_Status HAL_PWM_SetDeadZoneTime(PWM_GROUP_ID group_id, uint8_t dead_zone_value);
HAL_Status HAL_PWM_EnableDeadZone(PWM_GROUP_ID group_id, uint8_t en);
PWM_CapResult HAL_PWM_CaptureResult(PWM_CaptureMode mode, PWM_CH_ID ch_id);


#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_PWM_H_ */
