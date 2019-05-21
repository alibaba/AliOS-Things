/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) Beken Corp 2011-2020
 *
 ****************************************************************************************
 */
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "mem_pub.h"
#include "ll.h"
#include "uart_pub.h"
#include "app.h"
#include "ate_app.h"
#include "k_api.h"

void print_exception_addr(unsigned int pc, unsigned int lr, unsigned int sp)
{
    cpu_intrpt_save();
    os_printf("pc is %x, lr is %x, sp is %x\n", pc, lr, sp);
    while (1);
}

#if 1
#define RECORD_COUNT 128
int rec_id = 0;
int rec_array[RECORD_COUNT] = {0};

void rec_playback(void)
{
	int id = rec_id;
	int i;

	for(i = 0; i < 64; i ++)
	{
		id --;
		os_printf("%x,%x\r\n", (id) & (RECORD_COUNT - 1), rec_array[(id) & (RECORD_COUNT - 1)]);
	}
}

void rec_func(int val)
{
	rec_array[(rec_id ++) & (RECORD_COUNT - 1)] = val;
}
#endif

beken_mutex_t stdio_tx_mutex;

static void init_app_thread( void *arg )
{
	rtos_init_mutex( &stdio_tx_mutex );
	application_start();
}

extern void fclk_init(void);
extern void test_case_task_start(void);


static int test_cnt;

void task_test3(void *arg)
{
	beken_semaphore_t sem;

	rtos_init_semaphore(&sem, 0);
    while (1) {
        os_printf("test_cnt is %d\r\n", test_cnt++);
		rtos_get_semaphore(&sem, 1000);
    }
}



void entry_main(void)
{
    sys_start();
}

extern void hw_start_hal(void);

void soc_driver_init(void)
{
	#if ATE_APP_FUN
    ate_app_init();
	#endif

    driver_init();
}

void soc_system_init(void)
{
    func_init();

    fclk_init();

    hal_init();

#ifndef AOS_NO_WIFI
    app_start();

    hw_start_hal();
#endif
}

// eof

