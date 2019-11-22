#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "memory_attribute.h"
#include "hal_gpio.h"
#include "hal_gpio_internal.h"
GPIO_BASE_REGISTER_T *g_gpio_base = (GPIO_BASE_REGISTER_T *)(GPIO_BASE);

#define _CPOL     1
#define _CPHA     1

#define SCK_D(X)   hal_gpio_set_output(HAL_GPIO_16, X)//(X?(PORTA|=0X01):(PORTA&=0XFE))
#define MOSI_D(X)  hal_gpio_set_output(HAL_GPIO_15, X)//(X?(PORTA|=0X02):(PORTA&=0XFD))
#define SSEL_D(X)  hal_gpio_set_output(HAL_GPIO_17, X)//(X?(PORTA|=0X08):(PORTA&=0XF7))

#define MISO_I()  (g_gpio_base->GPIO_DIN.R & (GPIO_REG_ONE_BIT_SET_CLR<<HAL_GPIO_14))
void delay()
{
 unsigned char m,n;
     for(n=0;n<5;n++);
    for(m=0;m<100;m++);
}

void SPI_Init(void)
{
  
    mtk_hal_gpio_init(HAL_GPIO_17);//cs
    mtk_hal_gpio_init(HAL_GPIO_16);//clk
    mtk_hal_gpio_init(HAL_GPIO_15);//mosi
    mtk_hal_gpio_init(HAL_GPIO_14);//miso
    
    hal_pinmux_set_function(HAL_GPIO_17, HAL_GPIO_17_GPIO17);
    hal_pinmux_set_function(HAL_GPIO_16, HAL_GPIO_16_GPIO16);
    hal_pinmux_set_function(HAL_GPIO_15, HAL_GPIO_15_GPIO15);
    hal_pinmux_set_function(HAL_GPIO_14, HAL_GPIO_14_GPIO14);
    
    hal_gpio_set_direction(HAL_GPIO_17,HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_16,HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_15,HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_14,HAL_GPIO_DIRECTION_INPUT);
  SSEL_D(1);
  MOSI_D(1);
  #if _CPOL==0
  SCK_D(0);
  #else
  SCK_D(1);
  #endif
}


#if _CPOL==0&&_CPHA==0          //MODE   0  0   
void SPI_Send_Dat(unsigned char dat)
{
 unsigned char n;
 for(n=0;n<8;n++)
 {
  SCK_D(0);
  if(dat&0x80)MOSI_D(1);
  else MOSI_D(0);
  dat<<=1;
  SCK_D(1);
 }
  SCK_D(0);
}

unsigned char SPI_Receiver_Dat(void)
{
 unsigned char n ,dat,bit_t;
 for(n=0;n<8;n++)
 {
  SCK_D(0);
  dat<<=1;
  if(MISO_I())dat|=0x01;
  else dat&=0xfe;
  SCK_D(1);
 }
  SCK_D(0);
  return dat;
}
#endif

#if _CPOL==1&&_CPHA==0           //MODE   1  0
void SPI_Send_Dat(unsigned char dat)
{
 unsigned char n;
 for(n=0;n<8;n++)
 {
  SCK_D(1);
  if(dat&0x80)MOSI_D(1);
  else MOSI_D(0);
  dat<<=1;
  SCK_D(0);
 }
  SCK_D(1);
}

unsigned char SPI_Receiver_Dat(void)
{
 unsigned char n ,dat,bit_t;
 for(n=0;n<8;n++)
 {
  SCK_D(1);
  dat<<=1;
  if(MISO_I())dat|=0x01;
  else dat&=0xfe;
  SCK_D(0);
 }
  SCK_D(1);
  return dat;
}

#endif

#if _CPOL==0&&_CPHA==1           //MODE  0  1
void SPI_Send_Dat(unsigned char dat)
{
 unsigned char n;
 SCK_D(0);
 for(n=0;n<8;n++)
 {
  SCK_D(1);
  if(dat&0x80)MOSI_D(1);
  else MOSI_D(0);
  dat<<=1;
  SCK_D(0);
 }
}

unsigned char SPI_Receiver_Dat(void)
{
 unsigned char n ,dat,bit_t;
 for(n=0;n<8;n++)
 {
  SCK_D(1);
   dat<<=1;
  if(MISO_I())dat|=0x01;
  else dat&=0xfe;
  SCK_D(0);
 }
  SCK_D(0);
  return dat;
}
#endif
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

#if _CPOL==1&&_CPHA==1            //MODE  1  1
void SPI_Send_Dat(unsigned char dat)
{
 unsigned char n;
 SCK_D(1);
 for(n=0;n<8;n++)
 {
  SCK_D(0);
  if(dat&0x80)MOSI_D(1);
  else MOSI_D(0);
  dat<<=1;
  SCK_D(1);
 }
}

unsigned char SPI_Receiver_Dat(void)
{
 unsigned char n ,dat,bit_t;
 SCK_D(0);
 for(n=0;n<8;n++)
 { SCK_D(0);
  dat<<=1;
  if(MISO_I())dat|=0x01;
  else dat&=0xfe;
  SCK_D(1);
 }
  SCK_D(1);
  return dat;
}
#endif
