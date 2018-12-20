#include "hal_gpio_stm32f1.h"
#include "aos/hal/gpio.h"

static gpio_irq_trigger_t mode_rising = IRQ_TRIGGER_RISING_EDGE;
static gpio_irq_trigger_t mode_falling = IRQ_TRIGGER_FALLING_EDGE;
static gpio_irq_trigger_t mode_both = IRQ_TRIGGER_BOTH_EDGES;
static uint8_t gpio_set = 1;
static uint8_t gpio_reset = 0;

gpio_dev_t brd_gpio_table[] = {
    {LED_0, OUTPUT_PUSH_PULL, &gpio_set},
    {LED_1, OUTPUT_PUSH_PULL, &gpio_set},
    {KEY_0, IRQ_MODE, &mode_rising},
    {KEY_1, IRQ_MODE, &mode_rising},
    {KEY_2, IRQ_MODE, &mode_rising},
    {BEEP_0,OUTPUT_PUSH_PULL, &gpio_set}
};

int32_t brd_gpio_init(void)
{
    int32_t i;
    int32_t ret = 0;

    for (i = 0; i < sizeof(brd_gpio_table)/sizeof(gpio_dev_t); ++i) {
        ret = hal_gpio_init(&brd_gpio_table[i]);
        if (ret) {
            printf("gpio %d in gpio table init fail \r\n", i);
        }
    }

    return ret;
}