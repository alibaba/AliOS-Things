#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "core_cm4.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_eint.h"
#include "connsys_driver.h"
#include "hal_nvic.h"
#include "mt7686.h"
#include <stdio.h>
#include "k_api.h"

#define Systick_Handler SysTick_Handler

void Systick_Handler(hal_nvic_irq_t num)
{
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
}
int systick_init()
{
  int ret=-1;
  ret = SysTick_Config(SystemCoreClock/1000);
  if(ret==-1)
  {
    printf("systick init error\r\n");
  }
  else 
  {
    printf("systick init sucess\r\n");
  }

  return 0;
}
