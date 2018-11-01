#include <atmel_start.h>

extern void user_pub(char *usr_payload);
extern g_mqtt_enabled_flag;
/**
 * Example of using EXTERNAL_IRQ_0
 */

static void button_on_PB31_pressed(void)
{
    static uint8_t press_times = 0;
    char str[5];
    gpio_toggle_pin_level(LED0);
    LOG("button pressed %d times, online: %d\n", press_times, g_mqtt_enabled_flag);
    press_times = press_times + 1;
    itoa(press_times, str, 10);
    if (g_mqtt_enabled_flag)
        user_pub(str);
}
 
void same54brd_button0_irq_init(void)
{
    ext_irq_register(PIN_PB31, button_on_PB31_pressed);
    ext_irq_enable(PIN_PB31);
}
 


    