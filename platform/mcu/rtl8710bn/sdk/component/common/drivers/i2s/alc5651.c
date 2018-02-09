#include <stdio.h>
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

#define I2C_ALC5651_ADDR	(0x34/2)

#define RT5651_PRIV_INDEX	0x6a
#define RT5651_PRIV_DATA	0x6c
	 
#if defined (__ICCARM__)
i2c_t   alc5651_i2c;
#else
volatile i2c_t   alc5651_i2c;
#define printf  DBG_8195A
#endif

static void alc5651_delay(void)
{
    int i;

	i=10000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
}

void alc5651_reg_write(unsigned int reg, unsigned int value)
{
	char buf[4];
	buf[0] = (char)reg;
	buf[1] = (char)(value>>8);
	buf[2] = (char)(value&0xff);
	
	i2c_write(&alc5651_i2c, I2C_ALC5651_ADDR, &buf[0], 3, 1);
	alc5651_delay();
}

void alc5651_reg_read(unsigned int reg, unsigned int *value)
{
	int tmp;
	char *buf = (char*)&tmp;	
	
	buf[0] = (char)reg;
	i2c_write(&alc5651_i2c, I2C_ALC5651_ADDR, &buf[0], 1, 1);
	alc5651_delay();
	
	buf[0] = 0xaa;
	buf[1] = 0xaa;
	
	i2c_read(&alc5651_i2c, I2C_ALC5651_ADDR, &buf[0], 2, 1);
	alc5651_delay();
	
	*value= ((buf[0]&0xFF)<<8)|(buf[1]&0xFF); 
}

void alc5651_index_write(unsigned int reg, unsigned int value)
{
	alc5651_reg_write(RT5651_PRIV_INDEX, reg);
	alc5651_reg_write(RT5651_PRIV_DATA, value);
}

void alc5651_index_read(unsigned int reg, unsigned int *value)
{
	alc5651_reg_write(RT5651_PRIV_INDEX, reg);
	alc5651_reg_read(RT5651_PRIV_DATA, value);
}

void alc5651_reg_dump(void)
{
	int i;
	unsigned int value;
  
	printf("alc5651 codec reg dump\n\r");
	printf("------------------------\n\r");
	for(i=0;i<=0xff;i++){
		alc5651_reg_read(i, &value);
		printf("%02x : %04x\n\r", i, (unsigned short)value);
	}
	printf("------------------------\n\r");
}

void alc5651_index_dump(void)
{
	int i;
	unsigned int value;
  
	printf("alc5651 codec index dump\n\r");
	printf("------------------------\n\r");
	for(i=0;i<=0xff;i++){
		alc5651_index_read(i, &value);
		printf("%02x : %04x\n\r", i, (unsigned short)value);
	}
	printf("------------------------\n\r");
}

void alc5651_init(void)
{
    i2c_init(&alc5651_i2c, I2C_MTR_SDA, I2C_MTR_SCL);
    i2c_frequency(&alc5651_i2c, I2C_BUS_CLK);
}

void alc5651_set_word_len(int len_idx)	// interface2
{
	// 0: 16 1: 20 2: 24 3: 8
	unsigned int val;
	alc5651_reg_read(0x71,&val);
	val &= (~(0x3<<2));
	val |= (len_idx<<2);
	alc5651_reg_write(0x71,val);
	alc5651_reg_read(0x70,&val);
	val &= (~(0x3<<2));
	val |= (len_idx<<2);
	alc5651_reg_write(0x70,val);	
	
}

void alc5651_init_interface1(void)
{
    alc5651_reg_write(0x00,0x0021);
    alc5651_reg_write(0x63,0xE8FE);
    alc5651_reg_write(0x61,0x5800);
    alc5651_reg_write(0x62,0x0C00);
    alc5651_reg_write(0x73,0x0000);
    alc5651_reg_write(0x2A,0x4242);
    alc5651_reg_write(0x45,0x2000);
    alc5651_reg_write(0x02,0x4848);
    alc5651_reg_write(0x8E,0x0019);
    alc5651_reg_write(0x8F,0x3100);
    alc5651_reg_write(0x91,0x0E00);
    alc5651_index_write(0x3D,0x3E00);
    alc5651_reg_write(0xFA,0x0011);
    alc5651_reg_write(0x83,0x0800);
    alc5651_reg_write(0x84,0xA000);
    alc5651_reg_write(0xFA,0x0C11);
    alc5651_reg_write(0x64,0x4010);
    alc5651_reg_write(0x65,0x0C00);
    alc5651_reg_write(0x61,0x5806);
    alc5651_reg_write(0x62,0xCC00);
    alc5651_reg_write(0x3C,0x004F);
    alc5651_reg_write(0x3E,0x004F);
    alc5651_reg_write(0x27,0x3820);
    alc5651_reg_write(0x77,0x0000);	
}

