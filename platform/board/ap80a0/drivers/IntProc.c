#include "k_api.h"

#define CPU_CLK_HZ (96*1000*1000)
/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

extern volatile unsigned long gSysTick;
/**
* @brief This function handles System tick timer.
*/
void systick_interrupt(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  //HAL_IncTick();
  gSysTick++;
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
  //HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

#define NVIC_SYSTICK_CTRL		(*(volatile unsigned long*)0xe000e010)			//ctrl & status register
#define NVIC_SYSTICK_LOAD		(*(volatile unsigned long*)0xe000e014)			//reload reg
#define NVIC_SYSTICK_CURRENT	(*(volatile unsigned long*)0xe000e018)			//current count val reg
#define NVIC_SYSTICK_CAL		(*(volatile unsigned long*)0xe000e01C)			//calibration 

#define NVIC_SYSTICK_CLK		0x00000004
#define NVIC_SYSTICK_INT		0x00000002
#define NVIC_SYSTICK_ENABLE		0x00000001

void SysTickCfg(void)
{
	/* Configure SysTick to interrupt at the requested rate. */
	NVIC_SYSTICK_LOAD = ( CPU_CLK_HZ / RHINO_CONFIG_TICKS_PER_SECOND) - 1UL;
	NVIC_SYSTICK_CURRENT = 0;
	NVIC_SYSTICK_CTRL = NVIC_SYSTICK_CLK | NVIC_SYSTICK_INT | NVIC_SYSTICK_ENABLE;
}