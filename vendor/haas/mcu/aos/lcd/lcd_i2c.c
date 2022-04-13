/************************************************************************************
*  Copyright (c), 2019, Bestechnic.com
*            All rights reserved.
*
*
* File name: oled_i2c.c
*
*
* Others: none;
*
* History: none;
*
*************************************************************************************/
#include "lcd_i2c.h"

//#ifdef OLED_USED_IIC
#include "codetab.h"
#include "hal_timer.h"
//#include "hal_i2c.h"
#include "hal_trace.h"
#include "aos/vfs/i2c.h"
#include "aos/vfs.h"
#if defined(OLED_USED_SPI)
#error "OLED_USED_IIC and OLED_USED_SPI can not used together"
#endif

//static uint32_t g_fd = -1;
/*OLED address default ox78,but BES IIC driver address<<1 so defined address >> 1*/
#define OLED_ADDRESS	 (0x78>>1) //By adjusting the OR resistor, the screen can be 0x78 and 0x7a two addresses - the default 0x78
#define CMD_REG_ADDRESS  0x00
#define DATA_REG_ADDRESS 0x40
//struct HAL_GPIO_I2C_CONFIG_T oled_i2c_gpio_cfg = {HAL_GPIO_PIN_P2_6, HAL_GPIO_PIN_P2_7, 100*1000};

static int32_t g_fd = -1;
void i2c_writebyte(uint8_t regaddr,uint8_t data)
{
	uint8_t buf[3];
    buf[0]  = (0x78>>1);
	buf[1] 	= regaddr;
	buf[2]  = data;
    write(g_fd,buf,3);
}

static void i2c_readbyte(uint8_t regaddr)
{
	uint32_t device_addr = OLED_ADDRESS;
	uint8_t buf[2];
	buf[0] 	= regaddr;
	//hal_gpio_i2c_recv(device_addr, buf, 1, 1,0);
	TRACE("check after write:0x%02x 0x%02x", buf[0], buf[1]);
}

void writecmd(uint8_t I2C_Command)//write command
{
	//oled_i2c_open();
	i2c_writebyte(CMD_REG_ADDRESS, I2C_Command);
}

void writedata(uint8_t I2C_Data)//write data
{
	//oled_i2c_open();
	i2c_writebyte(DATA_REG_ADDRESS, I2C_Data);
}

static void oled_delay(uint32_t ms)
{
	//hal_sys_timer_delay(MS_TO_TICKS(ms));
	osDelay(ms);
}

void oled_set_pos(uint8_t x, uint8_t y) //Set the starting coordinates
{
	writecmd(0xb0+y);
	writecmd(((x&0xf0)>>4)|0x10);
	writecmd((x&0x0f)|0x01);
}

void oled_display_fill(uint8_t fill_Data)//Full screen fill
{
	uint8_t m,n;
	for(m=0; m<8; m++)
	{
		writecmd(0xb0+m);		//page0-page1
		writecmd(0x00);		//low column start address
		writecmd(0x10);		//high column start address
		for(n=0; n<132; n++)
		{
			writedata(fill_Data);
		}
	}
}

