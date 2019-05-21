/****************************************************************************
**
** Copyright (C) 2017 Beken Corporation, All rights reserved.
**
** Contact: Beken Corporation (www.beken.com)
**
**
** History: 2017/10/30 
**
** Version: 1.0
**
****************************************************************************/

#include "include.h"
#include "arm_arch.h"

#include "sys_rtos.h"
#include "rtos_pub.h"

#include "gpio_pub.h"
#include "gpio.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "uart_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"


#if (CONFIG_APP_MP3PLAYER==1)

// dac i2c interface 
#define I2C_CLK 26
#define I2C_DAT 27

typedef struct Codec_RegCfg_s
{
	unsigned int map;
	unsigned char reg;
}Codec_ES8374_RegCfg_t;

// 0-output&pullup, 1--input
static void gpio_config_c( UINT32 index, UINT32 mode ) 
{
	volatile UINT32 *gpio_cfg_addr;
	UINT32 val;
	UINT32 tmp;
	if(index >GPIONUM)
		return;
	gpio_cfg_addr = (volatile UINT32 *)(REG_GPIO_CFG_BASE_ADDR + index * 4);
	tmp =mode;
	mode &= 0xff;
	
	if(mode == 1)
		val =0x0c;
	else
	{
		val = 0x30;
		if(tmp>>8)
			val |= 1<<1;
	}
	REG_WRITE(gpio_cfg_addr, val);
}



static void gpio_output_c(UINT32 index, UINT32 val)
{
    UINT32 reg_val;
    volatile UINT32 *gpio_cfg_addr;

    gpio_cfg_addr = (volatile UINT32 *)(REG_GPIO_CFG_BASE_ADDR + index * 4);
    reg_val = REG_READ(gpio_cfg_addr);

    reg_val &= ~GCFG_OUTPUT_BIT;
    reg_val |= (val & 0x01) << GCFG_OUTPUT_POS;
    REG_WRITE(gpio_cfg_addr, reg_val);
}

static UINT32 DATA_INPUT(void)
{
    UINT32 val = 0;
    volatile UINT32 *gpio_cfg_addr;

    gpio_cfg_addr = (volatile UINT32 *)(REG_GPIO_CFG_BASE_ADDR + I2C_DAT * 4);
    val = REG_READ(gpio_cfg_addr);

    return (val & 1);
}


static void CLK_OUTPUT_HIGH(void)
{
	gpio_output_c(I2C_CLK,1);
}
static void CLK_OUTPUT_LOW(void)
{
	gpio_output_c(I2C_CLK,0);
}

static void DATA_OUTPUT_HIGH(void)
{
	gpio_output_c(I2C_DAT,1);
}

static void DATA_OUTPUT_LOW(void)
{
	gpio_output_c(I2C_DAT,0);
}

static void SET_DATA_INPUT(void)
{
	gpio_config_c(I2C_DAT,1);
}

static void set_data_output(void)
{
	UINT32 mode;
	UINT32 val = 	DATA_INPUT();
	mode = (val<<8) ;
	gpio_config_c(I2C_DAT,mode);
}

/*1 equals about 5 us*/
static void es8374_codec_i2c_delay(int us)
{
    volatile int i, j;
    for(i = 0; i < us; i++)
    {
        j = 50;
        while(j--);
    }	
}

static void es8374_codec_i2c_start(void)
{
    // start bit
	CLK_OUTPUT_HIGH();
	DATA_OUTPUT_HIGH();
    es8374_codec_i2c_delay(1); 
	DATA_OUTPUT_LOW();
    es8374_codec_i2c_delay(1); 
}

static void es8374_codec_i2c_stop(void)
{
	//stop bit
	CLK_OUTPUT_LOW();
	es8374_codec_i2c_delay(1); 
	CLK_OUTPUT_HIGH();
	es8374_codec_i2c_delay(1); 
	DATA_OUTPUT_HIGH();
	es8374_codec_i2c_delay(1); 
}


static void es8374_codec_i2c_write_byte(unsigned char data)
{
	int i;

	//data 0-7bit
	for(i = 7; i >= 0; i--)
	{
		CLK_OUTPUT_LOW();
		if(data & (0x1 << i)) // msb first, rising change and falling lock in codec
			DATA_OUTPUT_HIGH();
		else
			DATA_OUTPUT_LOW();
		es8374_codec_i2c_delay(1);
		CLK_OUTPUT_HIGH();
		es8374_codec_i2c_delay(1);      
	}
	//receive ack, bit9
	CLK_OUTPUT_LOW();
	SET_DATA_INPUT();

	es8374_codec_i2c_delay(1);          

	CLK_OUTPUT_HIGH();
	es8374_codec_i2c_delay(1);     
	set_data_output();
}

static void I2CWRNBYTE_CODEC(unsigned char reg, unsigned char val)
{
    UINT8 i2c_address = 0x20;

    es8374_codec_i2c_start();
    es8374_codec_i2c_write_byte(i2c_address); 	
    es8374_codec_i2c_write_byte(reg);
    es8374_codec_i2c_write_byte(val);
    es8374_codec_i2c_stop();
}


