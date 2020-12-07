/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include "aos/hal/gpio.h"
#include "ulog/ulog.h"
#include "hal_iomux_haas1000.h"
#include "flower_app.h"
#include "k_api.h"
#include "aos/cli.h"

extern void user_post_event(char type_t);
extern void user_post_property(char type_t,uint8_t value);
extern uint32_t hal_sys_timer_systick_hz();

extern uint32_t hal_fast_sys_timer_get();
extern uint32_t hal_cmu_get_crystal_freq();

#define CONFIG_FAST_SYSTICK_HZ      (hal_cmu_get_crystal_freq() / 4)
#define FAST_TICKS_TO_US(tick)      ((uint32_t)(tick) * 10 / (CONFIG_FAST_SYSTICK_HZ / 1000 / 100))

#define DH_INPUT	0
#define DH_OUTPUT	1

static gpio_dev_t key_gpio;
static gpio_dev_t temp_gpio;
static long long edge_low_ms = 0;
static char edge_flag = 0;
static uint8_t last_temp = 0,last_hum = 0;


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

static void gpio_edge_handle(void)
{
    long long ms;
    uint32_t left_ms;
    hal_gpio_clear_irq(&key_gpio);
    if(edge_flag == 0){
        edge_flag = 1;
        if(edge_low_ms == 0){
            LOGI("APP", "valid! ->%d\n",left_ms);
            user_post_event(1);
        }
        else{
            ms = aos_now_ms();
            left_ms = (uint32_t)(ms - edge_low_ms);
            if(left_ms >= 2000){
                LOGI("APP", "valid! ->%d\n",left_ms);
                user_post_event(1);
            }
            else{
                LOGI("APP", "invalid! ->%d\n",left_ms);
            }   
        }
        edge_low_ms = ms;
    }
    else{
        edge_flag = 0;
        ms = aos_now_ms();
        left_ms = (uint32_t)(ms - edge_low_ms);
        if(left_ms >= 2000){
            LOGI("APP", "valid! ->%d\n",left_ms);
            user_post_event(1);
        }
        else{
            LOGI("APP", "invalid! ->%d\n",left_ms);
        }   
        edge_low_ms = ms;  
    } 
    hal_gpio_disable_irq(&key_gpio);    
    hal_gpio_enable_irq(&key_gpio, IRQ_TRIGGER_FALLING_EDGE, gpio_edge_handle, (void *)HAL_IOMUX_PIN_P4_0);
}

// temp && humidity config
static void DHT11_GPIO_Config(unsigned char io_type)
{
	if(DH_INPUT == io_type){
		temp_gpio.config = INPUT_PULL_UP;
	}  	
	else if(DH_OUTPUT == io_type){
		temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
	}
    hal_gpio_init(&temp_gpio);
}
static void DHT11_GPIO_Set(unsigned char leve)
{
	if(leve == 1){
		hal_gpio_output_high(&temp_gpio);
	}
	else if(leve == 0){
		hal_gpio_output_low(&temp_gpio);
	}
}
static uint32_t DHT11_GPIO_Get(void)
{
	uint32_t ret=0xffffffff;

	hal_gpio_input_get(&temp_gpio,&ret);
	return ret;
}
//复位DHT11
void DHT11_Reset(void)	   
{                 
	DHT11_GPIO_Config(DH_OUTPUT); 	
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
	DHT11_GPIO_Config(DH_INPUT);
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

void report_event2cloud(void)
{
	uint8_t temp =0,humidity=0,d_flag = 0;

	d_flag = DHT11_Read_Data(&temp,&humidity); 
	LOGI("APP", "temp ->%d  humidity->%d --%d\n",temp,humidity,d_flag);
	if((last_temp != temp)&&(!d_flag)){
		user_post_property(1,temp);
		last_temp = temp;
	}
	if((last_hum != humidity)&&(!d_flag)){
		user_post_property(0,humidity);
		last_hum = humidity;
	}   
}

static void handle_temp_cmd(char *pwbuf, int blen, int argc, char **argv)
{
	uint8_t temp =0,humidity=0;

	if(0 == strcmp(argv[1],"0")){
		DHT11_GPIO_Config(DH_OUTPUT);
		DHT11_GPIO_Set(0);
	}
	else if(0 == strcmp(argv[1],"1")){
		DHT11_GPIO_Config(DH_OUTPUT);
		DHT11_GPIO_Set(1);
	}
	else if(0 == strcmp(argv[1],"2")){
		DHT11_Reset();
	}
	else if(0 == strcmp(argv[1],"3")){
		DHT11_Read_Data(&temp,&humidity); 
		LOGI("APP", "temp ->%d  humidity->%d\n",temp,humidity);
	}
	DHT11_Read_Data(&temp,&humidity); 
	LOGI("APP", "temp ->%d  humidity->%d\n",temp,humidity);
	user_post_property(0,humidity);
	user_post_property(1,temp);
}

static struct cli_command temp_cmd = {
    .name     = "temp",
    .help     = "temp [read]",
    .function = handle_temp_cmd
};

int flower_gpio_init(void)
{
    int ret = 0;

    key_gpio.port   = HAL_IOMUX_PIN_P0_0;
    key_gpio.config = IRQ_MODE;
    hal_gpio_init(&key_gpio);

    ret = hal_gpio_enable_irq(&key_gpio, IRQ_TRIGGER_FALLING_EDGE, gpio_edge_handle, (void *)HAL_IOMUX_PIN_P0_0);
    LOGI("APP", "flower_gpio_init! ->%d\n",ret);

    temp_gpio.port = HAL_IOMUX_PIN_P0_1;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
	DHT11_GPIO_Set(1);
	DHT11_Reset();
	aos_cli_register_command(&temp_cmd);
    return ret;
}
