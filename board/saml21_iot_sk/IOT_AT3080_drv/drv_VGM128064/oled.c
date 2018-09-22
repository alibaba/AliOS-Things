/**
 ******************************************************************************
 * @file    oled.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   oled driver VGM128064 functions
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */


#include <string.h>
#include <stdio.h>

#include "mx_hal.h"
#include "oled.h"
#include "oledfont.h"

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

#define OLED_MODE 0
#define CHAR_SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define OLED_CMD  0
#define OLED_DATA 1

/******************************************************************************
 *                           Static Function Declarations
 ******************************************************************************/

/**
 *  SSD1106 I2C bus driver
 */
static int ssd1106_i2c_bus_init(void);
static void ssd1106_delay_ms(uint16_t nms);
static int ssd1106_i2c_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

/* OLED operation functions */
void OLED_WR_Bytes(uint8_t *dat, uint8_t len, uint8_t cmd)
{  
  if(cmd)
    ssd1106_i2c_bus_write(0x40, dat, len);
  else
    ssd1106_i2c_bus_write(0x00, dat, len);
}

void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
  OLED_WR_Bytes(&dat, 1, cmd);
}


void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  uint8_t tmp[3] = {0xb0+y, ((x&0xf0)>>4)|0x10, (x&0x0f)|0x01};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}   	  

void OLED_Display_On(void)
{
  uint8_t tmp[3] = {0X8D, 0X14, 0XAF};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}

void OLED_Display_Off(void)
{
  uint8_t tmp[3] = {0X8D, 0X10, 0XAE};
  OLED_WR_Bytes( tmp, 3, OLED_CMD);
}		   			 

void OLED_Clear(void)  
{  
  uint8_t i;
  uint8_t tmp_cmd[3] = {0X0, 0x00, 0x10};
  uint8_t tmp[128];
  memset( tmp, 0x0, 128 );
  for(i=0;i<8;i++)  
  {  
    tmp_cmd[0] = 0xb0+i;
    OLED_WR_Bytes( tmp_cmd, 3, OLED_CMD);
    OLED_WR_Bytes( tmp, 128, OLED_DATA);
  }
}

void OLED_FillAll(void)
{
  uint8_t i;
  uint8_t tmp_cmd[3] = {0X0, 0x00, 0x10};
  uint8_t tmp[128];
  memset( tmp, 0xFF, 128 );
  for(i=0;i<8;i++)
  {
    tmp_cmd[0] = 0xb0+i;
    OLED_WR_Bytes( tmp_cmd, 3, OLED_CMD);
    OLED_WR_Bytes( tmp, 128, OLED_DATA);
  } 
}

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  unsigned char c=0;	
  c=chr-' ';
  if(x>Max_Column-1){x=0;y=y+2;}
  if(CHAR_SIZE == 16)
  {
    OLED_Set_Pos(x,y);	
    OLED_WR_Bytes( (uint8_t *)&F8X16[c*16], 8, OLED_DATA );
    OLED_Set_Pos(x,y+1);
    OLED_WR_Bytes( (uint8_t *)&F8X16[c*16+8], 8, OLED_DATA );
  }
  else {	
    OLED_Set_Pos(x,y+1);
    OLED_WR_Bytes( (uint8_t *)&F6x8[c*6], 6, OLED_DATA );
    
  }
}

uint32_t oled_pow(uint8_t m,uint8_t n)
{
  uint32_t result=1;	 
  while(n--)result*=m;    
  return result;
}				  

void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
  uint8_t t,temp;
  uint8_t enshow=0;						   
  for(t=0;t<len;t++)
  {
    temp=(num/oled_pow(10,len-t-1))%10;
    if(enshow==0&&t<(len-1))
    {
      if(temp==0)
      {
        OLED_ShowChar(x+(size/2)*t,y,' ');
        continue;
      }else enshow=1; 
      
    }
    OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
  }
} 

void OLED_ShowString(uint8_t x,uint8_t y, const char *chr)
{
  unsigned char j=0;
  uint8_t x_t = x,y_t = y;
  
  while (chr[j]!='\0')
  {	
    // add for CR/LF
    if( ('\r' == chr[j]) && ('\n' == chr[j+1]) ){  // CR LF
      while(x_t <= 120){  // fill rest chars in current line
        OLED_ShowChar(x_t,y_t,' ');
        x_t += 8;
      }
      j += 2;
    }
    else if( ('\r' == chr[j]) || ('\n' == chr[j]) ){   // CR or LF
      while(x_t <= 120){  // fill rest chars in current line
        OLED_ShowChar(x_t,y_t,' ');
        x_t += 8;
      }
      j += 1;
    }
    else{
      if(x_t>120){  // line end, goto next line
        x_t = 0;
        y_t += 2;
        if(y_t >= 8){  // can only display 4 line
          break;
        }
      }
      OLED_ShowChar(x_t,y_t,chr[j]);
      x_t += 8;
      j++;
    }
  }
}


void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
  uint8_t t,adder=0;
  OLED_Set_Pos(x,y);	
  for(t=0;t<16;t++)
  {
    OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
    adder+=1;
  }	
  OLED_Set_Pos(x,y+1);	
  for(t=0;t<16;t++)
  {	
    OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
    adder+=1;
  }					
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
  unsigned int j=0;
  unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {      
      OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
    }
  }
} 


void OLED_Init(void)
{
  if( ssd1106_i2c_bus_init() )
  {
      printf( "OLED_ERROR: I2C port init err." );
      return;
  }
  
  OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
  OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
  OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
  OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
  OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping
  OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction
  OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
  OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
  OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD);//-not offset
  OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
  OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02,OLED_CMD);//
  OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
  OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)   
  
  OLED_Clear();
  OLED_Set_Pos(0,0); 	
  OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 

  return;
}  



/******************************************************************************
 *                          SSD1106 bus driver
 ******************************************************************************/

/* I2C bus port */
#include <driver_init.h>

#include "mx_hal.h"

static void *i2c = NULL;

static void ssd1106_delay_ms(uint16_t nms)
{
	mx_hal_delay_ms(nms);
}


static int ssd1106_i2c_bus_init(void)
{
	i2c = mx_hal_i2c_init(NULL);
	
	gpio_set_pin_level(LCD_RST, true);
	ssd1106_delay_ms(100);
	gpio_set_pin_level(LCD_RST, false);
	ssd1106_delay_ms(100);
	gpio_set_pin_level(LCD_RST, true);
	  
	return 0;
}

static int ssd1106_i2c_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{	
	int ret = 0;
	ret = mx_hal_i2c_cmd_write(i2c, 0x3c, reg_addr, reg_data, cnt);
	
	if( ret ) printf("ssd1106_i2c_bus_write err\r\n");
	return ret;
}



