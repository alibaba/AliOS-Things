/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdint.h>
#include <mt_gic_v3.h>
#include <mt_reg_base.h>
#include <mt_printf.h>
#include <mt_gpio_keys.h>
#include <mt_gpio.h>
#include <eint.h>

static gpio_keys_event gpio_keys_map[GPIO_NUM] = {0};
static unsigned int pio_keys_pin_queue[KEY_NUM] = {
	VOLUMEUP_KEY_PIN,
	VOLUMEDOWN_KEY_PIN,
	MENU_KEY_PIN
};

static int gpio_keys_keycode_queue[KEY_NUM] = {
	VOLUMEUP_KEY,
	VOLUMEDOWN_KEY,
	MENU_KEY
};

static void gpio_keys_set_map(void)
{
	int i = 0;
	for(i = 0; i < KEY_NUM; i++) {
		gpio_keys_map[pio_keys_pin_queue[i]].keycode = gpio_keys_keycode_queue[i];
		gpio_keys_map[pio_keys_pin_queue[i]].status = 1;
	}
	return;
}

static void gpio_keys_set_gpio(void)
{
	int i = 0;
	for(i = 0; i < KEY_NUM; i++) {
		gpio_set_mode(pio_keys_pin_queue[i], GPIO_MODE_GPIO);
		gpio_set_dir(pio_keys_pin_queue[i], GPIO_DIR_IN);
		gpio_set_pull_enable(pio_keys_pin_queue[i], GPIO_PULL_ENABLE);
		gpio_set_pull_select(pio_keys_pin_queue[i], GPIO_PULL_UP);
	}
	return;
}

void gpio_keys_interrupt_handler(int irq_num)
{
	int pin = -1;
	int irq_flag = -1;
	gpio_keys_event key_event;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	pin = gpio_get_in(irq_num);
	if (pin != gpio_keys_map[irq_num].status) {
		printf("[gpiokeys] pin[%d] keycode[%d] is %s\n", irq_num, gpio_keys_map[irq_num].keycode, pin ? "released" : "pressed");
		if (pin) {
			mt_eint_set_polarity(irq_num, LOW_LEVEL_TRIGGER);
		} else {
			mt_eint_set_polarity(irq_num, HIGH_LEVEL_TRIGGER);
		}
		gpio_keys_map[irq_num].status = pin;
		key_event = gpio_keys_map[irq_num];
		xQueueSendFromISR(key_eventqueue, &key_event, &xHigherPriorityTaskWoken );
	} else {
		printf("[gpiokeys] error: gpio pin has recovered!!!\n");
	}
	return;
}

void gpio_keys_request_irq(void)
{
	int i = 0;

	for(i = 0; i < KEY_NUM; i ++) {
		mt_eint_registration(pio_keys_pin_queue[i], EDGE_SENSITIVE, LOW_LEVEL_TRIGGER, gpio_keys_interrupt_handler, 1, 1);
	}

	return;
}

void gpio_keys_init(void)
{
	printf("[gpiokeys] info: 1.keypad start\n");
	gpio_keys_set_map();
	gpio_keys_set_gpio();
	gpio_keys_request_irq();
	printf("[gpiokeys] info: 2.gpio_keys_request_irq done\n");

	key_eventqueue = xQueueCreate(EVENT_NUM, sizeof(gpio_keys_event));

	return;
}
