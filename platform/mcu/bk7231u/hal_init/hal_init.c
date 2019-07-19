<<<<<<< HEAD
#include <sys/unistd.h>
#include <sys/errno.h>

#include "aos/hal/flash.h"
#include "aos/hal/wdg.h"
#include "aos/hal/uart.h"

#include "board.h"

extern hal_flash_init();
extern int errno;
wdg_dev_t  wdg;
uart_dev_t uart_0;

void hal_init(void)
{
    uart_0.port 			   = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity	   = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
    hal_flash_init();
}

void hal_boot(hal_partition_t partition)
{
    uint32_t addr;

    intc_deinit();

    addr = hal_flash_get_info(partition)->partition_start_addr;
    __asm volatile ("BX %0" : : "r" (addr) );
}

void hal_reboot(void)
{
    wdg.port = 0;
    wdg.config.timeout = 1;

    hal_wdg_init(&wdg);
<<<<<<< HEAD
=======

    while(1);
>>>>>>> dd68a6ad6 (BugID:19092717: Merge Beken flash midea patch)
}

=======
#include <sys/unistd.h>
#include <sys/errno.h>

#include "aos/hal/flash.h"
#include "aos/hal/wdg.h"
#include "aos/hal/uart.h"

#include "board.h"

extern hal_flash_init();
extern int errno;
wdg_dev_t  wdg;
uart_dev_t uart_0;

void hal_init(void)
{
    uart_0.port 			   = BK_UART;
    uart_0.config.baud_rate    = BK_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity	   = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
    hal_flash_init();
}

void hal_boot(hal_partition_t partition)
{
    uint32_t addr;
    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    intc_deinit();

    if (hal_flash_info_get(partition, partition_info) != 0) {
        return -1;
    }

    addr = partition_info->partition_start_addr;

    __asm volatile ("BX %0" : : "r" (addr) );
}

void hal_reboot(void)
{
    wdg.port = 0;
    wdg.config.timeout = 1;

    hal_wdg_init(&wdg);

    while(1);
}

>>>>>>> 25702b9f5 (BugID:16846667: replace hal_flash_get_info with hal_flash_info_get)
