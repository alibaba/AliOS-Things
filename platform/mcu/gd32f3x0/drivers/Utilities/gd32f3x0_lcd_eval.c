/*!
    \file  gd32f3x0_lcd_eval.c
    \brief LCD driver functions (LCD_ILI9320)
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, firmware for GD32F3x0
*/

#include "usb_delay.h"
#include "lcd_font.h"
#include "gd32f3x0_lcd_eval.h"


#ifdef H_VIEW
    #define X_MAX_PIXEL         320
    #define Y_MAX_PIXEL         240
#else
    #define X_MAX_PIXEL         240
    #define Y_MAX_PIXEL         320
#endif

static uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte);
static void spi0_init(void);
static void lcd_write_index(uint8_t index);
static void lcd_write_data(uint8_t data);

static void lcd_write_data_16bit(uint8_t datah,uint8_t datal);
static void lcd_reset(void);


/*!
    \brief      write an unsigned 8-bit bytes
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  byte: write byte
    \param[out] none
    \retval     none
*/
static uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte)
{
    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_TBE));
    SPI_DATA(spi_periph) = byte;

    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_RBNE));
    return(SPI_DATA(spi_periph));
} 

/*!
    \brief      init SPI0
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi0_init(void)
{
    spi_parameter_struct spi_init_struct;

    /* enable the gpio clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SPI0);

    /* GPIOB config, PB3(LCD_SPI_CLK), PB4(SPI0_MISO), PB5(LCD_SPI_MOSI) */
    gpio_af_set(GPIOB, GPIO_AF_0,GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    /* GPIOC config */
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4);

    /* GPIOF config */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* set crc polynomial */
    spi_crc_polynomial_set(SPI0, 7);
    spi_enable(SPI0);
}

/*!
    \brief      write the register address
    \param[in]  index: register address
    \param[out] none
    \retval     none
*/
static void lcd_write_index(uint8_t index)
{
    LCD_RS_CLR;
    spi_write_byte(SPI0,index);
}

/*!
    \brief      write the register data
    \param[in]  data: register data
    \param[out] none
    \retval     none
*/
static void lcd_write_data(uint8_t data)
{
    LCD_RS_SET;
    spi_write_byte(SPI0,data);
}

/*!
    \brief      write the register data(an unsigned 16-bit data)
    \param[in]  datah: register data high 8bit
    \param[in]  datal: register data low 8bit
    \param[out] none
    \retval     none
*/
static void lcd_write_data_16bit(uint8_t datah,uint8_t datal)
{
    lcd_write_data(datah);
    lcd_write_data(datal);
}

/*!
    \brief      lcd reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_reset(void)
{
    LCD_RST_CLR;

    delay_ms(100);

    LCD_RST_SET;

    delay_ms(50);
}


/*!
    \brief      lcd init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    spi0_init();

    LCD_CS_CLR;
    lcd_reset();

    /* write the register address 0xCB*/
    lcd_write_index(0xCB);
    lcd_write_data(0x39);
    lcd_write_data(0x2C);
    lcd_write_data(0x00);
    lcd_write_data(0x34);
    lcd_write_data(0x02);

    /* write the register address 0xCF*/
    lcd_write_index(0xCF);
    lcd_write_data(0x00);
    lcd_write_data(0XC1);
    lcd_write_data(0X30);

    /* write the register address 0xE8*/
    lcd_write_index(0xE8);
    lcd_write_data(0x85);
    lcd_write_data(0x00);
    lcd_write_data(0x78);

    /* write the register address 0xEA*/
    lcd_write_index(0xEA);
    lcd_write_data(0x00);
    lcd_write_data(0x00);

    /* write the register address 0xED*/
    lcd_write_index(0xED);
    lcd_write_data(0x64);
    lcd_write_data(0x03);
    lcd_write_data(0X12);
    lcd_write_data(0X81);

    /* write the register address 0xF7*/
    lcd_write_index(0xF7);
    lcd_write_data(0x20);

    /* power control VRH[5:0] */
    lcd_write_index(0xC0);
    lcd_write_data(0x23);

    /* power control SAP[2:0];BT[3:0] */
    lcd_write_index(0xC1);
    lcd_write_data(0x10);

    /* vcm control */
    lcd_write_index(0xC5);
    lcd_write_data(0x3e);
    lcd_write_data(0x28); 

    /* vcm control2 */
    lcd_write_index(0xC7);
    lcd_write_data(0x86);

    lcd_write_index(0x36);
