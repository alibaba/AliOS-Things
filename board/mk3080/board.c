/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include <aos/kernel.h>
#include <aos/aos.h>

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0,
	    .partition_length           = 0x8000,    //32k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0xB000,
	    .partition_length           = 0xF2000, //568k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
        
	[HAL_PARTITION_PARAMETER_1] =
        {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0xFD000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
        
        [HAL_PARTITION_PARAMETER_2] =
        {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0xFE000,
        .partition_length           = 0x2000, // 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },

        [HAL_PARTITION_OTA_TEMP] =
        {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x100000,
        .partition_length          = 0xF2000, //568k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
        },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x1FD000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x1FE000,
        .partition_length           = 0x2000,// 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

#define KEY_AWSS   12

static uint64_t   awss_time = 0;
static gpio_dev_t gpio_key_awss;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_awss, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - awss_time;
        if (diff > 6000) { /*long long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            awss_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void handle_awss_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_awss, &gpio_value);
    if (gpio_value == 0 && awss_time == 0) {
        awss_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

void board_init(void)
{
    gpio_key_awss.port = KEY_AWSS;
    gpio_key_awss.config = INPUT_PULL_UP;

    hal_gpio_init(&gpio_key_awss);
    hal_gpio_enable_irq(&gpio_key_awss, IRQ_TRIGGER_FALLING_EDGE, handle_awss_key, NULL);
}
