#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__


#include "string.h"
#include "cmsis.h"

//#define OLED_USED_SPI
#define OLED_USED_IIC


#define OLED_MODE   0
#define SIZE 		16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;

 		     
#define OLED_CMD  0	
#define OLED_DATA 1	




void oled_configuration_init(void);	 
void oled_display_on(void);
void oled_display_off(void);	   							   		    
void oled_init(void);
void oled_clean_screen(void);
void oled_drawpoint(uint8_t x,uint8_t y,uint8_t t);
void oled_display_fill(uint8_t fill_Data);
void oled_shownum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void oled_showstring(uint8_t x,uint8_t y, uint8_t *chr,uint8_t text_size);	 
void oled_set_pos(uint8_t x, uint8_t y);
void oled_showchinese(uint8_t x,uint8_t y,uint8_t Numebr);
void oled_drawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);
void oled_testBMP(void);
void oled_test(void);
void oled_main(void);










#endif //__OLED_H__