static void es8374_dac_mute(BOOL mute)
{
	if(mute)//mute dac
		I2CWRNBYTE_CODEC(0x36, 0x20);
	else     //unmute
		I2CWRNBYTE_CODEC(0x36, 0x00);
}

static void es8374_dac_volume(unsigned char vol)
{
	unsigned int  vol_tmp;
	if(vol < 100) 
	{   // 99 is the maximum volume, 0 is the minimum volume
		vol_tmp = (99 -  vol)*2;
		I2CWRNBYTE_CODEC(0x38, vol_tmp);
	}
}

static void es8374_set_sample_rate(unsigned int sr)
{
	switch(sr) 
	{
		case  8000: 
		/*
		*	set pll, 26MHZ->12.288MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x17); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07); //pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x66);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x06); //LRCK div =0600H = 1536D, lrck = 12.288M/1536 = 8K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x95);  //bclk = mclk/ 24 = 512K
//		es8374_dac_mute(0);   		
		break;
		
		case 16000: 
		/*
		*	set pll, 26MHZ->12.288MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x17); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07); //pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x33);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x03); //LRCK div =0300H = 768D, lrck = 12.288M/768 = 16K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x8C);  //bclk = mclk/ 12 = 1.024M
	//	es8374_dac_mute(0);   
		break;

		
		case 24000: 
		/*
		*	set pll, 26MHZ->12.288MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x17); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07); //pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x22);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x02); //LRCK div =0200H = 412D, lrck = 12.288M/412 = 24K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x88);  //bclk = mclk/ 8 = 1.536M
//		es8374_dac_mute(0);   
		break;

		
		case 32000: 
		/*
		*	set pll, 26MHZ->8.192MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x01); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xBC); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x11); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x05); //pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x11);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x01); //LRCK div =0100H = 256D, lrck =8.192M/256 = 32K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x84);  //bclk = mclk/ 4 = 3..072M
//		es8374_dac_mute(0);   
		break;

		
		case 48000: 
		/*
		*	set pll, 26MHZ->12.288MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x17); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07); //pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x11);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x01); //LRCK div =0100H = 256D, lrck =12.288M/256 = 48K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x84);  //bclk = mclk/ 8 = 3.072M
	//	es8374_dac_mute(0);   
		break;
		
		case 44100: 
		/*
		*	set pll, 26MHZ->11.2896MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x6F,0xA0); //pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41); //pll set:mode set
		
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start
		
		I2CWRNBYTE_CODEC(0x0C,0x27); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xDC); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2B); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x06);//pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x11);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x01); //LRCK div =0100H = 256D, lrck =11.2896M/256 = 44.1K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x84);  //bclk = mclk/ 4 = 2.828M
//		es8374_dac_mute(0);  
		break;
		
		case 22050: 
		/*
		*	set pll, 26MHZ->11.2896MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x27); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xDC); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2B); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x06);//pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x22);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x02); //LRCK div =0200H = 512D, lrck =11.2896M/512 = 22.05K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x88);  //bclk = mclk/ 8 = 1.414m
//		es8374_dac_mute(0);   				
		break;   
		
		case 11025: 
		/*
		*	set pll, 26MHZ->11.2896MHZ
		*/
		es8374_dac_mute(1);
		I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start

		I2CWRNBYTE_CODEC(0x0C,0x27); //pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xDC); //pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2B); //pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x8A); //pll set:
		I2CWRNBYTE_CODEC(0x0B,0x06);//pll set:n

		I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop

		I2CWRNBYTE_CODEC(0x05, 0x44);//adc&dac clkdiv
		I2CWRNBYTE_CODEC(0x02, 0x08);//pll enable
		I2CWRNBYTE_CODEC(0x06, 0x04); //LRCK div =0400H = 1024D, lrck =11.2896M/1024 = 11.025K
		I2CWRNBYTE_CODEC(0x07, 0x00);  
		I2CWRNBYTE_CODEC(0x0f, 0x8C);  //bclk = mclk/ 12 = 705K
//		es8374_dac_mute(0);   				
		break; 
		
		default: 
		break;
	}
}


static void es8374_standby(void)
{
	I2CWRNBYTE_CODEC(0x38,0xC0);
	I2CWRNBYTE_CODEC(0x25,0xC0);
	I2CWRNBYTE_CODEC(0x28,0x1C);
	I2CWRNBYTE_CODEC(0x36,0x20);
	I2CWRNBYTE_CODEC(0x37,0x01);
	I2CWRNBYTE_CODEC(0x1A,0x08);
	I2CWRNBYTE_CODEC(0x1D,0x10);
	I2CWRNBYTE_CODEC(0x1E,0x40);
	I2CWRNBYTE_CODEC(0x1C,0x10);
	I2CWRNBYTE_CODEC(0x24,0x20);
	I2CWRNBYTE_CODEC(0x22,0x00);
	I2CWRNBYTE_CODEC(0x21,0xD4);
	I2CWRNBYTE_CODEC(0x14,0x94);
	I2CWRNBYTE_CODEC(0x15,0xBF);
	I2CWRNBYTE_CODEC(0x09,0x80);
	I2CWRNBYTE_CODEC(0x01,0xFF);
	I2CWRNBYTE_CODEC(0x01,0x03);
}