void oled_clean_screen(void)//clean screen
{
	oled_display_fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : OLED wake up from sleep
//--------------------------------------------------------------
void oled_display_on(void)
{
	writecmd(0X8D);  //Set the charge pump
	writecmd(0X14);  //Turn on the charge pump
	writecmd(0XAF);  //OLED wake up
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : OLED sleep ,Power consumption 10uA
//--------------------------------------------------------------
void oled_display_off(void)
{
	writecmd(0X8D);  //Set the charge pump
	writecmd(0X10);  //Turn off the charge pump
	writecmd(0XAE);  //OLED sleep
}
void oled_showchar(u8 x,u8 y,u8 chr,u8 Char_Size)
{
	unsigned char c=0,i=0;
	c=chr-' ';
	if(x>Max_Column-1)
	{
		x=0;
		y=y+2;
	}
	if(Char_Size ==16)
	{
		oled_set_pos(x,y);
		for(i=0; i<8; i++)
			writedata(F8X16[c*16+i]);
		oled_set_pos(x,y+1);
		for(i=0; i<8; i++)
			writedata(F8X16[c*16+i+8]);
	}
	else
	{
		oled_set_pos(x,y);
		for(i=0; i<6; i++)
			writedata(F6x8[c][i]);

	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowString(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          :
// Parameters     : x,y -- Starting coordinates(x:0~127, y:0~7); ch[] --The character to display; TextSize -- Character size(1:6*8 ; 2:8*16)
// Description    :
//--------------------------------------------------------------
void oled_showstring(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
{
	unsigned char j=0;
	while (ch[j]!='\0')
	{
		oled_showchar(x,y,ch[j],TextSize);
		x+=8;
		if(x>120)
		{
			x=0;
			y+=2;
		}
		j++;
	}

}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCHinese(unsigned char x, unsigned char y, unsigned char Numebr)
// Calls          :
// Parameters     : x,y -- Starting coordinates(x:0~127, y:0~7); N:Index of Chinese characters codetab.h
// Description    :
//--------------------------------------------------------------
void oled_showchinese(uint8_t x, uint8_t y, uint8_t Numebr)
{
	uint8_t 	wm=0;
	uint32_t   adder=32*Numebr;
	oled_set_pos(x, y);
	for(wm = 0; wm < 16; wm++)
	{
		writedata(F16x16[adder]);
		adder += 1;
	}
	oled_set_pos(x,y + 1);
	for(wm = 0; wm < 16; wm++)
	{
		writedata(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          :
// Parameters     : x0,y0 -- Starting coordinates(x0:0~127, y0:0~7); x1,y1 --Diagonal end coordinate(x1:1~128,y1:1~8)
// Description    :
//--------------------------------------------------------------
void oled_drawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[])
{
	uint32_t j=0;
	uint8_t x,y;

	if(y1%8==0)
		y = y1/8;
	else
		y = y1/8 + 1;
	for(y=y0; y<y1; y++)
	{
		oled_set_pos(x0,y);
		for(x=x0; x<x1; x++)
		{
			writedata(BMP[j++]);
		}
	}
}

void oled_test(void)
{
	uint32_t i =0;
	oled_clean_screen();
	osDelay(2000);
	oled_showstring(30,3,"2019-10-12",2);
	osDelay(1000);
	oled_display_fill(0xFF);//
	osDelay(1000);
	oled_display_fill(0x00);//
	osDelay(1000);
	for(i=0; i<5; i++)
	{
		oled_showchinese(25+i*16,0,i);
	}
	osDelay(1000);
	//OLED_ShowStr(0,3,"Hello World",1);//
	oled_showstring(0,3,"Bestechnic.com",2);
	osDelay(4000);
	oled_clean_screen();//
	osDelay(100);
	oled_display_off();//
	osDelay(500);
	oled_display_on();//
	oled_drawBMP(0,0,128,8,(uint8_t *)BMP1);
	osDelay(2000);
}

const uint8_t oled_init_cmd[27]=
{
	0xAE,//display off
	0x00,//---set low column address
	0x10,//---set high column address
	0x40,//--set start line address
	0xB0,//Set Page Start Address for Page Addressing Mode,0-7
	0x81,//--set contrast control register
	0xff,//dimming 0x00~0xff
	0xa1,//--set segment re-map 0 to 127
	0xa6,//--set normal display
	0xa8,//--set multiplex ratio(1 to 64)
	0x3F,
	0xc8,//Set COM Output Scan Direction
	0xd3,//-set display offset
	0x00,//-not offset
	0xd5,
	0x80,
	0xd8,//--set area color mode off
	0x05,
	0xd9,//--Set Pre-Charge Period
	0xF1,
	0xda,//--set com pins hardware configuration
	0x12,
	0xdb,//--set vcomh
	0x30,
	0x8d,//--set DC-DC enable
	0x14,
	0xaf,//--turn on oled panel
};

void oled_init(void)
{
    TRACE("%s\r\n",__FUNCTION__);
	oled_delay(200); //important
	uint32_t i;
	for(i=0; i<27; i++)
	{
		writecmd(oled_init_cmd[i]);
	}
	oled_clean_screen();
}
void oled_i2c_open(void)
{
    TRACE("%s\r\n",__FUNCTION__);
    char *dev_name = VFS_I2C_1_PATH;
    g_fd = open(dev_name,O_RDWR);
    TRACE("open %s %d\r\n",dev_name, g_fd);
}

void oled_main(void)
{
    TRACE("%s\r\n",__FUNCTION__);
	oled_i2c_open();
    oled_init();
    oled_test();
}

//#endif //OLED_USED_IIC

