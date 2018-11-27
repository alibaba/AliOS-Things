#include <atmel_start.h>

/**
 * Initializes MCU, drivers and middleware in the project
**/
void atmel_start_init(void)
{
	system_init();

	sd_mmc_stack_init();

	usart_os_enable(&USART_0);
	usart_os_enable(&USART_2);
}
