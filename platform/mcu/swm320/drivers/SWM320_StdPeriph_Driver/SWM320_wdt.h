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

#ifndef __SWM320_WDT_H__
#define __SWM320_WDT_H__

#define WDT_MODE_RESET      0
#define WDT_MODE_INTERRUPT  1

void WDT_Init(WDT_TypeDef * WDTx, uint32_t peroid, uint32_t mode);
void WDT_Start(WDT_TypeDef * WDTx);
void WDT_Stop(WDT_TypeDef * WDTx);

void WDT_Feed(WDT_TypeDef * WDTx);

int32_t WDT_GetValue(WDT_TypeDef * WDTx);


void WDT_INTClr(WDT_TypeDef * WDTx);
uint32_t WDT_INTStat(WDT_TypeDef * WDTx);

#endif //__SWM320_WDT_H__
