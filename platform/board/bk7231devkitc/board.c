/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <aos/kernel.h>
#include "aos/kernel.h"
#include "aos/yloop.h"

#include <aos/hal/flash.h>
#include <aos/hal/gpio.h>

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x00000,
        .partition_length           = 0x10000, //64k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x11000,//phy is 0x11000
        .partition_length           = 0xAF000, //700k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x100000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x101000,
        .partition_length           = 0x2000, // 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x103000,
        .partition_length          = 0xAF000, //700k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x1B2000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x1B3000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_RF_FIRMWARE] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "RF parameter",
	    .partition_start_addr       = 0x1B4000,// rf parameter is 0x1A2000
        .partition_length           = 0x1000,  // 4k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
};


#define KEY_BOOT   15
#define KEY_PS     14
static uint64_t   elink_time = 0;
static uint64_t   ps_time = 0;
static gpio_dev_t gpio_key_boot;
static gpio_dev_t gpio_key_ps;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_boot, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - elink_time;
		if (diff > 2000) { /* long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_ps_func(void *arg)
{
	#if 0
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_ps, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_ps_func, NULL);
    } else {
        diff = aos_now_ms() - ps_time;
		if (diff > 40) {
            ps_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_PSCLICK);
            
        }else if (diff > 2000) { /* short press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LPSCLICK);
        } else {
            aos_post_delayed_action(10, key_ps_func, NULL);
        }
    }
	#endif
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void key_ps_work(void *arg)
{
    aos_schedule_call(key_ps_func, NULL);
}

static void handle_elink_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_boot, &gpio_value);
    if (gpio_value == 0 && elink_time == 0) {
        elink_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}
static void handle_ps_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_ps, &gpio_value);
    if (gpio_value == 0 && ps_time == 0) {
        ps_time = aos_now_ms();
        aos_loop_schedule_work(0, key_ps_work, NULL, NULL, NULL);
    }
}

void board_init(void)
{
    gpio_key_boot.port = KEY_BOOT;
    gpio_key_boot.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_boot);

    hal_gpio_clear_irq(&gpio_key_boot);
    hal_gpio_enable_irq(&gpio_key_boot, IRQ_TRIGGER_FALLING_EDGE, handle_elink_key, NULL);

	gpio_key_ps.port = KEY_PS;
	gpio_key_ps.config = INPUT_PULL_UP;
	hal_gpio_init(&gpio_key_ps);
	
    hal_gpio_clear_irq(&gpio_key_ps);
    hal_gpio_enable_irq(&gpio_key_ps, IRQ_TRIGGER_FALLING_EDGE, handle_ps_key, NULL);
}

int board_cli_init(void)
{ 
    return -1;
}

