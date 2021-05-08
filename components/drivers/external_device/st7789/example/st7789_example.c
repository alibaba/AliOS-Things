#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

// example based on haaseduk1
#include "hal_iomux_haas1000.h"
#include "st7789.h"

st7789_dev_t my_st7789 = {0};

// 硬件初始化
static void example_st7789_hw_init(int argc, char **argv)
{
    my_st7789.spi_port      = 0;
    my_st7789.spi_freq      = 26000000;
    my_st7789.gpio_bgl_id   = HAL_GPIO_PIN_P0_6;
    my_st7789.gpio_dc_id    = HAL_GPIO_PIN_P0_4;
    my_st7789.gpio_reset_id = HAL_GPIO_PIN_P0_0;

    st7789_hw_init(&my_st7789);
    return;
}

// 绘画随机矩形
static void example_st7789_draw_ract(int argc, char **argv)
{
    int      x0    = rand() % 240;
    int      x1    = rand() % 240;
    int      y0    = rand() % 240;
    int      y1    = rand() % 240;
    uint16_t color = rand();
    printf("drawing rectangle at %d %d %d %d in color %04X\n", x0, y0, x1, y1,
           color);
    st7789_draw_rect(my_st7789, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, 0xFFFF);
    st7789_draw_rect(my_st7789, x0, y0, x1, y1, color);
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(example_st7789_hw_init,
                       example_st7789_hw_init,
                       st7789 init test example)
ALIOS_CLI_CMD_REGISTER(example_st7789_draw_ract,
                       example_st7789_draw_ract,
                       st7789 display test example
                       : draw random       ract)
#endif
