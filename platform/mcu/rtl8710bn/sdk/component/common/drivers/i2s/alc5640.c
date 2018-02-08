#include <stdio.h>
#include "FreeRTOS.h"
#include "PinNames.h"
#include "basic_types.h"
#include "diag.h" 
#include <osdep_api.h>

#include "i2c_api.h"
#include "pinmap.h"

//#define I2C_MTR_SDA			PC_4//PB_3
//#define I2C_MTR_SCL			PC_5//PB_2
#if defined(CONFIG_PLATFORM_8195A)
	#define I2C_MTR_SDA			PB_3
	#define I2C_MTR_SCL			PB_2
#elif defined(CONFIG_PLATFORM_8711B)
	#define I2C_MTR_SDA			PA_30
	#define I2C_MTR_SCL			PA_29
#endif 
#define I2C_BUS_CLK			100000  //hz

#define I2C_ALC5640_ADDR	(0x38/2)

#define RT5640_PRIV_INDEX	0x6a
#define RT5640_PRIV_DATA	0x6c
	 
#if defined (__ICCARM__)
i2c_t   alc5640_i2c;
#else
volatile i2c_t   alc5640_i2c;
#endif

static void alc5640_delay(void)
{
    int i;

	i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
}

void alc5640_reg_write(unsigned int reg, unsigned int value)
{
	char buf[4];
	buf[0] = (char)reg;
	buf[1] = (char)(value>>8);
	buf[2] = (char)(value&0xff);
	
	i2c_write(&alc5640_i2c, I2C_ALC5640_ADDR, &buf[0], 3, 1);
	alc5640_delay();
}

void alc5640_reg_read(unsigned int reg, unsigned int *value)
{
	int tmp;
	char *buf = (char*)&tmp;	
	
	buf[0] = (char)reg;
	i2c_write(&alc5640_i2c, I2C_ALC5640_ADDR, &buf[0], 1, 1);
	alc5640_delay();
	
	buf[0] = 0xaa;
	buf[1] = 0xaa;
	
	i2c_read(&alc5640_i2c, I2C_ALC5640_ADDR, &buf[0], 2, 1);
	alc5640_delay();
	
	*value= ((buf[0]&0xFF)<<8)|(buf[1]&0xFF); 
}

void alc5640_index_write(unsigned int reg, unsigned int value)
{
	alc5640_reg_write(RT5640_PRIV_INDEX, reg);
	alc5640_reg_write(RT5640_PRIV_DATA, value);
}

void alc5640_index_read(unsigned int reg, unsigned int *value)
{
	alc5640_reg_write(RT5640_PRIV_INDEX, reg);
	alc5640_reg_read(RT5640_PRIV_DATA, value);
}

void alc5640_reg_dump(void)
{
	int i;
	unsigned int value;
  
	printf("alc5640 codec reg dump\n\r");
	printf("------------------------\n\r");
	for(i=0;i<=0xff;i++){
		alc5640_reg_read(i, &value);
		printf("%02x : %04x\n\r", i, (unsigned short)value);
	}
	printf("------------------------\n\r");
}

void alc5640_index_dump(void)
{
	int i;
	unsigned int value;
  
	printf("alc5640 codec index dump\n\r");
	printf("------------------------\n\r");
	for(i=0;i<=0xff;i++){
		alc5640_index_read(i, &value);
		printf("%02x : %04x\n\r", i, (unsigned short)value);
	}
	printf("------------------------\n\r");
}

void alc5640_init(void)
{
    i2c_init(&alc5640_i2c, I2C_MTR_SDA, I2C_MTR_SCL);
    i2c_frequency(&alc5640_i2c, I2C_BUS_CLK);
}

void alc5640_set_word_len(int len_idx)	// interface2
{
	// 0: 16 1: 20 2: 24 3: 8
	unsigned int val;
	alc5640_reg_read(0x71,&val);
	val &= (~(0x3<<2));
	val |= (len_idx<<2);
	alc5640_reg_write(0x71,val);
	alc5640_reg_read(0x70,&val);
	val &= (~(0x3<<2));
	val |= (len_idx<<2);
	alc5640_reg_write(0x70,val);	
	
}

void alc5640_init_interface1(void)
{
    // I2S1 -> DAC -> SPK R/L
    alc5640_reg_write(0x00,0x0021);
    alc5640_reg_write(0xD9,0x0009);
    alc5640_reg_write(0x73,0x0014);
    alc5640_reg_write(0xFA,0x3401);
    alc5640_index_write(0x1C,0x0D21);
    alc5640_reg_write(0x63,0xA8F0);
    wait_ms(100);  // delay 100 ms.
    alc5640_reg_write(0x63,0xE8F8);
    alc5640_reg_write(0x61,0x9801);
    alc5640_index_write(0x3D,0x2600);
    alc5640_index_write(0x1C,0xFD21);
    alc5640_reg_write(0x2A,0x1414);
    alc5640_reg_write(0x48,0xB800);
    alc5640_reg_write(0x49,0x1800);
    alc5640_reg_write(0x01,0x4848);
    alc5640_reg_write(0xD9,0x0809);  
}

void alc5640_init_interface2(void)
{
    alc5640_reg_write(0x00,0x0021);
    alc5640_reg_write(0x63,0xE8FE);
    alc5640_reg_write(0x61,0x5800);
    alc5640_reg_write(0x62,0x0C00);
    alc5640_reg_write(0x73,0x0000);
    alc5640_reg_write(0x2A,0x4242);
    alc5640_reg_write(0x45,0x2000);
    alc5640_reg_write(0x02,0x4848);
    alc5640_reg_write(0x8E,0x0019);
    alc5640_reg_write(0x8F,0x3100);
    alc5640_reg_write(0x91,0x0E00);
    alc5640_index_write(0x3D,0x3E00);
    alc5640_reg_write(0xFA,0x0011);
    alc5640_reg_write(0x83,0x0800);
    alc5640_reg_write(0x84,0xA000);
    alc5640_reg_write(0xFA,0x0C11);
    alc5640_reg_write(0x64,0x4010);
    alc5640_reg_write(0x65,0x0C00);
    alc5640_reg_write(0x61,0x5806);
    alc5640_reg_write(0x62,0xCC00);
    alc5640_reg_write(0x3C,0x004F);
    alc5640_reg_write(0x3E,0x004F);
    alc5640_reg_write(0x28,0x3030);
    alc5640_reg_write(0x2F,0x0080);	
}