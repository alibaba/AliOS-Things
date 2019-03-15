/*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/

#ifndef __LIBLCDC_H__
#define __LIBLCDC_H__

#include "HR8P2xx.h"
#include "type.h"

typedef enum
{
    Sel_0 = 0x0,
    Sel_1 = 0x1,
    Sel_All=0x2,
} LCDSEL_TYPE;

typedef enum
{
    LCD_IE_OFF = 0x1,
    LCD_IE_RF  = 0x2,
} LCD_TYPE_IE;

typedef enum
{
    LCD_Coms_1 = 0, /*  */
    LCD_Coms_2 = 4, /* COM1~COM0*/
    LCD_Coms_3 = 6, /* COM2~COM0*/
    LCD_Coms_4 = 1, /* COM3~COM0*/
    LCD_Coms_6 = 2, /* COM5~COM0*/
    LCD_Coms_8 = 3, /* COM7~COM0*/
} LCD_TYPE_COMS;

typedef enum
{
    LCD_WaveMode_A = 0x0,
    LCD_WaveMode_B = 0x1,
} LCD_TYPE_WFS;

typedef enum
{
    LCD_Bias_3 = 0x0,
    LCD_Bias_4 = 0x3,
} LCD_TYPE_BIAS;

typedef enum
{
    LCD_Res_15k  = 0x0,
    LCD_Res_50k  = 0x1,
    LCD_Res_100k = 0x2,
    LCD_Res_200k = 0x3,
} LCD_TYPE_LCDRS; 

typedef enum
{
    LCD_Grayscale_3v6 = 0x0 , // Vbias = 3.6V
    LCD_Grayscale_3v5 = 0x1 , // Vbias = 3.5V
    LCD_Grayscale_3v4 = 0x2 , // Vbias = 3.4V
    LCD_Grayscale_3v2 = 0x3 , // Vbias = 3.2V
    LCD_Grayscale_3v1 = 0x4 , // Vbias = 3.1V
    LCD_Grayscale_3v0 = 0x5 , // Vbias = 3.0V
    LCD_Grayscale_2v9 = 0x6 , // Vbias = 2.9V
    LCD_Grayscale_2v8 = 0x7 , // Vbias = 2.8V
} LCD_TYPE_BVS;

typedef enum
{
    LCD_BiasSel_0 = 0x0,
    LCD_BiasSel_1 = 0x1,
    LCD_BiasSel_2 = 0x2,
    LCD_BiasSel_3 = 0x3,
} LCD_TYPE_BIAS_SEL;

typedef enum
{
    LCD_Bias_PwmSel_0 = 0x0,
    LCD_Bias_PwmSel_1 = 0x1,
    LCD_Bias_PwmSel_2 = 0x2,
    LCD_Bias_PwmSel_3 = 0x3,
    LCD_Bias_PwmSel_4 = 0x4,
    LCD_Bias_PwmSel_5 = 0x5,
    LCD_Bias_PwmSel_6 = 0x6, 
    LCD_Bias_PwmSel_7 = 0x7,
    LCD_Bias_PwmSel_8 = 0x8,
} LCD_TYPE_BIAS_PWMSEL;

typedef enum
{
    LCD_Byte_0 = 0x0, 
    LCD_Byte_1 = 0x1, 
    LCD_Byte_2 = 0x2, 
    LCD_Byte_3 = 0x3, 
} LCD_DD_BYTE; 

typedef enum
{
    LCD_HalfWord_0 = 0x0,
    LCD_HalfWord_1 = 0x1,
} LCD_DD_HALFWORD; 

typedef enum
{
    LCD_Pixel_LCDD0  = 0x0,
    LCD_Pixel_LCDD1  = 0x1,
    LCD_Pixel_LCDD2  = 0x2,
    LCD_Pixel_LCDD3  = 0x3,
    LCD_Pixel_LCDD4  = 0x4,
    LCD_Pixel_LCDD5  = 0x5,
    LCD_Pixel_LCDD6  = 0x6,
    LCD_Pixel_LCDD7  = 0x7,
    LCD_Pixel_LCDD8  = 0x8,
    LCD_Pixel_LCDD9  = 0x9,
    LCD_Pixel_LCDD10 = 0xA,
    LCD_Pixel_LCDD11 = 0xB,
    LCD_Pixel_LCDD12 = 0xC,
    LCD_Pixel_LCDD13 = 0xD,
} LCD_TYPE_PIXEL;

typedef struct
{
    LCD_TYPE_COMS        LCD_Coms;
    TYPE_FUNCEN          LCD_LcdFlik;
    LCD_TYPE_WFS         LCD_LcdWFS;
    LCD_TYPE_BIAS        LCD_Bias;
    LCD_TYPE_LCDRS       LCD_RS;
    LCD_TYPE_BVS         LCD_Grayscale;
    LCD_TYPE_BIAS_SEL    LCD_BIAS_SEL;
    LCD_TYPE_BIAS_PWMSEL LCD_BIAS_PWMSEL;
    uint8_t              LCD_PRS;
    uint32_t             LCD_SEG0;
#ifdef HR8P287
    uint32_t             LCD_SEG1 : 16;
#else
    uint32_t             LCD_SEG1 : 24;
#endif
} LCD_InitStruType;

#define LCD_Enable()  (LCDC->CON1.EN= 0x1)
#define LCD_Disable() (LCDC->CON1.EN= 0x0)

#define LCD_FLIK_Enable()  (LCDC->CON0.FLIK = 0x1)
#define LCD_FLIK_Disable() (LCDC->CON0.FLIK = 0x0)

#define LCD_SetRFREQ()   (LCDC->CON1.RFREQ = 0x1)
#define LCD_ClearRFREQ() (LCDC->CON1.RFREQ = 0x0)

#define LCD_ClearOFFIF() ( LCDC->IF.OFFIF = 0x1)

#define LCD_ClearRFIF() (LCDC->IF.RFIF = 0x1)

#define LCD_Rest() (LCDC->CON1.RST = 0x01)

ErrorStatus LCD_Init(LCD_InitStruType * LCD_InitStruct,LCDSEL_TYPE SELx);
void LCD_ITConfig(LCD_TYPE_IE LCD_IT,TYPE_FUNCEN NewState);
ITStatus LCD_GetRFITStatus(void);
ITStatus LCD_GetOFFITStatus(void);
void LCD_GrayscaleConfig(LCD_TYPE_BVS LCD_Grayscale );
ErrorStatus LCD_FlickerTimeConfig(uint8_t On_Time,uint8_t Off_Time);
ErrorStatus  LCD_PixelWriteByte(LCD_TYPE_PIXEL LCD_DD, LCD_DD_BYTE nByte ,uint8_t LCD_data);
ErrorStatus  LCD_PixelWriteHalfWord(LCD_TYPE_PIXEL LCD_DD, LCD_DD_HALFWORD nHalfWord , uint16_t LCD_data);
ErrorStatus  LCD_PixelWriteWord(LCD_TYPE_PIXEL LCD_DD,uint32_t LCD_data);

#endif

/*************************END OF FILE**********************/
