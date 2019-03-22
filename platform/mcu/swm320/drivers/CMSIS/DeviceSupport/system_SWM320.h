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

#ifndef __SYSTEM_SWM320_H__
#define __SYSTEM_SWM320_H__

#ifdef __cplusplus
 extern "C" {
#endif


extern uint32_t SystemCoreClock;        // System Clock Frequency (Core Clock)
extern uint32_t CyclesPerUs;            // Cycles per micro second


extern void SystemInit(void);

extern void SystemCoreClockUpdate (void);

extern void switchCLK_20MHz(void);
extern void switchCLK_40MHz(void);
extern void switchCLK_32KHz(void);
extern void switchCLK_XTAL(void);
extern void switchCLK_PLL(void);

extern void PLLInit(void);


#ifdef __cplusplus
}
#endif

#endif //__SYSTEM_SWM320_H__