#ifdef H_VIEW
    lcd_write_data(0xE8);
#else
    lcd_write_data(0x48); 
#endif
    /* write the register address 0x3A*/
    lcd_write_index(0x3A);
    lcd_write_data(0x55);

    /* write the register address 0xB1*/
    lcd_write_index(0xB1);
    lcd_write_data(0x00);
    lcd_write_data(0x18);

    /* display function control */
    lcd_write_index(0xB6);
    lcd_write_data(0x08); 
    lcd_write_data(0x82);
    lcd_write_data(0x27);  

    /* 3gamma function disable */
    lcd_write_index(0xF2);
    lcd_write_data(0x00); 

    /* gamma curve selected  */
    lcd_write_index(0x26);
    lcd_write_data(0x01); 

    /* set gamma  */
    lcd_write_index(0xE0);
    lcd_write_data(0x0F);
    lcd_write_data(0x31);
    lcd_write_data(0x2B);
    lcd_write_data(0x0C);
    lcd_write_data(0x0E);
    lcd_write_data(0x08);
    lcd_write_data(0x4E);
    lcd_write_data(0xF1);
    lcd_write_data(0x37);
    lcd_write_data(0x07);
    lcd_write_data(0x10);
    lcd_write_data(0x03);
    lcd_write_data(0x0E);
    lcd_write_data(0x09);
    lcd_write_data(0x00);

    /* set gamma  */
    lcd_write_index(0XE1);
    lcd_write_data(0x00);
    lcd_write_data(0x0E);
    lcd_write_data(0x14);
    lcd_write_data(0x03);
    lcd_write_data(0x11);
    lcd_write_data(0x07);
    lcd_write_data(0x31);
    lcd_write_data(0xC1);
    lcd_write_data(0x48);
    lcd_write_data(0x08);
    lcd_write_data(0x0F);
    lcd_write_data(0x0C);
    lcd_write_data(0x31);
    lcd_write_data(0x36);
    lcd_write_data(0x0F);

    /* exit sleep */
    lcd_write_index(0x11);
    delay_ms(120); 

    /* display on */
    lcd_write_index(0x29);
    lcd_write_index(0x2c);

    LCD_CS_SET;
}

/*!
    \brief      set lcd display region
    \param[in]  x_start: the x position of the start point
    \param[in]  y_start: the y position of the start point
    \param[in]  x_end: the x position of the end point
    \param[in]  y_end: the y position of the end point
    \param[out] none
    \retval     none
*/
void lcd_set_region(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
    LCD_CS_CLR;

    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x_start >> 8,x_start);
    lcd_write_data_16bit(x_end >> 8,x_end);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y_start >> 8,y_start);
    lcd_write_data_16bit(y_end >> 8,y_end);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
    LCD_CS_SET;
}

/*!
    \brief      set the start display point of lcd
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[out] none
    \retval     none
*/
void lcd_set_xy(uint16_t x,uint16_t y)
{
    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x >> 8,x);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y >> 8,y);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
}

/*!
    \brief      draw a point on the lcd
    \param[in]  x: the x position of the point 
    \param[in]  y: the y position of the point 
    \param[in]  data: write the register data
    \param[out] none
    \retval     none
*/
void lcd_draw_point(uint16_t x,uint16_t y,uint16_t data)
{
    lcd_set_xy(x,y);
    lcd_write_data(data >> 8);
    lcd_write_data(data);
}

