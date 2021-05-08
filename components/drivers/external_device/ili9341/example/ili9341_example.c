#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

// example based on haaseduk1
#include "hal_iomux_haas1000.h"
#include "ili9341.h"

ili9341_dev_t my_ili9341 = {0};

static void example_ili9341_hw_init(int argc, char **argv)
{
    my_ili9341.spi_port = 0;
    my_ili9341.spi_freq = 26000000;
    my_ili9341.gpio_bgl_id = HAL_GPIO_PIN_P0_6;
    my_ili9341.gpio_dc_id = HAL_GPIO_PIN_P0_1;
    my_ili9341.gpio_reset_id = HAL_GPIO_PIN_P0_0;

    ili9341_hw_init(&my_ili9341);
    return;
}

static void example_ili9341_draw_ract(int argc, char **argv)
{
    uint16_t value;
    value = strtoul(argv[1], NULL, 0);
    // ili9341_draw_rect(my_ili9341, 0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, 0xFFFF);
    // ili9341_draw_rect(my_ili9341, rand() % 240, rand() % 240, rand() % 240, rand() % 240, rand());
    ili9341_draw_rect(my_ili9341, rand() % 240, rand() % 240, rand() % 240, rand() % 240, value);
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(example_ili9341_hw_init, ili9341_hw_init_example, ili9341 init test example)
ALIOS_CLI_CMD_REGISTER(example_ili9341_draw_ract, ili9341_draw_ract_example, ili9341 display test example)
#endif
