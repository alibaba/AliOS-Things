#include "rec_sys.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "rec_uart.h"
#include "hal_boot_process.h"

extern void rec_uart_init();
extern void rec_uart_deinit();
extern void rec_flash_init();

unsigned int rec_xz_backup_flash_addr;
unsigned int rec_conf_backup_flash_addr;

void rec_delayms(volatile int timesMS)
{
	HAL_Delay(timesMS);
}

void rec_hal_init()
{
    rec_uart_init();
    rec_flash_init();
}

void rec_err_print(void *errinfo)
{
    boot_printf("rec Exception.\r\n");
}

void rec_reboot(void)
{
    HAL_NVIC_SystemReset();
    for(;;);
}

void rec_upgrade_reboot()
{
    sw_bank();
}
