/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>

#include "ulog/ulog.h"
#include "flower_app.h"
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


extern uint32_t hal_fast_sys_timer_get();
extern uint32_t hal_cmu_get_crystal_freq();

#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)
#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))


#define DHT11_PIN	HAL_IOMUX_PIN_P0_1

static int fd = 0;
static uint8_t last_temp = 0,last_hum = 0;
extern uint8_t mqtt_status;


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

// temp && humidity gpio set
static void DHT11_GPIO_Set(unsigned char leve)
{
    struct gpio_io_config config;

    config.id = DHT11_PIN;
    config.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_ODPU;
    if(leve == 1){
        config.data = 1;
    }
    else if(leve == 0){
        config.data = 0;
    }
    ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
}

static uint32_t DHT11_GPIO_Get(void)
{
    struct gpio_io_config config;
    int ret = 0xff;

    config.id = DHT11_PIN;
    config.config = GPIO_IO_INPUT | GPIO_IO_INPUT_PU;
    config.data = 0;
    ret = ioctl(fd, IOC_GPIO_GET, (unsigned long)&config);
    return ret;
}

//复位DHT11
void DHT11_Reset(void)
{
    DHT11_GPIO_Set(0);
    _msdelay(20);
    DHT11_GPIO_Set(1);
    _udelay(30);
}

uint8_t DHT11_IsOnline(void)
{
    uint8_t retry = 0;
    //DHT11会拉低40~80us
    while (DHT11_GPIO_Get() && retry < 100){
        retry ++;
        _udelay(1);
    }
    if(retry >= 100){
        LOGE("APP", "DHT Pin High!\n");
        return 1;
    }
    else{
        retry = 0;
    }
    //DHT11拉低后会再次拉高40~80us
    while (!DHT11_GPIO_Get() && retry < 100){
        retry ++;
        _udelay(1);
    }
    if(retry >= 100){
        LOGE("APP", "DHT Pin Low!\n");
        return 1;
    }
    return 0;
}

uint8_t DHT11_ReadBit(void)
{
    uint8_t retry = 0;
    while(DHT11_GPIO_Get() && retry < 100){
        retry ++;
        _udelay(1);
    }
    retry = 0;
    while(!DHT11_GPIO_Get() && retry < 100){
        retry ++;
        _udelay(1);
    }
    _udelay(40);//等待40us
    if(DHT11_GPIO_Get()){
        return 1;
    }
    else {
        return 0;
    }
}

uint8_t DHT11_ReadByte(void)
{
    uint8_t i,dat;
    dat = 0;
    for (i = 0; i < 8; i ++) {
        dat <<= 1;
        dat |= DHT11_ReadBit();
    }
    return dat;
}

uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;

    DHT11_Reset();
    if(DHT11_IsOnline() == 0){
        //读取40位数据
        for(i = 0; i < 5; i ++){
            buf[i] = DHT11_ReadByte();
        }
        if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]){
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else {
        LOGE("APP", "DHT is not online!\n");
        return 1;
    }
    return 0;
}

void report_2_cloud(void *dm_handle)
{
    uint8_t temp =0,humidity=0,d_flag = 0;
    char property_payload[30] = {0};

    if(mqtt_status == 0){
        printf("mqtt status :%d %p\r\n",mqtt_status,dm_handle);
        return;
    }

    d_flag = DHT11_Read_Data(&temp,&humidity);

    printf("temp ->%d  humidity->%d --%d\n",temp,humidity,d_flag);
    if((last_temp != temp)&&(!d_flag)){
        snprintf(property_payload, sizeof(property_payload), "{\"Temperature\": %d}", temp);
        printf("report:%s\r\n",property_payload);
        demo_send_property_post(dm_handle, property_payload);
        last_temp = temp;
    }
    if((last_hum != humidity)&&(!d_flag)){
        snprintf(property_payload, sizeof(property_payload), "{\"Humidity\": %d}", humidity);
        printf("report:%s\r\n",property_payload);
        demo_send_property_post(dm_handle, property_payload);
        last_hum = humidity;
    }
}

static void handle_temp_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uint8_t temp =0,humidity=0;

    if(0 == strcmp(argv[1],"0")){
        DHT11_GPIO_Set(0);
    }
    else if(0 == strcmp(argv[1],"1")){
        DHT11_GPIO_Set(1);
    }
    else if(0 == strcmp(argv[1],"2")){
        DHT11_Reset();
    }
    else if(0 == strcmp(argv[1],"3")){
        DHT11_Read_Data(&temp,&humidity);
        LOGI("APP", "temp ->%d  humidity->%d\n",temp,humidity);
    }
}

#if AOS_COMP_CLI
static struct cli_command temp_cmd = {
    .name     = "temp",
    .help     = "temp [read]",
    .function = handle_temp_cmd
};
#endif /* AOS_COMP_CLI */

int flower_gpio_init(void)
{
    gpio_dev_t temp_gpio;

    temp_gpio.port = DHT11_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    fd = open("/dev/gpio", 0);
    printf("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    DHT11_GPIO_Set(1);
    DHT11_Reset();
    #if AOS_COMP_CLI
    aos_cli_register_command(&temp_cmd);
    #endif /* AOS_COMP_CLI */
    return 0;
}
