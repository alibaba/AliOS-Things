#include "PinNames.h"
#include "basic_types.h"
#include <osdep_api.h>

#include "i2c_api.h"
#include "i2c_ex_api.h"
#include "pinmap.h"
#include "wait_api.h"
#include "alc5679.h"

#define I2C_ALC5679_ADDR	        (0X5A/2)//(0x58/2)
#if defined(CONFIG_PLATFORM_8195A)
	#define I2C_MTR_SDA			PB_3
	#define I2C_MTR_SCL			PB_2
#elif defined(CONFIG_PLATFORM_8711B)
	#define I2C_MTR_SDA			PA_30
	#define I2C_MTR_SCL			PA_29
#endif 
#define I2C_BUS_CLK			100000  //100K HZ

//i2c_t   rt5679_i2c;
#if defined (__ICCARM__)
i2c_t   rt5679_i2c;
#else
volatile i2c_t   rt5679_i2c;
#define printf  DBG_8195A
#endif


static void rt5679_delay(void)
{
    int i;
    
    i=10000;
    
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }
}

u8 rt5679_reg_write(u16 reg, u16 val)
{
	int length = 0;
	char buf[4];
	buf[0] = (char)(reg >> 8);
	buf[1] = (char)(reg&0xff);
	buf[2] = (char)(val>>8);
	buf[3] = (char)(val&0xff);

	length = i2c_write(&rt5679_i2c, I2C_ALC5679_ADDR, &buf[0], 4, 1);
        rt5679_delay();
	return (length==4)?0:1;
}

u8 rt5679_reg_read(u16 reg, u16* val)
{
	int tmp;
	char *buf = (char*)&tmp;	
	u8 ret = 0;
	
	buf[0] = (char)(reg >> 8);
	buf[1] = (char)(reg&0xff);
        
	if(i2c_write(&rt5679_i2c, I2C_ALC5679_ADDR, &buf[0], 2, 1) != 2){
		DBG_8195A("rt5679_reg_read(): write register addr fail\n");
		ret = 1;
	}
	rt5679_delay();
	
	buf[0] = 0xaa;
	buf[1] = 0xaa;
	
	if(i2c_read(&rt5679_i2c, I2C_ALC5679_ADDR, &buf[0], 2, 1) < 2){
		DBG_8195A("rt5679_reg_read(): read register value fail\n");
		ret = 1;
	}else
		*val = ((buf[0]&0xFF)<<8)|(buf[1]&0xFF); 
        rt5679_delay();
	return ret;
}

u16 rt5679_reg_modify(u16 reg, u16 val, u16 iMask)
{
	u16 val1;
	
	rt5679_reg_read(reg, &val1);
	
	u16 val2 = (val1 &(~iMask))|val;
	if(!rt5679_reg_write(reg, val2)) return 0;
	return val2;
}

void rt5679_i2c_init(void)
{
    i2c_init(&rt5679_i2c, I2C_MTR_SDA, I2C_MTR_SCL);
    i2c_frequency(&rt5679_i2c, I2C_BUS_CLK);
}

int rt5679_check_id(void)
{
	unsigned short ret = 0;
  	rt5679_reg_read(RT5679_VENDOR_ID2, &ret);
        printf("Device with ID register is %x \n", ret);
            if (ret != RT5679_DEVICE_ID) {
                    printf("Device with ID register %x is not rt5679\n", ret);
                    return 1;
            }else{
            return 0;
        }
}

void rt5679_reg_dump(void)
{
	int i;
	unsigned short value;
  
	printf("rt5679 codec reg dump\n\r");
	printf("------------------------\n\r");
	for(i=0;i<=0xff;i++){
		rt5679_reg_read(i, &value);
		printf("%02x : %04x\n\r", i, value);
	}
	printf("------------------------\n\r");
}

void rt5679_mic_to_i2s(void)
{
        rt5679_reg_write(0x0000,0x10EC);
	rt5679_reg_write(0x00FA,0x0001);
	rt5679_reg_write(0x0076,0x0777);
	rt5679_reg_write(0x0078,0x0000);
	rt5679_reg_write(0x004A,0x8080);
	rt5679_reg_write(0x0050,0x8553);
	rt5679_reg_write(0x0061,0x8000);
	rt5679_reg_write(0x0062,0x8000);
	rt5679_reg_write(0x00C2,0x5000);
}

