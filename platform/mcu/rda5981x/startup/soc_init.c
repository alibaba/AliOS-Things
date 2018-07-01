#include "k_api.h"
#include "soc_init.h"
#include "hal/soc/uart.h"
#include "rda5981.h"
#include "cmsis_nvic.h"

static volatile uint8_t button_flags = 0;

static void SystemClock_Config(void);

static void SystemClock_Config(void)
{
    uint32_t  cnts;
    /* TODO: update SystemCoreClock */
    cnts = SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND;

    SysTick_Config(cnts);
}

/**
 * This is the timer interrupt service routine.
 */
void SysTick_Handler(void)
{
    krhino_intrpt_enter();

    krhino_tick_proc();

    krhino_intrpt_exit();
}

void rda_soc_init(void)
{
    /* Configure the system clock */
    SystemClock_Config();

    NVIC_Remap();
}
