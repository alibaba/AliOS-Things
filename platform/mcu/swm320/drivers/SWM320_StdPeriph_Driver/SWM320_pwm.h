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

#ifndef __SWM320_PWM_H__
#define __SWM320_PWM_H__

typedef struct {
    uint8_t  clk_div;

    uint8_t  mode;

    uint16_t cycleA;
    uint16_t hdutyA;
    uint16_t deadzoneA;
    uint8_t  initLevelA;

    uint16_t cycleB;
    uint16_t hdutyB;
    uint16_t deadzoneB;
    uint8_t  initLevelB;

    uint8_t  HEndAIEn;
    uint8_t  NCycleAIEn;
    uint8_t  HEndBIEn;
    uint8_t  NCycleBIEn;
} PWM_InitStructure;

#define PWM_CLKDIV_1    0
#define PWM_CLKDIV_8    1

#define PWM_MODE_INDEP          0
#define PWM_MODE_COMPL          1
#define PWM_MODE_INDEP_CALIGN   3
#define PWM_MODE_COMPL_CALIGN   4

#define PWM_CH_A    0
#define PWM_CH_B    1


void PWM_Init(PWM_TypeDef * PWMx, PWM_InitStructure * initStruct);
void PWM_Start(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB);
void PWM_Stop(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB);

void PWM_SetCycle(PWM_TypeDef * PWMx, uint32_t chn, uint16_t cycle);
uint16_t PWM_GetCycle(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_SetHDuty(PWM_TypeDef * PWMx, uint32_t chn, uint16_t hduty);
uint16_t PWM_GetHDuty(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_SetDeadzone(PWM_TypeDef * PWMx, uint32_t chn, uint8_t deadzone);
uint8_t  PWM_GetDeadzone(PWM_TypeDef * PWMx, uint32_t chn);

void PWM_IntNCycleEn(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_IntNCycleDis(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_IntNCycleClr(PWM_TypeDef * PWMx, uint32_t chn);
uint32_t PWM_IntNCycleStat(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_IntHEndEn(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_IntHEndDis(PWM_TypeDef * PWMx, uint32_t chn);
void PWM_IntHEndClr(PWM_TypeDef * PWMx, uint32_t chn);
uint32_t PWM_IntHEndStat(PWM_TypeDef * PWMx, uint32_t chn);

#endif //__SWM320_PWM_H__
