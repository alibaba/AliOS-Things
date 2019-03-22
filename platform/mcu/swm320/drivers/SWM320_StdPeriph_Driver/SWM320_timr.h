/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#ifndef __SWM320_TIMR_H__
#define __SWM320_TIMR_H__

#define TIMR_MODE_TIMER     0
#define TIMR_MODE_COUNTER   1

void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint32_t period, uint32_t int_en);
void TIMR_Start(TIMR_TypeDef * TIMRx);
void TIMR_Stop(TIMR_TypeDef * TIMRx);
void TIMR_Halt(TIMR_TypeDef * TIMRx);
void TIMR_Resume(TIMR_TypeDef * TIMRx);

void TIMR_SetPeriod(TIMR_TypeDef * TIMRx, uint32_t period);
uint32_t TIMR_GetPeriod(TIMR_TypeDef * TIMRx);
uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx);

void TIMR_INTEn(TIMR_TypeDef * TIMRx);
void TIMR_INTDis(TIMR_TypeDef * TIMRx);
void TIMR_INTClr(TIMR_TypeDef * TIMRx);
uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx);


#endif //__SWM320_TIMR_H__
