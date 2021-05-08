/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>

#include "ulog/ulog.h"
#include "auto_app.h"
#include "k_api.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <sys/ioctl.h>
#include <vfsdev/gpio_dev.h>
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>
#include "aos/vfs.h"

#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"

static int fd = 0;


extern uint32_t hal_fast_sys_timer_get();
extern uint32_t hal_cmu_get_crystal_freq();

#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)
#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

#define IN1_PORT    0
#define IN2_PORT    1
#define IN3_PORT    2
#define IN4_PORT    3
#define ENA_PORT    4
#define ENB_PORT    5

#define IN1_PIN	HAL_IOMUX_PIN_P4_7
#define IN2_PIN	HAL_IOMUX_PIN_P4_0
#define IN3_PIN	HAL_IOMUX_PIN_P2_6
#define IN4_PIN	HAL_IOMUX_PIN_P4_6
#define ENA_PIN	HAL_IOMUX_PIN_P2_4
#define ENB_PIN	HAL_IOMUX_PIN_P2_5

#define DEMO_TIME_DEFAULT_MS    300

#if (RHINO_CONFIG_HW_COUNT > 0)
void _udelay(unsigned long x) {
    unsigned long now,t;

    t = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    now = t;
    while ((now - t) < x) {
        now = FAST_TICKS_TO_US(hal_fast_sys_timer_get());
    }
}

void _msdelay(unsigned long x) {
    _udelay(x * 1000);
}
#else
#error "RHINO_CONFIG_HW_COUNT should be configured to get us level delay"
#endif

//auto gpio set
static void GPIO_Set(unsigned char port,unsigned char leve)
{
    struct gpio_io_config config;

    switch(port){
        case IN1_PORT:
            config.id = IN1_PIN;//8*4 + 7;
        break;

        case IN2_PORT:
            config.id = IN2_PIN;//8*4 + 0;
        break;

        case IN3_PORT:
            config.id = IN3_PIN;//8*2 + 6;
        break;

        case IN4_PORT:
            config.id = IN4_PIN;//8*4 + 6;
        break;

        case ENA_PORT:
            config.id = ENA_PIN;//8*2 + 4;
        break;

        case ENB_PORT:
            config.id = ENB_PIN;//8*2 + 5;
        break;

        default:
        break;
    }
    config.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_ODPU;
    if(leve == 1){
        config.data = 1;
    }
    else if(leve == 0){
        config.data = 0;
    }
    ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
}

//停止
void stop_ctl(void)
{
    GPIO_Set(IN1_PORT,0);
    GPIO_Set(IN2_PORT,0);
    GPIO_Set(IN3_PORT,0);
    GPIO_Set(IN4_PORT,0);
}

//前进
void front_ctl(void)
{
    GPIO_Set(IN1_PORT,1);
    GPIO_Set(IN2_PORT,0);
    GPIO_Set(IN3_PORT,0);
    GPIO_Set(IN4_PORT,1);
    _msdelay(DEMO_TIME_DEFAULT_MS);
    stop_ctl();
}

//后退
void back_ctl(void)
{
    GPIO_Set(IN1_PORT,0);
    GPIO_Set(IN2_PORT,1);
    GPIO_Set(IN3_PORT,1);
    GPIO_Set(IN4_PORT,0);
    _msdelay(DEMO_TIME_DEFAULT_MS);
    stop_ctl();
}

//左转
void left_ctl(void)
{
    GPIO_Set(IN1_PORT,0);
    GPIO_Set(IN2_PORT,0);
    GPIO_Set(IN3_PORT,0);
    GPIO_Set(IN4_PORT,1);
    _msdelay(DEMO_TIME_DEFAULT_MS);
    stop_ctl();
}

//右转
void right_ctl(void)
{
    GPIO_Set(IN1_PORT,1);
    GPIO_Set(IN2_PORT,0);
    GPIO_Set(IN3_PORT,0);
    GPIO_Set(IN4_PORT,0);
    _msdelay(DEMO_TIME_DEFAULT_MS);
    stop_ctl();
}

static void handle_haas_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if(0 == strcmp(argv[1],"0")){
        stop_ctl();
        LOGI("APP", "stop\n");
    }
    else if(0 == strcmp(argv[1],"1")){
        front_ctl();
        LOGI("APP", "front\n");
    }
    else if(0 == strcmp(argv[1],"2")){
        back_ctl();
        LOGI("APP", "back\n");
    }
    else if(0 == strcmp(argv[1],"3")){
        left_ctl();
        LOGI("APP", "left\n");
    }
    else if(0 == strcmp(argv[1],"4")){
        right_ctl();
        LOGI("APP", "right\n");
    }
}

#if AOS_COMP_CLI

static struct cli_command haas_cmd = {
    .name     = "haas",
    .help     = "haas [read]",
    .function = handle_haas_cmd
};

int auto_app_init(void)
{
    gpio_dev_t temp_gpio;

    temp_gpio.port = IN1_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN2_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN3_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN4_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    temp_gpio.port = ENA_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = ENB_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    fd = open("/dev/gpio", 0);
    printf("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    GPIO_Set(ENA_PORT,1);
    GPIO_Set(ENB_PORT,1);
    stop_ctl();

    aos_cli_register_command(&haas_cmd);
    return 0;
}
#endif /* AOS_COMP_CLI */
