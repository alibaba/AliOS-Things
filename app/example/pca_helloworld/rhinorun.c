/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <stdint.h>

//#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"

#include "nrf_drv_systick.h"

#if LEDS_NUMBER <= 2
#error "Board is not equipped with enough amount of LEDs"
#endif

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

extern void stm32_soc_init(void);
extern ret_code_t app_timer_init(void);
static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];

void SysTick_Handler(void)
{
	krhino_intrpt_enter();
	krhino_tick_proc();
	krhino_intrpt_exit();
}	


void soc_init(void)
{
    ret_code_t err_code;
	
    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
	nrf_drv_clock_lfclk_request(NULL);
	app_timer_init();
    /* Configure LED-pins as outputs */
    bsp_board_leds_init();
	
	/* Init systick driver */
    nrf_drv_systick_init();

    /* Activate deep sleep mode */
//    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

}

void demo_task(void *arg)
{
#ifndef CONFIG_AOS_CLI
//    int count = 0;
#endif
    //
    soc_init();

    while (1)
    {
#ifndef CONFIG_AOS_CLI
        //printf("hello world! count %d\n", count++);
#endif
		bsp_board_led_on(LED_1);
		bsp_board_led_off(LED_2);
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

#ifdef BSP_BUTTON_0
    #define PIN_IN BSP_BUTTON_0
#endif
#ifndef PIN_IN
    #error "Please indicate input pin"
#endif

#ifdef BSP_LED_0
    #define PIN_OUT BSP_LED_0
#endif
#ifndef PIN_OUT
    #error "Please indicate output pin"
#endif

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_out_toggle(PIN_OUT);
}

static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(PIN_IN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN, true);
}


void demo_task_2(void *arg)
{
#ifndef CONFIG_AOS_CLI
//    int count = 0;
#endif
    //
    gpio_init();
    
	//printf("demo_task here!\n");
    //printf("rhino memory is %d!\n", krhino_global_space_get());

    while (1)
    {
#ifndef CONFIG_AOS_CLI
        //printf("hello world! count %d\n", count++);
#endif
//		bsp_board_led_on(LED_1);
//		bsp_board_led_off(LED_2);
//        //sleep 1 second
//        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

void demo_task_3(void *arg)
{
#ifndef CONFIG_AOS_CLI
//    int count = 0;
#endif
	soc_init();
//    bsp_board_leds_init();


    while (1)
    {
#ifndef CONFIG_AOS_CLI
        //printf("hello world! count %d\n", count++);
#endif

		bsp_board_led_on(0);
		bsp_board_led_invert(1);

//		nrf_delay_ms(500);

//        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    }
}


	
int main(void)
{
    krhino_init();

	
#ifdef CONFIG_AOS_CLI
    extern int aos_cli_init(void);
    aos_cli_init();
#endif

    krhino_task_create(&demo_task_obj, "demo_task", 0,DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task_3, 1);
    
    krhino_start();
    
    return 0;
}

