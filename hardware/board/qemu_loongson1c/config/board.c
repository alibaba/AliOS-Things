#include <k_api.h>
#include <board.h>
#include <aos/init.h>
#include <ls1c_sys_tick.h>

void board_stduart_init(void)
{
}

void board_dma_init(void)
{
}

void board_gpio_init(void)
{
}

void board_tick_init(void)
{
    sys_tick_init(RHINO_CONFIG_TICKS_PER_SECOND);
}

void board_kinit_init(kinit_t *init_args)
{
}

void board_flash_init(void)
{
}

void board_network_init(void)
{
}