/*!
    \brief      clear the lcd
    \param[in]  color: lcd display color 
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    unsigned int i,m;
    /* set lcd display region */
    lcd_set_region(0,0,X_MAX_PIXEL - 1,Y_MAX_PIXEL - 1);
    LCD_RS_SET;

    LCD_CS_CLR;
    for(i = 0;i < Y_MAX_PIXEL;i ++){
        for(m = 0;m < X_MAX_PIXEL;m ++){
            spi_write_byte(SPI0,color >> 8);
            spi_write_byte(SPI0,color);
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      bgr to rgb format conversion
    \param[in]  c: bgr color value
    \param[out] none
    \retval     rgb color value
*/
uint16_t lcd_bgr2rgb(uint16_t c)
{
    uint16_t r,g,b,rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

/*!
    \brief      gui circle
    \param[in]  x: the x position of the start point 
    \param[in]  y: the y position of the start point
    \param[in]  r: the radius of circle
    \param[in]  fc: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t fc)
{
    unsigned short a,b;
    int c;
    a = 0;
    b = r;
    c = 3 - 2 * r;
    LCD_CS_CLR;

    while(a < b){
        /* draw points on the lcd */
        lcd_draw_point(x + a,y + b,fc);
        lcd_draw_point(x - a,y + b,fc);
        lcd_draw_point(x + a,y - b,fc);
        lcd_draw_point(x - a,y - b,fc);
        lcd_draw_point(x + b,y + a,fc);
        lcd_draw_point(x - b,y + a,fc);
        lcd_draw_point(x + b,y - a,fc);
        lcd_draw_point(x - b,y - a,fc);

        if(c < 0)
            c = c + 4 * a + 6;
        else{ 
            c = c + 4 * (a - b) + 10;
            b -= 1; 
        } 
        a += 1;
    } 
    if(a == b){
        /* draw points on the lcd */
        lcd_draw_point(x + a,y + b,fc);
        lcd_draw_point(x + a,y + b,fc);
        lcd_draw_point(x + a,y - b,fc);
        lcd_draw_point(x - a,y - b,fc);
        lcd_draw_point(x + b,y + a,fc);
        lcd_draw_point(x - b,y + a,fc);
        lcd_draw_point(x + b,y - a,fc);
        lcd_draw_point(x - b,y - a,fc);
    } 
    LCD_CS_SET;
}

/*!
    \brief      gui draw line
    \param[in]  x0: the x position of the start point
    \param[in]  y0: the y position of the start point
    \param[in]  x1: the x position of the end point
    \param[in]  y1: the y position of the end point
    \param[in]  color: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_line_draw(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color)
{
    /* - difference in x's
       - difference in y's
       - dx,dy * 2
       - amount in pixel space to move during drawing
       - amount in pixel space to move during drawing
       - the discriminant i.e. error i.e. decision variable
       - used for looping */
    int dx,dy,dx2,dy2,x_inc,y_inc,error,index;

    LCD_CS_CLR;

    lcd_set_xy(x0,y0);
    /* calculate x distance */
    dx = x1 - x0;
    /* calculate y distance */
    dy = y1 - y0;

    if(dx >= 0){
        x_inc = 1;
    }else{
        x_inc = -1;
        dx = -dx;
    } 

    if(dy >= 0){
        y_inc = 1;
    }else{
        y_inc = -1;
        dy    = -dy; 
    } 

    dx2 = dx << 1;
    dy2 = dy << 1;

    if(dx > dy){
        /* initialize error */
        error = dy2 - dx;
        /* draw the line */
        for(index = 0;index <= dx;index ++){
            lcd_draw_point(x0,y0,color);
            /* test if error has overflowed */
            if(0 <= error){
                error -= dx2;
                /* move to next line */
                y0 += y_inc;
             }
            /* adjust the error term */
            error += dy2;
            /* move to the next pixel */
            x0 += x_inc;
         }
    }else{
        /* initialize error term */
        error = dx2 - dy;
        /* draw the linedraw the line*/
        for(index = 0;index <= dy;index ++){
            /* set the pixel */
            lcd_draw_point(x0,y0,color);

            /* test if error overflowed */
            if(0 <= error){
                error -= dy2;
                /* move to next line */
                x0 += x_inc;
            }
            /* adjust the error term */
            error += dx2;

            /* move to the next pixel */
            y0 += y_inc;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui box
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  bc: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_rect_draw(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t line_color)
{
    LCD_CS_CLR;

    /* gui draw line*/
    lcd_line_draw(x,y,x + w,y,line_color);
    lcd_line_draw(x + w,y,x + w,y + h,line_color);
    lcd_line_draw(x,y + h,x + w,y + h,line_color);
    lcd_line_draw(x,y,x,y + h,line_color);

    LCD_CS_SET;
}

/*!
    \brief      gui box2
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  mode: display color combination mode 
    \param[out] none
    \retval     none
*/
void gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode)
{
    LCD_CS_CLR;

    /* gui box2 display mode0 */
    if(0 == mode){
        lcd_line_draw(x,y,x + w,y,0xEF7D);
        lcd_line_draw(x + w - 1,y + 1,x + w - 1,y + 1 + h,0x2965);
        lcd_line_draw(x,y + h,x + w,y + h,0x2965);
        lcd_line_draw(x,y,x,y + h,0xEF7D);
    }
    /* gui box2 display mode1 */
    if(1 == mode){
        lcd_line_draw(x,y,x + w,y,0x2965);
        lcd_line_draw(x + w - 1,y + 1,x + w - 1,y + 1 + h,0xEF7D);
        lcd_line_draw(x,y + h,x + w,y + h,0xEF7D);
        lcd_line_draw(x,y,x,y + h,0x2965);
    }
    /* gui box2 display mode2 */
    if(2 == mode){
        lcd_line_draw(x,y,x + w,y,0xffff);
        lcd_line_draw(x + w - 1,y + 1,x + w - 1,y + 1 + h,0xffff);
        lcd_line_draw(x,y + h,x + w,y + h,0xffff);
        lcd_line_draw(x,y,x,y + h,0xffff);
    }
    LCD_CS_SET;
}

/*!
    \brief      gui rect
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[in]  fc: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_rect_color_draw(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t fc)
{
    int ix,iy;
    LCD_CS_CLR;
    for(ix = x1;ix < x2;ix ++){
        for(iy = y1;iy < y2;iy ++)
            /* set the pixel */
            lcd_draw_point(ix,iy,fc);
    }

    LCD_CS_SET;
}