void rt5679_linein_to_i2s(void)
{
	rt5679_reg_write(0x0000,0x10EC);
	rt5679_reg_write(0x061D,0x04DF);
	rt5679_reg_write(0x0007,0x1010);
	rt5679_reg_write(0x004A,0x4040);
	rt5679_reg_write(0x0060,0x0060);
	rt5679_reg_write(0x0061,0x8000);
	rt5679_reg_write(0x0062,0x8000);
	rt5679_reg_write(0x0063,0xE340);
	rt5679_reg_write(0x0064,0xC000);
	rt5679_reg_write(0x0066,0x2000);
	//rt5679_reg_write(0x0070,0x8020);//32 bit
        rt5679_reg_write(0x0070,0x8000);//16bit
        //rt5679_reg_write(0x0070,0x8040);//enable mono mode
	rt5679_reg_write(0x0076,0x0777);
	rt5679_reg_write(0x0078,0x0000);
	rt5679_reg_write(0x00FA,0x0001);
	rt5679_reg_write(0x0610,0xB490);
}

void rt5679_i2s_to_hp(void)
{
	rt5679_reg_write(0x0000,0x10EC);
	rt5679_reg_write(0x0609,0x1122);
	rt5679_reg_write(0x060A,0x3622);
	rt5679_reg_write(0x060B,0x1022);
	rt5679_reg_write(0x060C,0x3622);
	rt5679_reg_write(0x0671,0xC0D0);
	rt5679_reg_write(0x0603,0x0444);
	rt5679_reg_write(0x068F,0x0007);
	rt5679_reg_write(0x0690,0x0007);
	rt5679_reg_write(0x0684,0x0217);
	rt5679_reg_write(0x0122,0x0000);
	rt5679_reg_write(0x0121,0x0000);
	rt5679_reg_write(0x0014,0x5454);
	rt5679_reg_write(0x0673,0xAEAA);
	rt5679_reg_write(0x0660,0x3840);
	///////////////////////////////
	rt5679_reg_write(0x0661,0x3840);
	rt5679_reg_write(0x0665,0x0101);
	rt5679_reg_write(0x0681,0x0118);
	rt5679_reg_write(0x0682,0x0118);
	rt5679_reg_write(0x07F3,0x0008);
	rt5679_reg_write(0x061D,0xE4CF);
	rt5679_reg_write(0x00FA,0x0001);
	rt5679_reg_write(0x0076,0x0777);
	rt5679_reg_write(0x0078,0x0000);
	rt5679_reg_write(0x0660,0x3840);
	rt5679_reg_write(0x0661,0x3840);
	rt5679_reg_write(0x0070,0x8000);//16bit 8000 32bit 8020
	rt5679_reg_write(0x0040,0xF0AA);
	rt5679_reg_write(0x0046,0x8080);
	////////////////////////////////
	rt5679_reg_write(0x0061,0x8000);
	rt5679_reg_write(0x0062,0x0400);
	rt5679_reg_write(0x061D,0xE4CF);
	rt5679_reg_write(0x0063,0xA240);
	rt5679_reg_write(0x0066,0x1680);
	wait_ms(20); 
	rt5679_reg_write(0x0063,0xE340);
	rt5679_reg_write(0x0066,0x1F80);
	rt5679_reg_write(0x0066,0xDF80);
	rt5679_reg_write(0x0080,0x6000);
	rt5679_reg_write(0x0063,0xE342);
	rt5679_reg_write(0x0076,0x1777);
	rt5679_reg_write(0x0066,0xFF80);
	rt5679_reg_write(0x000A,0x5353);
	rt5679_reg_write(0x0614,0xB490);
	rt5679_reg_write(0x0060,0x0003);
	rt5679_reg_write(0x0401,0x0630);
	////////////////////////////////
	rt5679_reg_write(0x0403,0x0267);
	rt5679_reg_write(0x0404,0x9ECD);
	rt5679_reg_write(0x0400,0x7D00);
	rt5679_reg_write(0x0400,0xFD00);
	wait_ms(650); 
	rt5679_reg_write(0x0080,0x0000);
	rt5679_reg_write(0x0063,0xE340);
	rt5679_reg_write(0x0076,0x0777);
	rt5679_reg_write(0x019B,0x0003);
	rt5679_reg_write(0x0003,0x8080);
	rt5679_reg_write(0x0066,0xDF80);
	rt5679_reg_write(0x000A,0x5455);
	rt5679_reg_write(0x0614,0xA490);
	rt5679_reg_write(0x0060,0x0000);
	rt5679_reg_write(0x0404,0x9E0C);
}