static void ES8374_CODEC_Startup(void)
{
	I2CWRNBYTE_CODEC(0x00,0x3F); //IC Rst start
	I2CWRNBYTE_CODEC(0x00,0x03); //IC Rst stop
	I2CWRNBYTE_CODEC(0x01,0xFF); //IC clk on
	I2CWRNBYTE_CODEC(0x05,0x11); //clk div =1
	I2CWRNBYTE_CODEC(0x6F,0xA0); //pll set:mode enable
	I2CWRNBYTE_CODEC(0x72,0x41); //pll set:mode set
	I2CWRNBYTE_CODEC(0x09,0x01); //pll set:reset on ,set start
	/* PLL FOR 26MHZ/44.1KHZ */
	I2CWRNBYTE_CODEC(0x0C,0x27); //pll set:k
	I2CWRNBYTE_CODEC(0x0D,0xDC); //pll set:k
	I2CWRNBYTE_CODEC(0x0E,0x2B); //pll set:k
	I2CWRNBYTE_CODEC(0x0A,0x88); //pll set:
	I2CWRNBYTE_CODEC(0x0B,0x06); //pll set:n

	I2CWRNBYTE_CODEC(0x09,0x41); //pll set:reset off ,set stop
	I2CWRNBYTE_CODEC(0x0F,0x84); //MASTER MODE, BCLK = MCLK/4
	I2CWRNBYTE_CODEC(0x10,0x0C); //I2S-16BIT, ADC
	I2CWRNBYTE_CODEC(0x11,0x0C); //I2S-16BIT, DAC
	I2CWRNBYTE_CODEC(0x02,0x08);  //select PLL
	

	I2CWRNBYTE_CODEC(0x24,0x08); //adc set
	I2CWRNBYTE_CODEC(0x36,0x40); //dac set
	I2CWRNBYTE_CODEC(0x12,0x30); //timming set
	I2CWRNBYTE_CODEC(0x13,0x20); //timming set
	I2CWRNBYTE_CODEC(0x21,0x50); //adc set: SEL LIN1 CH+PGAGAIN=0DB
	I2CWRNBYTE_CODEC(0x22,0xFF); //adc set: PGA GAIN=0DB
	I2CWRNBYTE_CODEC(0x21,0x14); //adc set: SEL LIN1 CH+PGAGAIN=18DB
	I2CWRNBYTE_CODEC(0x22,0x55); //pga = +15db
	I2CWRNBYTE_CODEC(0x00,0x80); // IC START
	vTaskDelay(25); //DELAY_MS
	I2CWRNBYTE_CODEC(0x14,0x8A); // IC START
	I2CWRNBYTE_CODEC(0x15,0x40); // IC START
	I2CWRNBYTE_CODEC(0x1A,0xA0); // monoout set
	I2CWRNBYTE_CODEC(0x1B,0x19); // monoout set
	I2CWRNBYTE_CODEC(0x1C,0x90); // spk set
	I2CWRNBYTE_CODEC(0x1D,0x02); // spk set
	I2CWRNBYTE_CODEC(0x1F,0x00); // spk set
	I2CWRNBYTE_CODEC(0x1E,0xA0); // spk on
	I2CWRNBYTE_CODEC(0x28,0x00); // alc set
	I2CWRNBYTE_CODEC(0x25,0x00); // ADCVOLUME on
	I2CWRNBYTE_CODEC(0x38,0x0c); // DACVOLUMEL on
	I2CWRNBYTE_CODEC(0x37,0x30); // dac set
	I2CWRNBYTE_CODEC(0x6D,0x60); //SEL:GPIO1=DMIC CLK OUT+SEL:GPIO2=PLL CLK OUT

	I2CWRNBYTE_CODEC(0x36,0x20); //dac set
	I2CWRNBYTE_CODEC(0x37,0x00); // dac set                  
}

void es8374_codec_init(void)
{
	gpio_config_c(I2C_CLK,0);
	gpio_config_c(I2C_DAT,0);
	CLK_OUTPUT_HIGH();
	DATA_OUTPUT_HIGH();
	es8374_codec_i2c_delay(1);
	ES8374_CODEC_Startup();
}

void es8374_codec_configure(unsigned int fs, unsigned char datawidth)
{
	es8374_set_sample_rate(fs);
}

void es8374_codec_close(void)
{
	es8374_standby();
}

void es8374_codec_volume_control(unsigned char volume)
{  
	es8374_dac_volume(volume);
}

void es8374_codec_mute_control(BOOL enable)
{
    es8374_dac_mute(enable);
}
#endif