/*!
    \brief      display button down
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_down(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with gray color*/
    lcd_line_draw(x1,y1,x2,y1,GRAY2);
    lcd_line_draw(x1 + 1,y1 + 1,x2,y1 + 1,GRAY1);
    lcd_line_draw(x1,y1,x1,y2,GRAY2);
    lcd_line_draw(x1 + 1,y1 + 1,x1 + 1,y2,GRAY1);

    /* gui draw line with white color*/
    lcd_line_draw(x1,y2,x2,y2,WHITE);
    lcd_line_draw(x2,y1,x2,y2,WHITE);
    LCD_CS_SET;
}

/*!
    \brief      display button up
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_up(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with white color*/
    lcd_line_draw(x1,y1,x2,y1,WHITE);
    lcd_line_draw(x1,y1,x1,y2,WHITE);

    /* gui draw line with gray color*/
    lcd_line_draw(x1 + 1,y2 - 1,x2,y2 - 1,GRAY1);
    lcd_line_draw(x1,y2,x2,y2,GRAY2);
    lcd_line_draw(x2 - 1,y1 + 1,x2 - 1,y2,GRAY1);
    lcd_line_draw(x2,y1,x2,y2,GRAY2);
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk16
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void lcd_draw_font_gbk16(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,char *s)
{
    unsigned char i,j;
    unsigned short k,x0;
    x0 = x;

    LCD_CS_CLR;
    while(*s){
        /* ASCII character table from 32 to 128 */
        if(((uint8_t)(*s)) < 128){
            k = *s;
            if(13 == k){
                x = x0;
                y += 16;
            }else{
                if(k > 32)
                    k -= 32;
                else 
                    k = 0;
                for(i = 0;i < 16;i ++)
                    for(j = 0;j < 8;j ++){
                        if(asc16[k * 16 + i] & (0x80 >> j))
                            /* draw a point on the lcd */
                            lcd_draw_point(x + j,y + i,fc);
                        else{
                            if(fc != bc)
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j,y + i,bc);
                        }
                    }
                x += 8;
            }
            s ++;
        }else{
            for(k = 0;k < hz16_num;k ++){
                if((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1))){ 
                    for(i = 0;i < 16;i ++){
                        for(j = 0; j < 8; j ++){
                            if(hz16[k].Msk[i * 2] & (0x80 >> j))
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j,y + i,fc);
                            else{
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                    lcd_draw_point(x + j,y + i,bc);
                            }
                        }
                        for(j = 0;j < 8;j ++){
                            if(hz16[k].Msk[i * 2 + 1] & (0x80 >> j))
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j + 8,y + i,fc);
                            else{
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                    lcd_draw_point(x + j + 8,y + i,bc);
                            }
                        }
                    }
                }
            }
            s += 2;
            x += 16;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk24
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void lcd_draw_font_gbk24(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,char *s)
{
    unsigned char i,j;
    unsigned short k;

    LCD_CS_CLR;
    while(*s){
        /* ASCII character table from 32 to 128 */
        if(((uint8_t)(*s)) < 0x80){
            k = *s;
            if(k > 32)
                k -= 32;
            else
                k = 0;

            for(i = 0;i < 16;i ++)
                for(j = 0;j < 8;j ++){
                    if(asc16[k * 16 + i] & (0x80 >> j))
                        /* draw a point on the lcd */
                        lcd_draw_point(x + j,y + i,fc);
                    else{
                        if(fc != bc)
                            /* draw a point on the lcd */
                            lcd_draw_point(x + j,y + i,bc);
                    }
                }
            s ++;
            x += 8;
        }else{
            for(k = 0;k < hz24_num;k ++){
                if((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1))){ 
                    for(i = 0;i < 24;i ++){
                        for(j = 0;j < 8;j ++){
                            if(hz24[k].Msk[i * 3] & (0x80 >> j))
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j,y + i,fc);
                            else{
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                    lcd_draw_point(x + j,y + i,bc);
                            }
                        }
                        for(j = 0;j < 8;j ++){
                            if(hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j + 8,y + i,fc);
                            else{
                                if(fc != bc)
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j + 8,y + i,bc);
                            }
                        }
                        for(j = 0;j < 8;j ++){
                            if(hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                                /* draw a point on the lcd */
                                lcd_draw_point(x + j + 16,y + i,fc);
                            else{
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                    lcd_draw_point(x + j + 16,y + i,bc);
                            }
                        }
                    }
                }
            }
        s += 2;
        x += 24;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to num32
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  num: display num
    \param[out] none
    \retval     none
*/
void lcd_draw_font_num32(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,uint16_t num)
{
    unsigned char i,j,k,c;

    LCD_CS_CLR;
    for(i = 0;i < 32;i ++){
        for(j = 0;j < 4;j++){
            c = *(sz32 + num * 32 * 4 + i * 4 + j);
            for(k = 0;k < 8;k ++){
                if(c & (0x80 >> k))
                    /* draw a point on the lcd */
                    lcd_draw_point(x + j * 8 + k,y + i,fc);
                else{
                    if(fc != bc)
                        /* draw a point on the lcd */
                        lcd_draw_point(x + j * 8 + k,y + i,bc);
                }
            }
        }
    }
    LCD_CS_SET;
}
