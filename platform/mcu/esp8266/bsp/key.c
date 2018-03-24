
#include "espressif/esp_common.h"
#include "gpio.h"
#include <stdio.h>
#include <aos/aos.h>

#define KEY_GPIO_PIN GPIO_Pin_14
#define KEY_GPIO_MODE GPIO_Mode_Input
#define KEY_GPIO_PULLUP GPIO_PullUp_DIS
#define KEY_GPIO_INTRTYPE GPIO_PIN_INTR_NEGEDGE

static uint64_t   elink_time = 0;

typedef void (*click_cb_t)(void);

click_cb_t click_callback = NULL;

void register_click_callback(click_cb_t cb)
{
    printf("Click callback registered.\n");
    click_callback = cb;
}

static void key_poll_func(void *arg)
{
    uint32_t level = GPIO_INPUT_GET(14);
    uint64_t diff;

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - elink_time;
        if (diff > 6000) { /*long long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
            if (click_callback) click_callback();
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void handle_elink_key()
{
    uint32_t level = GPIO_INPUT_GET(14);

    if ((level == 0) && (elink_time == 0)) {
        elink_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

static void key_gpio_enable_isr(void)
{
    _xt_isr_unmask(1 << ETS_GPIO_INUM);
}

static void key_gpio_isr(void *arg)
{
    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    if (gpio_status & (1 << 14))
        handle_elink_key();
}

void key_gpio_init(void)
{
    GPIO_ConfigTypeDef key_gpio_cnf;

    gpio_intr_handler_register(key_gpio_isr, NULL);

    key_gpio_cnf.GPIO_Pin = KEY_GPIO_PIN;
    key_gpio_cnf.GPIO_Mode = KEY_GPIO_MODE;
    key_gpio_cnf.GPIO_Pullup = KEY_GPIO_PULLUP;
    key_gpio_cnf.GPIO_IntrType = KEY_GPIO_INTRTYPE;
    gpio_config(&key_gpio_cnf);

    key_gpio_enable_isr();
}