void alc5651_init_interface2(void)
{
  	int reg_value=0;
    alc5651_reg_write(0x00,0x0021);//reset all, device id 1
	alc5651_reg_write(0x63,0xE8FE);//Power managerment control 3: 
	//VREF1&2 on, both slow VREF, MBIAS on, MBIAS bandcap power on, L & R HP Amp on, improve HP Amp driving enabled
	alc5651_reg_write(0x61,0x5800);//power managerment control 1:
	//I2S2 digital interface on, Analog DACL1 & DACR1 on. 
    alc5651_reg_write(0x62,0x0C00);//stereo1 & 2 DAC filter power on
	alc5651_reg_write(0x73,0x0000);//ADC/DAC Clock control 1:
	//I2S Clock Pre-Divider 1 & 2: /1.  Stereo DAC Over Sample Rate : 128Fs
    alc5651_reg_write(0x2A,0x4242);//Stereo DAC digital mixer control
	//Un-mute DACL2 to Stereo DAC Left & Right Mixer
	alc5651_reg_write(0x45,0x2000);//HPOMIX: Un-mute DAC1 to HPOMIX
	alc5651_reg_write(0x02,0x4848);//HP Output Control:
	//Unmute HPOL, HPOR
//    alc5651_reg_write(0x0F,0x1F1F);//INL & INR Volume Control
//    alc5651_reg_write(0x0D,0x0800);//IN1/2 Input Control
//    alc5651_reg_write(0x1C,0x7F7F);//Stereo1 ADC Digital Volume Control
//    alc5651_reg_write(0x1E,0xF000);// ADC Digital Boost Gain Control
    alc5651_reg_write(0x8E,0x0019);//HP Amp Control 1
//	Enable HP Output, Charge Pump Power On, HP Amp All Power On
    alc5651_reg_write(0x8F,0x3100);//HP Amp Control 2, HP Depop Mode 2
    alc5651_reg_write(0x91,0x0E00);//HP Amp Control 3, select HP capless power mode
    alc5651_index_write(0x3D,0x3E00);//unknown
    alc5651_reg_write(0xFA,0x0011);//enable input clock
    alc5651_reg_write(0x83,0x0800);//default ASRC control 1
	alc5651_reg_write(0x84,0xA000);//ASRC control 2: I2S1 enable ASRC mode, Sterol1 DAC filter ASRC mode. 
//	alc5651_reg_write(0xFA,0x0C11);//?  ?  ?  MX-FAh[15:4]reserved
	alc5651_reg_write(0x64,0x4010);//power managerment control 4: 
	//MIC BST2 Power On; MIC2 SE Mode single-end mode or line-input mode
	alc5651_reg_write(0x65,0x0C00);//power managerment control 5:  RECMIX L & R power on
	alc5651_reg_write(0x61,0x5806);//power managerment control 1: 
//	I2S2 Digital Interface On, Analog DACL1, DACR1 power on; Analog ADCL, ADCR power on
	alc5651_reg_write(0x62,0xCC00);//power managerment control 2: Stereo1&2 ADC/DAC digital filter power on
    alc5651_reg_write(0x3C,0x004F);//RECMIXL
    alc5651_reg_write(0x3E,0x004F);//RECMIXR
	alc5651_reg_write(0x28,0x3030);//stereo2 ADC digital mixer control : Mute Stereo2 ADC L&R channel, ADCR
	alc5651_reg_write(0x2F,0x0080);	//Interface DAC/ADC Data control: Select IF2 ADCDAT Data Source IF1_ADC2 
}