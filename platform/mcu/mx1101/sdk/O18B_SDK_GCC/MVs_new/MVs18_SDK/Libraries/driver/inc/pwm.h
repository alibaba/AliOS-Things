/**
  *****************************************************************************
  * @file:			pwm.h
  * @author			YunWang
  * @version		V1.0.0
  * @data			4-May-2014
  * @Brief			PWM module driver header file.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

#ifndef __PWM_H__
#define	__PWM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
/*
**********************************************************
*					TYPE DEFINE
**********************************************************
*/
/**
 * PWM Channel select definition
 */
#define PWM_CH0_A0_B27		0
#define PWM_CH1_A1_B26		1
#define PWM_CH2_A9_B25		2
#define PWM_CH3_A10_B24		3
#define PWM_CH4_B6_B23		4
#define PWM_CH5_B7_B22		5
#define PWM_CH6_B8_B21		6
#define PWM_CH7_B9_B20		7

/**
 * PWM I/O select definition
 */
#define PWM_IO_SEL0			0
#define PWM_IO_SEL1			1

/**
 * PWM Mode select definition
 */
#define PWM_MODE_OUT		0			/**<PWM Mode voltage output*/
#define PWM_MODE_PD1		1			/**<PWM Mode current pulldown1 1.7mA*/
#define PWM_MODE_PD2		2			/**<PWM Mode current pulldown2 2.4mA*/
#define PWM_MODE_PD12		3			/**<PWM Mode current pulldown1+pulldown2 4.1mA*/

/*
**********************************************************
*					GLOBAL FUNCTION PROTOTYPE
**********************************************************
*/
/**
 * @Brief	PWM I/O reuse setting and pwm output/pulldown1/pulldown2 drive mode select
 * @Param	PwmChSel refer MACRO definition
 * @Param	PwmIoSel refer MACRO definition
 *			eg. When choosing PWM_CH0_A0_B27, selcet PWM_IO_0 is to use A0 or selct PWM_IO_1 is to use B27
 *			this two I/O cannot be used simultaneously because of mutex relationship .
 * @Param	PwmMode refer MACRO definition
 * @Return	None
 * @Note	Call API PwmConfig first and then PwmEnableChannel to generate PWM
 */
void PwmEnableChannel(uint8_t PwmChSel, uint8_t PwmIoSel, uint8_t PwmMode);

/**
 * @Brief	PWM io reuse disable
 * @Param	PwmChSel refer MACRO definition
 * @Return	None
 * @Note@ 	Disable the Channel of Group 0 and Group 1 together
 */
void PwmDisableChannel(uint8_t PwmChSel);

/**
 * @Brief	PWM Frequency and duty configuration
 * @Param	PwmChSel refer MACRO definition
 * @Param	FreqDiv
 *			pwm_freq = (12M/FreqDiv)Hz
 * @Param	Duty
 *			high level percentage = Duty/Freqdiv * 100%
 * @Return	None
 * @Note@	When Duty = FreqDivÊ±£¬output high level£¬When Duty = 0£¬output low level
 */
void PwmConfig(uint8_t PwmChSel, uint32_t FreqDiv, uint32_t Duty);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
