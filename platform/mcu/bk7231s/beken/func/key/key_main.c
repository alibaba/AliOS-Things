#include "include.h"
#include "typedef.h"
#include "arm_arch.h"
#include "gpio_pub.h"
#include "key_main.h"
#include "multi_button.h"
#include "rtos_pub.h"

beken_timer_t g_key_timer;


#if 1
BUTTON_S button_test;

void key_short_press(void *param)
{
	KEY_PRT("key_short_press\r\n");
}

void key_double_press(void *param)
{
	KEY_PRT("key_double_press\r\n");
}

void key_long_press_hold(void *param)
{
	KEY_PRT("key_long_press_hold\r\n");
}
#endif

void key_get_setting(void)
{
}

void key_configure(void)
{
	OSStatus result;
	key_item_configure();
	
    result = rtos_init_timer(&g_key_timer,
                            KEY_TMR_DURATION,
                            button_ticks,
                            (void *)0);
    ASSERT(kNoErr == result);
	
    result = rtos_start_timer(&g_key_timer);
    ASSERT(kNoErr == result);
}

void key_unconfig(void)
{
	OSStatus ret;
	
	if(rtos_is_timer_init(&g_key_timer))
	{
	    if (rtos_is_timer_running(&g_key_timer)) 
		{
	        ret = rtos_stop_timer(&g_key_timer);
			ASSERT(kNoErr == ret);
	    }

	    ret = rtos_deinit_timer(&g_key_timer);
		ASSERT(kNoErr == ret);
	}
}

uint8_t key_get_gpio_value(void)
{
	return bk_gpio_input(GPIO_TEST_ID);
}

uint8_t key_gpio_init(void)
{
	bk_gpio_config_input_pup(GPIO_TEST_ID);
}

void key_item_configure(void)
{
	key_gpio_init();
	button_init(&button_test, key_get_gpio_value, 0);
	
	button_attach(&button_test, SINGLE_CLICK,     key_short_press);
	button_attach(&button_test, DOUBLE_CLICK,     key_double_press);
	button_attach(&button_test, LONG_PRESS_HOLD,  key_long_press_hold);
	
	button_start(&button_test);
}

void key_initialization(void)
{
	key_configure();
}

void key_uninitialization(void)
{
	key_unconfig();
}

// eof

